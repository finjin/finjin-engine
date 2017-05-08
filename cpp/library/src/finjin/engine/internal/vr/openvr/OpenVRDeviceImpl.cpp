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

#if FINJIN_TARGET_VR_SYSTEM == FINJIN_TARGET_VR_SYSTEM_OPENVR

#include "OpenVRDeviceImpl.hpp"
#include "finjin/common/Convert.hpp"
#include "finjin/common/DebugLog.hpp"

using namespace Finjin::Engine;


//Local functions---------------------------------------------------------------
static Utf8String GetStringTrackedDeviceProperty
    (
    vr::IVRSystem* ivrSystem,
    vr::TrackedDeviceIndex_t deviceIndex,
    vr::TrackedDeviceProperty prop,
    vr::TrackedPropertyError* vrError = nullptr
    )
{
    Utf8String result;

    auto requiredLength = ivrSystem->GetStringTrackedDeviceProperty(deviceIndex, prop, nullptr, 0, vrError);
    if (requiredLength > 0)
    {
        result.resize(requiredLength - 1); //Don't include the terminating character
        ivrSystem->GetStringTrackedDeviceProperty(deviceIndex, prop, &result[0], requiredLength, vrError);
    }

    return result;
}


//Implementation----------------------------------------------------------------
OpenVRDeviceImpl::OpenVRDeviceImpl()
{
    this->deviceIndex = vr::k_unTrackedDeviceIndex_Hmd;
    this->isNewConnection = false;

    Reset(true);
}

void OpenVRDeviceImpl::Reset(bool isConstructing)
{
    this->requestedRenderModelName.clear();
    this->renderModel = nullptr;
    this->renderModelTexture = nullptr;

    this->semantic = InputDeviceSemantic::NONE;

    this->productDescriptor.clear();
    this->instanceDescriptor.clear();

    this->trackingSystemName.clear();
    this->modelNumber.clear();
    this->serialNumber.clear();
    this->manufacturer.clear();
    this->isWireless = false;
    this->vrDeviceClass = vr::TrackedDeviceClass_Invalid;
    this->hasCamera = false;

    this->displayFrequency = 0;
    this->universeID = vr::TrackingUniverseSeated;
    this->productID = 0;
    FINJIN_ZERO_ITEM(this->cameraToHeadTransform);
    this->onDesktop = false;

    this->deviceID.clear();
    this->supportedButtons = 0;
    FINJIN_ZERO_ITEM(this->axisTypes);
    this->role = vr::TrackedControllerRole_Invalid;

    FINJIN_ZERO_ITEM(this->controllerState);
    FINJIN_ZERO_ITEM(this->controllerPoseState);
    this->gameControllerState.Reset(isConstructing);
}

