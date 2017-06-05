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
#include "MacOSInputContext.hpp"
#include "finjin/common/StaticVector.hpp"
#include "InputDeviceSerializer.hpp"
#include "MacOSInputSystem.hpp"
#include "OSWindow.hpp"
#include "../gcinput/GCInputSystem.hpp"
#include "../gcinput/GCInputGameController.hpp"
#import <GameController/GameController.h>

using namespace Finjin::Engine;


//Local types-------------------------------------------------------------------
struct MacOSInputContext::Impl : public AllocatedClass, public OSWindowEventListener
{
    Impl(Allocator* allocator, MacOSInputSystem* inputSystem);

    void WindowOnKeyDown(OSWindow* osWindow, int softwareKey, int hardwareCode, bool controlDown, bool shiftDown, bool altDown) override;
    void WindowOnKeyUp(OSWindow* osWindow, int softwareKey, int hardwareCode, bool controlDown, bool shiftDown, bool altDown) override;

    void WindowOnMouseWheel(OSWindow* osWindow, PointerType pointerType, int pointerID, InputCoordinate x, InputCoordinate y, Buttons buttons, float wheelDelta) override;
    void WindowOnPointerMove(OSWindow* osWindow, PointerType pointerType, int pointerID, InputCoordinate x, InputCoordinate y, Buttons buttons) override;
    void WindowOnPointerDown(OSWindow* osWindow, PointerType pointerType, int pointerID, InputCoordinate x, InputCoordinate y, Buttons buttons) override;
    void WindowOnPointerUp(OSWindow* osWindow, PointerType pointerType, int pointerID, InputCoordinate x, InputCoordinate y, Buttons buttons) override;

    MacOSInputSystem* inputSystem;

    MacOSInputContext::Settings settings;

    AssetClassFileReader inputDevicesAssetReader;
    AssetClassFileReader gcinputDevicesAssetReader;
    AssetClassFileReader inputBindingsAssetReader;

    int gameControllerUpdateCount;
    int nongameControllerUpdateCount;

    StaticVector<GCInputGameController, GameControllerConstants::MAX_GAME_CONTROLLERS> gameControllers;

    MacOSMouse mouse;

    MacOSKeyboard keyboard;

    ByteBuffer configFileBuffer;
};


//Implementation----------------------------------------------------------------

//MacOSInputContext::Impl
MacOSInputContext::Impl::Impl(Allocator* allocator, MacOSInputSystem* inputSystem) :
    AllocatedClass(allocator),
    settings(allocator),
    mouse(allocator),
    keyboard(allocator)
{
    this->inputSystem = inputSystem;

    this->gameControllerUpdateCount = 0;
    this->nongameControllerUpdateCount = 0;
    
    this->gameControllers.maximize();
    for (auto& gameController : this->gameControllers)
        gameController.SetAllocator(allocator);
    this->gameControllers.clear();
}

void MacOSInputContext::Impl::WindowOnKeyDown(OSWindow* osWindow, int softwareKey, int hardwareCode, bool controlDown, bool shiftDown, bool altDown)
{
    auto componentIndex = static_cast<size_t>(hardwareCode);
    if (componentIndex < this->keyboard.GetButtonCount())
    {
        auto component = this->keyboard.GetButton(componentIndex);
        component->Update(true);
    }
}

void MacOSInputContext::Impl::WindowOnKeyUp(OSWindow* osWindow, int softwareKey, int hardwareCode, bool controlDown, bool shiftDown, bool altDown)
{
    auto componentIndex = static_cast<size_t>(hardwareCode);
    if (componentIndex < this->keyboard.GetButtonCount())
    {
        auto component = this->keyboard.GetButton(componentIndex);
        component->Update(false);
    }
}

void MacOSInputContext::Impl::WindowOnMouseWheel(OSWindow* osWindow, PointerType pointerType, int pointerID, InputCoordinate x, InputCoordinate y, Buttons buttons, float wheelDelta)
{
    if (this->mouse.GetAxisCount() >= 3)
    {
        auto component = this->mouse.GetAxis(2);
        component->Update(wheelDelta * MouseConstants::WHEEL_STEP_SIZE, false);
    }
}

