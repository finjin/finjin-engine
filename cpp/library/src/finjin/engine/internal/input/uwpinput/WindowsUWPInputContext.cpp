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
#include "WindowsUWPInputContext.hpp"
#include "finjin/common/DebugLog.hpp"
#include "finjin/engine/InputCoordinate.hpp"
#include "finjin/engine/InputDeviceSerializer.hpp"
#include "finjin/engine/OSWindow.hpp"
#include "finjin/engine/OSWindowEventListener.hpp"
#include "WindowsUWPInputSystem.hpp"
#include "../xinput/XInputSystem.hpp"
#include <Xinput.h>

using namespace Finjin::Engine;


//Local types-------------------------------------------------------------------
struct WindowsUWPInputContext::Impl : public AllocatedClass, public OSWindowEventListener
{
    Impl(Allocator* allocator, WindowsUWPInputSystem* inputSystem);

    void WindowOnKeyDown(OSWindow* osWindow, int softwareKey, int hardwareCode, bool controlDown, bool shiftDown, bool altDown) override;
    void WindowOnKeyUp(OSWindow* osWindow, int softwareKey, int hardwareCode, bool controlDown, bool shiftDown, bool altDown) override;

    void WindowOnMouseWheel(OSWindow* osWindow, PointerType pointerType, int pointerID, InputCoordinate x, InputCoordinate y, Buttons buttons, float wheelDelta) override;
    void WindowOnPointerMove(OSWindow* osWindow, PointerType pointerType, int pointerID, InputCoordinate x, InputCoordinate y, Buttons buttons) override;
    void WindowOnPointerDown(OSWindow* osWindow, PointerType pointerType, int pointerID, InputCoordinate x, InputCoordinate y, Buttons buttons) override;
    void WindowOnPointerUp(OSWindow* osWindow, PointerType pointerType, int pointerID, InputCoordinate x, InputCoordinate y, Buttons buttons) override;

    WindowsUWPInputSystem* inputSystem;

    WindowsUWPInputContext::Settings settings;

    AssetClassFileReader inputDevicesAssetReader;
    AssetClassFileReader xinputDevicesAssetReader;
    AssetClassFileReader inputBindingsAssetReader;

    int gameControllerID;

    int updateCount;

    std::array<XInputGameController, XUSER_MAX_COUNT> gameControllers;

    StaticVector<InputGenericGameController*, GameControllerConstants::MAX_GAME_CONTROLLERS - XUSER_MAX_COUNT> externalGameControllers;

    WindowsUWPMouse mouse;

    WindowsUWPKeyboard keyboard;

    InputTouchScreen touchScreen;

    ByteBuffer configFileBuffer;
};


//Implementation----------------------------------------------------------------

//WindowsUWPInputContext::Impl
WindowsUWPInputContext::Impl::Impl(Allocator* allocator, WindowsUWPInputSystem* inputSystem) : AllocatedClass(allocator), settings(allocator)
{
    this->inputSystem = inputSystem;

    this->gameControllerID = 0;

    this->updateCount = 0;

    this->configFileBuffer.Create(EngineConstants::DEFAULT_CONFIGURATION_BUFFER_SIZE, allocator);
}

void WindowsUWPInputContext::Impl::WindowOnKeyDown(OSWindow* osWindow, int softwareKey, int hardwareCode, bool controlDown, bool shiftDown, bool altDown)
{
    auto componentIndex = static_cast<size_t>(softwareKey);
    if (componentIndex < this->keyboard.GetButtonCount())
    {
        auto component = this->keyboard.GetButton(componentIndex);
        component->Update(true);
    }
}

void WindowsUWPInputContext::Impl::WindowOnKeyUp(OSWindow* osWindow, int softwareKey, int hardwareCode, bool controlDown, bool shiftDown, bool altDown)
{
    auto componentIndex = static_cast<size_t>(softwareKey);
    if (componentIndex < this->keyboard.GetButtonCount())
    {
        auto component = this->keyboard.GetButton(componentIndex);
        component->Update(false);
    }
}

