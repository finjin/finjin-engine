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
#include "AndroidInputContext.hpp"
#include "finjin/common/AndroidUtilities.hpp"
#include "finjin/common/DebugLog.hpp"
#include "finjin/engine/InputCoordinate.hpp"
#include "finjin/engine/InputDeviceSerializer.hpp"
#include "AndroidAccelerometer.hpp"
#include "AndroidGameController.hpp"
#include "AndroidInputSystem.hpp"
#include "AndroidKeyboard.hpp"
#include "AndroidMouse.hpp"
#include <android/input.h>
#include <android/sensor.h>

using namespace Finjin::Engine;

#define MAX_EXTERNAL_GAME_CONTROLLER_COUNT (GameControllerConstants::MAX_GAME_CONTROLLERS - AndroidGameControllerConstants::MAX_NATIVE_GAME_CONTROLLERS)


//Local functions---------------------------------------------------------------
static void UpdateButtonFromAction(InputButton* button, int action)
{
    if (action == AKEY_EVENT_ACTION_DOWN)
        button->Update(true);
    else if (action == AKEY_EVENT_ACTION_UP)
        button->Update(false);
}


//Local types-------------------------------------------------------------------
struct AndroidInputContext::Impl : public AllocatedClass
{
    Impl(Allocator* allocator, AndroidInputSystem* inputSystem);

    void DetectNewDevices();

    bool AnyGameControllersConnected() const;

    AndroidInputSystem* inputSystem;

    AndroidInputContext::Settings settings;

    AssetClassFileReader inputDevicesAssetReader;
    AssetClassFileReader inputBindingsAssetReader;

    StaticVector<AndroidGameController, AndroidGameControllerConstants::MAX_NATIVE_GAME_CONTROLLERS> gameControllers;
    StaticVector<AndroidGameController, AndroidGameControllerConstants::MAX_NATIVE_GAME_CONTROLLERS> gameControllersBuffer;

    StaticVector<InputGenericGameController*, MAX_EXTERNAL_GAME_CONTROLLER_COUNT> externalGameControllers;

    StaticVector<AndroidMouse, MouseConstants::MAX_MICE> mice;
    StaticVector<AndroidMouse, MouseConstants::MAX_MICE> miceBuffer;

    StaticVector<AndroidKeyboard, KeyboardConstants::MAX_KEYBOARDS> keyboards;
    StaticVector<AndroidKeyboard, KeyboardConstants::MAX_KEYBOARDS> keyboardsBuffer;

    InputTouchScreen touchScreen;

    AndroidAccelerometer accelerometer;

    ByteBuffer configFileBuffer;
};


//Implementation----------------------------------------------------------------

//AndroidInputContext::Impl
AndroidInputContext::Impl::Impl(Allocator* allocator, AndroidInputSystem* inputSystem) : AllocatedClass(allocator), settings(allocator)
{
    this->inputSystem = inputSystem;

    this->configFileBuffer.Create(EngineConstants::DEFAULT_CONFIGURATION_BUFFER_SIZE, allocator);
}

void AndroidInputContext::Impl::DetectNewDevices()
{
    auto androidApp = static_cast<android_app*>(this->settings.applicationHandle);

    AndroidKeyboard::CheckConfigurationChanged(this->keyboards, androidApp);
    AndroidGameController::RefreshGameControllers(this->gameControllers, this->gameControllersBuffer, androidApp);

    AndroidMouse::CheckConfigurationChanged(this->mice, androidApp);
    AndroidMouse::RefreshMice(this->mice, this->miceBuffer, androidApp);

    AndroidGameController::CheckConfigurationChanged(this->gameControllers, androidApp);
    AndroidKeyboard::RefreshKeyboards(this->keyboards, this->keyboardsBuffer, androidApp);
}

bool AndroidInputContext::Impl::AnyGameControllersConnected() const
{
    for (const auto& device : this->gameControllers)
    {
        if (device.IsConnected())
            return true;
    }

    return false;
}

//AndroidInputContext
AndroidInputContext::AndroidInputContext(Allocator* allocator, AndroidInputSystem* inputSystem) :
    AllocatedClass(allocator),
    impl(AllocatedClass::New<Impl>(allocator, FINJIN_CALLER_ARGUMENTS, inputSystem))
{
}

AndroidInputContext::~AndroidInputContext()
{
}

