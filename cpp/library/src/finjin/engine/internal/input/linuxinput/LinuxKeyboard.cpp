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
#include "LinuxKeyboard.hpp"
#include "finjin/common/Convert.hpp"
#include "finjin/engine/OSWindow.hpp"
#include "LinuxInputContext.hpp"
#include <xcb/xcb.h>
#include <xcb/xcb_keysyms.h>
#define XK_MISCELLANY
#define XK_TECHNICAL
#define XK_XKB_KEYS
#include <X11/Xlib.h>
#include <X11/keysymdef.h>
#include <X11/keysym.h>

using namespace Finjin::Engine;


//Implementation----------------------------------------------------------------
LinuxKeyboard::LinuxKeyboard(Allocator* allocator) : Super(allocator)
{
}

void LinuxKeyboard::Reset()
{
    Super::Reset();

    this->state.Reset();
}

bool LinuxKeyboard::Create(LinuxInputContext* context, size_t index)
{
    this->context = context;

    this->instanceName = "Keyboard ";
    this->instanceName += Convert::ToString(index + 1);

    this->productName = "Keyboard";
    this->displayName = this->productName;

    this->instanceDescriptor = "generic-keyboard-";
    this->instanceDescriptor += Convert::ToString(index);

    this->productDescriptor = "generic-keyboard";

    InputComponentSemantic inputSemantic;
    Utf8String keyName;

    auto keySyms = xcb_key_symbols_alloc(this->context->GetSettings().osWindow->GetConnection());

    this->state.buttons.resize(KeyboardConstants::MAX_BUTTON_COUNT);
    for (size_t buttonIndex = 0; buttonIndex < this->state.buttons.size(); buttonIndex++)
    {
        auto keySym = xcb_key_symbols_get_keysym(keySyms, buttonIndex, 0);

        keyName.clear();
        inputSemantic = InputComponentSemantic::NONE;

        if (keySym != XCB_NO_SYMBOL)
        {
            keyName = XKeysymToString(keySym);

            switch (keySym)
            {
                case XK_Up: inputSemantic = InputComponentSemantic::TOGGLE_UP; break;
                case XK_Left: inputSemantic = InputComponentSemantic::TOGGLE_LEFT; break;
                case XK_Down: inputSemantic = InputComponentSemantic::TOGGLE_DOWN; break;
                case XK_Right: inputSemantic = InputComponentSemantic::TOGGLE_RIGHT; break;
                case XK_Return: inputSemantic = InputComponentSemantic::ACCEPT; break;
                case XK_Escape: inputSemantic = InputComponentSemantic::CANCEL; break;
                case XK_F1: inputSemantic = InputComponentSemantic::SETTINGS; break;
            }
        }

        this->state.buttons[buttonIndex]
            .SetIndex(buttonIndex)
            .SetCode(keySym)
            .SetDisplayName(keyName)
            .SetSemantic(inputSemantic)
            .SetProcessing(InputButtonProcessing::EVENT_DRIVEN)
            .Enable(!keyName.empty());
    }

    xcb_key_symbols_free(keySyms);

    return true;
}

void LinuxKeyboard::Destroy()
{
    this->state.Reset();
}

void LinuxKeyboard::Update(SimpleTimeDelta elapsedTime, bool isFirstUpdate)
{
}

void LinuxKeyboard::ClearChanged()
{
    auto keyCount = GetButtonCount();
    for (size_t keyIndex = 0; keyIndex < keyCount; keyIndex++)
    {
        auto key = GetButton(keyIndex);
        key->ClearChanged();
    }
}

bool LinuxKeyboard::IsConnected() const
{
    return true;
}

bool LinuxKeyboard::GetConnectionChanged() const
{
    return false;
}

size_t LinuxKeyboard::GetButtonCount() const
{
    return this->state.buttons.size();
}

InputButton* LinuxKeyboard::GetButton(size_t index)
{
    return &this->state.buttons[index];
}

size_t LinuxKeyboard::GetAxisCount() const
{
    return 0;
}

InputAxis* LinuxKeyboard::GetAxis(size_t index)
{
    return nullptr;
}

size_t LinuxKeyboard::GetPovCount() const
{
    return 0;
}

InputPov* LinuxKeyboard::GetPov(size_t index)
{
    return nullptr;
}

size_t LinuxKeyboard::GetLocatorCount() const
{
    return 0;
}

InputLocator* LinuxKeyboard::GetLocator(size_t locatorIndex)
{
    return nullptr;
}
