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
#include "Win32InputContext.hpp"
#include "finjin/common/StaticVector.hpp"
#include "finjin/common/WindowsUtilities.hpp"
#include "finjin/engine/InputDeviceSerializer.hpp"
#include "finjin/engine/OSWindow.hpp"
#include "finjin/engine/internal/input/xinput/XInputSystem.hpp"
#include "DInputDevice.hpp"
#include <Windows.h>
#include <Xinput.h>

using namespace Finjin::Engine;


//Macros------------------------------------------------------------------------
#define MAX_DINPUT_GAME_CONTROLLER_COUNT 8 //DirectInput has no inherent limit but we impose one
#define MAX_EXTERNAL_GAME_CONTROLLER_COUNT (GameControllerConstants::MAX_GAME_CONTROLLERS - XUSER_MAX_COUNT - MAX_DINPUT_GAME_CONTROLLER_COUNT)


//Locals------------------------------------------------------------------------
static const Utf8String DINPUT_SYSTEM_INTERNAL_NAME("dinput");


//Local types-------------------------------------------------------------------
struct Win32InputContext::Impl : public AllocatedClass
{
    Impl(Allocator* allocator, Win32InputSystem* inputSystem) : AllocatedClass(allocator), settings(allocator)
    {
        this->inputSystem = inputSystem;

        this->dinput = nullptr;

        this->updateCount = 0;

        this->dinputGameControllerCount = 0;

        this->configFileBuffer.Create(EngineConstants::DEFAULT_CONFIGURATION_BUFFER_SIZE, allocator);
    }

    Win32InputSystem* inputSystem;

    Win32InputContext::Settings settings;

    AssetClassFileReader xinputDevicesAssetReader;
    AssetClassFileReader dinputDevicesAssetReader;
    AssetClassFileReader inputBindingsAssetReader;

    IDInput dinput;

    int updateCount;

    std::array<XInputGameController, XUSER_MAX_COUNT> xinputGameControllers;

    std::array<DInputDeviceConfiguration, MAX_DINPUT_GAME_CONTROLLER_COUNT> dinputGameControllerConfigs;
    std::array<DInputGameController, MAX_DINPUT_GAME_CONTROLLER_COUNT> dinputGameControllers;
    size_t dinputGameControllerCount;

    StaticVector<InputGenericGameController*, MAX_EXTERNAL_GAME_CONTROLLER_COUNT> externalGameControllers;

    StaticVector<DInputDeviceConfiguration, MouseConstants::MAX_MICE> mouseFoundConfigs;
    StaticVector<DInputMouse, MouseConstants::MAX_MICE> mice;

    StaticVector<DInputDeviceConfiguration, KeyboardConstants::MAX_KEYBOARDS> keyboardFoundConfigs;
    StaticVector<DInputKeyboard, KeyboardConstants::MAX_KEYBOARDS> keyboards;

    StaticVector<InputGenericHeadset*, HeadsetConstants::MAX_HEADSETS> externalHeadsets;

    ByteBuffer configFileBuffer;
};


//Local functions---------------------------------------------------------------
static BOOL CALLBACK EnumGameControllersCallback(const DIDEVICEINSTANCE* ddi, void* data)
{
    //Only handle non-XInput devices
    if (!WindowsUtilities::IsXInputDevice(ddi->guidProduct))
    {
        auto impl = reinterpret_cast<Win32InputContext*>(data)->GetImpl();
        if (impl->dinputGameControllerCount < impl->dinputGameControllerConfigs.max_size())
            impl->dinputGameControllerConfigs[impl->dinputGameControllerCount++] = DInputDeviceConfiguration(ddi);
        else
            return DIENUM_STOP;
    }

    return DIENUM_CONTINUE;
}

static BOOL CALLBACK EnumKeyboardsCallback(const DIDEVICEINSTANCE* ddi, void* data)
{
    auto impl = reinterpret_cast<Win32InputContext*>(data)->GetImpl();
    return impl->keyboardFoundConfigs.push_back(DInputDeviceConfiguration(ddi)).HasValue(true) ? DIENUM_CONTINUE : DIENUM_STOP;
}

