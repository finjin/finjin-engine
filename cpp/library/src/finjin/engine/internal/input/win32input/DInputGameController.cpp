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
#include "DInputGameController.hpp"
#include "finjin/common/ConfigDocumentReader.hpp"
#include "finjin/common/Math.hpp"
#include "finjin/engine/InputDeviceSerializer.hpp"
#include "finjin/engine/OSWindow.hpp"
#include "DInputDevice.hpp"
#include "Win32InputContext.hpp"

using namespace Finjin::Engine;


//Macros------------------------------------------------------------------------
#define DINPUT_AXIS_MIN -32767
#define DINPUT_AXIS_MAX 32767

#define CUSTOM_DEAD_ZONE 8689 //Same as XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE


//Local types-------------------------------------------------------------------
struct DInputGameController::Impl : public DInputDevice
{
    Impl()
    {
        this->forceEffect = nullptr;
        this->forceFeedbackAxisCount = 0;

        this->sliderCount = 0;
    }

    void Destroy()
    {
        FINJIN_SAFE_RELEASE(this->forceEffect);

        DInputDevice::Destroy();
    }

    DInputDeviceState<DIJOYSTATE2, DInputButton, DInputAxis, DInputPov, GameControllerConstants::MAX_BUTTON_COUNT, GameControllerConstants::MAX_AXIS_COUNT, GameControllerConstants::MAX_POV_COUNT> state;

    size_t sliderCount;

    //Force feedback
    LPDIRECTINPUTEFFECT forceEffect;
    size_t forceFeedbackAxisCount;
    HapticFeedback forceFeedback;
};

//Local functions---------------------------------------------------------------
static BOOL CALLBACK EnumComponentsCallback(const DIDEVICEOBJECTINSTANCE* did, void* data)
{
    auto gameController = static_cast<DInputGameController*>(data);
    auto impl = gameController->GetImpl();

    if (did->dwType & DIDFT_BUTTON)
    {
        if (impl->state.buttons.full())
            return DIENUM_CONTINUE; //Skip

        auto buttonIndex = impl->state.buttons.size();

        DInputButton button;
        button
            .SetIndex(buttonIndex)
            .SetDisplayName(did->tszName);
        button.dinputOffset = DIJOFS_BUTTON(buttonIndex);
        impl->state.buttons.push_back(button);
    }
    else if (did->dwType & DIDFT_POV)
    {
        if (impl->state.povs.full())
            return DIENUM_CONTINUE; //Skip

        auto povIndex = impl->state.povs.size();

        auto inputSemantic = povIndex == 0 ? InputComponentSemantic::TOGGLE_ALL : InputComponentSemantic::NONE;

        DInputPov pov;
        pov
            .SetIndex(povIndex)
            .SetSemantic(inputSemantic)
            .SetDisplayName(did->tszName);
        pov.dinputOffset = DIJOFS_POV(povIndex);
        impl->state.povs.push_back(pov);
    }
    else if (did->dwType & DIDFT_AXIS)
    {
        if (impl->state.axes.full())
            return DIENUM_CONTINUE; //Skip

        auto inputSemantic = InputComponentSemantic::NONE;

        auto axisIndex = impl->state.axes.size();

        int axisMin = DINPUT_AXIS_MIN;
        int axisMax = DINPUT_AXIS_MAX;

        int inputCode = InputSource::NO_COMPONENT_CODE;

        size_t dinputOffset = 0;
        if (IsEqualGUID(did->guidType, GUID_XAxis))
        {
            dinputOffset = DIJOFS_X;
            inputSemantic = InputComponentSemantic::MOVE_LEFT_RIGHT;
            inputCode = DIJOFS_X;
        }
        else if (IsEqualGUID(did->guidType, GUID_YAxis))
        {
            dinputOffset = DIJOFS_Y;
            inputSemantic = InputComponentSemantic::MOVE_UP_DOWN;
            inputCode = DIJOFS_Y;
        }
        else if (IsEqualGUID(did->guidType, GUID_ZAxis))
        {
            dinputOffset = DIJOFS_Z;
        }
        else if (IsEqualGUID(did->guidType, GUID_RxAxis))
            dinputOffset = DIJOFS_RX;
        else if (IsEqualGUID(did->guidType, GUID_RyAxis))
            dinputOffset = DIJOFS_RY;
        else if (IsEqualGUID(did->guidType, GUID_RzAxis))
            dinputOffset = DIJOFS_RZ;
        else if (IsEqualGUID(did->guidType, GUID_Slider))
        {
            axisMin = 0;
            dinputOffset = DIJOFS_SLIDER(impl->sliderCount++);
        }

        if (FAILED(impl->SetPropertyDWord(DIPROP_DEADZONE, did->dwType, DIPH_BYID, 0)))
            return DIENUM_CONTINUE; //Skip

        if (FAILED(impl->SetPropertyRange(DIPROP_RANGE, did->dwType, DIPH_BYID, axisMin, axisMax)))
            return DIENUM_CONTINUE; //Skip

        DInputAxis axis;
        axis
            .SetIndex(axisIndex)
            .SetCode(inputCode)
            .SetSemantic(inputSemantic)
            .SetProcessing(InputAxisProcessing::NORMALIZE)
            .SetMinMax(RoundToFloat(axisMin), RoundToFloat(axisMax))
            .SetDeadZone(CUSTOM_DEAD_ZONE)
            .SetDisplayName(did->tszName);
        axis.dinputOffset = dinputOffset;
        impl->state.axes.push_back(axis);
    }
    else if (did->dwType & DIDFT_FFACTUATOR)
    {
        impl->forceFeedbackAxisCount++;
    }

    return DIENUM_CONTINUE;
}


