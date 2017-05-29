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

#include "OpenVRDevice.hpp"
#include "OpenVRDeviceImpl.hpp"
#include "OpenVRSystem.hpp"

using namespace Finjin::Engine;


//Implementation----------------------------------------------------------------
OpenVRDevice::OpenVRDevice() : impl(new OpenVRDeviceImpl)
{
}

OpenVRDevice::~OpenVRDevice()
{
}

const Utf8String& OpenVRDevice::GetSystemInternalName() const
{
    return OpenVRSystem::GetSystemInternalName();
}

const Utf8String& OpenVRDevice::GetProductDescriptor() const
{
    return impl->productDescriptor;
}

const Utf8String& OpenVRDevice::GetInstanceDescriptor() const
{
    return impl->instanceDescriptor;
}

const Utf8String& OpenVRDevice::GetDisplayName() const
{
    return impl->displayName;
}

void OpenVRDevice::SetDisplayName(const Utf8String& value)
{
    impl->displayName = value;
}

InputDeviceSemantic OpenVRDevice::GetSemantic() const
{
    return impl->semantic;
}

void OpenVRDevice::SetSemantic(InputDeviceSemantic value)
{
    impl->semantic = value;
}

bool OpenVRDevice::IsConnected() const
{
    return impl->gameControllerState.isConnected;
}

bool OpenVRDevice::ConnectionChanged() const
{
    return impl->gameControllerState.connectionChanged;
}

bool OpenVRDevice::IsNewConnection() const
{
    return impl->isNewConnection;
}

size_t OpenVRDevice::GetAxisCount() const
{
    return impl->gameControllerState.axes.size();
}

InputAxis* OpenVRDevice::GetAxis(size_t index)
{
    return &impl->gameControllerState.axes[index];
}

size_t OpenVRDevice::GetButtonCount() const
{
    return impl->gameControllerState.buttons.size();
}

InputButton* OpenVRDevice::GetButton(size_t index)
{
    return &impl->gameControllerState.buttons[index];
}

size_t OpenVRDevice::GetLocatorCount() const
{
    return impl->locators.size();
}

InputLocator* OpenVRDevice::GetLocator(size_t index)
{
    return &impl->locators[index];
}

void OpenVRDevice::AddHapticFeedback(const HapticFeedbackSettings* forces, size_t count)
{
    if (count > 0 && forces != nullptr)
        impl->forceFeedback = forces[0];
}

void OpenVRDevice::StopHapticFeedback()
{
    impl->forceFeedback.Reset();
}

InputDeviceClass OpenVRDevice::GetDeviceClass() const
{
    switch (impl->vrDeviceClass)
    {
        case vr::TrackedDeviceClass_HMD: return InputDeviceClass::HEADSET;
        case vr::TrackedDeviceClass_Controller: return InputDeviceClass::GAME_CONTROLLER;
        case vr::TrackedDeviceClass_TrackingReference: return InputDeviceClass::NONE;
        default: return InputDeviceClass::NONE;
    }
}

VRDeviceRenderModelPointer OpenVRDevice::GetRenderModel()
{
    VRDeviceRenderModelPointer result;

    if (impl->renderModel != nullptr)
    {
        result.vertexData = impl->renderModel->rVertexData;
        result.vertexCount = impl->renderModel->unVertexCount;

        result.vertexElements = impl->renderModelVertexElements.data();
        result.vertexElementCount = impl->renderModelVertexElements.size();
                
        result.indexData = impl->renderModel->rIndexData;
        
        result.primitiveCount = impl->renderModel->unTriangleCount;

        if (impl->renderModelTexture != nullptr)
        {
            result.textureWidth = impl->renderModelTexture->unWidth;
            result.textureHeight = impl->renderModelTexture->unHeight;
            result.textureImage = impl->renderModelTexture->rubTextureMapData;
        }
    }

    return result;
}

OpenVRDeviceImpl* OpenVRDevice::GetImpl()
{
    return impl.get();
}

#endif