static BOOL CALLBACK EnumMiceCallback(const DIDEVICEINSTANCE* ddi, void* data)
{
    auto impl = reinterpret_cast<Win32InputContext*>(data)->GetImpl();
    return impl->mouseFoundConfigs.push_back(DInputDeviceConfiguration(ddi)).HasValue(true) ? DIENUM_CONTINUE : DIENUM_STOP;
}

template <typename Configs, typename StaticDevices>
void CheckForNewDevices
    (
    Configs& configs,
    StaticDevices& devices,
    size_t previousDeviceCount,
    size_t newDeviceCount,
    Win32InputContext* context,
    AssetClassFileReader& assetReader,
    ByteBuffer& configFileBuffer
    )
{
    for (size_t i = previousDeviceCount; i < newDeviceCount; i++)
    {
        auto& config = configs[i];

        //Handle new controller
        //std::cout << "Found a new device: " << config.GetDebugName() << std::endl;

        auto& newDevice = devices[i];

        //Create
        FINJIN_DECLARE_ERROR(error);
        newDevice.Create(context, config, error);
        if (error)
        {
            //Failed
        }
        else
        {
            //Configure
            ConfigureInputDevice(newDevice, newDevice.GetProductDescriptor(), assetReader, configFileBuffer);
        }
    }
}

template <typename Configs, typename DynamicDevices>
void CheckForNewDevices
    (
    Configs& configs,
    DynamicDevices& devices,
    Win32InputContext* context,
    AssetClassFileReader& assetReader,
    ByteBuffer& configFileBuffer
    )
{
    for (auto& config : configs)
    {
        if (devices.full())
            break;

        auto result = std::find_if(devices.begin(), devices.end(), [&config](auto& device) { return device.GetInstanceDescriptor() == config.instanceDescriptor; });
        if (result == devices.end())
        {
            //Found a new controller
            //std::cout << "Found a new device: " << config.GetDebugName() << std::endl;

            devices.push_back();
            auto& newDevice = devices.back();

            //Create
            FINJIN_DECLARE_ERROR(error);
            newDevice.Create(context, config, error);
            if (error)
            {
                //Failed, so remove
                devices.pop_back();
            }
            else
            {
                //Configure
                ConfigureInputDevice(newDevice, newDevice.GetProductDescriptor(), assetReader, configFileBuffer);
            }
        }
    }
}


//Implementation----------------------------------------------------------------
Win32InputContext::Win32InputContext(Allocator* allocator, Win32InputSystem* inputSystem) :
    AllocatedClass(allocator),
    impl(AllocatedClass::New<Impl>(allocator, FINJIN_CALLER_ARGUMENTS, inputSystem))
{
}

Win32InputContext::~Win32InputContext()
{
}