void WindowsUWPInputContext::Impl::WindowOnMouseWheel(OSWindow* osWindow, PointerType pointerType, int pointerID, InputCoordinate x, InputCoordinate y, Buttons buttons, float wheelDelta)
{
    if (this->mouse.GetAxisCount() >= 3)
    {
        auto component = this->mouse.GetAxis(2);
        component->Update(wheelDelta * MouseConstants::WHEEL_STEP_SIZE, false);
    }
}

void WindowsUWPInputContext::Impl::WindowOnPointerMove(OSWindow* osWindow, PointerType pointerType, int pointerID, InputCoordinate x, InputCoordinate y, Buttons buttons)
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
    else
    {
        auto pointer = this->touchScreen.GetPointer(pointerType, pointerID);
        if (pointer != nullptr)
        {
            pointer->Update(true, x.ToDipsValue(), y.ToDipsValue());
        }
    }
}

void WindowsUWPInputContext::Impl::WindowOnPointerDown(OSWindow* osWindow, PointerType pointerType, int pointerID, InputCoordinate x, InputCoordinate y, Buttons buttons)
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
    else
    {
        auto pointer = this->touchScreen.ConnectPointer(pointerType, pointerID);
        if (pointer != nullptr)
        {
            pointer->Update(true, x.ToDipsValue(), y.ToDipsValue());
        }
    }
}

void WindowsUWPInputContext::Impl::WindowOnPointerUp(OSWindow* osWindow, PointerType pointerType, int pointerID, InputCoordinate x, InputCoordinate y, Buttons buttons)
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
    else
    {
        auto pointer = this->touchScreen.GetPointer(pointerType, pointerID);
        if (pointer != nullptr)
        {
            pointer->Update(false, x.ToDipsValue(), y.ToDipsValue());
        }
    }
}

//WindowsUWPInputContext
WindowsUWPInputContext::WindowsUWPInputContext(Allocator* allocator, WindowsUWPInputSystem* inputSystem) :
    AllocatedClass(allocator),
    impl(AllocatedClass::New<Impl>(allocator, FINJIN_CALLER_ARGUMENTS, inputSystem))
{
}

WindowsUWPInputContext::~WindowsUWPInputContext()
{
}

void WindowsUWPInputContext::Create(const Settings& settings, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    assert(settings.osWindow != nullptr);
    assert(settings.assetFileReader != nullptr);

    FINJIN_ENGINE_CHECK_IMPL_NOT_NULL(impl, error);

    //Copy settings---------------------------------------------
    impl->settings = settings;

    impl->inputDevicesAssetReader.Create(*impl->settings.assetFileReader, impl->settings.initialAssetFileSelector, AssetClass::INPUT_DEVICE, GetAllocator(), error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to create input device asset file reader.");
        return;
    }

    auto xinputDevicesAssetSelector = impl->settings.initialAssetFileSelector;
    xinputDevicesAssetSelector.Set(AssetPathComponent::INPUT_API, XInputSystem::GetSystemInternalName()); //xinput
    impl->xinputDevicesAssetReader.Create(*impl->settings.assetFileReader, xinputDevicesAssetSelector, AssetClass::INPUT_DEVICE, GetAllocator(), error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to create XInput device asset file reader.");
        return;
    }

    impl->inputBindingsAssetReader.Create(*impl->settings.assetFileReader, impl->settings.initialAssetFileSelector, AssetClass::INPUT_BINDINGS, GetAllocator(), error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to create input bindings asset file reader.");
        return;
    }

    //Enumerate the devices-------------------------------------

    //XInput game controllers-----------------------
    for (size_t gameControllerIndex = 0; gameControllerIndex < impl->gameControllers.size(); gameControllerIndex++)
    {
        auto& gameController = impl->gameControllers[gameControllerIndex];

        gameController.Create(gameControllerIndex);
        ConfigureInputDevice(gameController, gameController.GetProductDescriptor(), impl->xinputDevicesAssetReader, impl->configFileBuffer);
    }

    //Mice
    impl->mouse.Create(0);
    ConfigureInputDevice(impl->mouse, impl->mouse.GetProductDescriptor(), impl->inputDevicesAssetReader, impl->configFileBuffer);

    //Keyboard
    impl->keyboard.Create(0);
    ConfigureInputDevice(impl->keyboard, impl->keyboard.GetProductDescriptor(), impl->inputDevicesAssetReader, impl->configFileBuffer);

    impl->touchScreen.SetDisplayName("Touch screen");
    impl->touchScreen.Connect(true);

    impl->updateCount = 0;
    Update(0);

    //Set up window event listener
    impl->settings.osWindow->AddWindowEventListener(impl.get());
}

