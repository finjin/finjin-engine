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
#include "AndroidGameController.hpp"
#include "finjin/common/AndroidJniUtilities.hpp"
#include "finjin/common/Convert.hpp"
#include "finjin/common/DebugLog.hpp"
#include "finjin/common/NvAndroidNativeAppGlue.h"
#include "finjin/common/Utf8StringFormatter.hpp"

using namespace Finjin::Engine;


//Local functions---------------------------------------------------------------
static bool IsSemanticAxis(int axis)
{
    //Determines whether the specified axis is a semantically-defined Android axis
    //These are redundant axes that are not needed
    switch (axis)
    {
        case AMOTION_EVENT_AXIS_GAS:
        case AMOTION_EVENT_AXIS_BRAKE:
            return true;
    }

    return false;
}

static InputComponentSemantic AxisCodeToSemantic(int axisCode)
{
    switch (axisCode)
    {
        case AMOTION_EVENT_AXIS_X: return InputComponentSemantic::MOVE_LEFT_RIGHT;
        case AMOTION_EVENT_AXIS_Y: return InputComponentSemantic::MOVE_UP_DOWN;
        case AMOTION_EVENT_AXIS_Z: return InputComponentSemantic::LOOK_LEFT_RIGHT;
        case AMOTION_EVENT_AXIS_RZ: return InputComponentSemantic::LOOK_UP_DOWN;
        case AMOTION_EVENT_AXIS_BRAKE:
        case AMOTION_EVENT_AXIS_LTRIGGER: return InputComponentSemantic::BRAKE;
        case AMOTION_EVENT_AXIS_GAS:
        case AMOTION_EVENT_AXIS_RTRIGGER: return InputComponentSemantic::GAS;
        case AMOTION_EVENT_AXIS_HAT_X: return InputComponentSemantic::TOGGLE_LEFT_RIGHT;
        case AMOTION_EVENT_AXIS_HAT_Y: return InputComponentSemantic::TOGGLE_UP_DOWN;
        default: return InputComponentSemantic::NONE;
    }
}

static InputComponentSemantic ButtonKeyCodeToSemantic(int keyCode)
{
    switch (keyCode)
    {
        case AKEYCODE_DPAD_UP: return InputComponentSemantic::TOGGLE_UP;
        case AKEYCODE_DPAD_DOWN: return InputComponentSemantic::TOGGLE_DOWN;
        case AKEYCODE_DPAD_LEFT: return InputComponentSemantic::TOGGLE_LEFT;
        case AKEYCODE_DPAD_RIGHT: return InputComponentSemantic::TOGGLE_RIGHT;
        case AKEYCODE_BUTTON_A: return InputComponentSemantic::ACCEPT;
        case AKEYCODE_BUTTON_B: return InputComponentSemantic::CANCEL;
        case AKEYCODE_BUTTON_X: return InputComponentSemantic::HANDBRAKE;
        case AKEYCODE_BUTTON_Y: return InputComponentSemantic::CHANGE_VIEW;
        case AKEYCODE_BUTTON_L1: return InputComponentSemantic::SHIFT_LEFT;
        case AKEYCODE_BUTTON_R1: return InputComponentSemantic::SHIFT_RIGHT;
        case AKEYCODE_BUTTON_THUMBL: return InputComponentSemantic::MOVE_TOGGLE;
        case AKEYCODE_BUTTON_THUMBR: return InputComponentSemantic::LOOK_TOGGLE;
        case AKEYCODE_BUTTON_START: return InputComponentSemantic::SETTINGS;
        case AKEYCODE_BUTTON_SELECT: return InputComponentSemantic::SYSTEM_SETTINGS;
        default: return InputComponentSemantic::NONE;
    }
}


//Implementation----------------------------------------------------------------
AndroidGameController::AndroidGameController()
{
    Reset();
}

