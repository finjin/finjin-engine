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
#include "WindowsUWPKeyboard.hpp"
#include "finjin/common/Convert.hpp"

using namespace Finjin::Engine;


//Implementation----------------------------------------------------------------
WindowsUWPKeyboard::WindowsUWPKeyboard()
{
}

void WindowsUWPKeyboard::Reset()
{
    this->state.Reset();
}

bool WindowsUWPKeyboard::Create(size_t index)
{
    this->instanceName = "Keyboard ";
    this->instanceName += Convert::ToString(index + 1);

    this->productName = "Keyboard";
    this->displayName = this->productName;

    this->semantic = InputDeviceSemantic::NONE;

    this->instanceDescriptor = "generic-keyboard-";
    this->instanceDescriptor += Convert::ToString(index);

    this->productDescriptor = "generic-keyboard";

    Utf8String keyName;

    this->state.buttons.resize(KeyboardConstants::MAX_BUTTON_COUNT);
    for (size_t i = 0; i < this->state.buttons.size(); i++)
    {
        auto inputSemantic = InputComponentSemantic::NONE;

        try
        {
            auto virtualKey = (Windows::System::VirtualKey)i;

            switch (virtualKey)
            {
                case Windows::System::VirtualKey::Up: inputSemantic = InputComponentSemantic::TOGGLE_UP; break;
                case Windows::System::VirtualKey::Left: inputSemantic = InputComponentSemantic::TOGGLE_LEFT; break;
                case Windows::System::VirtualKey::Down: inputSemantic = InputComponentSemantic::TOGGLE_DOWN; break;
                case Windows::System::VirtualKey::Right: inputSemantic = InputComponentSemantic::TOGGLE_RIGHT; break;
                case Windows::System::VirtualKey::Enter: inputSemantic = InputComponentSemantic::ACCEPT; break;
                case Windows::System::VirtualKey::Cancel: inputSemantic = InputComponentSemantic::CANCEL; break;
                case Windows::System::VirtualKey::F1: inputSemantic = InputComponentSemantic::SETTINGS; break;
            }

            keyName = virtualKey.ToString()->Data();
        }
        catch (...)
        {
            keyName.clear();
        }

        this->state.buttons[i]
            .SetIndex(i)
            .SetCode(i)
            .SetDisplayName(keyName)
            .SetSemantic(inputSemantic)
            .SetProcessing(InputButtonProcessing::EVENT_DRIVEN)
            .Enable(!keyName.empty());
    }

    return true;
}

void WindowsUWPKeyboard::Destroy()
{
    this->state.Reset();
}

void WindowsUWPKeyboard::ClearChanged()
{
    auto keyCount = GetButtonCount();
    for (size_t keyIndex = 0; keyIndex < keyCount; keyIndex++)
    {
        auto key = GetButton(keyIndex);
        key->ClearChanged();
    }
}

bool WindowsUWPKeyboard::IsConnected() const
{
    return true;
}

bool WindowsUWPKeyboard::GetConnectionChanged() const
{
    return false;
}

const Utf8String& WindowsUWPKeyboard::GetDisplayName() const
{
    return this->displayName;
}

void WindowsUWPKeyboard::SetDisplayName(const Utf8String& value)
{
    this->displayName = value;
}

InputDeviceSemantic WindowsUWPKeyboard::GetSemantic() const
{
    return this->semantic;
}

void WindowsUWPKeyboard::SetSemantic(InputDeviceSemantic value)
{
    this->semantic = value;
}

const Utf8String& WindowsUWPKeyboard::GetProductDescriptor() const
{
    return this->productDescriptor;
}

const Utf8String& WindowsUWPKeyboard::GetInstanceDescriptor() const
{
    return this->instanceDescriptor;
}

size_t WindowsUWPKeyboard::GetButtonCount() const
{
    return this->state.buttons.size();
}

InputButton* WindowsUWPKeyboard::GetButton(size_t index)
{
    return &this->state.buttons[index];
}

size_t WindowsUWPKeyboard::GetAxisCount() const
{
    return 0;
}

InputAxis* WindowsUWPKeyboard::GetAxis(size_t index)
{
    return nullptr;
}

size_t WindowsUWPKeyboard::GetPovCount() const
{
    return 0;
}

InputPov* WindowsUWPKeyboard::GetPov(size_t index)
{
    return nullptr;
}

size_t WindowsUWPKeyboard::GetLocatorCount() const
{
    return 0;
}

InputLocator* WindowsUWPKeyboard::GetLocator(size_t index)
{
    return nullptr;
}
