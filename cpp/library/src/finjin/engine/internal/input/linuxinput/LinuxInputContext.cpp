//Copyright (c) 2017 Finjin
//
//This file is part of Finjin Engine (finjin-engine).
//
//Finjin Engine is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
//This Source Code Form is subject to the terms of the Mozilla Public
//License, v. 2.0. If a copy of the MPL was not distributed with this
//file, You can obtain one at http://mozilla.org/MPL/2.0/.


//Includes----------------------------------------------------------------------
#include "FinjinPrecompiled.hpp"
#include "LinuxInputContext.hpp"
#include "finjin/common/StaticVector.hpp"
#include "finjin/engine/InputDeviceSerializer.hpp"
#include "finjin/engine/OSWindow.hpp"
#include "LinuxInputSystem.hpp"

using namespace Finjin::Engine;


//Local types-------------------------------------------------------------------
struct LinuxInputContext::Impl : public AllocatedClass, public OSWindowEventListener
{
    Impl(Allocator* allocator, LinuxInputSystem* inputSystem);

    void WindowOnKeyDown(OSWindow* osWindow, int softwareKey, int hardwareCode, bool controlDown, bool shiftDown, bool altDown) override;
    void WindowOnKeyUp(OSWindow* osWindow, int softwareKey, int hardwareCode, bool controlDown, bool shiftDown, bool altDown) override;

    void WindowOnMouseWheel(OSWindow* osWindow, PointerType pointerType, int pointerID, InputCoordinate x, InputCoordinate y, Buttons buttons, float wheelDelta) override;
    void WindowOnPointerMove(OSWindow* osWindow, PointerType pointerType, int pointerID, InputCoordinate x, InputCoordinate y, Buttons buttons) override;
    void WindowOnPointerDown(OSWindow* osWindow, PointerType pointerType, int pointerID, InputCoordinate x, InputCoordinate y, Buttons buttons) override;
    void WindowOnPointerUp(OSWindow* osWindow, PointerType pointerType, int pointerID, InputCoordinate x, InputCoordinate y, Buttons buttons) override;

    LinuxInputSystem* inputSystem;

    LinuxInputContext::Settings settings;
    AssetClassFileReader inputDevicesAssetReader;
    AssetClassFileReader inputBindingsAssetReader;

    int updateCount;

    StaticVector<LinuxGameController, GameControllerConstants::MAX_GAME_CONTROLLERS> gameControllers;

    LinuxMouse mouse;

    LinuxKeyboard keyboard;

    ByteBuffer configFileBuffer;
};


//Implementation----------------------------------------------------------------

//LinuxInputContext::Impl
LinuxInputContext::Impl::Impl(Allocator* allocator, LinuxInputSystem* inputSystem) : AllocatedClass(allocator), settings(allocator), mouse(allocator), keyboard(allocator)
{
    this->inputSystem = inputSystem;

    this->updateCount = 0;

    this->gameControllers.maximize();
    for (auto& gameController : this->gameControllers)
        gameController.SetAllocator(allocator);
    this->gameControllers.clear();
}

void LinuxInputContext::Impl::WindowOnKeyDown(OSWindow* osWindow, int softwareKey, int hardwareCode, bool controlDown, bool shiftDown, bool altDown)
{
    auto componentIndex = static_cast<size_t>(hardwareCode);
    if (componentIndex < this->keyboard.GetButtonCount())
    {
        auto component = this->keyboard.GetButton(componentIndex);
        component->Update(true);
    }
}

void LinuxInputContext::Impl::WindowOnKeyUp(OSWindow* osWindow, int softwareKey, int hardwareCode, bool controlDown, bool shiftDown, bool altDown)
{
    auto componentIndex = static_cast<size_t>(hardwareCode);
    if (componentIndex < this->keyboard.GetButtonCount())
    {
        auto component = this->keyboard.GetButton(componentIndex);
        component->Update(false);
    }
}

void LinuxInputContext::Impl::WindowOnMouseWheel(OSWindow* osWindow, PointerType pointerType, int pointerID, InputCoordinate x, InputCoordinate y, Buttons buttons, float wheelDelta)
{
    if (this->mouse.GetAxisCount() >= 3)
    {
        auto component = this->mouse.GetAxis(2);
        component->Update(wheelDelta * MouseConstants::WHEEL_STEP_SIZE, false);
    }
}

void LinuxInputContext::Impl::WindowOnPointerMove(OSWindow* osWindow, PointerType pointerType, int pointerID, InputCoordinate x, InputCoordinate y, Buttons buttons)
{
    if (pointerType == PointerType::MOUSE)
    {
        if (this->mouse.GetAxisCount() >= 2)
        {
            auto xComponent = this->mouse.GetAxis(0);
            xComponent->Update(x.ToDipsValue(), false);

            auto yComponent = this->mouse.GetAxis(1);
            yComponent->Update(y.ToDipsValue(), false);
        }
    }
}

