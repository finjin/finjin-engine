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
#include "AndroidKeyboard.hpp"
#include "finjin/common/AndroidJniUtilities.hpp"
#include "finjin/common/Convert.hpp"
#include "finjin/common/DebugLog.hpp"
#include "finjin/common/Utf8StringFormatter.hpp"

using namespace Finjin::Engine;


//Implementation----------------------------------------------------------------
AndroidKeyboard::AndroidKeyboard()
{
    Reset();
}

void AndroidKeyboard::Reset()
{
    AndroidInputDevice::Reset();

    this->state.Reset();

    this->isNewConnection = false;
}

bool AndroidKeyboard::Create(android_app* androidApp)
{
    AndroidJniUtilities jniUtils(androidApp);

    //Do a quick test to see if it's some other type of device that reports itself as a keyboard
    Utf8String productName;
    jniUtils.GetStringField(productName, "keyboardName");
    if (productName.find("Mouse") != Utf8String::npos || productName.find("gpio") != Utf8String::npos || productName.find("Controller") != Utf8String::npos)
        return false;

    //Assume the keyboard is connected since Create() is being called on it
    this->state.isConnected = true;

    //ID, name, descriptor, keys----
    jniUtils.GetIntField(this->id, "keyboardId");

    this->productName = productName;
    this->displayName = this->productName;
    //FINJIN_DEBUG_LOG_INFO("Keyboard found: %1%", this->productName);

    this->instanceName = this->productName;

    jniUtils.GetStringField(this->productDescriptor, "keyboardProductDescriptor");
    jniUtils.GetStringField(this->instanceDescriptor, "keyboardInstanceDescriptor");

    jniUtils.GetBoolField(this->isVirtual, "keyboardIsVirtual");

    this->state.buttons.resize(KeyboardConstants::MAX_BUTTON_COUNT);
    Utf8String keyName;
    for (size_t i = 0; i < this->state.buttons.size(); i++)
    {
        if (jniUtils.GetStringArrayFieldElement(keyName, "keyCodeStrings", static_cast<int>(i)))
        {
            SimplifyButtonName(keyName);

            auto inputSemantic = InputComponentSemantic::NONE;

            switch (i)
            {
                case AKEYCODE_DPAD_UP: inputSemantic = InputComponentSemantic::TOGGLE_UP; break;
                case AKEYCODE_DPAD_LEFT: inputSemantic = InputComponentSemantic::TOGGLE_LEFT; break;
                case AKEYCODE_DPAD_DOWN: inputSemantic = InputComponentSemantic::TOGGLE_DOWN; break;
                case AKEYCODE_DPAD_RIGHT: inputSemantic = InputComponentSemantic::TOGGLE_RIGHT; break;
                case AKEYCODE_ENTER: inputSemantic = InputComponentSemantic::ACCEPT; break;
                case AKEYCODE_ESCAPE: inputSemantic = InputComponentSemantic::CANCEL; break;
                case AKEYCODE_F1: inputSemantic = InputComponentSemantic::SETTINGS; break;
            }

            this->state.buttons[i]
                .SetIndex(i)
                .SetCode(i)
                .SetSemantic(inputSemantic)
                .SetProcessing(InputButtonProcessing::EVENT_DRIVEN)
                .SetDisplayName(keyName)
                .Enable(!keyName.empty());
        }
    }

    return true;
}

void AndroidKeyboard::Destroy()
{
    Reset();
}

void AndroidKeyboard::ClearChanged()
{
    //Keys
    auto keyCount = GetButtonCount();
    for (size_t keyIndex = 0; keyIndex < keyCount; keyIndex++)
    {
        auto key = GetButton(keyIndex);
        key->ClearChanged();
    }

    this->state.connectionChanged = false;

    this->isNewConnection = false;
}

bool AndroidKeyboard::IsConnected() const
{
    return this->state.isConnected;
}

bool AndroidKeyboard::ConnectionChanged() const
{
    return this->state.connectionChanged;
}

bool AndroidKeyboard::IsNewConnection() const
{
    return this->isNewConnection;
}

size_t AndroidKeyboard::GetAxisCount() const
{
    return this->state.axes.size();
}

InputAxis* AndroidKeyboard::GetAxis(size_t axisIndex)
{
    return &this->state.axes[axisIndex];
}

size_t AndroidKeyboard::GetButtonCount() const
{
    return this->state.buttons.size();
}

InputButton* AndroidKeyboard::GetButton(size_t buttonIndex)
{
    return &this->state.buttons[buttonIndex];
}

size_t AndroidKeyboard::GetPovCount() const
{
    return 0;
}

InputPov* AndroidKeyboard::GetPov(size_t povIndex)
{
    return nullptr;
}

