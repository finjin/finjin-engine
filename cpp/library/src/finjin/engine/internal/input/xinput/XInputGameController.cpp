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
#include "XInputGameController.hpp"
#include "finjin/common/Convert.hpp"
#include "finjin/common/Math.hpp"
#include "finjin/engine/InputComponents.hpp"
#include <Windows.h>
#include <Xinput.h>

#define XINPUT_AXIS_MAGNITUDE 32767
#define XINPUT_TRIGGER_MAGNITUDE 255

using namespace Finjin::Engine;


//Local functions--------------------------------------------------------------
static size_t XInputButtonToButtonIndex(WORD xinputButtonCode)
{
    size_t buttonIndex = 0;

    for (xinputButtonCode >>= 1; xinputButtonCode != 0; xinputButtonCode >>= 1)
        buttonIndex++;

    return buttonIndex;
}

template <size_t count>
void SetXInputButton(StaticVector<InputButton, count>& buttons, WORD xinputButton, const char* displayName, InputComponentSemantic semantic)
{
    auto buttonIndex = XInputButtonToButtonIndex(xinputButton);    
    assert(buttonIndex < buttons.size());
    buttons[buttonIndex]
        .SetIndex(buttonIndex)
        .SetCode(xinputButton)
        .SetDisplayName(displayName)
        .SetSemantic(semantic)
        .Enable(true);
}

template <size_t count>
void SetXInputAxis(StaticVector<InputAxis, count>& axes, size_t index, float minValue, float maxValue, float deadZone, const char* displayName, InputComponentSemantic semantic)
{    
    axes[index]
        .SetIndex(index)
        .SetMinMax(minValue, maxValue)
        .SetDeadZone(deadZone)
        .SetDisplayName(displayName)
        .SetSemantic(semantic)
        .SetProcessing(InputAxisProcessing::NORMALIZE);
}

static Utf8String XInputSubtypeToString(BYTE subtype)
{
#if _WIN32_WINNT >= _WIN32_WINNT_WIN8
    switch (subtype)
    {
        case XINPUT_DEVSUBTYPE_GAMEPAD: return "xinput-gamepad";
        case XINPUT_DEVSUBTYPE_WHEEL: return "xinput-wheel";
        case XINPUT_DEVSUBTYPE_ARCADE_STICK: return "xinput-arcade-stick";
        case XINPUT_DEVSUBTYPE_FLIGHT_STICK: return "xinput-flight-stick";
        case XINPUT_DEVSUBTYPE_DANCE_PAD: return "xinput-dance-pad";
        case XINPUT_DEVSUBTYPE_GUITAR: return "xinput-guitar";
        case XINPUT_DEVSUBTYPE_GUITAR_ALTERNATE: return "xinput-guitar-alt";
        case XINPUT_DEVSUBTYPE_DRUM_KIT: return "xinput-drum-kit";
        case XINPUT_DEVSUBTYPE_GUITAR_BASS: return "xinput-guitar-bass";
        case XINPUT_DEVSUBTYPE_ARCADE_PAD: return "xinput-arcade-pad";
        default: return "xinput-gamepad";
    }
#else
    return "xinput-gamepad";
#endif
}


//Local classes----------------------------------------------------------------
struct XInputGameController::Impl
{
    Impl()
    {
        this->index = 0;

        this->semantic = InputDeviceSemantic::NONE;
    }

    size_t index;

    InputDeviceSemantic semantic;

    Utf8String displayName;

    Utf8String instanceName;
    Utf8String productName;

    Utf8String instanceDescriptor;
    Utf8String productDescriptor;

    XINPUT_CAPABILITIES xinputCapabilities;
    XINPUT_STATE xinputState;

    InputDeviceState<InputButton, InputAxis, InputPov, GameControllerConstants::MAX_BUTTON_COUNT, GameControllerConstants::MAX_AXIS_COUNT> state;

    HapticFeedbackSettings forceFeedback[2]; //0 = left motor, 1 = right motor
};


//Implementation---------------------------------------------------------------

//XInputGameController
XInputGameController::XInputGameController() : impl(new Impl)
{
    Reset();
}

XInputGameController::~XInputGameController()
{    
}