void LinuxInputContext::Impl::WindowOnPointerDown(OSWindow* osWindow, PointerType pointerType, int pointerID, InputCoordinate x, InputCoordinate y, Buttons buttons)
{
    if (pointerType == PointerType::MOUSE)
    {
        for (size_t componentIndex = 0; componentIndex < this->mouse.GetButtonCount(); componentIndex++)
        {
            if (buttons.IsUsed(componentIndex) && buttons.IsPressed(componentIndex))
            {
                auto component = this->mouse.GetButton(componentIndex);
                component->Update(true);
            }
        }
    }
}

void LinuxInputContext::Impl::WindowOnPointerUp(OSWindow* osWindow, PointerType pointerType, int pointerID, InputCoordinate x, InputCoordinate y, Buttons buttons)
{
    if (pointerType == PointerType::MOUSE)
    {
        for (size_t componentIndex = 0; componentIndex < this->mouse.GetButtonCount(); componentIndex++)
        {
            if (buttons.IsUsed(componentIndex) && !buttons.IsPressed(componentIndex))
            {
                auto component = this->mouse.GetButton(componentIndex);
                component->Update(false);
            }
        }
    }
}

//LinuxInputContext
LinuxInputContext::LinuxInputContext(Allocator* allocator, LinuxInputSystem* inputSystem) :
    AllocatedClass(allocator),
    impl(AllocatedClass::New<Impl>(allocator, FINJIN_CALLER_ARGUMENTS, inputSystem))
{
}

LinuxInputContext::~LinuxInputContext()
{
}

LinuxInputContext::Impl* LinuxInputContext::GetImpl()
{
    return impl.get();
}

void LinuxInputContext::Create(const Settings& settings, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    assert(settings.osWindow != nullptr);
    assert(settings.assetFileReader != nullptr);

    FINJIN_ENGINE_CHECK_IMPL_NOT_NULL(impl, error);

    impl->settings = settings;

    if (!impl->configFileBuffer.Create(EngineConstants::DEFAULT_CONFIGURATION_BUFFER_SIZE, impl->GetAllocator()))
    {
        FINJIN_SET_ERROR(error, "Failed to allocate config buffer.");
        return;
    }

    impl->inputDevicesAssetReader.Create(*impl->settings.assetFileReader, impl->settings.initialAssetFileSelector, AssetClass::INPUT_DEVICE, impl->GetAllocator(), error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to create input device asset file reader.");
        return;
    }

    impl->inputBindingsAssetReader.Create(*impl->settings.assetFileReader, impl->settings.initialAssetFileSelector, AssetClass::INPUT_BINDINGS, impl->GetAllocator(), error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to create input bindings asset file reader.");
        return;
    }

    //Enumerate the devices-------------------------------------
    LinuxGameController::CreateGameControllers(impl->gameControllers);
    for (auto& gameController : impl->gameControllers)
    {
        ConfigureInputDevice(gameController, gameController.GetProductDescriptor(), impl->inputDevicesAssetReader, impl->configFileBuffer);
    }

    //Mice
    impl->mouse.Create(this, 0);
    ConfigureInputDevice(impl->mouse, impl->mouse.GetProductDescriptor(), impl->inputDevicesAssetReader, impl->configFileBuffer);

    //Keyboard
    impl->keyboard.Create(this, 0);
    ConfigureInputDevice(impl->keyboard, impl->keyboard.GetProductDescriptor(), impl->inputDevicesAssetReader, impl->configFileBuffer);

    impl->updateCount = 0;
    Update(0);

    //Set up window event listener
    impl->settings.osWindow->AddWindowEventListener(impl.get());
}

void LinuxInputContext::Destroy()
{
    //Remove window event listener
    if (impl->settings.osWindow != nullptr)
        impl->settings.osWindow->RemoveWindowEventListener(impl.get());

    //Destroy devices
    for (auto& device : impl->gameControllers)
        device.Destroy();
    impl->gameControllers.clear();

    impl->mouse.Destroy();

    impl->keyboard.Destroy();
}

void LinuxInputContext::GetSelectorComponents(AssetPathSelector& result)
{
}

const LinuxInputContext::Settings& LinuxInputContext::GetSettings() const
{
    return impl->settings;
}

