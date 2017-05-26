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
#include "AndroidMouse.hpp"
#include "finjin/common/AndroidJniUtilities.hpp"
#include "finjin/common/Convert.hpp"
#include "finjin/common/DebugLog.hpp"
#include "finjin/common/Utf8StringFormatter.hpp"

using namespace Finjin::Engine;


//Implementation----------------------------------------------------------------
AndroidMouse::AndroidMouse()
{
    Reset();
}

void AndroidMouse::Reset()
{
    AndroidInputDevice::Reset();

    this->connectedButtons.clear();
    this->connectedAxes.clear();

    this->isNewConnection = false;
}

bool AndroidMouse::Create(android_app* androidApp)
{
    AndroidJniUtilities jniUtils(androidApp);

    //Do a quick test to see if it's some other type of device that reports itself as a mouse
    Utf8String productName;
    jniUtils.GetStringField(productName, "mouseName");
    if (productName.find("Keyboard") != Utf8String::npos || productName.find("Controller") != Utf8String::npos)
        return false;

    //Assume the mouse is connected since Create() is being called on it
    this->state.isConnected = true;

    //ID, name, descriptor, buttons----
    jniUtils.GetIntField(this->id, "mouseId");

    this->productName = productName;
    this->displayName = this->productName;
    //FINJIN_DEBUG_LOG_INFO("Mouse found: %1%", this->productName);

    this->instanceName = this->productName;

    jniUtils.GetStringField(this->productDescriptor, "mouseProductDescriptor");
    jniUtils.GetStringField(this->instanceDescriptor, "mouseInstanceDescriptor");

    jniUtils.GetBoolField(this->isVirtual, "mouseIsVirtual");

    StaticVector<int, MouseConstants::MAX_BUTTON_COUNT> connectedButtonsIndices;
    jniUtils.GetIntArrayField(connectedButtonsIndices, "mouseButtonIndices");

    this->state.buttons.resize(MouseConstants::MAX_BUTTON_COUNT);
    this->connectedButtons.clear();
    Utf8String buttonName;
    for (size_t i = 0; i < connectedButtonsIndices.size(); i++)
    {
        auto buttonKeyCode = connectedButtonsIndices[i];
        if (buttonKeyCode < this->state.buttons.size())
        {
            jniUtils.GetStringArrayFieldElement(buttonName, "mouseButtonNames", i);
            SimplifyButtonName(buttonName);

            this->state.buttons[buttonKeyCode]
                .SetIndex(this->connectedButtons.size())
                .SetCode(buttonKeyCode)
                .SetProcessing(InputButtonProcessing::EVENT_DRIVEN)
                .SetDisplayName(buttonName);

            this->connectedButtons.push_back(&this->state.buttons[buttonKeyCode]);

            //FINJIN_DEBUG_LOG_INFO("  Found mouse button: %1%: %2%", buttonName, buttonKeyCode);
        }
    }

    //Axes-----------------------------
    StaticVector<int, MouseConstants::MAX_AXIS_COUNT> indices;
    jniUtils.GetIntArrayField(indices, "mouseAxisIndices");

    this->state.axes.resize(MouseConstants::MAX_AXIS_COUNT);
    this->connectedAxes.clear();
    Utf8String axisName;
    for (size_t i = 0; i < indices.size(); i++)
    {
        auto axisCode = indices[i];
        if (axisCode < this->state.axes.size())
        {
            jniUtils.GetStringArrayFieldElement(axisName, "mouseAxisNames", i);
            SimplifyAxisName(axisName);

            auto inputSemantic = InputComponentSemantic::NONE;
            switch (axisCode)
            {
                case AMOTION_EVENT_AXIS_X: inputSemantic = InputComponentSemantic::MOVE_LEFT_RIGHT; break;
                case AMOTION_EVENT_AXIS_Y: inputSemantic = InputComponentSemantic::MOVE_UP_DOWN; break;
                case AMOTION_EVENT_AXIS_PRESSURE: inputSemantic = InputComponentSemantic::ACCEPT; break; //Primary mouse button
                case AMOTION_EVENT_AXIS_VSCROLL: inputSemantic = InputComponentSemantic::TOGGLE_UP_DOWN; break; //Wheel
            }

            this->state.axes[axisCode]
                .SetIndex(this->connectedAxes.size())
                .SetCode(axisCode)
                .SetSemantic(inputSemantic)
                .SetDisplayName(axisName);

            this->connectedAxes.push_back(&this->state.axes[axisCode]);

            //FINJIN_DEBUG_LOG_INFO("  Found axis: %1%: %2%", axisName, axisCode);
        }
    }

    return !this->connectedButtons.empty() || !this->connectedAxes.empty();
}

void AndroidMouse::Destroy()
{
    Reset();
}

void AndroidMouse::ClearChanged()
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

    this->state.connectionChanged = false;

    this->isNewConnection = false;
}

bool AndroidMouse::IsConnected() const
{
    return this->state.isConnected;
}

bool AndroidMouse::ConnectionChanged() const
{
    return this->state.connectionChanged;
}

bool AndroidMouse::IsNewConnection() const
{
    return this->isNewConnection;
}

size_t AndroidMouse::GetAxisCount() const
{
    return this->connectedAxes.size();
}

InputAxis* AndroidMouse::GetAxis(size_t axisIndex)
{
    return this->connectedAxes[axisIndex];
}

size_t AndroidMouse::GetButtonCount() const
{
    return this->connectedButtons.size();
}