void Win32InputContext::Create(const Settings& settings, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    assert(settings.osWindow != nullptr);
    assert(settings.applicationHandle != nullptr);
    assert(settings.assetFileReader != nullptr);

    FINJIN_ENGINE_CHECK_IMPL_NOT_NULL(impl, error);

    //Copy settings---------------------------------------------
    impl->settings = settings;

    auto devicesAssetSelector = impl->settings.initialAssetFileSelector;
    devicesAssetSelector.Set(AssetPathComponent::INPUT_API, XInputSystem::GetSystemInternalName()); //xinput
    impl->xinputDevicesAssetReader.Create(*impl->settings.assetFileReader, devicesAssetSelector, AssetClass::INPUT_DEVICE, GetAllocator(), error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to create XInput device asset file reader.");
        return;
    }

    devicesAssetSelector.Set(AssetPathComponent::INPUT_API, DINPUT_SYSTEM_INTERNAL_NAME); //dinput
    impl->dinputDevicesAssetReader.Create(*impl->settings.assetFileReader, devicesAssetSelector, AssetClass::INPUT_DEVICE, GetAllocator(), error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to create DirectInput device asset file reader.");
        return;
    }

    impl->inputBindingsAssetReader.Create(*impl->settings.assetFileReader, impl->settings.initialAssetFileSelector, AssetClass::INPUT_BINDINGS, GetAllocator(), error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to create input bindings asset file reader.");
        return;
    }

    //XInput game controllers-----------------------
    for (size_t gameControllerIndex = 0; gameControllerIndex < impl->xinputGameControllers.size(); gameControllerIndex++)
    {
        impl->xinputGameControllers[gameControllerIndex].Create(gameControllerIndex);
        ConfigureInputDevice(impl->xinputGameControllers[gameControllerIndex], impl->xinputGameControllers[gameControllerIndex].GetProductDescriptor(), impl->xinputDevicesAssetReader, impl->configFileBuffer);
    }

    //DirectInput----------------------------------------
    //Create the main DirectInput object
    if (FAILED(DirectInput8Create(static_cast<HINSTANCE>(impl->settings.applicationHandle), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&impl->dinput, 0)))
    {
        FINJIN_SET_ERROR(error, "Failed to initialize DirectInput.");
        return;
    }

    //Enumerate the devices
    impl->dinput->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumGameControllersCallback, this, DIEDFL_ATTACHEDONLY);
    for (size_t gameControllerIndex = 0; gameControllerIndex < impl->dinputGameControllerCount; gameControllerIndex++)
    {
        impl->dinputGameControllers[gameControllerIndex].Create(this, impl->dinputGameControllerConfigs[gameControllerIndex], error);
        if (error)
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to initialize game controller '%1%'.", impl->dinputGameControllerConfigs[gameControllerIndex].GetDebugName()));
            return;
        }

        ConfigureInputDevice(impl->dinputGameControllers[gameControllerIndex], impl->dinputGameControllerConfigs[gameControllerIndex].GetProductDescriptor(), impl->dinputDevicesAssetReader, impl->configFileBuffer);
    }

    impl->dinput->EnumDevices(DI8DEVCLASS_POINTER, EnumMiceCallback, this, DIEDFL_ATTACHEDONLY);
    impl->mice.resize(impl->mouseFoundConfigs.size());
    for (size_t mouseIndex = 0; mouseIndex < impl->mouseFoundConfigs.size(); mouseIndex++)
    {
        impl->mice[mouseIndex].Create(this, impl->mouseFoundConfigs[mouseIndex], error);
        if (error)
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to initialize mouse '%1%'.", impl->mouseFoundConfigs[mouseIndex].GetDebugName()));
            return;
        }

        ConfigureInputDevice(impl->mice[mouseIndex], impl->mouseFoundConfigs[mouseIndex].GetProductDescriptor(), impl->dinputDevicesAssetReader, impl->configFileBuffer);
    }

    impl->dinput->EnumDevices(DI8DEVCLASS_KEYBOARD, EnumKeyboardsCallback, this, DIEDFL_ATTACHEDONLY);
    impl->keyboards.resize(impl->keyboardFoundConfigs.size());
    for (size_t keyboardIndex = 0; keyboardIndex < impl->keyboardFoundConfigs.size(); keyboardIndex++)
    {
        impl->keyboards[keyboardIndex].Create(this, impl->keyboardFoundConfigs[keyboardIndex], error);
        if (error)
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to initialize keyboard '%1%'.", impl->keyboardFoundConfigs[keyboardIndex].GetDebugName()));
            return;
        }

        ConfigureInputDevice(impl->keyboards[keyboardIndex], impl->keyboardFoundConfigs[keyboardIndex].GetProductDescriptor(), impl->dinputDevicesAssetReader, impl->configFileBuffer);
    }

    impl->updateCount = 0;
    Update(0);
}

void Win32InputContext::Destroy()
{
    for (auto& device : impl->xinputGameControllers)
        device.Destroy();
    for (auto& device : impl->dinputGameControllers)
        device.Destroy();
    impl->dinputGameControllerCount = 0;

    for (auto& device : impl->mice)
        device.Destroy();
    impl->mouseFoundConfigs.clear();
    impl->mice.clear();

    for (auto& device : impl->keyboards)
        device.Destroy();
    impl->keyboardFoundConfigs.clear();
    impl->keyboards.clear();

    FINJIN_SAFE_RELEASE(impl->dinput);
}

