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
#include "WindowsUWPMouse.hpp"
#include "finjin/common/Convert.hpp"

using namespace Finjin::Engine;
using namespace Finjin::Common;


//Implementation---------------------------------------------------------------
WindowsUWPMouse::WindowsUWPMouse()
{   
}

void WindowsUWPMouse::Reset()
{
    this->state.Reset();
}

bool WindowsUWPMouse::Create(size_t index)
{
    this->instanceName = "Mouse ";
    this->instanceName += Convert::ToString(index + 1);

    this->productName = "Mouse";
    this->displayName = this->productName;

    this->semantic = InputDeviceSemantic::NONE;

    this->instanceDescriptor = "generic-mouse-";
    this->instanceDescriptor += Convert::ToString(index);

    this->productDescriptor = "generic-mouse";

    Utf8String componentName;

    this->state.buttons.resize(8);
    for (size_t i = 0; i < this->state.buttons.size(); i++)
    {
        auto inputSemantic = InputComponentSemantic::NONE;
        
        switch (i)
        {
            case 0: 
            {
                inputSemantic = InputComponentSemantic::ACCEPT;
                componentName = "Left mouse button";
                break;
            }
            case 1: 
            {
                inputSemantic = InputComponentSemantic::CANCEL;
                componentName = "Right mouse button";
                break;
            }
            case 2: 
            {
                inputSemantic = InputComponentSemantic::SETTINGS;
                componentName = "Middle mouse button";
                break;
            }
            case 3:
            {
                inputSemantic = InputComponentSemantic::SHIFT_LEFT; //This is probably a button on the left side of the mouse
                componentName = "Button ";
                componentName += Convert::ToString(i + 1);
                break;
            }
            case 4:
            {
                inputSemantic = InputComponentSemantic::SHIFT_RIGHT; //This is probably a button on the right side of the mouse
                componentName = "Button ";
                componentName += Convert::ToString(i + 1);
                break;
            }
            default: 
            {
                componentName = "Button "; 
                componentName += Convert::ToString(i + 1); 
                break;
            }
        }

        this->state.buttons[i]
            .SetIndex(i)
            .SetSemantic(inputSemantic)
            .SetDisplayName(componentName);
    }

    this->state.axes.resize(3);
    for (size_t i = 0; i < this->state.axes.size(); i++)
    {
        auto inputSemantic = InputComponentSemantic::NONE;
        auto inputProcessing = InputAxisProcessing::NONE;

        switch (i)
        {
            case 0: 
            {
                inputSemantic = InputComponentSemantic::MOVE_LEFT_RIGHT;
                inputProcessing = InputAxisProcessing::IS_ABSOLUTE;
                componentName = "X";
                break;
            }
            case 1:
            {
                inputSemantic = InputComponentSemantic::MOVE_UP_DOWN;
                inputProcessing = InputAxisProcessing::IS_ABSOLUTE;
                componentName = "Y";
                break;
            }
            case 2: 
            {
                inputSemantic = InputComponentSemantic::TOGGLE_UP_DOWN;
                inputProcessing = InputAxisProcessing::REST_ON_CLEAR_CHANGES | InputAxisProcessing::ACCUMULATE_MULTIPLE_CHANGES;
                componentName = "Wheel";
                break;
            }
            default: 
            {
                componentName = "Axis ";
                componentName += Convert::ToString(i + 1);
                break;
            }
        }

        this->state.axes[i]
            .SetIndex(i)
            .SetSemantic(inputSemantic)
            .SetProcessing(inputProcessing)
            .SetDisplayName(componentName);
    }

    return true;
}

void WindowsUWPMouse::Destroy()
{
    this->state.Reset();
}

void WindowsUWPMouse::ClearChanged()
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

bool WindowsUWPMouse::IsConnected() const
{
    return true;
}

bool WindowsUWPMouse::GetConnectionChanged() const
{
    return false;
}

const Utf8String& WindowsUWPMouse::GetDisplayName() const
{
    return this->displayName;
}

void WindowsUWPMouse::SetDisplayName(const Utf8String& value)
{
    this->displayName = value;
}

InputDeviceSemantic WindowsUWPMouse::GetSemantic() const
{
    return this->semantic;
}

void WindowsUWPMouse::SetSemantic(InputDeviceSemantic value)
{
    this->semantic = value;
}

const Utf8String& WindowsUWPMouse::GetProductDescriptor() const
{
    return this->productDescriptor;
}

const Utf8String& WindowsUWPMouse::GetInstanceDescriptor() const
{
    return this->instanceDescriptor;
}

size_t WindowsUWPMouse::GetButtonCount() const
{
    return this->state.buttons.size();
}

InputButton* WindowsUWPMouse::GetButton(size_t index)
{
    return &this->state.buttons[index];
}

size_t WindowsUWPMouse::GetAxisCount() const
{
    return this->state.axes.size();
}

InputAxis* WindowsUWPMouse::GetAxis(size_t index)
{
    return &this->state.axes[index];
}

size_t WindowsUWPMouse::GetPovCount() const
{
    return 0;
}

InputPov* WindowsUWPMouse::GetPov(size_t index)
{
    return nullptr;
}

size_t WindowsUWPMouse::GetLocatorCount() const
{
    return 0;
}

InputLocator* WindowsUWPMouse::GetLocator(size_t index)
{
    return nullptr;
}