void AndroidKeyboard::CreateKeyboards(StaticVector<AndroidKeyboard, KeyboardConstants::MAX_KEYBOARDS>& keyboards, android_app* androidApp)
{
    AndroidJniUtilities jniUtils(androidApp);

    int detectedDeviceCount;
    jniUtils.GetIntField(detectedDeviceCount, "keyboardCount");

    detectedDeviceCount = keyboards.resize(detectedDeviceCount);

    size_t okDeviceCount = 0;
    for (int detectedDeviceIndex = 0; detectedDeviceIndex < detectedDeviceCount; detectedDeviceIndex++)
    {
        jniUtils.CallVoidMethod_Int("selectKeyboard", detectedDeviceIndex);

        if (keyboards[okDeviceCount].Create(androidApp))
            okDeviceCount++;
    }
    keyboards.resize(okDeviceCount);
}

void AndroidKeyboard::CheckConfigurationChanged(StaticVector<AndroidKeyboard, KeyboardConstants::MAX_KEYBOARDS>& keyboards, android_app* androidApp)
{
    bool changed = false;

    AndroidJniUtilities jniUtils(androidApp);
    jniUtils.CallBoolMethod(changed, "checkKeyboardConfigurationChanged");

    if (changed)
    {
        //Handle disconnected
        StaticVector<int, KeyboardConstants::MAX_KEYBOARDS> disconnectedKeyboardIDs;
        int disconnectedKeyboardCount = 0;
        jniUtils.GetIntField(disconnectedKeyboardCount, "disconnectedKeyboardCount");
        jniUtils.GetIntArrayField(disconnectedKeyboardIDs, "disconnectedKeyboardIds");
        for (int i = 0; i < disconnectedKeyboardCount; i++)
        {
            auto keyboard = GetKeyboardByID(keyboards.data(), keyboards.size(), disconnectedKeyboardIDs[i]);
            if (keyboard != nullptr && keyboard->state.isConnected)
            {
                FINJIN_DEBUG_LOG_INFO("Keyboard %1% detected as disconnected. Instance descriptor: %2%", keyboard->GetInstanceName(), keyboard->GetInstanceDescriptor());

                keyboard->state.isConnected = false;
                keyboard->state.connectionChanged = true;
            }
        }
    }
}

void AndroidKeyboard::RefreshKeyboards
    (
    StaticVector<AndroidKeyboard, KeyboardConstants::MAX_KEYBOARDS>& devices,
    StaticVector<AndroidKeyboard, KeyboardConstants::MAX_KEYBOARDS>& devicesBuffer,
    android_app* androidApp
    )
{
    if (devices.full())
        return;

    //Refresh internal info on the Java side
    AndroidJniUtilities jniUtils(androidApp);
    jniUtils.CallVoidMethod("refreshKeyboards");

    //Get the refreshed devices and add new devices to the end
    CreateKeyboards(devicesBuffer, androidApp);
    for (size_t i = 0; i < devicesBuffer.size(); i++)
    {
        auto oldDevice = GetKeyboardByInstanceDescriptor(devices.data(), devices.size(), devicesBuffer[i].GetInstanceDescriptor());
        if (oldDevice == nullptr)
        {
            FINJIN_DEBUG_LOG_INFO("New keyboard found: %1%", devicesBuffer[i].productName);

            if (!devices.full())
            {
                auto newDeviceIndex = devices.size();
                devices.push_back();
                devices[newDeviceIndex] = devicesBuffer[i];
                devices[newDeviceIndex].isNewConnection = true;
            }
        }
        else if (!oldDevice->state.isConnected)
        {
            FINJIN_DEBUG_LOG_INFO("Keyboard %1% detected as connected. Instance descriptor: %2%", oldDevice->GetInstanceName(), oldDevice->GetInstanceDescriptor());

            oldDevice->id = devicesBuffer[i].id;
            oldDevice->state.isConnected = true;
            oldDevice->state.connectionChanged = true;
        }
    }
}

AndroidKeyboard* AndroidKeyboard::GetKeyboardByInstanceDescriptor(AndroidKeyboard* keyboards, size_t keyboardCount, const Utf8String& descriptor)
{
    for (size_t i = 0; i < keyboardCount; i++)
    {
        if (keyboards[i].GetInstanceDescriptor() == descriptor)
            return &keyboards[i];
    }

    return nullptr;
}

AndroidKeyboard* AndroidKeyboard::GetKeyboardByID(AndroidKeyboard* keyboards, size_t keyboardCount, int deviceID)
{
    for (size_t i = 0; i < keyboardCount; i++)
    {
        if (keyboards[i].GetID() == deviceID)
            return &keyboards[i];
    }

    return nullptr;
}