void Win32InputContext::GetSelectorComponents(AssetPathSelector& result)
{
}

const Win32InputContext::Settings& Win32InputContext::GetSettings() const
{
    return impl->settings;
}

void* Win32InputContext::GetDInput()
{
    return impl->dinput;
}

void Win32InputContext::Update(SimpleTimeDelta elapsedTime, InputDevicePollFlag flags)
{
    bool isFirstUpdate = impl->updateCount++ == 0;

    for (auto& device : impl->xinputGameControllers)
        device.Update(elapsedTime, isFirstUpdate);

    for (size_t gameControllerIndex = 0; gameControllerIndex < impl->dinputGameControllerCount; gameControllerIndex++)
    {
        auto& device = impl->dinputGameControllers[gameControllerIndex];
        device.Update(elapsedTime, isFirstUpdate);
    }

    for (auto& device : impl->mice)
        device.Update(elapsedTime, isFirstUpdate);

    for (auto& device : impl->keyboards)
        device.Update(elapsedTime, isFirstUpdate);

    if (AnySet(flags & InputDevicePollFlag::CHECK_FOR_NEW_DEVICES))
    {
        if (impl->dinputGameControllerCount < impl->dinputGameControllers.size())
        {
            auto countBefore = impl->dinputGameControllerCount;
            impl->dinput->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumGameControllersCallback, this, DIEDFL_ATTACHEDONLY);
            CheckForNewDevices(impl->dinputGameControllerConfigs, impl->dinputGameControllers, countBefore, impl->dinputGameControllerCount, this, impl->dinputDevicesAssetReader, impl->configFileBuffer);
        }

        if (!impl->mice.full())
        {
            impl->mouseFoundConfigs.clear();
            impl->dinput->EnumDevices(DI8DEVCLASS_POINTER, EnumMiceCallback, this, DIEDFL_ATTACHEDONLY);
            CheckForNewDevices(impl->mouseFoundConfigs, impl->mice, this, impl->dinputDevicesAssetReader, impl->configFileBuffer);
        }

        if (!impl->keyboards.full())
        {
            impl->keyboardFoundConfigs.clear();
            impl->dinput->EnumDevices(DI8DEVCLASS_KEYBOARD, EnumKeyboardsCallback, this, DIEDFL_ATTACHEDONLY);
            CheckForNewDevices(impl->keyboardFoundConfigs, impl->keyboards, this, impl->dinputDevicesAssetReader, impl->configFileBuffer);
        }
    }
}

void Win32InputContext::Execute(InputEvents& events, InputCommands& commands, Error& error)
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

void Win32InputContext::HandleDeviceChanges()
{
    for (auto& device : impl->xinputGameControllers)
        device.ClearChanged();

    for (size_t gameControllerIndex = 0; gameControllerIndex < impl->dinputGameControllerCount; gameControllerIndex++)
    {
        auto& device = impl->dinputGameControllers[gameControllerIndex];
        device.ClearChanged();
    }

    for (auto& device : impl->mice)
        device.ClearChanged();

    for (auto& device : impl->keyboards)
        device.ClearChanged();
}

void Win32InputContext::HandleApplicationViewportLostFocus()
{
    for (auto& device : impl->xinputGameControllers)
        device.StopHapticFeedback();

    for (size_t gameControllerIndex = 0; gameControllerIndex < impl->dinputGameControllerCount; gameControllerIndex++)
    {
        auto& device = impl->dinputGameControllers[gameControllerIndex];
        device.StopHapticFeedback();
    }

    for (auto& device : impl->externalGameControllers)
        device->StopHapticFeedback();
}

void Win32InputContext::HandleApplicationViewportGainedFocus()
{
}

size_t Win32InputContext::GetDeviceCount(InputDeviceClass deviceClass) const
{
    switch (deviceClass)
    {
        case InputDeviceClass::KEYBOARD: return GetKeyboardCount();
        case InputDeviceClass::MOUSE: return GetMouseCount();
        case InputDeviceClass::GAME_CONTROLLER: return GetGameControllerCount();
        case InputDeviceClass::HEADSET: return GetExternalHeadsetCount();
    }

    return 0;
}