void WindowsUWPInputContext::Destroy()
{
    //Remove window event listener
    if (impl->settings.osWindow != nullptr)
        impl->settings.osWindow->RemoveWindowEventListener(impl.get());

    //Destroy devices
    for (auto& device : impl->gameControllers)
        device.Destroy();

    impl->mouse.Destroy();

    impl->keyboard.Destroy();

    impl->touchScreen.Reset();
}

void WindowsUWPInputContext::GetSelectorComponents(AssetPathSelector& result)
{
}

const WindowsUWPInputContext::Settings& WindowsUWPInputContext::GetSettings() const
{
    return impl->settings;
}

void WindowsUWPInputContext::Update(SimpleTimeDelta elapsedTime, InputDevicePollFlag flags)
{
    bool isFirstUpdate = impl->updateCount++ == 0;

    for (auto& device : impl->gameControllers)
        device.Update(elapsedTime, isFirstUpdate);
}

void WindowsUWPInputContext::Execute(InputEvents& events, InputCommands& commands, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    for (auto& command : commands)
    {
        switch (command.type)
        {
            case InputCommand::Type::NOTIFY:
            {
                if (command.eventInfo.HasEventID())
                {
                    if (!events.push_back())
                    {
                        FINJIN_SET_ERROR(error, "Failed to record 'notify' input event.");
                        return;
                    }

                    auto& event = events.back();
                    event.type = InputEvent::Type::NOTIFY;
                    event.eventInfo = command.eventInfo;
                }

                break;
            }
            case InputCommand::Type::ADD_HAPTIC_FEEDBACK:
            {
                AddHapticFeedback(command.deviceClass, command.deviceIndex, &command.force, 1);

                if (command.eventInfo.HasEventID())
                {
                    if (!events.push_back())
                    {
                        FINJIN_SET_ERROR(error, "Failed to record 'add haptic feedback' input event.");
                        return;
                    }

                    auto& event = events.back();
                    event.type = InputEvent::Type::ADDED_HAPTIC_FEEDBACK;
                    event.eventInfo = command.eventInfo;
                }

                break;
            }
            case InputCommand::Type::STOP_HAPTIC_FEEDBACK:
            {
                StopHapticFeedback(command.deviceClass, command.deviceIndex);

                if (command.eventInfo.HasEventID())
                {
                    if (!events.push_back())
                    {
                        FINJIN_SET_ERROR(error, "Failed to record 'stop haptic feedback' input event.");
                        return;
                    }

                    auto& event = events.back();
                    event.type = InputEvent::Type::STOPPED_HAPTIC_FEEDBACK;
                    event.eventInfo = command.eventInfo;
                }

                break;
            }
        }
    }
}

void WindowsUWPInputContext::HandleDeviceChanges()
{
    for (auto& device : impl->gameControllers)
        device.ClearChanged();

    impl->keyboard.ClearChanged();

    impl->mouse.ClearChanged();

    impl->touchScreen.ClearChanged();
}

void WindowsUWPInputContext::HandleApplicationViewportLostFocus()
{
    for (auto& device : impl->gameControllers)
        device.StopHapticFeedback();
}

void WindowsUWPInputContext::HandleApplicationViewportGainedFocus()
{
}

size_t WindowsUWPInputContext::GetDeviceCount(InputDeviceClass deviceClass) const
{
    switch (deviceClass)
    {
        case InputDeviceClass::KEYBOARD: return 1;
        case InputDeviceClass::MOUSE: return 1;
        case InputDeviceClass::GAME_CONTROLLER: return GetGameControllerCount();
    }

    return 0;
}