void XInputGameController::Reset()
{
    impl->index = 0;
    impl->displayName.clear();
    impl->instanceName.clear();
    impl->productName.clear();
    impl->instanceDescriptor.clear();
    impl->productDescriptor.clear();
    FINJIN_ZERO_ITEM(impl->xinputState);
    impl->state.Reset();
}

bool XInputGameController::Create(size_t index)
{
    impl->index = index;

    impl->instanceName = "XInput Game Controller ";
    impl->instanceName += Convert::ToString(impl->index + 1);

    impl->productName = "XInput Game Controller";
    impl->displayName = impl->productName;

    BYTE subtype = XINPUT_DEVSUBTYPE_GAMEPAD;

    FINJIN_ZERO_ITEM(impl->xinputCapabilities);
    auto capsResult = XInputGetCapabilities(static_cast<DWORD>(impl->index), XINPUT_FLAG_GAMEPAD, &impl->xinputCapabilities);
    if (capsResult == 0)
    {
        subtype = impl->xinputCapabilities.SubType;

        impl->state.isConnected = true;

        XInputGetState(static_cast<DWORD>(impl->index), &impl->xinputState);
    }
    else
    {        
        FINJIN_ZERO_ITEM(impl->xinputState);
    }
    
    impl->productDescriptor = XInputSubtypeToString(subtype);

    impl->instanceDescriptor = impl->productDescriptor;
    impl->instanceDescriptor += "-";
    impl->instanceDescriptor += Convert::ToString(index);

    //Buttons
    impl->state.buttons.resize(sizeof(impl->xinputState.Gamepad.wButtons) * 8);    

    //Button codes are irrengularly defined, so initialize all to disabled, then enable one by one
    for (auto& button : impl->state.buttons) 
        button.Enable(false);

    SetXInputButton(impl->state.buttons, XINPUT_GAMEPAD_A, "A", InputComponentSemantic::ACCEPT);
    SetXInputButton(impl->state.buttons, XINPUT_GAMEPAD_B, "B", InputComponentSemantic::CANCEL);
    SetXInputButton(impl->state.buttons, XINPUT_GAMEPAD_X, "X", InputComponentSemantic::HANDBRAKE);
    SetXInputButton(impl->state.buttons, XINPUT_GAMEPAD_Y, "Y", InputComponentSemantic::CHANGE_VIEW);
    SetXInputButton(impl->state.buttons, XINPUT_GAMEPAD_LEFT_SHOULDER, "Left shoulder button", InputComponentSemantic::SHIFT_LEFT);
    SetXInputButton(impl->state.buttons, XINPUT_GAMEPAD_RIGHT_SHOULDER, "Right shoulder button", InputComponentSemantic::SHIFT_RIGHT);
    SetXInputButton(impl->state.buttons, XINPUT_GAMEPAD_LEFT_THUMB, "Left thumbstick button", InputComponentSemantic::STEER_TOGGLE);
    SetXInputButton(impl->state.buttons, XINPUT_GAMEPAD_RIGHT_THUMB, "Right thumbstick button", InputComponentSemantic::LOOK_TOGGLE);
    SetXInputButton(impl->state.buttons, XINPUT_GAMEPAD_START, "Start", InputComponentSemantic::SETTINGS);
    SetXInputButton(impl->state.buttons, XINPUT_GAMEPAD_BACK, "Back", InputComponentSemantic::SYSTEM_SETTINGS);
    SetXInputButton(impl->state.buttons, XINPUT_GAMEPAD_DPAD_UP, "D-pad up", InputComponentSemantic::TOGGLE_UP);
    SetXInputButton(impl->state.buttons, XINPUT_GAMEPAD_DPAD_DOWN, "D-pad down", InputComponentSemantic::TOGGLE_DOWN);
    SetXInputButton(impl->state.buttons, XINPUT_GAMEPAD_DPAD_LEFT, "D-pad left", InputComponentSemantic::TOGGLE_LEFT);
    SetXInputButton(impl->state.buttons, XINPUT_GAMEPAD_DPAD_RIGHT, "D-pad right", InputComponentSemantic::TOGGLE_RIGHT);
    
    //Axes
    impl->state.axes.resize(6);
    SetXInputAxis(impl->state.axes, 0, -XINPUT_AXIS_MAGNITUDE, XINPUT_AXIS_MAGNITUDE, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE, "Left thumbstick X", InputComponentSemantic::STEER_LEFT_RIGHT);
    SetXInputAxis(impl->state.axes, 1, -XINPUT_AXIS_MAGNITUDE, XINPUT_AXIS_MAGNITUDE, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE, "Left thumbstick Y", InputComponentSemantic::STEER_UP_DOWN);
    SetXInputAxis(impl->state.axes, 2, -XINPUT_AXIS_MAGNITUDE, XINPUT_AXIS_MAGNITUDE, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE, "Right thumbstick X", InputComponentSemantic::LOOK_LEFT_RIGHT);
    SetXInputAxis(impl->state.axes, 3, -XINPUT_AXIS_MAGNITUDE, XINPUT_AXIS_MAGNITUDE, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE, "Right thumbstick Y", InputComponentSemantic::LOOK_UP_DOWN);
    SetXInputAxis(impl->state.axes, 4, 0, XINPUT_TRIGGER_MAGNITUDE, XINPUT_GAMEPAD_TRIGGER_THRESHOLD, "Left trigger", InputComponentSemantic::BRAKE);
    SetXInputAxis(impl->state.axes, 5, 0, XINPUT_TRIGGER_MAGNITUDE, XINPUT_GAMEPAD_TRIGGER_THRESHOLD, "Right trigger", InputComponentSemantic::GAS);

    return true;
}