//Implementation----------------------------------------------------------------
DInputGameController::DInputGameController() : impl(new Impl)
{
}

DInputGameController::~DInputGameController()
{
}

void DInputGameController::Create(Win32InputContext* context, const DInputDeviceConfiguration& config, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    //Create the input device
    impl->Create(context, config, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create DirectInput game controller '%1%'", config.GetDebugName()));
        return;
    }

    //Set game controller cooperative level
    if (FAILED(impl->dinputDevice->SetCooperativeLevel(static_cast<HWND>(context->GetSettings().osWindow->GetWindowHandle()), DISCL_EXCLUSIVE | DISCL_FOREGROUND)))
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to set DirectInput cooperative level for game controller '%1%'", config.GetDebugName()));
        return;
    }

    //Set data format for game controller using DirectInput constant
    if (FAILED(impl->dinputDevice->SetDataFormat(&c_dfDIJoystick2)))
    {
        Destroy();

        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to set DirectInput data format for game controller '%1%'", config.GetDebugName()));
        return;
    }

    //Enumerate and components of the device
    impl->dinputDevice->EnumObjects(EnumComponentsCallback, this, DIDFT_ALL);
    if (impl->state.axes.empty() && !impl->state.povs.empty())
    {
        //There are no axes but there is at least one POV
        //By default the POV was assigned to "toggle" but if there aren't any axes it makes more sense to make it a "move"
        impl->state.povs[0].SetSemantic(InputComponentSemantic::MOVE_ALL);
    }

    if (impl->forceFeedbackAxisCount > 0)
    {
        impl->dinputDevice->SendForceFeedbackCommand(DISFFC_RESET);

        //Turn on auto centering
        impl->SetPropertyDWord(DIPROP_AUTOCENTER, 0, DIPH_DEVICE, FALSE);

        //No more than 2 feedback axes
        impl->forceFeedbackAxisCount = std::min(impl->forceFeedbackAxisCount, (size_t)2);

        //This application needs only one effect: Applying raw forces.
        DWORD rgdwAxes[2] = {DIJOFS_X, DIJOFS_Y};
        LONG rglDirection[2] = {0, 0};
        DICONSTANTFORCE constantForce = {0};

        DIEFFECT effectSettings = {};
        effectSettings.dwSize = sizeof(DIEFFECT);
        effectSettings.dwFlags = DIEFF_CARTESIAN | DIEFF_OBJECTOFFSETS;
        effectSettings.dwDuration = INFINITE;
        effectSettings.dwSamplePeriod = 0;
        effectSettings.dwGain = DI_FFNOMINALMAX;
        effectSettings.dwTriggerButton = DIEB_NOTRIGGER;
        effectSettings.dwTriggerRepeatInterval = 0;
        effectSettings.cAxes = static_cast<DWORD>(impl->forceFeedbackAxisCount);
        effectSettings.rgdwAxes = rgdwAxes;
        effectSettings.rglDirection = rglDirection;
        effectSettings.lpEnvelope = 0;
        effectSettings.cbTypeSpecificParams = sizeof(DICONSTANTFORCE);
        effectSettings.lpvTypeSpecificParams = &constantForce;
        effectSettings.dwStartDelay = 0;

        //Create the prepared effect
        if (FAILED(impl->dinputDevice->CreateEffect(GUID_ConstantForce, &effectSettings, &impl->forceEffect, nullptr)))
        {
            //Creation failed. Just disable force feedback
            impl->forceFeedbackAxisCount = 0;
        }
    }
}