const Utf8String& WindowsUWPInputContext::GetDeviceProductDescriptor(InputDeviceClass deviceClass, size_t index) const
{
    switch (deviceClass)
    {
        case InputDeviceClass::KEYBOARD: return impl->keyboard.GetProductDescriptor();
        case InputDeviceClass::MOUSE: return impl->mouse.GetProductDescriptor();
        case InputDeviceClass::GAME_CONTROLLER: return GetGameControllerProductDescriptor(index);
    }

    return Utf8String::GetEmpty();
}

const Utf8String& WindowsUWPInputContext::GetDeviceInstanceDescriptor(InputDeviceClass deviceClass, size_t index) const
{
    switch (deviceClass)
    {
        case InputDeviceClass::KEYBOARD: return impl->keyboard.GetInstanceDescriptor();
        case InputDeviceClass::MOUSE: return impl->mouse.GetInstanceDescriptor();
        case InputDeviceClass::GAME_CONTROLLER: return GetGameControllerInstanceDescriptor(index);
    }

    return Utf8String::GetEmpty();
}

InputDeviceSemantic WindowsUWPInputContext::GetDeviceSemantic(InputDeviceClass deviceClass, size_t index) const
{
    switch (deviceClass)
    {
        case InputDeviceClass::KEYBOARD: return impl->keyboard.GetSemantic();
        case InputDeviceClass::MOUSE: return impl->mouse.GetSemantic();
        case InputDeviceClass::GAME_CONTROLLER: return GetGameControllerSemantic(index);
    }

    return InputDeviceSemantic::NONE;
}

bool WindowsUWPInputContext::IsDeviceConnected(InputDeviceClass deviceClass, size_t index) const
{
    switch (deviceClass)
    {
        case InputDeviceClass::KEYBOARD: return impl->keyboard.IsConnected();
        case InputDeviceClass::MOUSE: return impl->mouse.IsConnected();
        case InputDeviceClass::GAME_CONTROLLER: return IsGameControllerConnected(index);
    }

    return false;
}

void WindowsUWPInputContext::AddHapticFeedback(InputDeviceClass deviceClass, size_t index, const HapticFeedbackSettings* forces, size_t count)
{
    if (deviceClass == InputDeviceClass::GAME_CONTROLLER)
    {
        if (index < impl->gameControllers.size())
            impl->gameControllers[index].AddHapticFeedback(forces, count);
        else
            impl->externalGameControllers[index - impl->gameControllers.size()]->AddHapticFeedback(forces, count);
    }
}

void WindowsUWPInputContext::StopHapticFeedback(InputDeviceClass deviceClass, size_t index)
{
    if (deviceClass == InputDeviceClass::GAME_CONTROLLER)
    {
        if (index < impl->gameControllers.size())
            impl->gameControllers[index].StopHapticFeedback();
        else
            impl->externalGameControllers[index - impl->gameControllers.size()]->StopHapticFeedback();
    }
}

size_t WindowsUWPInputContext::GetGameControllerCount() const
{
    return impl->gameControllers.size() + impl->externalGameControllers.size();
}

bool WindowsUWPInputContext::IsGameControllerConnected(size_t index) const
{
    if (index < impl->gameControllers.size())
        return impl->gameControllers[index].IsConnected();
    else
        return impl->externalGameControllers[index - impl->gameControllers.size()]->IsConnected();
}

const Utf8String& WindowsUWPInputContext::GetGameControllerProductDescriptor(size_t index) const
{
    if (index < impl->gameControllers.size())
        return impl->gameControllers[index].GetProductDescriptor();
    else
        return impl->externalGameControllers[index - impl->gameControllers.size()]->GetProductDescriptor();
}

const Utf8String& WindowsUWPInputContext::GetGameControllerInstanceDescriptor(size_t index) const
{
    if (index < impl->gameControllers.size())
        return impl->gameControllers[index].GetInstanceDescriptor();
    else
        return impl->externalGameControllers[index - impl->gameControllers.size()]->GetInstanceDescriptor();
}

InputDeviceSemantic WindowsUWPInputContext::GetGameControllerSemantic(size_t index) const
{
    if (index < impl->gameControllers.size())
        return impl->gameControllers[index].GetSemantic();
    else
        return impl->externalGameControllers[index - impl->gameControllers.size()]->GetSemantic();
}

