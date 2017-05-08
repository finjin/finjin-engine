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
#include "MacOSKeyboard.hpp"
#include "finjin/common/Convert.hpp"
#include <Carbon/Carbon.h>

using namespace Finjin::Engine;


//Implementation----------------------------------------------------------------
MacOSKeyboard::MacOSKeyboard()
{
    this->context = nullptr;

    Reset();
}

void MacOSKeyboard::Reset()
{
    this->semantic = InputDeviceSemantic::NONE;

    this->state.Reset();
}

bool MacOSKeyboard::Create(MacOSInputContext* context, size_t index)
{
    this->context = context;

    this->instanceName = "Keyboard ";
    this->instanceName += Convert::ToString(index + 1);

    this->displayName = "Keyboard";
    this->productName = "Keyboard";

    this->instanceDescriptor = "generic-keyboard-";
    this->instanceDescriptor += Convert::ToString(index);

    this->productDescriptor = "generic-keyboard";

    InputComponentSemantic inputSemantic;
    Utf8String keyName;

    auto currentKeyboard = TISCopyCurrentKeyboardLayoutInputSource();
    auto uchr = (CFDataRef)TISGetInputSourceProperty(currentKeyboard, kTISPropertyUnicodeKeyLayoutData);
    auto keyboardLayout = (const UCKeyboardLayout*)CFDataGetBytePtr(uchr);
    auto keyboardType = LMGetKbdType();
    const int maxKeyNameCount = 100;
    UniChar keyNameUnicode[maxKeyNameCount] = {};
    UniCharCount actualKeyNameLength;

    this->state.buttons.resize(KeyboardConstants::MAX_BUTTON_COUNT);
    for (size_t i = 0; i < this->state.buttons.size(); i++)
    {
        auto virtualKey = static_cast<int>(i);

        switch (virtualKey)
        {
            case kVK_UpArrow: inputSemantic = InputComponentSemantic::TOGGLE_UP; break;
            case kVK_LeftArrow: inputSemantic = InputComponentSemantic::TOGGLE_LEFT; break;
            case kVK_DownArrow: inputSemantic = InputComponentSemantic::TOGGLE_DOWN; break;
            case kVK_RightArrow: inputSemantic = InputComponentSemantic::TOGGLE_RIGHT; break;
            case kVK_Return: inputSemantic = InputComponentSemantic::ACCEPT; break;
            case kVK_Escape: inputSemantic = InputComponentSemantic::CANCEL; break;
            case kVK_F1: inputSemantic = InputComponentSemantic::SETTINGS; break;
            default: inputSemantic = InputComponentSemantic::NONE; break;
        }

        switch (virtualKey)
        {
            case kVK_Return: keyName = "Return"; break;
            case kVK_Tab: keyName = "Tab"; break;
            case kVK_Space: keyName = "Space bar"; break;
            case kVK_Delete: keyName = "Delete"; break;
            case kVK_Escape: keyName = "Escape"; break;
            case kVK_Command: keyName = "Command"; break;
            case kVK_Shift: keyName = "Shift"; break;
            case kVK_CapsLock: keyName = "Caps lock"; break;
            case kVK_Option: keyName = "Option"; break;
            case kVK_Control: keyName = "Control"; break;
            case kVK_RightShift: keyName = "Right shift"; break;
            case kVK_RightOption: keyName = "Right option"; break;
            case kVK_RightControl: keyName = "Right control"; break;
            case kVK_Function: keyName = "Function"; break;
            case kVK_F17: keyName = "F17"; break;
            case kVK_VolumeUp: keyName = "Volume up"; break;
            case kVK_VolumeDown: keyName = "Volume down"; break;
            case kVK_Mute: keyName = "Mute"; break;
            case kVK_F18: keyName = "F18"; break;
            case kVK_F19: keyName = "F19"; break;
            case kVK_F20: keyName = "F20"; break;
            case kVK_F5: keyName = "F5"; break;
            case kVK_F6: keyName = "F6"; break;
            case kVK_F7: keyName = "F7"; break;
            case kVK_F3: keyName = "F3"; break;
            case kVK_F8: keyName = "F8"; break;
            case kVK_F9: keyName = "F9"; break;
            case kVK_F11: keyName = "F11"; break;
            case kVK_F13: keyName = "F13"; break;
            case kVK_F16: keyName = "F16"; break;
            case kVK_F14: keyName = "F14"; break;
            case kVK_F10: keyName = "F10"; break;
            case kVK_F12: keyName = "F12"; break;
            case kVK_F15: keyName = "F15"; break;
            case kVK_Help: keyName = "Help"; break;
            case kVK_Home: keyName = "Home"; break;
            case kVK_PageUp: keyName = "Page up"; break;
            case kVK_ForwardDelete: keyName = "Forward delete"; break;
            case kVK_F4: keyName = "F4"; break;
            case kVK_End: keyName = "End"; break;
            case kVK_F2: keyName = "F2"; break;
            case kVK_PageDown: keyName = "Page down"; break;
            case kVK_F1: keyName = "F1"; break;
            case kVK_LeftArrow: keyName = "Left arrow"; break;
            case kVK_RightArrow: keyName = "Right arrow"; break;
            case kVK_DownArrow: keyName = "Down arrow"; break;
            case kVK_UpArrow: keyName = "Up arrow"; break;
            default:
            {
                UInt32 deadKeyState = 0;
                actualKeyNameLength = 0;
                auto status = UCKeyTranslate(keyboardLayout, virtualKey, kUCKeyActionDown, 0, keyboardType, kUCKeyTranslateNoDeadKeysMask, &deadKeyState, maxKeyNameCount, &actualKeyNameLength, keyNameUnicode);
                if (status == noErr && actualKeyNameLength > 0)
                {
                    auto nsKeyName = [[NSString stringWithCharacters:keyNameUnicode length:(NSInteger)actualKeyNameLength] uppercaseString];
                    keyName = nsKeyName.UTF8String;
                }
                else
                    keyName.clear();

                break;
            }
        }

        this->state.buttons[i]
            .SetIndex(i)
            .SetCode(virtualKey)
            .SetDisplayName(keyName)
            .SetSemantic(inputSemantic)
            .SetProcessing(InputButtonProcessing::EVENT_DRIVEN)
            .Enable(!keyName.empty());
    }

    return true;
}