void DInputGameController::Destroy()
{
    impl->Destroy();
}

void DInputGameController::Update(SimpleTimeDelta elapsedTime, bool isFirstUpdate)
{
    impl->state.Update(impl->dinputDevice, InputDeviceComponentClass::ALL, isFirstUpdate);

    if (impl->state.isConnected && impl->forceEffect != nullptr)
    {
        if (impl->forceFeedback.IsActive())
        {
            _SetForce(impl->forceFeedback);

            impl->forceFeedback.Update(elapsedTime);
        }
        else
            _StopForce(impl->forceFeedback);
    }
}

void DInputGameController::ClearChanged()
{
    impl->state.ClearChanged();
}

bool DInputGameController::IsConnected() const
{
    return impl->state.isConnected;
}

const Utf8String& DInputGameController::GetDisplayName() const
{
    return impl->displayName;
}

void DInputGameController::SetDisplayName(const Utf8String& value)
{
    impl->displayName = value;
}

InputDeviceSemantic DInputGameController::GetSemantic() const
{
    return impl->semantic;
}

void DInputGameController::SetSemantic(InputDeviceSemantic value)
{
    impl->semantic = value;
}

const Utf8String& DInputGameController::GetProductDescriptor() const
{
    return impl->config.productDescriptor;
}

const Utf8String& DInputGameController::GetInstanceDescriptor() const
{
    return impl->config.instanceDescriptor;
}

size_t DInputGameController::GetAxisCount() const
{
    return impl->state.axes.size();
}

DInputAxis* DInputGameController::GetAxis(size_t axisIndex)
{
    return &impl->state.axes[axisIndex];
}

size_t DInputGameController::GetButtonCount() const
{
    return impl->state.buttons.size();
}

DInputButton* DInputGameController::GetButton(size_t buttonIndex)
{
    return &impl->state.buttons[buttonIndex];
}

size_t DInputGameController::GetPovCount() const
{
    return impl->state.povs.size();
}

DInputPov* DInputGameController::GetPov(size_t povIndex)
{
    return &impl->state.povs[povIndex];
}

size_t DInputGameController::GetLocatorCount() const
{
    return 0;
}

InputLocator* DInputGameController::GetLocator(size_t locatorIndex)
{
    return nullptr;
}

void DInputGameController::AddHapticFeedback(const HapticFeedback* forces, size_t count)
{
    //Modifying an effect is basically the same as creating a new one, except you need only specify the parameters you are modifying

    if (impl->forceEffect == nullptr || forces == nullptr || count == 0)
        return;

    auto& force = forces[0];

    if (force.intensity <= 0 || force.decayRate < 0 || force.duration <= 0)
        return;

    impl->forceFeedback = force;
    _SetForce(impl->forceFeedback);
}

void DInputGameController::StopHapticFeedback()
{
    if (impl->forceEffect != nullptr)
        _StopForce(impl->forceFeedback);
}

DInputGameController::Impl* DInputGameController::GetImpl()
{
    return impl.get();
}

void DInputGameController::_SetForce(HapticFeedback& force)
{
    DICONSTANTFORCE constantForce = {};
    constantForce.lMagnitude = long(force.intensity * DI_FFNOMINALMAX);

    DIEFFECT effectSettings = {};
    effectSettings.dwSize = sizeof(DIEFFECT);
    effectSettings.dwFlags = DIEFF_CARTESIAN | DIEFF_OBJECTOFFSETS;
    effectSettings.cAxes = static_cast<DWORD>(impl->forceFeedbackAxisCount);
    effectSettings.lpEnvelope = 0;
    effectSettings.cbTypeSpecificParams = sizeof(DICONSTANTFORCE);
    effectSettings.lpvTypeSpecificParams = &constantForce;
    effectSettings.dwStartDelay = 0;

    //Now set the new parameters and start the effect immediately.
    impl->forceEffect->SetParameters(&effectSettings, DIEP_TYPESPECIFICPARAMS | DIEP_START);
    impl->forceEffect->Start(1, 0); //Start the effect
}

void DInputGameController::_StopForce(HapticFeedback& force)
{
    impl->dinputDevice->SendForceFeedbackCommand(DISFFC_STOPALL);

    force.Reset();
}