size_t WindowsUWPInputContext::GetXInputGameControllerCount() const
{
    return impl->gameControllers.size();
}

XInputGameController* WindowsUWPInputContext::GetXInputGameController(size_t index)
{
    return &impl->gameControllers[index];
}

void WindowsUWPInputContext::AddExternalGameController(InputGenericGameController* gameController, bool configure, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto addResult = impl->externalGameControllers.push_back(gameController);
    if (addResult.HasError())
    {
        FINJIN_SET_ERROR(error, "Failed to add external game controller.");
        return;
    }

    if (addResult.value && configure)
    {
        auto devicesAssetSelector = impl->settings.initialAssetFileSelector;
        devicesAssetSelector.Set(AssetPathComponent::INPUT_API, gameController->GetSystemInternalName());

        AssetClassFileReader inputDevicesAssetReader;
        inputDevicesAssetReader.Create(*impl->settings.assetFileReader, devicesAssetSelector, AssetClass::INPUT_DEVICE, GetAllocator(), error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to create input device asset file reader.");
            return;
        }

        ConfigureInputDevice(*gameController, gameController->GetProductDescriptor(), inputDevicesAssetReader, impl->configFileBuffer);
    }
}

void WindowsUWPInputContext::RemoveExternalGameController(InputGenericGameController* gameController)
{
    auto foundAt = impl->externalGameControllers.find(gameController);
    if (foundAt != impl->externalGameControllers.end())
        impl->externalGameControllers.erase(foundAt);
}

size_t WindowsUWPInputContext::GetExternalGameControllerCount() const
{
    return impl->externalGameControllers.size();
}

InputGenericGameController* WindowsUWPInputContext::GetExternalGameController(size_t index)
{
    return impl->externalGameControllers[index];
}

size_t WindowsUWPInputContext::GetMouseCount() const
{
    return 1;
}

WindowsUWPMouse* WindowsUWPInputContext::GetMouse(size_t index)
{
    return &impl->mouse;
}

size_t WindowsUWPInputContext::GetKeyboardCount() const
{
    return 1;
}

WindowsUWPKeyboard* WindowsUWPInputContext::GetKeyboard(size_t index)
{
    return &impl->keyboard;
}

size_t WindowsUWPInputContext::GetTouchScreenCount() const
{
    return impl->touchScreen.IsConnected() ? 1 : 0;
}

InputTouchScreen* WindowsUWPInputContext::GetTouchScreen(size_t index)
{
    return &impl->touchScreen;
}

void WindowsUWPInputContext::AddExternalHeadset(InputGenericHeadset* headset, bool configure, Error& error)
{
}

void WindowsUWPInputContext::RemoveExternalHeadset(InputGenericHeadset* headset)
{
}

size_t WindowsUWPInputContext::GetExternalHeadsetCount() const
{
    return 0;
}

InputGenericHeadset* WindowsUWPInputContext::GetExternalHeadset(size_t index)
{
    return nullptr;
}

FileOperationResult WindowsUWPInputContext::ReadInputBinding(InputDeviceClass deviceClass, size_t deviceIndex, const AssetReference& configAssetRef, ByteBuffer& fileBuffer)
{
    impl->inputBindingsAssetReader.GetSelector().Set(AssetPathComponent::INPUT_DEVICE_TYPE, InputDeviceClassUtilities::ToString(deviceClass));

    if (deviceClass == InputDeviceClass::GAME_CONTROLLER)
        impl->inputBindingsAssetReader.GetSelector().Set(AssetPathComponent::INPUT_API, XInputSystem::GetSystemInternalName());
    else
        impl->inputBindingsAssetReader.GetSelector().Set(AssetPathComponent::INPUT_API, Utf8String::GetEmpty());

    impl->inputBindingsAssetReader.GetSelector().Set(AssetPathComponent::INPUT_DEVICE_DESCRIPTOR, GetDeviceProductDescriptor(deviceClass, deviceIndex));

    return impl->inputBindingsAssetReader.ReadAsset(fileBuffer, configAssetRef);
}

WindowsUWPInputContext::Impl* WindowsUWPInputContext::GetImpl()
{
    return impl.get();
}