const Utf8String& Win32InputContext::GetDeviceProductDescriptor(InputDeviceClass deviceClass, size_t index) const
{
    switch (deviceClass)
    {
        case InputDeviceClass::KEYBOARD: return GetKeyboardProductDescriptor(index);
        case InputDeviceClass::MOUSE: return GetMouseProductDescriptor(index);
        case InputDeviceClass::GAME_CONTROLLER: return GetGameControllerProductDescriptor(index);
        case InputDeviceClass::HEADSET: return impl->externalHeadsets[index]->GetProductDescriptor();
    }

    return Utf8String::GetEmpty();
}

const Utf8String& Win32InputContext::GetDeviceInstanceDescriptor(InputDeviceClass deviceClass, size_t index) const
{
    switch (deviceClass)
    {
        case InputDeviceClass::KEYBOARD: return GetKeyboardInstanceDescriptor(index);
        case InputDeviceClass::MOUSE: return GetMouseInstanceDescriptor(index);
        case InputDeviceClass::GAME_CONTROLLER: return GetGameControllerInstanceDescriptor(index);
        case InputDeviceClass::HEADSET: return impl->externalHeadsets[index]->GetInstanceDescriptor();
    }

    return Utf8String::GetEmpty();
}

InputDeviceSemantic Win32InputContext::GetDeviceSemantic(InputDeviceClass deviceClass, size_t index) const
{
    switch (deviceClass)
    {
        case InputDeviceClass::KEYBOARD: return GetKeyboardSemantic(index);
        case InputDeviceClass::MOUSE: return GetMouseSemantic(index);
        case InputDeviceClass::GAME_CONTROLLER: return GetGameControllerSemantic(index);
        case InputDeviceClass::HEADSET: return impl->externalHeadsets[index]->GetSemantic();
    }

    return InputDeviceSemantic::NONE;
}

bool Win32InputContext::IsDeviceConnected(InputDeviceClass deviceClass, size_t index) const
{
    switch (deviceClass)
    {
        case InputDeviceClass::KEYBOARD: return IsKeyboardConnected(index);
        case InputDeviceClass::MOUSE: return IsMouseConnected(index);
        case InputDeviceClass::GAME_CONTROLLER: return IsGameControllerConnected(index);
        case InputDeviceClass::HEADSET: return impl->externalHeadsets[index]->IsConnected();
    }

    return false;
}

void Win32InputContext::AddHapticFeedback(InputDeviceClass deviceClass, size_t index, const HapticFeedbackSettings* forces, size_t count)
{
    if (deviceClass == InputDeviceClass::GAME_CONTROLLER)
    {
        if (index < impl->xinputGameControllers.size())
            impl->xinputGameControllers[index].AddHapticFeedback(forces, count);
        else if (index < impl->xinputGameControllers.size() + impl->dinputGameControllers.size())
            impl->dinputGameControllers[index - impl->xinputGameControllers.size()].AddHapticFeedback(forces, count);
        else
            impl->externalGameControllers[index - impl->xinputGameControllers.size() - impl->dinputGameControllers.size()]->AddHapticFeedback(forces, count);
    }
}

void Win32InputContext::StopHapticFeedback(InputDeviceClass deviceClass, size_t index)
{
    if (deviceClass == InputDeviceClass::GAME_CONTROLLER)
    {
        if (index < impl->xinputGameControllers.size())
            impl->xinputGameControllers[index].StopHapticFeedback();
        else if (index < impl->xinputGameControllers.size() + impl->dinputGameControllers.size())
            impl->dinputGameControllers[index - impl->xinputGameControllers.size()].StopHapticFeedback();
        else
            impl->externalGameControllers[index - impl->xinputGameControllers.size() - impl->dinputGameControllers.size()]->StopHapticFeedback();
    }
}

size_t Win32InputContext::GetXInputGameControllerCount() const
{
    return impl->xinputGameControllers.size();
}

size_t Win32InputContext::GetGameControllerCount() const
{
    return impl->xinputGameControllers.size() + impl->dinputGameControllers.size() + impl->externalGameControllers.size();
}

