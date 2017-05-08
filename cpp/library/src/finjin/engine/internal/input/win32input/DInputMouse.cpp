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
#include "DInputMouse.hpp"
#include "finjin/common/Math.hpp"
#include "finjin/engine/OSWindow.hpp"
#include "DInputDevice.hpp"
#include "Win32InputContext.hpp"

using DInputMouseState = DIMOUSESTATE2;
using namespace Finjin::Engine;


//Local types-------------------------------------------------------------------
struct DInputMouse::Impl : public DInputDevice
{
    DInputDeviceState<DInputMouseState, DInputButton, DInputAxis, DInputPov, MouseConstants::MAX_BUTTON_COUNT, MouseConstants::MAX_AXIS_COUNT> state;
};


//Local functions---------------------------------------------------------------
static BOOL CALLBACK EnumComponentsCallback(const DIDEVICEOBJECTINSTANCE* did, void* data)
{
    auto mouse = static_cast<DInputMouse*>(data);
    auto impl = mouse->GetImpl();

    if (did->dwType & DIDFT_BUTTON)
    {
        if (impl->state.buttons.full())
            return DIENUM_CONTINUE; //Skip

        auto buttonIndex = impl->state.buttons.size();

        auto inputSemantic = InputComponentSemantic::NONE;
        switch (impl->state.buttons.size())
        {
            case 0: inputSemantic = InputComponentSemantic::ACCEPT; break;
            case 1: inputSemantic = InputComponentSemantic::CANCEL; break;
            case 2: inputSemantic = InputComponentSemantic::SETTINGS; break;
            case 3: inputSemantic = InputComponentSemantic::SHIFT_LEFT; break;
            case 4: inputSemantic = InputComponentSemantic::SHIFT_RIGHT; break;
        }

        DInputButton button;
        button
            .SetIndex(buttonIndex)
            .SetCode(DIMOFS_BUTTON0 + static_cast<int>(buttonIndex))
            .SetSemantic(inputSemantic)
            .SetDisplayName(did->tszName);
        button.dinputOffset = DIMOFS_BUTTON0 + buttonIndex;
        impl->state.buttons.push_back(button);
    }
    else if (did->dwType & DIDFT_AXIS)
    {
        if (impl->state.axes.full())
            return DIENUM_CONTINUE; //Skip

        auto inputSemantic = InputComponentSemantic::NONE;
        auto inputProcessing = InputAxisProcessing::NONE;

        int inputCode = InputSource::NO_COMPONENT_CODE;

        size_t dinputOffset = 0;
        if (IsEqualGUID(did->guidType, GUID_XAxis))
        {
            dinputOffset = DIMOFS_X;
            inputSemantic = InputComponentSemantic::MOVE_LEFT_RIGHT;
            inputProcessing = InputAxisProcessing::IS_ABSOLUTE;
            inputCode = DIMOFS_X;
        }
        else if (IsEqualGUID(did->guidType, GUID_YAxis))
        {
            dinputOffset = DIMOFS_Y;
            inputSemantic = InputComponentSemantic::MOVE_UP_DOWN;
            inputProcessing = InputAxisProcessing::IS_ABSOLUTE;
            inputCode = DIMOFS_Y;
        }
        else if (IsEqualGUID(did->guidType, GUID_ZAxis))
        {
            dinputOffset = DIMOFS_Z;
            inputSemantic = InputComponentSemantic::TOGGLE_UP_DOWN;
            inputCode = DIMOFS_Z;
        }
        else
            return DIENUM_CONTINUE; //Skip

        //Set all axes to relative
        if (FAILED(impl->SetPropertyDWord(DIPROP_AXISMODE, 0, DIPH_DEVICE, DIPROPAXISMODE_REL)))
            return DIENUM_CONTINUE; //Skip

        auto axisIndex = impl->state.axes.size();

        DInputAxis axis;
        axis
            .SetIndex(axisIndex)
            .SetCode(inputCode)
            .SetSemantic(inputSemantic)
            .SetProcessing(inputProcessing)
            .SetDisplayName(did->tszName);
        axis.dinputOffset = dinputOffset;
        impl->state.axes.push_back(axis);
    }

    return DIENUM_CONTINUE;
}

static OSWindowPosition GetMousePosition(OSWindow* osWindow)
{
    OSWindowPosition result;

    POINT cursorPosition = { 0, 0 };
    if (GetCursorPos(&cursorPosition))
    {
        if (osWindow != nullptr)
        {
            auto windowRect = osWindow->GetRect();
            result.x = cursorPosition.x - windowRect.x;
            result.y = cursorPosition.y - windowRect.y;
        }
        else
        {
            result.x = cursorPosition.x;
            result.y = cursorPosition.y;
        }
    }

    return result;
}


