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


#pragma once


//Includes----------------------------------------------------------------------
#include "finjin/common/Chrono.hpp"
#include "finjin/common/Error.hpp"
#include "finjin/common/StaticVector.hpp"
#include "finjin/engine/GenericGpuNumericStructs.hpp"
#include "finjin/engine/InputComponents.hpp"
#include <openvr.h>


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    struct OpenVRDeviceImpl
    {
        OpenVRDeviceImpl();

        void Reset(bool isConstructing = false);

        bool GetInfo(vr::IVRSystem* ivrSystem);

        Utf8String GetRenderModelName(vr::IVRSystem* ivrSystem) const;

        vr::TrackedDeviceIndex_t deviceIndex;
        bool isNewConnection;

        Utf8String requestedRenderModelName;
        vr::RenderModel_t* renderModel;
        vr::RenderModel_TextureMap_t* renderModelTexture;

        InputDeviceSemantic semantic;

        Utf8String displayName;

        Utf8String productDescriptor;
        Utf8String instanceDescriptor;

        Utf8String trackingSystemName;
        Utf8String modelNumber;
        Utf8String serialNumber;
        Utf8String manufacturer;
        bool isWireless;
        vr::ETrackedDeviceClass vrDeviceClass;
        bool hasCamera;

        float displayFrequency;
        uint64_t universeID;
        int32_t productID;
        vr::HmdMatrix34_t cameraToHeadTransform;
        bool onDesktop;

        Utf8String deviceID;
        uint64_t supportedButtons;
        vr::EVRControllerAxisType axisTypes[vr::k_unControllerStateAxisCount];
        vr::ETrackedControllerRole role;

        vr::TrackedDevicePose_t controllerPoseState;
        vr::VRControllerState_t controllerState;

        HapticFeedback forceFeedback;

        StaticVector<InputLocator, HeadsetConstants::MAX_LOCATOR_COUNT> locators;

        //vr::k_EButton_Max buttons
        //vr::k_unControllerStateAxisCount * 2 axes (each VR axis has 2 components)
        InputDeviceState<InputButton, InputAxis, InputPov, vr::k_EButton_Max, vr::k_unControllerStateAxisCount * 2> gameControllerState; //The 'isConnected' and 'connectionChanged' members are used for all devices, not just controllers

        StaticVector<GpuInputFormatStruct::Element, 3> renderModelVertexElements;
    };

} }