void AndroidGameController::Reset()
{
    AndroidInputDevice::Reset();

    this->connectedButtons.clear();
    this->connectedAxes.clear();

    this->controllerNumber = -1;

    this->isNewConnection = false;
}

bool AndroidGameController::Create(android_app* androidApp)
{
    AndroidJniUtilities jniUtils(androidApp);

    //Do a quick test to see if it's some other type of device that reports itself as a game controller
    Utf8String productName;
    jniUtils.GetStringField(productName, "gameControllerName");
    if (productName.find("Mouse") != Utf8String::npos || productName.find("Keyboard") != Utf8String::npos)
        return false;

    //Assume the gameController is connected since Create() is being called on it
    this->state.isConnected = true;

    //Number, ID, descriptor, buttons----
    jniUtils.GetIntField(this->id, "gameControllerId");

    jniUtils.GetIntField(this->controllerNumber, "gameControllerNumber");

    this->productName = productName;
    this->displayName = this->productName;
    FINJIN_DEBUG_LOG_INFO("Game controller found: %1%", this->productName);

    this->instanceName = this->productName;

    jniUtils.GetStringField(this->productDescriptor, "gameControllerProductDescriptor");
    jniUtils.GetStringField(this->instanceDescriptor, "gameControllerInstanceDescriptor");

    jniUtils.GetBoolField(this->isVirtual, "gameControllerIsVirtual");

    StaticVector<int, GameControllerConstants::MAX_BUTTON_COUNT> buttonsIndices;
    jniUtils.GetIntArrayField(buttonsIndices, "gameControllerButtonIndices");

    this->state.buttons.resize(GameControllerConstants::MAX_BUTTON_COUNT);
    this->connectedButtons.clear();
    Utf8String buttonName;
    for (size_t i = 0; i < buttonsIndices.size(); i++)
    {
        auto buttonKeyCode = buttonsIndices[i];
        if (buttonKeyCode < this->state.buttons.size())
        {
            jniUtils.GetStringArrayFieldElement(buttonName, "gameControllerButtonNames", i);
            SimplifyButtonName(buttonName);

            this->state.buttons[buttonKeyCode]
                .SetIndex(this->connectedButtons.size())
                .SetCode(buttonKeyCode)
                .SetDisplayName(buttonName)
                .SetSemantic(ButtonKeyCodeToSemantic(buttonKeyCode))
                .SetProcessing(InputButtonProcessing::EVENT_DRIVEN);

            this->connectedButtons.push_back(&this->state.buttons[buttonKeyCode]);

            FINJIN_DEBUG_LOG_INFO("  Found button: %1%: %2%", buttonName, buttonKeyCode);
        }
    }

    //Axes-----------------------------
    StaticVector<int, GameControllerConstants::MAX_AXIS_COUNT> indices;
    StaticVector<float, GameControllerConstants::MAX_AXIS_COUNT> mins;
    StaticVector<float, GameControllerConstants::MAX_AXIS_COUNT> maxes;
    StaticVector<float, GameControllerConstants::MAX_AXIS_COUNT> flats;
    //StaticVector<float, GameControllerConstants::MAX_AXIS_COUNT> fuzzes;
    jniUtils.GetIntArrayField(indices, "gameControllerAxisIndices");
    jniUtils.GetFloatArrayField(mins, "gameControllerAxisMinVals");
    jniUtils.GetFloatArrayField(maxes, "gameControllerAxisMaxVals");
    jniUtils.GetFloatArrayField(flats, "gameControllerAxisFlats");
    //jniUtils.GetFloatArrayField(fuzzes, "gameControllerAxisFuzzes");

    this->state.axes.resize(GameControllerConstants::MAX_AXIS_COUNT);
    this->connectedAxes.clear();
    Utf8String axisName;
    for (size_t i = 0; i < indices.size(); i++)
    {
        auto axisCode = indices[i];
        if (axisCode < this->state.axes.size() && !IsSemanticAxis(axisCode))
        {
            auto skip = false;
            switch (axisCode)
            {
                case AMOTION_EVENT_AXIS_X:
                case AMOTION_EVENT_AXIS_Y:
                {
                    //AMOTION_EVENT_AXIS_X and AMOTION_EVENT_AXIS_Y come in more than once.
                    //One of the times it comes with the screen bounds, skip if such bounds are encountered
                    if (mins[i] > 2 || maxes[i] > 2)
                        skip = true;

                    break;
                }
            }

            if (!skip)
            {
                jniUtils.GetStringArrayFieldElement(axisName, "gameControllerAxisNames", i);
                SimplifyAxisName(axisName);

                this->state.axes[axisCode]
                    .SetIndex(this->connectedAxes.size())
                    .SetCode(axisCode)
                    .SetDisplayName(axisName)
                    .SetMinMax(mins[i], maxes[i])
                    .SetDeadZone(flats[i])
                    .SetSemantic(AxisCodeToSemantic(axisCode))
                    .SetProcessing(InputAxisProcessing::NORMALIZE);

                this->connectedAxes.push_back(&this->state.axes[axisCode]);

                {
                    auto& axis = this->state.axes[axisCode];
                    FINJIN_DEBUG_LOG_INFO("  Found axis %1%: name: %2%, min/max: %3%/%4%, dead zone: %5%", axisCode, axisName, axis.GetMinValue(), axis.GetMaxValue(), axis.GetDeadZone());
                }
            }
        }
    }

    return !this->connectedButtons.empty() || !this->connectedAxes.empty();
}