void MacOSInputContext::Impl::WindowOnPointerMove(OSWindow* osWindow, PointerType pointerType, int pointerID, InputCoordinate x, InputCoordinate y, Buttons buttons)
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

void MacOSInputContext::Impl::WindowOnPointerDown(OSWindow* osWindow, PointerType pointerType, int pointerID, InputCoordinate x, InputCoordinate y, Buttons buttons)
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

void MacOSInputContext::Impl::WindowOnPointerUp(OSWindow* osWindow, PointerType pointerType, int pointerID, InputCoordinate x, InputCoordinate y, Buttons buttons)
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

//MacOSInputContext
MacOSInputContext::MacOSInputContext(Allocator* allocator, MacOSInputSystem* inputSystem) :
    AllocatedClass(allocator),
    impl(AllocatedClass::New<Impl>(allocator, FINJIN_CALLER_ARGUMENTS, inputSystem))
{
}

MacOSInputContext::~MacOSInputContext()
{
}

MacOSInputContext::Impl* MacOSInputContext::GetImpl()
{
    return impl.get();
}

void MacOSInputContext::Create(const Settings& settings, Error& error)
{
    assert(settings.osWindow != nullptr);
    assert(settings.assetFileReader != nullptr);

    FINJIN_ERROR_METHOD_START(error);

    FINJIN_ENGINE_CHECK_IMPL_NOT_NULL(impl, error);

    impl->settings = settings;
    
    if (!impl->configFileBuffer.Create(EngineConstants::DEFAULT_CONFIGURATION_BUFFER_SIZE, GetAllocator()))
    {
        FINJIN_SET_ERROR(error, "Failed to allocate config buffer.");
        return;
    }
    
    impl->inputDevicesAssetReader.Create(*impl->settings.assetFileReader, impl->settings.initialAssetFileSelector, AssetClass::INPUT_DEVICE, GetAllocator(), error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to create input devices asset reader.");
        return;
    }

    auto gcinputDevicesAssetSelector = impl->settings.initialAssetFileSelector;
    gcinputDevicesAssetSelector.Set(AssetPathComponent::INPUT_API, GCInputSystem::GetSystemInternalName());
    impl->gcinputDevicesAssetReader.Create(*impl->settings.assetFileReader, gcinputDevicesAssetSelector, AssetClass::INPUT_DEVICE, GetAllocator(), error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to create GameController input devices asset reader.");
        return;
    }

    impl->inputBindingsAssetReader.Create(*impl->settings.assetFileReader, impl->settings.initialAssetFileSelector, AssetClass::INPUT_BINDINGS, GetAllocator(), error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to create input bindings asset reader.");
        return;
    }

    //Enumerate the devices-------------------------------------

    //Mice
    impl->mouse.Create(0);
    ConfigureInputDevice(impl->mouse, impl->mouse.GetProductDescriptor(), impl->inputDevicesAssetReader, impl->configFileBuffer);

    //Keyboard
    impl->keyboard.Create(0);
    ConfigureInputDevice(impl->keyboard, impl->keyboard.GetProductDescriptor(), impl->inputDevicesAssetReader, impl->configFileBuffer);

    impl->gameControllerUpdateCount = 0;
    impl->nongameControllerUpdateCount = 0;
    Update(0);

    //Set up window event listener
    impl->settings.osWindow->AddWindowEventListener(impl.get());
}

void MacOSInputContext::Destroy()
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

void MacOSInputContext::GetSelectorComponents(AssetPathSelector& result)
{
}

const MacOSInputContext::Settings& MacOSInputContext::GetSettings() const
{
    return impl->settings;
}