void XInputGameController::Destroy()
{
    Reset();
}

void XInputGameController::Update(SimpleTimeDelta elapsedTime, bool isFirstUpdate)
{
    impl->state.connectionChanged = false;

    if (impl->state.isConnected)
    {
        if (XInputGetState(static_cast<DWORD>(impl->index), &impl->xinputState) == 0)
        {
            _AcceptUpdate(isFirstUpdate);

            _SetForce();

            for (size_t i = 0; i < 2; i++)
            {
                auto& forceFeedback = impl->forceFeedback[i];

                if (forceFeedback.IsActive())
                {
                    forceFeedback.Update(elapsedTime);
                }
            }
        }
        else
        {
            //Became disconnected
            impl->state.isConnected = false;
            impl->state.connectionChanged = true;

            FINJIN_ZERO_ITEM(impl->xinputState);

            for (auto& button : impl->state.buttons)
                button.Update(false, isFirstUpdate);

            for (auto& axis : impl->state.axes)
                axis.Update(0, isFirstUpdate);

            StopHapticFeedback();
        }
    }
    else
    {
        if (XInputGetCapabilities(static_cast<DWORD>(impl->index), XINPUT_FLAG_GAMEPAD, &impl->xinputCapabilities) == 0 &&
            XInputGetState(static_cast<DWORD>(impl->index), &impl->xinputState) == 0)
        {
            //Became connected
            impl->state.isConnected = true;
            impl->state.connectionChanged = true;

            _AcceptUpdate(isFirstUpdate);
        }
    }
}

void XInputGameController::ClearChanged()
{
    impl->state.ClearChanged();
}

bool XInputGameController::IsConnected() const
{
    return impl->state.isConnected;
}

bool XInputGameController::GetConnectionChanged() const
{
    return impl->state.connectionChanged;
}

size_t XInputGameController::GetIndex() const
{
    return impl->index;
}

const Utf8String& XInputGameController::GetDisplayName() const
{
    return impl->displayName;
}

void XInputGameController::SetDisplayName(const Utf8String& value)
{
    impl->displayName = value;
}

InputDeviceSemantic XInputGameController::GetSemantic() const
{
    return impl->semantic;
}

void XInputGameController::SetSemantic(InputDeviceSemantic value)
{
    impl->semantic = value;
}

const Utf8String& XInputGameController::GetProductDescriptor() const
{
    return impl->productDescriptor;
}

const Utf8String& XInputGameController::GetInstanceDescriptor() const
{
    return impl->instanceDescriptor;
}

