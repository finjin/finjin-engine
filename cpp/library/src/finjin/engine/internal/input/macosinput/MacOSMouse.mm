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
#include "MacOSMouse.hpp"
#include "finjin/common/Convert.hpp"
#include "MacOSInputContext.hpp"
#include "OSWindow.hpp"
#include <Carbon/Carbon.h>

using namespace Finjin::Engine;


//Implementation----------------------------------------------------------------
MacOSMouse::MacOSMouse()
{
    this->context = nullptr;

    Reset();
}

void MacOSMouse::Reset()
{
    this->semantic = InputDeviceSemantic::NONE;

    this->state.Reset();
}

bool MacOSMouse::Create(MacOSInputContext* context, size_t index)
{
    this->context = context;

    this->instanceName = "Mouse ";
    this->instanceName += Convert::ToString(index + 1);

    this->displayName = "Mouse";
    this->productName = "Mouse";

    this->instanceDescriptor = "generic-mouse-";
    this->instanceDescriptor += Convert::ToString(index);

    this->productDescriptor = "generic-mouse";

    this->state.buttons.resize(8);
    this->state.buttons[0].SetIndex(0).SetSemantic(InputComponentSemantic::ACCEPT).SetProcessing(InputButtonProcessing::EVENT_DRIVEN).SetDisplayName("Left mouse button");
    this->state.buttons[1].SetIndex(1).SetSemantic(InputComponentSemantic::CANCEL).SetProcessing(InputButtonProcessing::EVENT_DRIVEN).SetDisplayName("Right mouse button");
    this->state.buttons[2].SetIndex(2).SetSemantic(InputComponentSemantic::SETTINGS).SetProcessing(InputButtonProcessing::EVENT_DRIVEN).SetDisplayName("Middle mouse button");
    this->state.buttons[3].SetIndex(3).SetSemantic(InputComponentSemantic::SHIFT_LEFT).SetProcessing(InputButtonProcessing::EVENT_DRIVEN).SetDisplayName("Mouse button 4");
    this->state.buttons[4].SetIndex(4).SetSemantic(InputComponentSemantic::SHIFT_RIGHT).SetProcessing(InputButtonProcessing::EVENT_DRIVEN).SetDisplayName("Mouse button 5");
    Utf8String buttonName;
    for (size_t i = 5; i < this->state.buttons.size(); i++)
    {
        buttonName = "Mouse button ";
        buttonName += Convert::ToString(i + 1);
        this->state.buttons[i].SetIndex(i).SetDisplayName(displayName);
    }

    this->state.axes.resize(3);
    this->state.axes[0].SetIndex(0).SetSemantic(InputComponentSemantic::MOVE_LEFT_RIGHT).SetProcessing(InputAxisProcessing::IS_ABSOLUTE).SetDisplayName("X");
    this->state.axes[1].SetIndex(1).SetSemantic(InputComponentSemantic::MOVE_UP_DOWN).SetProcessing(InputAxisProcessing::IS_ABSOLUTE).SetDisplayName("Y");
    this->state.axes[2].SetIndex(2).SetSemantic(InputComponentSemantic::TOGGLE_UP_DOWN).SetProcessing(InputAxisProcessing::REST_ON_CLEAR_CHANGES | InputAxisProcessing::ACCUMULATE_MULTIPLE_CHANGES).SetDisplayName("Wheel");

    return true;
}

void MacOSMouse::Destroy()
{
    this->state.Reset();
}

void MacOSMouse::Update(SimpleTimeDelta elapsedTime, bool isFirstUpdate)
{
}

void MacOSMouse::ClearChanged()
{
    //Buttons
    auto buttonCount = GetButtonCount();
    for (size_t buttonIndex = 0; buttonIndex < buttonCount; buttonIndex++)
    {
        auto button = GetButton(buttonIndex);
        button->ClearChanged();
    }

    //Axes
    auto axisCount = GetAxisCount();
    for (size_t axisIndex = 0; axisIndex < axisCount; axisIndex++)
    {
        auto axis = GetAxis(axisIndex);
        axis->ClearChanged();
    }
}

bool MacOSMouse::IsConnected() const
{
    return true;
}

bool MacOSMouse::GetConnectionChanged() const
{
    return false;
}

const Utf8String& MacOSMouse::GetDisplayName() const
{
    return this->displayName;
}

void MacOSMouse::SetDisplayName(const Utf8String& value)
{
    this->displayName = value;
}

InputDeviceSemantic MacOSMouse::GetSemantic() const
{
    return this->semantic;
}

void MacOSMouse::SetSemantic(InputDeviceSemantic value)
{
    this->semantic = value;
}

const Utf8String& MacOSMouse::GetProductDescriptor() const
{
    return this->productDescriptor;
}

const Utf8String& MacOSMouse::GetInstanceDescriptor() const
{
    return this->instanceDescriptor;
}

size_t MacOSMouse::GetButtonCount() const
{
    return this->state.buttons.size();
}

InputButton* MacOSMouse::GetButton(size_t index)
{
    return &this->state.buttons[index];
}

size_t MacOSMouse::GetAxisCount() const
{
    return this->state.axes.size();
}

InputAxis* MacOSMouse::GetAxis(size_t index)
{
    return &this->state.axes[index];
}

size_t MacOSMouse::GetPovCount() const
{
    return 0;
}

InputPov* MacOSMouse::GetPov(size_t index)
{
    return nullptr;
}

size_t MacOSMouse::GetLocatorCount() const
{
    return 0;
}

InputLocator* MacOSMouse::GetLocator(size_t locatorIndex)
{
    return nullptr;
}
