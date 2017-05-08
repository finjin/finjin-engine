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
#include "DInputKeyboard.hpp"
#include "finjin/engine/OSWindow.hpp"
#include "DInputDevice.hpp"
#include "Win32InputContext.hpp"

using namespace Finjin::Engine;


//Local types-------------------------------------------------------------------
struct DInputKeyboard::Impl : public DInputDevice
{
    enum { MAX_DINPUT_BUTTON_COUNT = 256 }; //Should be <= KeyboardConstants::MAX_BUTTON_COUNT
    struct KeyState
    {
        unsigned char keys[MAX_DINPUT_BUTTON_COUNT];
    };
    DInputDeviceState<KeyState, DInputButton, DInputAxis, DInputPov, KeyboardConstants::MAX_BUTTON_COUNT> state;
};


//Local functions---------------------------------------------------------------
static BOOL CALLBACK EnumComponentsCallback(const DIDEVICEOBJECTINSTANCE* did, void* data)
{
    auto keyboard = static_cast<DInputKeyboard*>(data);

    auto impl = keyboard->GetImpl();

    if (did->dwType & DIDFT_BUTTON)
    {
        if (impl->state.buttons.full())
            return DIENUM_STOP; //Done

        auto buttonIndex = impl->state.buttons.size();
        auto buttonCode = did->dwOfs;

        auto inputSemantic = InputComponentSemantic::NONE;

        switch (buttonCode)
        {
            case DIK_UP: inputSemantic = InputComponentSemantic::TOGGLE_UP; break;
            case DIK_LEFT: inputSemantic = InputComponentSemantic::TOGGLE_LEFT; break;
            case DIK_DOWN: inputSemantic = InputComponentSemantic::TOGGLE_DOWN; break;
            case DIK_RIGHT: inputSemantic = InputComponentSemantic::TOGGLE_RIGHT; break;
            case DIK_RETURN: inputSemantic = InputComponentSemantic::ACCEPT; break;
            case DIK_ESCAPE: inputSemantic = InputComponentSemantic::CANCEL; break;
            case DIK_F1: inputSemantic = InputComponentSemantic::SETTINGS; break;
        }

        DInputButton button;
        button
            .SetIndex(buttonIndex)
            .SetCode(buttonCode)
            .SetSemantic(inputSemantic)
            .SetDisplayName(did->tszName);

        button.dinputOffset = did->dwOfs;
        impl->state.buttons.push_back(button);
    }

    return DIENUM_CONTINUE;
}


//Implementation----------------------------------------------------------------
DInputKeyboard::DInputKeyboard() : impl(new Impl)
{
}

DInputKeyboard::~DInputKeyboard()
{
}

void DInputKeyboard::Create(Win32InputContext* context, const DInputDeviceConfiguration& config, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    //Create the input device
    impl->Create(context, config, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create DirectInput keyboard '%1%'", config.GetDebugName()));
        return;
    }

    //Set device cooperative level
    if (FAILED(impl->dinputDevice->SetCooperativeLevel(static_cast<HWND>(context->GetSettings().osWindow->GetWindowHandle()), DISCL_EXCLUSIVE | DISCL_FOREGROUND | DISCL_NOWINKEY)))
    {
        Destroy();

        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to set DirectInput cooperative level for keyboard '%1%'", config.GetDebugName()));
        return;
    }

    //Set data format for keyboard using DirectInput constant
    if (FAILED(impl->dinputDevice->SetDataFormat(&c_dfDIKeyboard)))
    {
        Destroy();

        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to set DirectInput data format for keyboard '%1%'", config.GetDebugName()));
        return;
    }

    impl->dinputDevice->EnumObjects(EnumComponentsCallback, this, DIDFT_ALL);
}

void DInputKeyboard::Destroy()
{
    impl->Destroy();
}

void DInputKeyboard::Update(SimpleTimeDelta elapsedTime, bool isFirstUpdate)
{
    impl->state.Update(impl->dinputDevice, InputDeviceComponentClass::BUTTON, isFirstUpdate);
}

void DInputKeyboard::ClearChanged()
{
    impl->state.ClearChanged();
}

bool DInputKeyboard::IsConnected() const
{
    return impl->state.isConnected;
}

const Utf8String& DInputKeyboard::GetDisplayName() const
{
    return impl->displayName;
}

void DInputKeyboard::SetDisplayName(const Utf8String& value)
{
    impl->displayName = value;
}

InputDeviceSemantic DInputKeyboard::GetSemantic() const
{
    return impl->semantic;
}

void DInputKeyboard::SetSemantic(InputDeviceSemantic value)
{
    impl->semantic = value;
}

const Utf8String& DInputKeyboard::GetProductDescriptor() const
{
    return impl->config.productDescriptor;
}

const Utf8String& DInputKeyboard::GetInstanceDescriptor() const
{
    return impl->config.instanceDescriptor;
}

size_t DInputKeyboard::GetAxisCount() const
{
    return 0;
}

DInputAxis* DInputKeyboard::GetAxis(size_t axisIndex)
{
    return nullptr;
}

size_t DInputKeyboard::GetButtonCount() const
{
    return impl->state.buttons.size();
}

DInputButton* DInputKeyboard::GetButton(size_t keyIndex)
{
    return &impl->state.buttons[keyIndex];
}

size_t DInputKeyboard::GetPovCount() const
{
    return 0;
}

DInputPov* DInputKeyboard::GetPov(size_t povIndex)
{
    return nullptr;
}

size_t DInputKeyboard::GetLocatorCount() const
{
    return 0;
}

InputLocator* DInputKeyboard::GetLocator(size_t index)
{
    return nullptr;
}

DInputKeyboard::Impl* DInputKeyboard::GetImpl()
{
    return impl.get();
}