void AndroidInputContext::Create(const Settings& settings, Error& error)
{
    assert(settings.applicationHandle != nullptr);
    assert(settings.assetFileReader != nullptr);

    FINJIN_ERROR_METHOD_START(error);

    FINJIN_ENGINE_CHECK_IMPL_NOT_NULL(impl, error);

    auto androidApp = static_cast<android_app*>(settings.applicationHandle);

    //Copy settings---------------------------------------------
    impl->settings = settings;

    impl->inputDevicesAssetReader.Create(*impl->settings.assetFileReader, impl->settings.initialAssetFileSelector, AssetClass::INPUT_DEVICE, GetAllocator(), error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to create input device asset file reader.");
        return;
    }

    impl->inputBindingsAssetReader.Create(*impl->settings.assetFileReader, impl->settings.initialAssetFileSelector, AssetClass::INPUT_BINDINGS, GetAllocator(), error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to create input bindings asset file reader.");
        return;
    }

    //Enumerate the devices-------------------------------------

    //Game controllers
    AndroidGameController::CreateGameControllers(impl->gameControllers, androidApp);
    FINJIN_DEBUG_LOG_INFO("Discovered %1% game controllers", impl->gameControllers.size());
    for (size_t i = 0; i < impl->gameControllers.size(); i++)
    {
        FINJIN_DEBUG_LOG_INFO("Game controller %1% ID: %2%, instance descriptor: %3%, product descriptor: %4%", i, impl->gameControllers[i].GetID(), impl->gameControllers[i].GetInstanceDescriptor(), impl->gameControllers[i].GetProductDescriptor());
        ConfigureInputDevice(impl->gameControllers[i], impl->gameControllers[i].GetProductDescriptor(), impl->inputDevicesAssetReader, impl->configFileBuffer);
    }

    //Mice
    AndroidMouse::CreateMice(impl->mice, androidApp);
    FINJIN_DEBUG_LOG_INFO("Discovered %1% mice", impl->mice.size());
    for (size_t i = 0; i < impl->mice.size(); i++)
    {
        FINJIN_DEBUG_LOG_INFO("Mouse %1% ID: %2%", i, impl->mice[i].GetID());
        ConfigureInputDevice(impl->mice[i], impl->mice[i].GetProductDescriptor(), impl->inputDevicesAssetReader, impl->configFileBuffer);
    }

    //Keyboard
    AndroidKeyboard::CreateKeyboards(impl->keyboards, androidApp);
    FINJIN_DEBUG_LOG_INFO("Discovered %1% keyboards", impl->keyboards.size());
    for (size_t i = 0; i < impl->keyboards.size(); i++)
    {
        FINJIN_DEBUG_LOG_INFO("Keyboard %1% ID: %2%", i, impl->keyboards[i].GetID());
        ConfigureInputDevice(impl->keyboards[i], impl->keyboards[i].GetProductDescriptor(), impl->inputDevicesAssetReader, impl->configFileBuffer);
    }

    //Touch screen
    impl->touchScreen.SetDisplayName("Touch screen");
    impl->touchScreen.Connect(!AndroidUtilities::IsAndroidTV(androidApp));

    //Accelerometer
    if (impl->settings.useAccelerometer)
        impl->accelerometer.Initialize(androidApp, impl->settings.accelerometerAlooperID);
}

void AndroidInputContext::Destroy()
{
    for (auto& device : impl->gameControllers)
        device.Destroy();
    impl->gameControllers.clear();

    for (auto& device : impl->mice)
        device.Destroy();
    impl->mice.clear();

    for (auto& device : impl->keyboards)
        device.Destroy();
    impl->keyboards.clear();

    impl->touchScreen.Reset();

    impl->accelerometer.Destroy();
}

void AndroidInputContext::GetSelectorComponents(AssetPathSelector& result)
{
}

const AndroidInputContext::Settings& AndroidInputContext::GetSettings() const
{
    return impl->settings;
}