bool Win32InputContext::IsGameControllerConnected(size_t index) const
{
    if (index < impl->xinputGameControllers.size())
        return impl->xinputGameControllers[index].IsConnected();
    else if (index < impl->xinputGameControllers.size() + impl->dinputGameControllers.size())
        return impl->dinputGameControllers[index - impl->xinputGameControllers.size()].IsConnected();
    else
        return impl->externalGameControllers[index - impl->xinputGameControllers.size() - impl->dinputGameControllers.size()]->IsConnected();
}

const Utf8String& Win32InputContext::GetGameControllerProductDescriptor(size_t index) const
{
    if (index < impl->xinputGameControllers.size())
        return impl->xinputGameControllers[index].GetProductDescriptor();
    else if (index < impl->xinputGameControllers.size() + impl->dinputGameControllers.size())
        return impl->dinputGameControllerConfigs[index - impl->xinputGameControllers.size()].productDescriptor;
    else
        return impl->externalGameControllers[index - impl->xinputGameControllers.size() - impl->dinputGameControllers.size()]->GetProductDescriptor();
}

const Utf8String& Win32InputContext::GetGameControllerInstanceDescriptor(size_t index) const
{
    if (index < impl->xinputGameControllers.size())
        return impl->xinputGameControllers[index].GetInstanceDescriptor();
    else if (index < impl->xinputGameControllers.size() + impl->dinputGameControllers.size())
        return impl->dinputGameControllerConfigs[index - impl->xinputGameControllers.size()].instanceDescriptor;
    else
        return impl->externalGameControllers[index - impl->xinputGameControllers.size() - impl->dinputGameControllers.size()]->GetInstanceDescriptor();
}

InputDeviceSemantic Win32InputContext::GetGameControllerSemantic(size_t index) const
{
    if (index < impl->xinputGameControllers.size())
        return impl->xinputGameControllers[index].GetSemantic();
    else if (index < impl->xinputGameControllers.size() + impl->dinputGameControllers.size())
        return impl->dinputGameControllers[index - impl->xinputGameControllers.size()].GetSemantic();
    else
        return impl->externalGameControllers[index - impl->xinputGameControllers.size() - impl->dinputGameControllers.size()]->GetSemantic();
}

XInputGameController* Win32InputContext::GetXInputGameController(size_t index)
{
    return &impl->xinputGameControllers[index];
}

size_t Win32InputContext::GetDInputGameControllerCount() const
{
    return impl->dinputGameControllers.size();
}

DInputGameController* Win32InputContext::GetDInputGameController(size_t index)
{
    return &impl->dinputGameControllers[index];
}

void Win32InputContext::AddExternalGameController(InputGenericGameController* gameController, bool configure, Error& error)
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

void Win32InputContext::RemoveExternalGameController(InputGenericGameController* gameController)
{
    auto foundAt = impl->externalGameControllers.find(gameController);
    if (foundAt != impl->externalGameControllers.end())
        impl->externalGameControllers.erase(foundAt);
}

size_t Win32InputContext::GetExternalGameControllerCount() const
{
    return impl->externalGameControllers.size();
}

InputGenericGameController* Win32InputContext::GetExternalGameController(size_t index)
{
    return impl->externalGameControllers[index];
}

size_t Win32InputContext::GetMouseCount() const
{
    return impl->mice.size();
}

DInputMouse* Win32InputContext::GetMouse(size_t index)
{
    return &impl->mice[index];
}

bool Win32InputContext::IsMouseConnected(size_t index) const
{
    return impl->mice[index].IsConnected();
}

const Utf8String& Win32InputContext::GetMouseProductDescriptor(size_t index) const
{
    return impl->mouseFoundConfigs[index].productDescriptor;
}

const Utf8String& Win32InputContext::GetMouseInstanceDescriptor(size_t index) const
{
    return impl->mouseFoundConfigs[index].instanceDescriptor;
}

InputDeviceSemantic Win32InputContext::GetMouseSemantic(size_t index) const
{
    return impl->mice[index].GetSemantic();
}