void LinuxInputContext::Update(SimpleTimeDelta elapsedTime, InputDevicePollFlag flags)
{
    auto isFirstUpdate = impl->updateCount++ == 0;

    LinuxGameController::UpdateGameControllers(impl->gameControllers, elapsedTime, isFirstUpdate);

    if (AnySet(flags & InputDevicePollFlag::CHECK_FOR_NEW_DEVICES))
    {
        if (LinuxGameController::CheckForNewGameControllers(impl->gameControllers))
        {
            for (auto& device : impl->gameControllers)
            {
                if (device.IsNewConnection())
                    ConfigureInputDevice(device, device.GetProductDescriptor(), impl->inputDevicesAssetReader, impl->configFileBuffer);
            }
        }
    }

    impl->mouse.Update(elapsedTime, isFirstUpdate);

    impl->keyboard.Update(elapsedTime, isFirstUpdate);
}

void LinuxInputContext::Execute(InputEvents& events, InputCommands& commands, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);
}

void LinuxInputContext::HandleDeviceChanges()
{
    for (auto& device : impl->gameControllers)
        device.ClearChanged();

    impl->keyboard.ClearChanged();

    impl->mouse.ClearChanged();
}

void LinuxInputContext::HandleApplicationViewportLostFocus()
{
}

void LinuxInputContext::HandleApplicationViewportGainedFocus()
{
}

size_t LinuxInputContext::GetDeviceCount(InputDeviceClass deviceClass) const
{
    switch (deviceClass)
    {
        case InputDeviceClass::KEYBOARD: return 1;
        case InputDeviceClass::MOUSE: return 1;
        case InputDeviceClass::GAME_CONTROLLER: return impl->gameControllers.size();
        default: break;
    }

    return 0;
}

const Utf8String& LinuxInputContext::GetDeviceProductDescriptor(InputDeviceClass deviceClass, size_t index) const
{
    switch (deviceClass)
    {
        case InputDeviceClass::KEYBOARD: return impl->keyboard.GetProductDescriptor();
        case InputDeviceClass::MOUSE: return impl->mouse.GetProductDescriptor();
        case InputDeviceClass::GAME_CONTROLLER: return impl->gameControllers[index].GetProductDescriptor();
        default: break;
    }

    return Utf8String::GetEmpty();
}

const Utf8String& LinuxInputContext::GetDeviceInstanceDescriptor(InputDeviceClass deviceClass, size_t index) const
{
    switch (deviceClass)
    {
        case InputDeviceClass::KEYBOARD: return impl->keyboard.GetInstanceDescriptor();
        case InputDeviceClass::MOUSE: return impl->mouse.GetInstanceDescriptor();
        case InputDeviceClass::GAME_CONTROLLER: return impl->gameControllers[index].GetInstanceDescriptor();
        default: break;
    }

    return Utf8String::GetEmpty();
}

bool LinuxInputContext::IsDeviceConnected(InputDeviceClass deviceClass, size_t index) const
{
    switch (deviceClass)
    {
        case InputDeviceClass::KEYBOARD: return impl->keyboard.IsConnected();
        case InputDeviceClass::MOUSE: return impl->mouse.IsConnected();
        case InputDeviceClass::GAME_CONTROLLER: return impl->gameControllers[index].IsConnected();
        default: break;
    }

    return false;
}

InputDeviceSemantic LinuxInputContext::GetDeviceSemantic(InputDeviceClass deviceClass, size_t index) const
{
    switch (deviceClass)
    {
        case InputDeviceClass::KEYBOARD: return impl->keyboard.GetSemantic();
        case InputDeviceClass::MOUSE: return impl->mouse.GetSemantic();
        case InputDeviceClass::GAME_CONTROLLER: return impl->gameControllers[index].GetSemantic();
    }

    return InputDeviceSemantic::NONE;
}

size_t LinuxInputContext::GetGameControllerCount() const
{
    return impl->gameControllers.size();
}

LinuxGameController* LinuxInputContext::GetGameController(size_t index)
{
    return &impl->gameControllers[index];
}

size_t LinuxInputContext::GetMouseCount() const
{
    return 1;
}

LinuxMouse* LinuxInputContext::GetMouse(size_t index)
{
    return &impl->mouse;
}

size_t LinuxInputContext::GetKeyboardCount() const
{
    return 1;
}

LinuxKeyboard* LinuxInputContext::GetKeyboard(size_t index)
{
    return &impl->keyboard;
}

FileOperationResult LinuxInputContext::ReadInputBinding(InputDeviceClass deviceClass, size_t deviceIndex, const AssetReference& configAssetRef, ByteBuffer& fileBuffer)
{
    impl->inputBindingsAssetReader.GetSelector().Set(AssetPathComponent::INPUT_DEVICE_TYPE, InputDeviceClassUtilities::ToString(deviceClass));

    impl->inputBindingsAssetReader.GetSelector().Set(AssetPathComponent::INPUT_DEVICE_DESCRIPTOR, GetDeviceProductDescriptor(deviceClass, deviceIndex));

    return impl->inputBindingsAssetReader.ReadAsset(fileBuffer, configAssetRef);
}