void AndroidInputContext::Update(SimpleTimeDelta elapsedTime, InputDevicePollFlag flags)
{
    auto androidApp = static_cast<android_app*>(impl->settings.applicationHandle);

    auto checked = false;

    if (AnySet(flags & InputDevicePollFlag::CHECK_FOR_NEW_DEVICES))
    {
        impl->DetectNewDevices();
        checked = true;
    }

    //On Android TV finding a suitable input device is critical, so try to detect new devices if necessary
    if (!checked && AndroidUtilities::IsAndroidTV(androidApp) && !impl->AnyGameControllersConnected())
    {
        impl->DetectNewDevices();
        checked = true;
    }

    if (checked)
    {
        for (auto& device : impl->gameControllers)
        {
            if (device.IsNewConnection())
                ConfigureInputDevice(device, device.GetProductDescriptor(), impl->inputDevicesAssetReader, impl->configFileBuffer);
        }

        for (auto& device : impl->mice)
        {
            if (device.IsNewConnection())
                ConfigureInputDevice(device, device.GetProductDescriptor(), impl->inputDevicesAssetReader, impl->configFileBuffer);
        }

        for (auto& device : impl->keyboards)
        {
            if (device.IsNewConnection())
                ConfigureInputDevice(device, device.GetProductDescriptor(), impl->inputDevicesAssetReader, impl->configFileBuffer);
        }
    }
}

void AndroidInputContext::Execute(InputEvents& events, InputCommands& commands, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);
}

void AndroidInputContext::HandleDeviceChanges()
{
    for (auto& device : impl->gameControllers)
        device.ClearChanged();

    for (auto& device : impl->mice)
        device.ClearChanged();

    for (auto& device : impl->keyboards)
        device.ClearChanged();

    impl->touchScreen.ClearChanged();

    impl->accelerometer.ClearChanged();
}

void AndroidInputContext::HandleApplicationViewportLostFocus()
{
    impl->accelerometer.Enable(false);
}

void AndroidInputContext::HandleApplicationViewportGainedFocus()
{
    impl->accelerometer.Enable(true);
}

size_t AndroidInputContext::GetDeviceCount(InputDeviceClass deviceClass) const
{
    switch (deviceClass)
    {
        case InputDeviceClass::KEYBOARD: return impl->keyboards.size();
        case InputDeviceClass::MOUSE: return impl->mice.size();
        case InputDeviceClass::GAME_CONTROLLER: return impl->gameControllers.size();
        case InputDeviceClass::TOUCH_SCREEN: return impl->touchScreen.IsConnected() ? 1 : 0;
    }

    return 0;
}

const Utf8String& AndroidInputContext::GetDeviceProductDescriptor(InputDeviceClass deviceClass, size_t index) const
{
    switch (deviceClass)
    {
        case InputDeviceClass::KEYBOARD: return impl->keyboards[index].GetProductDescriptor();
        case InputDeviceClass::MOUSE: return impl->mice[index].GetProductDescriptor();
        case InputDeviceClass::GAME_CONTROLLER: return impl->gameControllers[index].GetProductDescriptor();
    }

    return Utf8String::Empty();
}

const Utf8String& AndroidInputContext::GetDeviceInstanceDescriptor(InputDeviceClass deviceClass, size_t index) const
{
    switch (deviceClass)
    {
        case InputDeviceClass::KEYBOARD: return impl->keyboards[index].GetInstanceDescriptor();
        case InputDeviceClass::MOUSE: return impl->mice[index].GetInstanceDescriptor();
        case InputDeviceClass::GAME_CONTROLLER: return impl->gameControllers[index].GetInstanceDescriptor();
    }

    return Utf8String::Empty();
}

InputDeviceSemantic AndroidInputContext::GetDeviceSemantic(InputDeviceClass deviceClass, size_t index) const
{
    switch (deviceClass)
    {
        case InputDeviceClass::KEYBOARD: return impl->keyboards[index].GetSemantic();
        case InputDeviceClass::MOUSE: return impl->mice[index].GetSemantic();
        case InputDeviceClass::GAME_CONTROLLER: return impl->gameControllers[index].GetSemantic();
    }

    return InputDeviceSemantic::NONE;
}

bool AndroidInputContext::IsDeviceConnected(InputDeviceClass deviceClass, size_t index) const
{
    switch (deviceClass)
    {
        case InputDeviceClass::KEYBOARD: return impl->keyboards[index].IsConnected();
        case InputDeviceClass::MOUSE: return impl->mice[index].IsConnected();
        case InputDeviceClass::GAME_CONTROLLER: return impl->gameControllers[index].IsConnected();
        case InputDeviceClass::TOUCH_SCREEN: return impl->touchScreen.IsConnected();
    }

    return false;
}

void AndroidInputContext::AddHapticFeedback(InputDeviceClass deviceClass, size_t index, const HapticFeedbackSettings* forces, size_t count)
{
}

void AndroidInputContext::StopHapticFeedback(InputDeviceClass deviceClass, size_t index)
{
}