void MacOSKeyboard::Destroy()
{
    this->state.Reset();
}

void MacOSKeyboard::Update(SimpleTimeDelta elapsedTime, bool isFirstUpdate)
{
}

void MacOSKeyboard::ClearChanged()
{
    auto keyCount = GetButtonCount();
    for (size_t keyIndex = 0; keyIndex < keyCount; keyIndex++)
    {
        auto key = GetButton(keyIndex);
        key->ClearChanged();
    }
}

bool MacOSKeyboard::IsConnected() const
{
    return true;
}

bool MacOSKeyboard::GetConnectionChanged() const
{
    return false;
}

const Utf8String& MacOSKeyboard::GetDisplayName() const
{
    return this->displayName;
}

void MacOSKeyboard::SetDisplayName(const Utf8String& value)
{
    this->displayName = value;
}

InputDeviceSemantic MacOSKeyboard::GetSemantic() const
{
    return this->semantic;
}

void MacOSKeyboard::SetSemantic(InputDeviceSemantic value)
{
    this->semantic = value;
}

const Utf8String& MacOSKeyboard::GetProductDescriptor() const
{
    return this->productDescriptor;
}

const Utf8String& MacOSKeyboard::GetInstanceDescriptor() const
{
    return this->instanceDescriptor;
}

size_t MacOSKeyboard::GetButtonCount() const
{
    return this->state.buttons.size();
}

InputButton* MacOSKeyboard::GetButton(size_t index)
{
    return &this->state.buttons[index];
}

size_t MacOSKeyboard::GetAxisCount() const
{
    return 0;
}

InputAxis* MacOSKeyboard::GetAxis(size_t index)
{
    return nullptr;
}

size_t MacOSKeyboard::GetPovCount() const
{
    return 0;
}

InputPov* MacOSKeyboard::GetPov(size_t index)
{
    return nullptr;
}

size_t MacOSKeyboard::GetLocatorCount() const
{
    return 0;
}

InputLocator* MacOSKeyboard::GetLocator(size_t locatorIndex)
{
    return nullptr;
}