bool OpenVRDeviceImpl::GetInfo(vr::IVRSystem* ivrSystem)
{
    auto deviceClass = static_cast<vr::ETrackedDeviceClass>(ivrSystem->GetInt32TrackedDeviceProperty(this->deviceIndex, vr::Prop_DeviceClass_Int32));
    if (deviceClass == vr::TrackedDeviceClass_Invalid)
        return false;

    this->trackingSystemName = GetStringTrackedDeviceProperty(ivrSystem, this->deviceIndex, vr::Prop_TrackingSystemName_String);
    this->modelNumber = GetStringTrackedDeviceProperty(ivrSystem, this->deviceIndex, vr::Prop_ModelNumber_String);
    this->serialNumber = GetStringTrackedDeviceProperty(ivrSystem, this->deviceIndex, vr::Prop_SerialNumber_String);
    this->manufacturer = GetStringTrackedDeviceProperty(ivrSystem, this->deviceIndex, vr::Prop_ManufacturerName_String);
    this->isWireless = ivrSystem->GetBoolTrackedDeviceProperty(this->deviceIndex, vr::Prop_DeviceIsWireless_Bool);
    this->vrDeviceClass = deviceClass;
    this->hasCamera = ivrSystem->GetBoolTrackedDeviceProperty(this->deviceIndex, vr::Prop_HasCamera_Bool);

    switch (this->vrDeviceClass)
    {
        case vr::TrackedDeviceClass_HMD:
        {
            //Headset
            this->displayFrequency = ivrSystem->GetFloatTrackedDeviceProperty(this->deviceIndex, vr::Prop_DisplayFrequency_Float);
            this->universeID = ivrSystem->GetUint64TrackedDeviceProperty(this->deviceIndex, vr::Prop_CurrentUniverseId_Uint64);
            this->productID = ivrSystem->GetInt32TrackedDeviceProperty(this->deviceIndex, vr::Prop_EdidProductID_Int32);
            this->cameraToHeadTransform = ivrSystem->GetMatrix34TrackedDeviceProperty(this->deviceIndex, vr::Prop_CameraToHeadTransform_Matrix34);
            this->onDesktop = ivrSystem->GetBoolTrackedDeviceProperty(this->deviceIndex, vr::Prop_IsOnDesktop_Bool);

            break;
        }
        case vr::TrackedDeviceClass_Controller:
        {
            //Controller
            this->deviceID = GetStringTrackedDeviceProperty(ivrSystem, this->deviceIndex, vr::Prop_AttachedDeviceId_String);
            this->supportedButtons = ivrSystem->GetUint64TrackedDeviceProperty(this->deviceIndex, vr::Prop_SupportedButtons_Uint64);
            for (int axisIndex = 0; axisIndex < vr::k_unControllerStateAxisCount; axisIndex++)
                this->axisTypes[axisIndex] = static_cast<vr::EVRControllerAxisType>(ivrSystem->GetInt32TrackedDeviceProperty(this->deviceIndex, static_cast<vr::ETrackedDeviceProperty>(vr::Prop_Axis0Type_Int32 + axisIndex)));
            this->role = static_cast<vr::ETrackedControllerRole>(ivrSystem->GetInt32TrackedDeviceProperty(this->deviceIndex, vr::Prop_ControllerRoleHint_Int32));
            switch (this->role)
            {
                case vr::TrackedControllerRole_LeftHand: this->semantic = InputDeviceSemantic::LEFT_HAND; break;
                case vr::TrackedControllerRole_RightHand: this->semantic = InputDeviceSemantic::RIGHT_HAND; break;
                default: this->semantic = InputDeviceSemantic::NONE; break;
            }

            this->locators.resize(1);
            auto& locator = this->locators[0];
            locator.SetIndex(0).SetDisplayName("Locator").SetSemantic(InputComponentSemantic::LOCATOR);

            this->gameControllerState.buttons.resize(this->gameControllerState.buttons.max_size());
            this->gameControllerState.axes.resize(this->gameControllerState.axes.max_size());

            for (size_t i = 0; i < this->gameControllerState.buttons.size(); i++)
                this->gameControllerState.buttons[i].SetIndex(i);

            this->gameControllerState.buttons[vr::k_EButton_System].SetCode(vr::k_EButton_System).SetDisplayName("System").SetSemantic(InputComponentSemantic::SYSTEM_HOME);
            this->gameControllerState.buttons[vr::k_EButton_ApplicationMenu].SetCode(vr::k_EButton_ApplicationMenu).SetDisplayName("Application").SetSemantic(InputComponentSemantic::SETTINGS);
            this->gameControllerState.buttons[vr::k_EButton_Grip].SetCode(vr::k_EButton_Grip).SetDisplayName("Grip").SetSemantic(InputComponentSemantic::CANCEL);
            this->gameControllerState.buttons[vr::k_EButton_DPad_Left].SetCode(vr::k_EButton_DPad_Left).SetDisplayName("D-pad left").SetSemantic(InputComponentSemantic::TOGGLE_LEFT);
            this->gameControllerState.buttons[vr::k_EButton_DPad_Up].SetCode(vr::k_EButton_DPad_Up).SetDisplayName("D-pad up").SetSemantic(InputComponentSemantic::TOGGLE_UP);
            this->gameControllerState.buttons[vr::k_EButton_DPad_Right].SetCode(vr::k_EButton_DPad_Right).SetDisplayName("D-pad right").SetSemantic(InputComponentSemantic::TOGGLE_RIGHT);
            this->gameControllerState.buttons[vr::k_EButton_DPad_Down].SetCode(vr::k_EButton_DPad_Down).SetDisplayName("D-pad down").SetSemantic(InputComponentSemantic::TOGGLE_DOWN);
            this->gameControllerState.buttons[vr::k_EButton_A].SetCode(vr::k_EButton_A).SetDisplayName("A");
            this->gameControllerState.buttons[vr::k_EButton_Axis0].SetCode(vr::k_EButton_Axis0).SetDisplayName("Touchpad").SetSemantic(InputComponentSemantic::MOVE_TOGGLE);
            this->gameControllerState.buttons[vr::k_EButton_Axis1].SetCode(vr::k_EButton_Axis1).SetDisplayName("Trigger").SetSemantic(InputComponentSemantic::ACCEPT);
            this->gameControllerState.buttons[vr::k_EButton_Axis2].SetCode(vr::k_EButton_Axis2).SetDisplayName("Axis 2");
            this->gameControllerState.buttons[vr::k_EButton_Axis3].SetCode(vr::k_EButton_Axis3).SetDisplayName("Axis 3");
            this->gameControllerState.buttons[vr::k_EButton_Axis4].SetCode(vr::k_EButton_Axis4).SetDisplayName("Axis 4");

            Utf8String axisName;
            auto hasTrackPad = false;
            auto hasJoystick = false;
            for (int i = 0; i < vr::k_unControllerStateAxisCount; i++)
            {
                auto& xAxis = this->gameControllerState.axes[i * 2];
                auto& yAxis = this->gameControllerState.axes[i * 2 + 1];

                xAxis.SetIndex(i * 2);
                yAxis.SetIndex(i * 2 + 1);

                switch (this->axisTypes[i])
                {
                    case vr::k_eControllerAxis_None:
                    {
                        axisName.clear();
                        break;
                    }
                    case vr::k_eControllerAxis_TrackPad:
                    {
                        axisName = "Trackpad";
                        hasTrackPad = true;
                        break;
                    }
                    case vr::k_eControllerAxis_Joystick:
                    {
                        axisName = "Joystick";
                        hasJoystick = true;
                        break;
                    }
                    case vr::k_eControllerAxis_Trigger:
                    {
                        axisName = "Trigger";
                        break;
                    }
                }

                auto axisDisplayName = axisName;
                axisDisplayName += " X";
                xAxis.SetDisplayName(axisDisplayName);

                axisDisplayName = axisName;
                axisDisplayName += " Y";
                yAxis.SetDisplayName(axisDisplayName);
            }

            for (int i = 0; i < vr::k_unControllerStateAxisCount; i++)
            {
                auto& xAxis = this->gameControllerState.axes[i * 2];
                auto& yAxis = this->gameControllerState.axes[i * 2 + 1];

                switch (this->axisTypes[i])
                {
                    case vr::k_eControllerAxis_TrackPad:
                    {
                        if (hasJoystick)
                        {
                            //Track pad is used for "look" (joystick is used for "move")
                            xAxis.SetSemantic(InputComponentSemantic::LOOK_LEFT_RIGHT);
                            yAxis.SetSemantic(InputComponentSemantic::LOOK_UP_DOWN);
                        }
                        else
                        {
                            //Track pad is used for "move"
                            xAxis.SetSemantic(InputComponentSemantic::MOVE_LEFT_RIGHT);
                            yAxis.SetSemantic(InputComponentSemantic::MOVE_UP_DOWN);
                        }
                        break;
                    }
                    case vr::k_eControllerAxis_Joystick:
                    {
                        if (hasTrackPad)
                        {
                            //Joystick is used for "move" (track pad is used for "look")
                            xAxis.SetSemantic(InputComponentSemantic::MOVE_LEFT_RIGHT);
                            yAxis.SetSemantic(InputComponentSemantic::MOVE_UP_DOWN);
                        }
                        else
                        {
                            //Joystick is used for "look"
                            xAxis.SetSemantic(InputComponentSemantic::LOOK_LEFT_RIGHT);
                            yAxis.SetSemantic(InputComponentSemantic::LOOK_UP_DOWN);
                        }
                        break;
                    }
                    case vr::k_eControllerAxis_Trigger:
                    {
                        xAxis.SetSemantic(InputComponentSemantic::GAS);
                        yAxis.Enable(false);
                        break;
                    }
                }
            }

            break;
        }
        case vr::TrackedDeviceClass_TrackingReference:
        {
            //Camera/tracking box

            break;
        }
    }

    auto manufacturerAndModel = this->manufacturer;
    if (!manufacturerAndModel.empty())
        manufacturerAndModel += " ";
    manufacturerAndModel += this->modelNumber;

    this->productDescriptor = InputDeviceUtilities::MakeInputDeviceIdentifier(manufacturerAndModel);

    this->instanceDescriptor = this->productDescriptor;
    this->instanceDescriptor += "-";
    this->instanceDescriptor += Convert::ToString(deviceIndex + 1);

    return true;
}

Utf8String OpenVRDeviceImpl::GetRenderModelName(vr::IVRSystem* ivrSystem) const
{
    return GetStringTrackedDeviceProperty(ivrSystem, this->deviceIndex, vr::Prop_RenderModelName_String);
}

#endif