void MacOSInputContext::Update(SimpleTimeDelta elapsedTime, InputDevicePollFlag flags)
{
    if (impl->gameControllerUpdateCount == 0)
    {
        auto controllers = [GCController controllers];
        if (controllers.count > 0)
        {
            GCInputGameController::UpdateGameControllers(impl->gameControllers, true, elapsedTime);
            impl->gameControllerUpdateCount++;
        }
    }
    else if (impl->gameControllerUpdateCount > 0)
    {
        GCInputGameController::UpdateGameControllers(impl->gameControllers, false, elapsedTime);
        impl->gameControllerUpdateCount++;
    }

    for (auto& gameController : impl->gameControllers)
    {
        if (gameController.IsNewConnection())
            ConfigureInputDevice(gameController, gameController.GetProductDescriptor(), impl->gcinputDevicesAssetReader, impl->configFileBuffer);
    }

    auto isFirstNongameControllerUpdate = impl->nongameControllerUpdateCount++ == 0;
    impl->mouse.Update(elapsedTime, isFirstNongameControllerUpdate);
    impl->keyboard.Update(elapsedTime, isFirstNongameControllerUpdate);
}

void MacOSInputContext::Execute(InputEvents& events, InputCommands& commands, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);
}

void MacOSInputContext::HandleDeviceChanges()
{
    for (auto& device : impl->gameControllers)
        device.ClearChanged();

    impl->keyboard.ClearChanged();

    impl->mouse.ClearChanged();
}

void MacOSInputContext::HandleApplicationViewportLostFocus()
{
}

void MacOSInputContext::HandleApplicationViewportGainedFocus()
{
}

size_t MacOSInputContext::GetDeviceCount(InputDeviceClass deviceClass) const
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

const Utf8String& MacOSInputContext::GetDeviceProductDescriptor(InputDeviceClass deviceClass, size_t index) const
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

const Utf8String& MacOSInputContext::GetDeviceInstanceDescriptor(InputDeviceClass deviceClass, size_t index) const
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

InputDeviceSemantic MacOSInputContext::GetDeviceSemantic(InputDeviceClass deviceClass, size_t index) const
{
    switch (deviceClass)
    {
        case InputDeviceClass::KEYBOARD: return impl->keyboard.GetSemantic();
        case InputDeviceClass::MOUSE: return impl->mouse.GetSemantic();
        case InputDeviceClass::GAME_CONTROLLER: return impl->gameControllers[index].GetSemantic();
        default: break;
    }

    return InputDeviceSemantic::NONE;
}

bool MacOSInputContext::IsDeviceConnected(InputDeviceClass deviceClass, size_t index) const
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

size_t MacOSInputContext::GetGameControllerCount() const
{
    return impl->gameControllers.size();
}

GCInputGameController* MacOSInputContext::GetGameController(size_t index)
{
    return &impl->gameControllers[index];
}

size_t MacOSInputContext::GetMouseCount() const
{
    return 1;
}

MacOSMouse* MacOSInputContext::GetMouse(size_t index)
{
    return &impl->mouse;
}

size_t MacOSInputContext::GetKeyboardCount() const
{
    return 1;
}

MacOSKeyboard* MacOSInputContext::GetKeyboard(size_t index)
{
    return &impl->keyboard;
}

size_t MacOSInputContext::GetTouchScreenCount() const
{
    return 0;
}

InputTouchScreen* MacOSInputContext::GetTouchScreen(size_t index)
{
    return nullptr;
}

FileOperationResult MacOSInputContext::ReadInputBinding(InputDeviceClass deviceClass, size_t deviceIndex, const AssetReference& configAssetRef, ByteBuffer& fileBuffer)
{
    impl->inputBindingsAssetReader.GetSelector().Set(AssetPathComponent::INPUT_DEVICE_TYPE, InputDeviceClassUtilities::ToString(deviceClass));

    if (deviceClass == InputDeviceClass::GAME_CONTROLLER)
        impl->inputBindingsAssetReader.GetSelector().Set(AssetPathComponent::INPUT_API, GCInputSystem::GetSystemInternalName());
    else
        impl->inputBindingsAssetReader.GetSelector().Set(AssetPathComponent::INPUT_API, Utf8String::GetEmpty());

    impl->inputBindingsAssetReader.GetSelector().Set(AssetPathComponent::INPUT_DEVICE_DESCRIPTOR, GetDeviceProductDescriptor(deviceClass, deviceIndex));

    return impl->inputBindingsAssetReader.ReadAsset(fileBuffer, configAssetRef);
}