size_t AndroidInputContext::GetGameControllerCount() const
{
    return impl->gameControllers.size();
}

AndroidGameController* AndroidInputContext::GetGameController(size_t index)
{
    return &impl->gameControllers[index];
}

void AndroidInputContext::AddExternalGameController(InputGenericGameController* gameController, bool configure, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    if (impl->externalGameControllers.push_back(gameController).HasErrorOrValue(false))
    {
        FINJIN_SET_ERROR(error, "Failed to add external game controller.");
        return;
    }

    if (configure)
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

void AndroidInputContext::RemoveExternalGameController(InputGenericGameController* gameController)
{
    auto foundAt = impl->externalGameControllers.find(gameController);
    if (foundAt != impl->externalGameControllers.end())
        impl->externalGameControllers.erase(foundAt);
}

size_t AndroidInputContext::GetExternalGameControllerCount() const
{
    return impl->externalGameControllers.size();
}

InputGenericGameController* AndroidInputContext::GetExternalGameController(size_t index)
{
    return impl->externalGameControllers[index];
}

size_t AndroidInputContext::GetMouseCount() const
{
    return impl->mice.size();
}

AndroidMouse* AndroidInputContext::GetMouse(size_t index)
{
    return &impl->mice[index];
}

size_t AndroidInputContext::GetKeyboardCount() const
{
    return impl->keyboards.size();
}

AndroidKeyboard* AndroidInputContext::GetKeyboard(size_t index)
{
    return &impl->keyboards[index];
}

size_t AndroidInputContext::GetTouchScreenCount() const
{
    return impl->touchScreen.IsConnected() ? 1 : 0;
}

InputTouchScreen* AndroidInputContext::GetTouchScreen(size_t index)
{
    return &impl->touchScreen;
}

size_t AndroidInputContext::GetAccelerometerCount() const
{
    return impl->accelerometer.IsInitialized() ? 1 : 0;
}

InputAccelerometer* AndroidInputContext::GetAccelerometer(size_t index)
{
    return &impl->accelerometer;
}

FileOperationResult AndroidInputContext::ReadInputBinding(InputDeviceClass deviceClass, size_t deviceIndex, const AssetReference& configAssetRef, ByteBuffer& fileBuffer)
{
    impl->inputBindingsAssetReader.GetSelector().Set(AssetPathComponent::INPUT_DEVICE_TYPE, InputDeviceClassUtilities::ToStringLower(deviceClass));

    impl->inputBindingsAssetReader.GetSelector().Set(AssetPathComponent::INPUT_DEVICE_DESCRIPTOR, GetDeviceProductDescriptor(deviceClass, deviceIndex));

    return impl->inputBindingsAssetReader.ReadAsset(fileBuffer, configAssetRef);
}

AndroidInputContext::Impl* AndroidInputContext::GetImpl()
{
    return impl.get();
}

int32_t AndroidInputContext::HandleApplicationInputEvent(void* _androidInputEvent)
{
    int32_t ignoreSystemHandler = 0; //Set to 1 if the event is handled and should not be handled by the system

    auto androidInputEvent = static_cast<AInputEvent*>(_androidInputEvent);
    auto androidApp = static_cast<android_app*>(impl->settings.applicationHandle);
    auto screenDensity = AndroidUtilities::GetScreenDensity(androidApp);

    auto eventType = AInputEvent_getType(androidInputEvent);

    auto eventDeviceID = AInputEvent_getDeviceId(androidInputEvent);

    auto eventSource = AInputEvent_getSource(androidInputEvent);
    auto eventSourceIsGameController = (eventSource & (AINPUT_SOURCE_GAMEPAD | AINPUT_SOURCE_JOYSTICK)) != 0;
    auto eventSourceIsKeyboard = (eventSource & AINPUT_SOURCE_KEYBOARD) != 0;
    auto eventSourceIsMouse = (eventSource & AINPUT_SOURCE_MOUSE) != 0;

    AndroidGameController* gameController = eventSourceIsGameController ? AndroidGameController::GetGameControllerByID(impl->gameControllers.data(), impl->gameControllers.size(), eventDeviceID) : nullptr;
    AndroidKeyboard* keyboard = eventSourceIsKeyboard ? AndroidKeyboard::GetKeyboardByID(impl->keyboards.data(), impl->keyboards.size(), eventDeviceID) : nullptr;
    AndroidMouse* mouse = eventSourceIsMouse ? AndroidMouse::GetMouseByID(impl->mice.data(), impl->mice.size(), eventDeviceID) : nullptr;

    //FINJIN_DEBUG_LOG_INFO("Event generating device ID: %1%, keyboard: %2%, mouse: %3%, game controller: %4%, event source: %5%, eventType: %6%", eventDeviceID, keyboard, mouse, gameController, eventSource, eventType);

    auto handledKey = false;
    if (eventType == AINPUT_EVENT_TYPE_KEY)
    {
        auto keyCode = AKeyEvent_getKeyCode(androidInputEvent);
        auto action = AKeyEvent_getAction(androidInputEvent);
        auto keyFlags = AKeyEvent_getFlags(androidInputEvent);
        auto isVirtualHardKey = (keyFlags & AKEY_EVENT_FLAG_VIRTUAL_HARD_KEY) != 0;
        auto eventSourceActsLikeMouseOrGameController = eventSourceIsGameController | eventSourceIsMouse;

        //FINJIN_DEBUG_LOG_INFO("Received keycode : %1% from device id: %2%, keyboard: %3%, mouse: %4%, game controller: %5%, is virtual hard key: %6%, event source: %7%", keyCode, eventDeviceID, keyboard, mouse, gameController, (int)isVirtualHardKey, eventSource);

        if (!handledKey && gameController != nullptr)
        {
            //FINJIN_DEBUG_LOG_INFO("Game controller %1% button event keyCode: %2%, action: %3%", gameController->GetID(), keyCode, action);

            auto button = GetButtonByCode(gameController, keyCode);
            if (button != nullptr)
            {
                UpdateButtonFromAction(button, action);

                //FINJIN_DEBUG_LOG_INFO("Game controller key event: Pressed button %1%", button->GetDisplayName());

                handledKey = true;
                if (keyCode != AKEYCODE_HOME)
                    ignoreSystemHandler = 1;
            }
        }
        if (!handledKey && mouse != nullptr)
        {
            //FINJIN_DEBUG_LOG_INFO("Mouse button event keyCode: %1%, action: %2%", keyCode, action);

            auto button = GetButtonByCode(mouse, keyCode);
            if (button != nullptr)
            {
                UpdateButtonFromAction(button, action);

                //FINJIN_DEBUG_LOG_INFO("Mouse key event: Pressed button %1%", button->GetDisplayName());

                handledKey = true;
                if (keyCode != AKEYCODE_HOME)
                    ignoreSystemHandler = 1;
            }
        }
        if (!handledKey && keyboard != nullptr && eventSource == AINPUT_SOURCE_KEYBOARD && (keyCode != AKEYCODE_BACK || !isVirtualHardKey))
        {
            auto key = GetButtonByCode(keyboard, keyCode);
            if (key != nullptr)
            {
                UpdateButtonFromAction(key, action);

                //FINJIN_DEBUG_LOG_INFO("Keyboard key event: Pressed keyboard key %1%, pressed %2%, %3%", keyCode, (int)key->GetValue(), key->GetDisplayName());

                handledKey = true;
                if (keyCode != AKEYCODE_HOME)
                    ignoreSystemHandler = 1;
            }
        }

        if (!handledKey && keyCode == AKEYCODE_BACK && !eventSourceActsLikeMouseOrGameController && impl->settings.useSystemBackButton)
        {
            //if (action == AKEY_EVENT_ACTION_UP)
                //impl->inputEvents.AddSystemButtonPressedEvent(SystemButtonEvent::WhichButton::BACK);

            handledKey = true;
            //ignoreSystemHandler = 1; //For now, just let the system use the back button
        }

        if (keyCode == AKEYCODE_BACK && eventSourceActsLikeMouseOrGameController && impl->settings.useSystemBackButton)
        {
            //Mouse right click. Never use the default handling
            ignoreSystemHandler = 1;
        }
    }
    else if (eventType == AINPUT_EVENT_TYPE_MOTION)
    {
        switch (eventSource)
        {
            case AINPUT_SOURCE_GAMEPAD:
            case AINPUT_SOURCE_JOYSTICK:
            {
                //FINJIN_DEBUG_LOG_INFO("Game controller ID: %1%", eventDeviceID);

                if (gameController != nullptr)
                {
                    for (size_t i = 0; i < gameController->GetAxisCount(); i++)
                    {
                        auto axis = gameController->GetAxis(i);
                        auto axisValue = AMotionEvent_getAxisValue(androidInputEvent, axis->GetCode(), 0);

                        axis->Update(axisValue);
                    }

                    ignoreSystemHandler = 1;
                }

                break;
            }
            case AINPUT_SOURCE_MOUSE:
            {
                //FINJIN_DEBUG_LOG_INFO("Mouse ID: %1%", eventDeviceID);

                if (mouse != nullptr)
                {
                    for (size_t i = 0; i < mouse->GetAxisCount(); i++)
                    {
                        auto axis = mouse->GetAxis(i);
                        auto axisValue = AMotionEvent_getAxisValue(androidInputEvent, axis->GetCode(), 0);
                        switch (axis->GetCode())
                        {
                            case AMOTION_EVENT_AXIS_X:
                            case AMOTION_EVENT_AXIS_Y:
                            {
                                axisValue = InputCoordinate(axisValue, InputCoordinate::Type::PIXELS, screenDensity).ToDipsValue();
                                break;
                            }
                        }

                        axis->Update(axisValue, false);

                        //FINJIN_DEBUG_LOG_INFO("  Mouse axis %1% '%2%' was updated to %3%", axis->GetCode(), axis->GetDisplayName(), axisValue);
                    }

                    ignoreSystemHandler = 1;
                }

                break;
            }
            case AINPUT_SOURCE_STYLUS:
            case AINPUT_SOURCE_TOUCHSCREEN:
            {
                auto pointerType = PointerType::TOUCH_SCREEN;

                auto rawAction = AMotionEvent_getAction(androidInputEvent);
                auto action = rawAction & AMOTION_EVENT_ACTION_MASK;

                if (action == AMOTION_EVENT_ACTION_DOWN || action == AMOTION_EVENT_ACTION_POINTER_DOWN)
                {
                    int i = (rawAction & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
                    auto pointerID = AMotionEvent_getPointerId(androidInputEvent, i);
                    auto pointer = impl->touchScreen.ConnectPointer(pointerType, pointerID);
                    if (pointer != nullptr)
                    {
                        auto inputX = InputCoordinate(AMotionEvent_getX(androidInputEvent, i), InputCoordinate::Type::PIXELS, screenDensity).ToDipsValue();
                        auto inputY = InputCoordinate(AMotionEvent_getY(androidInputEvent, i), InputCoordinate::Type::PIXELS, screenDensity).ToDipsValue();
                        pointer->Update(true, inputX, inputY);
                    }
                }
                else if (action == AMOTION_EVENT_ACTION_MOVE)
                {
                    size_t pointerCount = AMotionEvent_getPointerCount(androidInputEvent);
                    for (size_t i = 0; i < pointerCount; i++)
                    {
                        auto pointerID = AMotionEvent_getPointerId(androidInputEvent, i);
                        auto pointer = impl->touchScreen.GetPointer(pointerType, pointerID);
                        if (pointer != nullptr)
                        {
                            auto inputX = InputCoordinate(AMotionEvent_getX(androidInputEvent, i), InputCoordinate::Type::PIXELS, screenDensity).ToDipsValue();
                            auto inputY = InputCoordinate(AMotionEvent_getY(androidInputEvent, i), InputCoordinate::Type::PIXELS, screenDensity).ToDipsValue();
                            pointer->Update(true, inputX, inputY);
                        }
                    }
                }
                else if (action == AMOTION_EVENT_ACTION_UP || action == AMOTION_EVENT_ACTION_POINTER_UP)
                {
                    int i = (rawAction & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
                    auto pointerID = AMotionEvent_getPointerId(androidInputEvent, i);
                    auto pointer = impl->touchScreen.GetPointer(pointerType, pointerID);
                    if (pointer != nullptr)
                    {
                        auto inputX = InputCoordinate(AMotionEvent_getX(androidInputEvent, i), InputCoordinate::Type::PIXELS, screenDensity).ToDipsValue();
                        auto inputY = InputCoordinate(AMotionEvent_getY(androidInputEvent, i), InputCoordinate::Type::PIXELS, screenDensity).ToDipsValue();
                        pointer->Update(false, inputX, inputY);
                    }
                }

                break;
            }
        }
    }

    return ignoreSystemHandler;
}

bool AndroidInputContext::ProcessQueue(int ident)
{
    if (ident == impl->settings.accelerometerAlooperID)
    {
        impl->accelerometer.ProcessQueue();
        return true;
    }

    return false;
}