size_t Win32InputContext::GetKeyboardCount() const
{
    return impl->keyboards.size();
}

DInputKeyboard* Win32InputContext::GetKeyboard(size_t index)
{
    return &impl->keyboards[index];
}

bool Win32InputContext::IsKeyboardConnected(size_t index) const
{
    return impl->keyboards[index].IsConnected();
}

const Utf8String& Win32InputContext::GetKeyboardProductDescriptor(size_t index) const
{
    return impl->keyboardFoundConfigs[index].productDescriptor;
}

const Utf8String& Win32InputContext::GetKeyboardInstanceDescriptor(size_t index) const
{
    return impl->keyboardFoundConfigs[index].instanceDescriptor;
}

InputDeviceSemantic Win32InputContext::GetKeyboardSemantic(size_t index) const
{
    return impl->keyboards[index].GetSemantic();
}

void Win32InputContext::AddExternalHeadset(InputGenericHeadset* headset, bool configure, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    if (impl->externalHeadsets.push_back(headset).HasErrorOrValue(false))
    {
        FINJIN_SET_ERROR(error, "Failed to add external headset.");
        return;
    }
}

void Win32InputContext::RemoveExternalHeadset(InputGenericHeadset* headset)
{
    auto foundAt = impl->externalHeadsets.find(headset);
    if (foundAt != impl->externalHeadsets.end())
        impl->externalHeadsets.erase(foundAt);
}

size_t Win32InputContext::GetExternalHeadsetCount() const
{
    return impl->externalHeadsets.size();
}

InputGenericHeadset* Win32InputContext::GetExternalHeadset(size_t index)
{
    return impl->externalHeadsets[index];
}

FileOperationResult Win32InputContext::ReadInputBinding(InputDeviceClass deviceClass, size_t deviceIndex, const AssetReference& configAssetRef, ByteBuffer& fileBuffer)
{
    impl->inputBindingsAssetReader.GetSelector().Set(AssetPathComponent::INPUT_DEVICE_TYPE, InputDeviceClassUtilities::ToString(deviceClass));

    switch (deviceClass)
    {
        case InputDeviceClass::KEYBOARD: //Fall through
        case InputDeviceClass::MOUSE: impl->inputBindingsAssetReader.GetSelector().Set(AssetPathComponent::INPUT_API, DINPUT_SYSTEM_INTERNAL_NAME); break;
        case InputDeviceClass::GAME_CONTROLLER:
        {
            if (deviceIndex < impl->xinputGameControllers.size())
                impl->inputBindingsAssetReader.GetSelector().Set(AssetPathComponent::INPUT_API, XInputSystem::GetSystemInternalName());
            else if (deviceIndex < impl->xinputGameControllers.size() + impl->dinputGameControllers.size())
                impl->inputBindingsAssetReader.GetSelector().Set(AssetPathComponent::INPUT_API, DINPUT_SYSTEM_INTERNAL_NAME);
            else if (deviceIndex < impl->xinputGameControllers.size() + impl->dinputGameControllers.size() + impl->externalGameControllers.size())
            {
                auto gameController = impl->externalGameControllers[deviceIndex - impl->xinputGameControllers.size() - impl->dinputGameControllers.size()];
                impl->inputBindingsAssetReader.GetSelector().Set(AssetPathComponent::INPUT_API, gameController->GetSystemInternalName());
            }

            break;
        }        
        case InputDeviceClass::HEADSET:
        {
            auto headset = impl->externalHeadsets[deviceIndex];
            impl->inputBindingsAssetReader.GetSelector().Set(AssetPathComponent::INPUT_API, headset->GetSystemInternalName());
            break;
        }
    }
    
    impl->inputBindingsAssetReader.GetSelector().Set(AssetPathComponent::INPUT_DEVICE_DESCRIPTOR, GetDeviceProductDescriptor(deviceClass, deviceIndex));

    return impl->inputBindingsAssetReader.ReadAsset(fileBuffer, configAssetRef);
}

Win32InputContext::Impl* Win32InputContext::GetImpl()
{
    return impl.get();
}