void AndroidGameController::Destroy()
{
    Reset();
}

void AndroidGameController::ClearChanged()
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

bool AndroidGameController::IsConnected() const
{
    return this->state.isConnected;
}

bool AndroidGameController::ConnectionChanged() const
{
    return this->state.connectionChanged;
}

bool AndroidGameController::IsNewConnection() const
{
    return this->isNewConnection;
}

int AndroidGameController::GetControllerNumber() const
{
    return this->controllerNumber;
}

size_t AndroidGameController::GetAxisCount() const
{
    return this->connectedAxes.size();
}

InputAxis* AndroidGameController::GetAxis(size_t axisIndex)
{
    return this->connectedAxes[axisIndex];
}

size_t AndroidGameController::GetButtonCount() const
{
    return this->connectedButtons.size();
}

InputButton* AndroidGameController::GetButton(size_t buttonIndex)
{
    return this->connectedButtons[buttonIndex];
}

size_t AndroidGameController::GetPovCount() const
{
    return 0;
}

InputPov* AndroidGameController::GetPov(size_t povIndex)
{
    return nullptr;
}

void AndroidGameController::CreateGameControllers(StaticVector<AndroidGameController, AndroidGameControllerConstants::MAX_NATIVE_GAME_CONTROLLERS>& gameControllers, android_app* androidApp)
{
    AndroidJniUtilities jniUtils(androidApp);

    int detectedDeviceCount;
    jniUtils.GetIntField(detectedDeviceCount, "gameControllerCount");

    detectedDeviceCount = gameControllers.resize(detectedDeviceCount);

    size_t okDeviceCount = 0;
    for (int detectedDeviceIndex = 0; detectedDeviceIndex < detectedDeviceCount; detectedDeviceIndex++)
    {
        jniUtils.CallVoidMethod_Int("selectGameController", detectedDeviceIndex);

        if (gameControllers[okDeviceCount].Create(androidApp))
            okDeviceCount++;
    }
    gameControllers.resize(okDeviceCount);
}