InputButton* AndroidMouse::GetButton(size_t buttonIndex)
{
    return this->connectedButtons[buttonIndex];
}

size_t AndroidMouse::GetPovCount() const
{
    return 0;
}

InputPov* AndroidMouse::GetPov(size_t povIndex)
{
    return nullptr;
}

bool AndroidMouse::GetCurrentPosition(float& x, float& y) const
{
    int foundFlags = 0;
    x = y = 0;

    for (const auto axis : this->connectedAxes)
    {
        if (axis->GetCode() == AMOTION_EVENT_AXIS_X)
        {
            x = axis->GetValue();
            foundFlags |= 1;
        }
        else if (axis->GetCode() == AMOTION_EVENT_AXIS_Y)
        {
            y = axis->GetValue();
            foundFlags |= 2;
        }

        if (foundFlags == 3)
            return true;
    }

    return false;
}

void AndroidMouse::CreateMice(StaticVector<AndroidMouse, MouseConstants::MAX_MICE>& mice, android_app* androidApp)
{
    AndroidJniUtilities jniUtils(androidApp);

    int detectedDeviceCount;
    jniUtils.GetIntField(detectedDeviceCount, "mouseCount");

    detectedDeviceCount = mice.resize(detectedDeviceCount);

    size_t okDeviceCount = 0;
    for (int detectedDeviceIndex = 0; detectedDeviceIndex < detectedDeviceCount; detectedDeviceIndex++)
    {
        jniUtils.CallVoidMethod_Int("selectMouse", detectedDeviceIndex);

        if (mice[okDeviceCount].Create(androidApp))
            okDeviceCount++;
    }
    mice.resize(okDeviceCount);
}

void AndroidMouse::CheckConfigurationChanged(StaticVector<AndroidMouse, MouseConstants::MAX_MICE>& mice, android_app* androidApp)
{
    bool changed = false;

    AndroidJniUtilities jniUtils(androidApp);
    jniUtils.CallBoolMethod(changed, "checkMouseConfigurationChanged");

    if (changed)
    {
        //Handle disconnected
        StaticVector<int, MouseConstants::MAX_MICE> disconnectedMouseIDs;
        int disconnectedMouseCount = 0;
        jniUtils.GetIntField(disconnectedMouseCount, "disconnectedMouseCount");
        jniUtils.GetIntArrayField(disconnectedMouseIDs, "disconnectedMouseIds");
        for (int i = 0; i < disconnectedMouseCount; i++)
        {
            auto mouse = GetMouseByID(mice.data(), mice.size(), disconnectedMouseIDs[i]);
            if (mouse != nullptr && mouse->state.isConnected)
            {
                FINJIN_DEBUG_LOG_INFO("Mouse %1% detected as connected. Instance descriptor: %2%", mouse->GetInstanceName(), mouse->GetInstanceDescriptor());

                mouse->state.isConnected = false;
                mouse->state.connectionChanged = true;
            }
        }
    }
}

void AndroidMouse::RefreshMice
    (
    StaticVector<AndroidMouse, MouseConstants::MAX_MICE>& devices,
    StaticVector<AndroidMouse, MouseConstants::MAX_MICE>& devicesBuffer,
    android_app* androidApp
    )
{
    if (devices.full())
        return;

    //Refresh internal info on the Java side
    AndroidJniUtilities jniUtils(androidApp);
    jniUtils.CallVoidMethod("refreshMice");

    //Get the refreshed devices and add new devices to the end
    CreateMice(devicesBuffer, androidApp);
    for (size_t mouseIndex = 0; mouseIndex < devicesBuffer.size(); mouseIndex++)
    {
        auto oldDevice = GetMouseByInstanceDescriptor(devices.data(), devices.size(), devicesBuffer[mouseIndex].GetInstanceDescriptor());
        if (oldDevice == nullptr)
        {
            FINJIN_DEBUG_LOG_INFO("New mouse found: %1%", devicesBuffer[mouseIndex].productName);

            if (!devices.full())
            {
                auto newDeviceIndex = devices.size();
                devices.push_back();
                devices[newDeviceIndex] = devicesBuffer[mouseIndex];
                devices[newDeviceIndex].isNewConnection = true;
            }
        }
        else if (!oldDevice->state.isConnected)
        {
            FINJIN_DEBUG_LOG_INFO("Mouse %1% detected as connected. Instance descriptor: %2%", oldDevice->GetInstanceName(), oldDevice->GetInstanceDescriptor());

            oldDevice->id = devicesBuffer[mouseIndex].id;
            oldDevice->state.isConnected = true;
            oldDevice->state.connectionChanged = true;
        }
    }
}

AndroidMouse* AndroidMouse::GetMouseByInstanceDescriptor(AndroidMouse* mice, size_t mouseCount, const Utf8String& descriptor)
{
    for (size_t mouseIndex = 0; mouseIndex < mouseCount; mouseIndex++)
    {
        if (mice[mouseIndex].GetInstanceDescriptor() == descriptor)
            return &mice[mouseIndex];
    }

    return nullptr;
}

AndroidMouse* AndroidMouse::GetMouseByID(AndroidMouse* mice, size_t mouseCount, int deviceID)
{
    for (size_t mouseIndex = 0; mouseIndex < mouseCount; mouseIndex++)
    {
        if (mice[mouseIndex].GetID() == deviceID)
            return &mice[mouseIndex];
    }

    return nullptr;
}