void XInputGameController::_AcceptUpdate(bool isFirstUpdate)
{
    //Buttons
    for (WORD i = 0; i < impl->state.buttons.size(); i++)
        impl->state.buttons[i].Update((impl->xinputState.Gamepad.wButtons & (1 << i)) != 0, isFirstUpdate);

    //Axes
    impl->state.axes[0].Update(impl->xinputState.Gamepad.sThumbLX, isFirstUpdate);
    impl->state.axes[1].Update(impl->xinputState.Gamepad.sThumbLY, isFirstUpdate);
    impl->state.axes[2].Update(impl->xinputState.Gamepad.sThumbRX, isFirstUpdate);
    impl->state.axes[3].Update(impl->xinputState.Gamepad.sThumbRY, isFirstUpdate);
    impl->state.axes[4].Update(impl->xinputState.Gamepad.bLeftTrigger, isFirstUpdate);
    impl->state.axes[5].Update(impl->xinputState.Gamepad.bRightTrigger, isFirstUpdate);
}

size_t XInputGameController::GetAxisCount() const
{
    return impl->state.axes.size();
}

InputAxis* XInputGameController::GetAxis(size_t axisIndex)
{
    return &impl->state.axes[axisIndex];
}

size_t XInputGameController::GetButtonCount() const
{
    return impl->state.buttons.size();
}

InputButton* XInputGameController::GetButton(size_t buttonIndex)
{
    return &impl->state.buttons[buttonIndex];
}

size_t XInputGameController::GetPovCount() const
{
    return 0;
}

InputPov* XInputGameController::GetPov(size_t povIndex)
{
    return nullptr;
}

size_t XInputGameController::GetLocatorCount() const
{
    return 0;
}

InputLocator* XInputGameController::GetLocator(size_t locatorIndex)
{
    return nullptr;
}

void XInputGameController::AddHapticFeedback(const HapticFeedbackSettings* forces, size_t count)
{
    for (size_t i = 0; i < count; i++)
    {
        auto& force = forces[i];

        switch (force.motorIndex)
        {
            case 0: impl->forceFeedback[0] = force; break;
            case 1: impl->forceFeedback[1] = force; break;
            case -1: impl->forceFeedback[0] = impl->forceFeedback[1] = force; break;
        }
    }
}

void XInputGameController::StopHapticFeedback()
{
    //Don't pause the game or deactive the window without first stopping rumble otherwise the controller will continue to rumble

    XINPUT_VIBRATION vibration;
    vibration.wLeftMotorSpeed = 0;
    vibration.wRightMotorSpeed = 0;
    XInputSetState(static_cast<DWORD>(impl->index), &vibration);

    impl->forceFeedback[0].Reset();
    impl->forceFeedback[1].Reset();
}

size_t XInputGameController::CreateGameControllers(XInputGameController* gameControllers, size_t gameControllerCount)
{
    gameControllerCount = std::min(gameControllerCount, (size_t)XUSER_MAX_COUNT);
    
    for (size_t i = 0; i < gameControllerCount; i++)
        gameControllers[i].Create(i);
    
    return gameControllerCount;
}

size_t XInputGameController::UpdateGameControllers(SimpleTimeDelta elapsedTime, XInputGameController* gameControllers, size_t gameControllerCount)
{
    for (size_t i = 0; i < gameControllerCount; i++)
        gameControllers[i].Update(elapsedTime, false);

    return gameControllerCount;
}

XInputGameController* XInputGameController::GetGameControllerByInstanceDescriptor(XInputGameController* gameControllers, size_t gameControllerCount, const Utf8String& descriptor)
{
    for (size_t i = 0; i < gameControllerCount; i++)
    {
        if (gameControllers[i].GetInstanceDescriptor() == descriptor)
            return &gameControllers[i];
    }

    return nullptr;
}

XInputGameController* XInputGameController::GetGameControllerByIndex(XInputGameController* gameControllers, size_t gameControllerCount, size_t deviceIndex)
{
    return &gameControllers[deviceIndex];
}

void XInputGameController::_SetForce()
{
    XINPUT_VIBRATION vibration;
    vibration.wLeftMotorSpeed = RoundToInt((impl->forceFeedback[0].duration > 0 ? impl->forceFeedback[0].intensity : 0.0f) * 65535);
    vibration.wRightMotorSpeed = RoundToInt((impl->forceFeedback[1].duration > 0 ? impl->forceFeedback[1].intensity : 0.0f) * 65535);
    XInputSetState(static_cast<DWORD>(impl->index), &vibration);
}