void AndroidGameController::CheckConfigurationChanged(StaticVector<AndroidGameController, AndroidGameControllerConstants::MAX_NATIVE_GAME_CONTROLLERS>& gameControllers, android_app* androidApp)
{
    bool changed = false;

    AndroidJniUtilities jniUtils(androidApp);
    jniUtils.CallBoolMethod(changed, "checkGameControllerConfigurationChanged");

    if (changed)
    {
        //Handle disconnected
        StaticVector<int, AndroidGameControllerConstants::MAX_NATIVE_GAME_CONTROLLERS> disconnectedGameControllerIds;
        int disconnectedGameControllerCount = 0;
        jniUtils.GetIntField(disconnectedGameControllerCount, "disconnectedGameControllerCount");
        jniUtils.GetIntArrayField(disconnectedGameControllerIds, "disconnectedGameControllerIds");
        for (int i = 0; i < disconnectedGameControllerCount; i++)
        {
            auto gameController = GetGameControllerByID(gameControllers.data(), gameControllers.size(), disconnectedGameControllerIds[i]);
            if (gameController != nullptr && gameController->state.isConnected)
            {
                FINJIN_DEBUG_LOG_INFO("GameController %1% detected as disconnected. Instance descriptor: %2%", gameController->GetInstanceName(), gameController->GetInstanceDescriptor());

                gameController->state.isConnected = false;
                gameController->state.connectionChanged = true;
            }
        }
    }
}

void AndroidGameController::RefreshGameControllers
    (
    StaticVector<AndroidGameController, AndroidGameControllerConstants::MAX_NATIVE_GAME_CONTROLLERS>& devices,
    StaticVector<AndroidGameController, AndroidGameControllerConstants::MAX_NATIVE_GAME_CONTROLLERS>& devicesBuffer,
    android_app* androidApp
    )
{
    if (devices.full())
        return;

    //Refresh internal info on the Java side
    AndroidJniUtilities jniUtils(androidApp);
    jniUtils.CallVoidMethod("refreshGameControllers");

    //Get the refreshed devices and add new devices to the end
    CreateGameControllers(devicesBuffer, androidApp);
    for (size_t gameControllerIndex = 0; gameControllerIndex < devicesBuffer.size(); gameControllerIndex++)
    {
        auto oldDevice = GetGameControllerByInstanceDescriptor(devices.data(), devices.size(), devicesBuffer[gameControllerIndex].GetInstanceDescriptor());
        if (oldDevice == nullptr)
        {
            FINJIN_DEBUG_LOG_INFO("New game controller found: %1%, id=%2%, instance descriptor=%3%", devicesBuffer[gameControllerIndex].productName, devicesBuffer[gameControllerIndex].id, devicesBuffer[gameControllerIndex].instanceDescriptor);

            if (!devices.full())
            {
                auto newDeviceIndex = devices.size();
                devices.push_back();
                devices[newDeviceIndex] = devicesBuffer[gameControllerIndex];
                devices[newDeviceIndex].isNewConnection = true;
            }
        }
        else if (!oldDevice->state.isConnected)
        {
            FINJIN_DEBUG_LOG_INFO("GameController %1% detected as connected. Instance descriptor: %2%", oldDevice->GetInstanceName(), oldDevice->GetInstanceDescriptor());

            oldDevice->id = devicesBuffer[gameControllerIndex].id;
            oldDevice->state.isConnected = true;
            oldDevice->state.connectionChanged = true;
        }
    }
}

AndroidGameController* AndroidGameController::GetGameControllerByInstanceDescriptor(AndroidGameController* gameControllers, size_t gameControllerCount, const Utf8String& descriptor)
{
    for (size_t gameControllerIndex = 0; gameControllerIndex < gameControllerCount; gameControllerIndex++)
    {
        if (gameControllers[gameControllerIndex].GetInstanceDescriptor() == descriptor)
            return &gameControllers[gameControllerIndex];
    }

    return nullptr;
}

AndroidGameController* AndroidGameController::GetGameControllerByID(AndroidGameController* gameControllers, size_t gameControllerCount, int deviceID)
{
    for (size_t gameControllerIndex = 0; gameControllerIndex < gameControllerCount; gameControllerIndex++)
    {
        if (gameControllers[gameControllerIndex].GetID() == deviceID)
            return &gameControllers[gameControllerIndex];
    }

    return nullptr;
}