//Implementation----------------------------------------------------------------
DInputMouse::DInputMouse() : impl(new Impl)
{
}

DInputMouse::~DInputMouse()
{
}

void DInputMouse::Create(Win32InputContext* context, const DInputDeviceConfiguration& config, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    //Create the input device
    impl->Create(context, config, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create DirectInput mouse '%1%'", config.GetDebugName()));
        return;
    }

    //Set device cooperative level
    if (FAILED(impl->dinputDevice->SetCooperativeLevel(static_cast<HWND>(context->GetSettings().osWindow->GetWindowHandle()), DISCL_NONEXCLUSIVE | DISCL_FOREGROUND)))
    {
        Destroy();

        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to set DirectInput cooperative level for mouse '%1%'", config.GetDebugName()));
        return;
    }

    //Set data format for mouse using DirectInput constants
    if (FAILED(impl->dinputDevice->SetDataFormat(&c_dfDIMouse2)))
    {
        Destroy();

        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to set DirectInput data format for mouse '%1%'", config.GetDebugName()));
        return;
    }

    //Enumerate and components of the device
    impl->dinputDevice->EnumObjects(EnumComponentsCallback, this, DIDFT_ALL);
}

void DInputMouse::Destroy()
{
    impl->Destroy();
}

void DInputMouse::Update(SimpleTimeDelta elapsedTime, bool isFirstUpdate)
{
    DInputMouseState dinputState;
    auto gotState = impl->state.GetState(impl->dinputDevice, dinputState);
    if (gotState)
    {
        auto mousePosition = GetMousePosition(impl->context->GetSettings().osWindow);
        auto stateBytes = reinterpret_cast<uint8_t*>(&dinputState);
        for (auto& axis : impl->state.axes)
        {
            switch (axis.GetSemantic())
            {
                case InputComponentSemantic::MOVE_LEFT_RIGHT: axis.Update(RoundToFloat(mousePosition.x), isFirstUpdate); break;
                case InputComponentSemantic::MOVE_UP_DOWN: axis.Update(RoundToFloat(mousePosition.y), isFirstUpdate); break;
                case InputComponentSemantic::TOGGLE_UP_DOWN: axis.Update(RoundToFloat(*reinterpret_cast<LONG*>(stateBytes + axis.dinputOffset)) / (float)WHEEL_DELTA * MouseConstants::WHEEL_STEP_SIZE, isFirstUpdate); break;
                default: axis.Update(RoundToFloat(*reinterpret_cast<LONG*>(stateBytes + axis.dinputOffset)), isFirstUpdate); break;
            }
        }
    }

    impl->state.Update(dinputState, gotState, InputDeviceComponentClass::BUTTON, isFirstUpdate);
}

void DInputMouse::ClearChanged()
{
    impl->state.ClearChanged();
}

bool DInputMouse::IsConnected() const
{
    return impl->state.isConnected;
}

const Utf8String& DInputMouse::GetDisplayName() const
{
    return impl->displayName;
}

void DInputMouse::SetDisplayName(const Utf8String& value)
{
    impl->displayName = value;
}

InputDeviceSemantic DInputMouse::GetSemantic() const
{
    return impl->semantic;
}

void DInputMouse::SetSemantic(InputDeviceSemantic value)
{
    impl->semantic = value;
}

const Utf8String& DInputMouse::GetProductDescriptor() const
{
    return impl->config.productDescriptor;
}

const Utf8String& DInputMouse::GetInstanceDescriptor() const
{
    return impl->config.instanceDescriptor;
}

size_t DInputMouse::GetAxisCount() const
{
    return impl->state.axes.size();
}

DInputAxis* DInputMouse::GetAxis(size_t axisIndex)
{
    return &impl->state.axes[axisIndex];
}

size_t DInputMouse::GetButtonCount() const
{
    return impl->state.buttons.size();
}

DInputButton* DInputMouse::GetButton(size_t buttonIndex)
{
    return &impl->state.buttons[buttonIndex];
}

size_t DInputMouse::GetPovCount() const
{
    return 0;
}

DInputPov* DInputMouse::GetPov(size_t povIndex)
{
    return nullptr;
}

size_t DInputMouse::GetLocatorCount() const
{
    return 0;
}

InputLocator* DInputMouse::GetLocator(size_t index)
{
    return nullptr;
}

DInputMouse::Impl* DInputMouse::GetImpl()
{
    return impl.get();
}
