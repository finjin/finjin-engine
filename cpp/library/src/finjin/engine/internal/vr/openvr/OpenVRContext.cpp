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

#include "OpenVRContext.hpp"
#include "OpenVRSystem.hpp"
#include "OpenVRDeviceImpl.hpp"
#include "finjin/common/Convert.hpp"
#include "finjin/common/DebugLog.hpp"
#include "finjin/common/Vector.hpp"
#include "finjin/engine/InputComponents.hpp"
#include "finjin/engine/GeometryGenerator.hpp"

using namespace Finjin::Common;
using namespace Finjin::Engine;

#define HAPTIC_PULSE_LENGTH_MICROSECONDS 3000


//Local classes----------------------------------------------------------------
struct OpenVRContext::Impl : public AllocatedClass
{
    using Vector2 = GeometryGenerator::Vector2;

    struct DistortionVertex
    {
        Vector2 position;
        Vector2 texCoordRed;
        Vector2 texCoordGreen;
        Vector2 texCoordBlue;
    };

    struct DistortionMesh
    {
        AllocatedVector<DistortionVertex> verts;
        AllocatedVector<uint16_t> indices;
    };
        
    Impl(Allocator* allocator, OpenVRContext* context) : AllocatedClass(allocator), settings(allocator)
    {
        this->context = context;

        this->hasRuntime = false;

        this->ivrSystem = nullptr;        
    }

    bool HasFocus()
    {
        return this->ivrSystem != nullptr && !this->ivrSystem->IsInputFocusCapturedByAnotherProcess();
    }

    bool CaptureInput()
    {
        return this->ivrSystem != nullptr && this->ivrSystem->CaptureInputFocus();
    }

    void ReleaseCaptureInput()
    {
        if (this->ivrSystem != nullptr)
            this->ivrSystem->ReleaseInputFocus();
    }

    void SubmitToCompositor()
    {
        //vr::VRCompositor()->Submit(vr::Eye_Left, &leftEyeTexture);
        //vr::VRCompositor()->Submit(vr::Eye_Right, &rightEyeTexture);
    }

    std::array<uint32_t, 2> GetPreferredRenderTargetDimensions()
    {
        uint32_t width = 0, height = 0;
        if (this->ivrSystem != nullptr)
            this->ivrSystem->GetRecommendedRenderTargetSize(&width, &height);
        return { width, height };
    }

    bool IsRenderModelLoading(const Utf8String& name) const
    {
        for (auto& model : this->loadingRenderModels)
        {
            if (model.name == name)
                return true;
        }

        return false;
    }

    vr::RenderModel_t* GetRenderModel(const Utf8String& name) const
    {
        for (auto& model : this->loadedRenderModels)
        {
            if (model.name == name)
                return model.model;
        }

        return nullptr;
    }
    
    void StartLoadingRenderModel(const Utf8String& modelName)
    {
        if (this->loadingRenderModels.size() + this->loadedRenderModels.size() < this->loadedRenderModels.max_size())
        {
            vr::RenderModel_t* renderModel = nullptr;

            vr::EVRRenderModelError vrError = vr::VRRenderModels()->LoadRenderModel_Async(modelName.c_str(), &renderModel);
            if (vrError == vr::VRRenderModelError_None)
                this->loadedRenderModels.push_back(RenderModelInfo(modelName, renderModel));
            else if (vrError == vr::VRRenderModelError_Loading)
                this->loadingRenderModels.push_back(RenderModelInfo(modelName, renderModel));
            else
                FINJIN_DEBUG_LOG_ERROR("Failed to start loading VR render model '%1%'.", modelName);
        }
    }

    vr::RenderModel_TextureMap_t* GetRenderModelTexture(vr::TextureID_t renderModelTextureID) const
    {
        for (auto& texture : this->loadedRenderModelTextures)
        {
            if (texture.textureID == renderModelTextureID)
                return texture.texture;
        }

        return nullptr;
    }

    bool IsRenderModelTextureLoading(vr::TextureID_t renderModelTextureID) const
    {
        for (auto& texture : this->loadingRenderModelTextures)
        {
            if (texture.textureID == renderModelTextureID)
                return true;
        }

        return false;
    }

    void StartLoadingTexture(vr::TextureID_t renderModelTextureID)
    {
        if (this->loadingRenderModelTextures.size() + this->loadedRenderModelTextures.size() < this->loadedRenderModelTextures.max_size())
        {
            vr::RenderModel_TextureMap_t* renderModelTexture = nullptr;

            vr::EVRRenderModelError vrError = vr::VRRenderModels()->LoadTexture_Async(renderModelTextureID, &renderModelTexture);
            if (vrError == vr::VRRenderModelError_None)
                this->loadedRenderModelTextures.push_back(RenderModelTextureInfo(renderModelTextureID, renderModelTexture));
            else if (vrError == vr::VRRenderModelError_Loading)
                this->loadingRenderModelTextures.push_back(RenderModelTextureInfo(renderModelTextureID, renderModelTexture));
            else
                FINJIN_DEBUG_LOG_ERROR("Failed to start loading VR render model texture '%1%'.", renderModelTextureID);
        }
    }

    void UpdateDeviceRenderModels(const Utf8String& renderModelName, vr::RenderModel_t* renderModel, vr::RenderModel_TextureMap_t* texture)
    {
        for (size_t deviceIndex = 0; deviceIndex < this->devices.size(); deviceIndex++)
        {
            auto& device = this->devices[deviceIndex];
            auto deviceImpl = device.GetImpl();

            if (deviceImpl->requestedRenderModelName == renderModelName)
            {
                deviceImpl->renderModel = renderModel;
                deviceImpl->renderModelTexture = texture;
                deviceImpl->requestedRenderModelName.clear();

                if (this->settings.deviceRenderModelLoadedHandler != nullptr)
                    this->settings.deviceRenderModelLoadedHandler(this->context, deviceIndex);
            }
        }
    }

    void UpdateDeviceRenderModelsForTexture(vr::TextureID_t textureID, vr::RenderModel_TextureMap_t* texture)
    {
        for (size_t i = 0; i < this->loadingRenderModels.size();)
        {
            auto& loadingRenderModel = this->loadingRenderModels[i];
            if (loadingRenderModel.model->diffuseTextureId == textureID)
            {
                UpdateDeviceRenderModels(loadingRenderModel.name, loadingRenderModel.model, texture);
                this->loadingRenderModels.erase(this->loadingRenderModels.begin() + i);
            }
            else
                i++;
        }
    }

    void UpdateAsyncResources()
    {
        for (size_t i = 0; i < this->loadingRenderModels.size();)
        {
            auto& loadingRenderModel = this->loadingRenderModels[i];

            auto vrError = vr::VRRenderModels()->LoadRenderModel_Async(loadingRenderModel.name.c_str(), &loadingRenderModel.model);
            if (vrError == vr::VRRenderModelError_None)
            {
                //Model finished loading

                //Add to "loaded" vector
                this->loadedRenderModels.push_back(loadingRenderModel);

                auto texture = GetRenderModelTexture(loadingRenderModel.model->diffuseTextureId);
                if (texture != nullptr)
                {
                    //Model and texture are loaded. Update devices that need the model
                    UpdateDeviceRenderModels(loadingRenderModel.name, loadingRenderModel.model, texture);
                }
                else if (!IsRenderModelTextureLoading(loadingRenderModel.model->diffuseTextureId))
                {
                    //Texture not yet loaded. Start loading it
                    StartLoadingTexture(loadingRenderModel.model->diffuseTextureId);
                }

                //Remove from "loading" vector
                this->loadingRenderModels.erase(this->loadingRenderModels.begin() + i);
            }
            else if (vrError != vr::VRRenderModelError_Loading)
            {
                FINJIN_DEBUG_LOG_ERROR("Failed to continue loading VR render model '%1%'.", loadingRenderModel.name);
                this->loadingRenderModels.erase(this->loadingRenderModels.begin() + i);
            }
            else
                i++;
        }

        for (size_t i = 0; i < this->loadingRenderModelTextures.size();)
        {
            auto& loadingRenderModelTexture = this->loadingRenderModelTextures[i];

            auto vrError = vr::VRRenderModels()->LoadTexture_Async(loadingRenderModelTexture.textureID, &loadingRenderModelTexture.texture);
            if (vrError == vr::VRRenderModelError_None)
            {
                //Texture finished loading

                //Add to "loaded" vector
                this->loadedRenderModelTextures.push_back(loadingRenderModelTexture);

                //Update devices that need the model that is now finished (due to the texture being finished)
                UpdateDeviceRenderModelsForTexture(loadingRenderModelTexture.textureID, loadingRenderModelTexture.texture);
                
                //Remove from "loading" vector
                this->loadingRenderModelTextures.erase(this->loadingRenderModelTextures.begin() + i);
            }
            else if (vrError != vr::VRRenderModelError_Loading)
            {
                FINJIN_DEBUG_LOG_ERROR("Failed to continue loading VR render model texture '%1%'.", loadingRenderModelTexture.textureID);
                this->loadingRenderModelTextures.erase(this->loadingRenderModelTextures.begin() + i);
            }
            else
                i++;
        }
    }

    void GetDeviceInfo(OpenVRDevice& device)
    {
        auto deviceImpl = device.GetImpl();

        deviceImpl->gameControllerState.isConnected = deviceImpl->GetInfo(this->ivrSystem);

        if (this->settings.loadStandardRenderModels)
        {
            auto renderModelName = deviceImpl->GetRenderModelName(this->ivrSystem);
            if (!renderModelName.empty())
            {
                auto renderModel = GetRenderModel(renderModelName);
                if (renderModel != nullptr)
                {
                    //Render model is already loaded
                    deviceImpl->renderModel = renderModel;
                }
                else if (!IsRenderModelLoading(renderModelName))
                {
                    //Render model is not loaded and isn't being loaded. Start loading it
                    deviceImpl->requestedRenderModelName = renderModelName;
                    StartLoadingRenderModel(renderModelName);
                }
            }
        }
    }

    void SetForces(OpenVRDeviceImpl* deviceImpl)
    {
        this->ivrSystem->TriggerHapticPulse(deviceImpl->deviceIndex, 0, HAPTIC_PULSE_LENGTH_MICROSECONDS);
    }

    void InitializeDistortionMesh()
    {
        uint16_t lensGridSegmentCountHorz = 43;
        uint16_t lensGridSegmentCountVert = 43;

        float w = (float)(1.0 / float(lensGridSegmentCountHorz - 1));
        float h = (float)(1.0 / float(lensGridSegmentCountVert - 1));

        float u, v = 0;

        auto& verts = this->distortionMesh.verts;
        verts.Create(lensGridSegmentCountVert * lensGridSegmentCountHorz * 2, GetAllocator());
        
        DistortionVertex vert;

        //Left eye distortion vertices
        float xOffset = -1;
        for (int y = 0; y < lensGridSegmentCountVert; y++)
        {
            for (int x = 0; x < lensGridSegmentCountHorz; x++)
            {
                u = x * w; 
                v = 1 - y * h;
                
                vert.position = Vector2(xOffset + u, -1 + 2 * y*h);

                vr::DistortionCoordinates_t dc0 = this->ivrSystem->ComputeDistortion(vr::Eye_Left, u, v);

                vert.texCoordRed = Vector2(dc0.rfRed[0], 1 - dc0.rfRed[1]);
                vert.texCoordGreen = Vector2(dc0.rfGreen[0], 1 - dc0.rfGreen[1]);
                vert.texCoordBlue = Vector2(dc0.rfBlue[0], 1 - dc0.rfBlue[1]);

                verts.push_back(vert);
            }
        }

        //Right eye distortion vertices
        xOffset = 0;
        for (int y = 0; y < lensGridSegmentCountVert; y++)
        {
            for (int x = 0; x < lensGridSegmentCountHorz; x++)
            {
                u = x * w; 
                v = 1 - y * h;

                vert.position = Vector2(xOffset + u, -1 + 2 * y*h);

                vr::DistortionCoordinates_t dc0 = this->ivrSystem->ComputeDistortion(vr::Eye_Right, u, v);

                vert.texCoordRed = Vector2(dc0.rfRed[0], 1 - dc0.rfRed[1]);
                vert.texCoordGreen = Vector2(dc0.rfGreen[0], 1 - dc0.rfGreen[1]);
                vert.texCoordBlue = Vector2(dc0.rfBlue[0], 1 - dc0.rfBlue[1]);

                verts.push_back(vert);
            }
        }

        auto& indices = this->distortionMesh.indices;
        indices.Create((lensGridSegmentCountVert - 1) * (lensGridSegmentCountHorz - 1) * 6 * 2, GetAllocator());

        uint16_t a, b, c, d;

        uint16_t offset = 0;
        for (uint16_t y = 0; y < lensGridSegmentCountVert - 1; y++)
        {
            for (uint16_t x = 0; x < lensGridSegmentCountHorz - 1; x++)
            {
                a = lensGridSegmentCountHorz * y + x + offset;
                b = lensGridSegmentCountHorz * y + x + 1 + offset;
                c = (y + 1) * lensGridSegmentCountHorz + x + 1 + offset;
                d = (y + 1) * lensGridSegmentCountHorz + x + offset;
                
                indices.push_back(a);
                indices.push_back(b);
                indices.push_back(c);

                indices.push_back(a);
                indices.push_back(c);
                indices.push_back(d);
            }
        }

        offset = lensGridSegmentCountHorz * lensGridSegmentCountVert;
        for (uint16_t y = 0; y < lensGridSegmentCountVert - 1; y++)
        {
            for (uint16_t x = 0; x < lensGridSegmentCountHorz - 1; x++)
            {
                a = lensGridSegmentCountHorz * y + x + offset;
                b = lensGridSegmentCountHorz * y + x + 1 + offset;
                c = (y + 1) * lensGridSegmentCountHorz + x + 1 + offset;
                d = (y + 1) * lensGridSegmentCountHorz + x + offset;
                
                indices.push_back(a);
                indices.push_back(b);
                indices.push_back(c);

                indices.push_back(a);
                indices.push_back(c);
                indices.push_back(d);
            }
        }
    }

    OpenVRContext* context;

    OpenVRSystem* vrSystem;

    OpenVRContextSettings settings;

    OperationStatus initializationStatus;
    bool hasRuntime;
    
    vr::IVRSystem* ivrSystem;
    
    vr::Texture_t vrLeftEyeTexture; //{ (void*)leftEyeDesc.m_nResolveTextureId, vr::API_DirectX, vr::ColorSpace_Gamma };
    vr::Texture_t vrRightEyeTexture; //{ (void*)rightEyeDesc.m_nResolveTextureId, vr::API_DirectX, vr::ColorSpace_Gamma };

    std::array<OpenVRDevice, vr::k_unMaxTrackedDeviceCount> devices;
    std::array<vr::TrackedDevicePose_t, vr::k_unMaxTrackedDeviceCount> deviceRenderPoseStates; //One for each device. Check bPoseIsValid before using 

    DistortionMesh distortionMesh;

    struct RenderModelInfo
    {
        RenderModelInfo()
        {
            this->model = nullptr;
        }

        RenderModelInfo(const Utf8String& name, vr::RenderModel_t* model)
        {
            this->name = name;
            this->model = model;
        }

        Utf8String name;
        vr::RenderModel_t* model;
    };
    StaticVector<RenderModelInfo, vr::k_unMaxTrackedDeviceCount> loadingRenderModels;
    StaticVector<RenderModelInfo, vr::k_unMaxTrackedDeviceCount> loadedRenderModels;

    struct RenderModelTextureInfo
    {
        RenderModelTextureInfo()
        {
            this->textureID = 0;
            this->texture = nullptr;
        }

        RenderModelTextureInfo(vr::TextureID_t textureID, vr::RenderModel_TextureMap_t* texture)
        {
            this->textureID = textureID;
            this->texture = texture;
        }

        vr::TextureID_t textureID;
        vr::RenderModel_TextureMap_t* texture;
    };
    StaticVector<RenderModelTextureInfo, vr::k_unMaxTrackedDeviceCount> loadingRenderModelTextures;
    StaticVector<RenderModelTextureInfo, vr::k_unMaxTrackedDeviceCount> loadedRenderModelTextures;
};


//Implementation---------------------------------------------------------------
OpenVRContext::OpenVRContext(Allocator* allocator, OpenVRSystem* vrSystem) : 
    AllocatedClass(allocator), 
    impl(AllocatedClass::New<Impl>(allocator, FINJIN_CALLER_ARGUMENTS, this))
{    
    impl->vrSystem = vrSystem;
}

OpenVRContext::~OpenVRContext()
{    
}

void OpenVRContext::Create(const Settings& settings, Error& error)
{
    assert(settings.assetReader != nullptr);

    FINJIN_ERROR_METHOD_START(error);

    FINJIN_ENGINE_CHECK_IMPL_NOT_NULL(impl, error);

    //Copy settings
    impl->settings = settings;

    impl->initializationStatus.SetStatus(OperationStatus::STARTED);

    //Detect runtime
    impl->hasRuntime = vr::VR_IsRuntimeInstalled();
    if (!impl->hasRuntime)
    {
        //No runtime drivers installed. This isn't an error condition
        impl->initializationStatus.SetStatus(OperationStatus::SUCCESS);

        return;
    }

    if (!vr::VR_IsHmdPresent())
    {
        //No headset. This isn't an error condition.
        impl->initializationStatus.SetStatus(OperationStatus::SUCCESS);

        return;
    }

    //Initialize
    auto vrError = vr::VRInitError_None;
    impl->ivrSystem = vr::VR_Init(&vrError, vr::VRApplication_Scene);
    if (vrError != vr::VRInitError_None)
    {
        impl->initializationStatus.SetStatus(OperationStatus::FAILURE);
        
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to initialize VR runtime: '%1%'", vr::VR_GetVRInitErrorAsEnglishDescription(vrError)));
        return;
    }

    impl->InitializeDistortionMesh();

    //Handle already connected devices
    for (vr::TrackedDeviceIndex_t deviceIndex = 0; deviceIndex < vr::k_unMaxTrackedDeviceCount; deviceIndex++)
    {
        auto& device = impl->devices[deviceIndex];
        auto deviceImpl = device.GetImpl();

        //Set device index
        deviceImpl->deviceIndex = deviceIndex;

        //Get device initial state
        if (impl->ivrSystem->GetControllerState(deviceIndex, &deviceImpl->controllerState))
        {
            impl->GetDeviceInfo(device);
            if (device.IsConnected() && impl->settings.addDeviceHandler != nullptr)
                impl->settings.addDeviceHandler(this, deviceIndex);
        }
    }

    //Done
    impl->initializationStatus.SetStatus(OperationStatus::SUCCESS);
}

void OpenVRContext::Destroy()
{    
    for (vr::TrackedDeviceIndex_t deviceIndex = 0; deviceIndex < vr::k_unMaxTrackedDeviceCount; deviceIndex++)
    {
        auto& device = impl->devices[deviceIndex];

        if (impl->settings.removeDeviceHandler != nullptr)
            impl->settings.removeDeviceHandler(this, deviceIndex);
    }

    vr::VR_Shutdown();
}

const OperationStatus& OpenVRContext::GetInitializationStatus() const
{
    return impl->initializationStatus;
}

void OpenVRContext::GetSelectorComponents(AssetPathSelector& result)
{
}

const OpenVRContext::Settings& OpenVRContext::GetSettings() const
{
    return impl->settings;
}

void OpenVRContext::UpdateInputDevices(SimpleTimeDelta elapsedTime)
{
    //Initialize if necessary
    if (impl->ivrSystem == nullptr && impl->hasRuntime && vr::VR_IsHmdPresent())
    {
        auto vrError = vr::VRInitError_None;
        impl->ivrSystem = vr::VR_Init(&vrError, vr::VRApplication_Scene);
        if (vrError == vr::VRInitError_None)
            return;
    }
    if (impl->ivrSystem == nullptr)
        return;
    
    //Clear previously changed state
    for (vr::TrackedDeviceIndex_t deviceIndex = 0; deviceIndex < vr::k_unMaxTrackedDeviceCount; deviceIndex++)
    {
        auto& device = impl->devices[deviceIndex];
        auto deviceImpl = device.GetImpl();

        deviceImpl->isNewConnection = false;
        
        deviceImpl->gameControllerState.ClearChanged();
    }

    //Consume all OpenVR events
    vr::VREvent_t event;
    while (impl->ivrSystem->PollNextEvent(&event, sizeof(event)))
    {
        switch (event.eventType)
	    {
	        case vr::VREvent_TrackedDeviceActivated:
            {
                auto& device = impl->devices[event.trackedDeviceIndex];
                auto deviceImpl = device.GetImpl();

                //The connection is only considered new if the deviceClass has not yet been set 
                deviceImpl->isNewConnection = deviceImpl->vrDeviceClass == vr::TrackedDeviceClass_Invalid;
                
                impl->GetDeviceInfo(device);
                
                deviceImpl->gameControllerState.connectionChanged = true;

                break;
            }
            case vr::VREvent_TrackedDeviceDeactivated:
            {
                auto& device = impl->devices[event.trackedDeviceIndex];
                auto deviceImpl = device.GetImpl();
                
                FINJIN_ZERO_ITEM(deviceImpl->controllerState);
                FINJIN_ZERO_ITEM(deviceImpl->controllerPoseState);
                deviceImpl->gameControllerState.Reset();
                                
                deviceImpl->gameControllerState.connectionChanged = true;

                break;
            }
            case vr::VREvent_TrackedDeviceUpdated:
            {
                //One or more of the properties of a tracked device have changed.Data is not used for this event.
                //impl->devices[event.trackedDeviceIndex].GetImpl()->GetInfo(impl->ivrSystem);
                FINJIN_DEBUG_LOG_INFO("Device %1% properties updated.", event.trackedDeviceIndex);
                break;
            }
            case vr::VREvent_InputFocusCaptured:
            {
                //Another application has called CaptureInputFocus.The process struct in data identifies the process.This event is not sent to the application that captured the input focus.
                FINJIN_DEBUG_LOG_INFO("Input focus captured by another app.");
                break;
            }
            case vr::VREvent_InputFocusReleased:
            {
                //Another application has called ReleaseInputFocus.The process struct in data identifies the process.This event is not sent to the application that released the input focus.
                FINJIN_DEBUG_LOG_INFO("Input focus released by another app.");
                break;
            }
            case vr::VREvent_InputFocusChanged:
            {
                //Another application has called ReleaseInputFocus.The process struct in data identifies the process.This event is not sent to the application that released the input focus.
                FINJIN_DEBUG_LOG_INFO("Input focus changed.");
                break;
            }
            case vr::VREvent_ChaperoneUniverseHasChanged:
            {
                FINJIN_DEBUG_LOG_INFO("Chaperone universe changed.");
                break;
            }
            case vr::VREvent_DashboardActivated:
            {
                FINJIN_DEBUG_LOG_INFO("Dashboard activated.");
                break;
            }
            case vr::VREvent_DashboardDeactivated:
            {
                FINJIN_DEBUG_LOG_INFO("Dashboard deactivated.");
                break;
            }
            case vr::VREvent_StatusUpdate:
            {
                FINJIN_DEBUG_LOG_INFO("VR status update.");
                break;
            }
            default:
            {
                //FINJIN_DEBUG_LOG_INFO("VR Event: %1%", event.eventType);
                break;
            }
		}
    }

    //Poll devices
    for (vr::TrackedDeviceIndex_t deviceIndex = 0; deviceIndex < vr::k_unMaxTrackedDeviceCount; deviceIndex++)
    {
        auto& device = impl->devices[deviceIndex];
        auto deviceImpl = device.GetImpl();

        if (device.IsConnected())
        {
            if (deviceImpl->vrDeviceClass == vr::TrackedDeviceClass_Controller)
            {
                impl->ivrSystem->GetControllerStateWithPose(vr::TrackingUniverseStanding, deviceIndex, &deviceImpl->controllerState, &deviceImpl->controllerPoseState);

                for (int axisIndex = 0; axisIndex < vr::k_unControllerStateAxisCount; axisIndex++)
                {
                    deviceImpl->gameControllerState.axes[axisIndex * 2].Update(deviceImpl->controllerState.rAxis[axisIndex].x);
                    deviceImpl->gameControllerState.axes[axisIndex * 2 + 1].Update(deviceImpl->controllerState.rAxis[axisIndex].y);
                }

                for (int buttonIndex = 0; buttonIndex < vr::k_EButton_Max; buttonIndex++)
                {
                    deviceImpl->gameControllerState.buttons[buttonIndex].Update((deviceImpl->controllerState.ulButtonPressed & 1ull << buttonIndex) != 0);
                }

                if (deviceImpl->controllerPoseState.bPoseIsValid && !deviceImpl->locators.empty())
                {
                    auto& locator = deviceImpl->locators[0];

                    locator.orientationMatrix(0, 0) = deviceImpl->controllerPoseState.mDeviceToAbsoluteTracking.m[0][0];
                    locator.orientationMatrix(0, 1) = deviceImpl->controllerPoseState.mDeviceToAbsoluteTracking.m[1][0];
                    locator.orientationMatrix(0, 2) = deviceImpl->controllerPoseState.mDeviceToAbsoluteTracking.m[2][0];

                    locator.orientationMatrix(1, 0) = deviceImpl->controllerPoseState.mDeviceToAbsoluteTracking.m[0][1];
                    locator.orientationMatrix(1, 1) = deviceImpl->controllerPoseState.mDeviceToAbsoluteTracking.m[1][1];
                    locator.orientationMatrix(1, 2) = deviceImpl->controllerPoseState.mDeviceToAbsoluteTracking.m[2][1];

                    locator.orientationMatrix(2, 0) = deviceImpl->controllerPoseState.mDeviceToAbsoluteTracking.m[0][2];
                    locator.orientationMatrix(2, 1) = deviceImpl->controllerPoseState.mDeviceToAbsoluteTracking.m[1][2];
                    locator.orientationMatrix(2, 2) = deviceImpl->controllerPoseState.mDeviceToAbsoluteTracking.m[2][2];

                    MathVector3 position(deviceImpl->controllerPoseState.mDeviceToAbsoluteTracking.m[0][3], deviceImpl->controllerPoseState.mDeviceToAbsoluteTracking.m[1][3], deviceImpl->controllerPoseState.mDeviceToAbsoluteTracking.m[2][3]);
                    locator.position.SetMeters(position);

                    MathVector3 velocity(deviceImpl->controllerPoseState.vVelocity.v[0], deviceImpl->controllerPoseState.vVelocity.v[1], deviceImpl->controllerPoseState.vVelocity.v[2]);
                    locator.velocity.SetMeters(velocity);
                }
                
                if (deviceImpl->forceFeedback.IsActive())
                {
                    impl->SetForces(deviceImpl);

                    deviceImpl->forceFeedback.Update(elapsedTime);
                }
            }

            if (device.IsNewConnection() && impl->settings.addDeviceHandler != nullptr)
                impl->settings.addDeviceHandler(this, deviceIndex);
        }
    }

    //Update async resource requests
    impl->UpdateAsyncResources();
}

void OpenVRContext::UpdateCompositor()
{
    //Uncomment this once the compositor is actually being used (when OpenVR properly supports D3D12 and/or Vulkan)
    //vr::VRCompositor()->WaitGetPoses(impl->deviceRenderPoseStates.data(), impl->deviceRenderPoseStates.size(), nullptr, 0);
}

void OpenVRContext::Execute(VREvents& events, VRCommands& commands, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);
}

size_t OpenVRContext::GetDeviceCount() const
{
    return impl->devices.size();
}

size_t OpenVRContext::GetDeviceCount(InputDeviceClass deviceClass) const
{
    size_t count = 0;

    for (auto& device : impl->devices)
    {
        if (AnySet(deviceClass & device.GetDeviceClass()))
            count++;
    }

    return count;
}

InputDeviceClass OpenVRContext::GetDeviceClass(size_t index) const
{
    return impl->devices[index].GetDeviceClass();
}

const Utf8String& OpenVRContext::GetDeviceSystemInternalName(size_t index) const
{
    return impl->devices[index].GetSystemInternalName();
}

const Utf8String& OpenVRContext::GetDeviceProductDescriptor(size_t index) const
{
    return impl->devices[index].GetProductDescriptor();
}

const Utf8String& OpenVRContext::GetDeviceInstanceDescriptor(size_t index) const
{
    return impl->devices[index].GetInstanceDescriptor();
}

InputDeviceSemantic OpenVRContext::GetDeviceSemantic(size_t index) const
{
    return impl->devices[index].GetSemantic();
}

void OpenVRContext::SetDeviceSemantic(size_t index, InputDeviceSemantic value)
{
    impl->devices[index].SetSemantic(value);
}

const Utf8String& OpenVRContext::GetDeviceDisplayName(size_t index) const
{
    return impl->devices[index].GetDisplayName();
}

void OpenVRContext::SetDeviceDisplayName(size_t index, const Utf8String& value)
{
    return impl->devices[index].SetDisplayName(value);
}

bool OpenVRContext::IsDeviceConnected(size_t index) const
{
    return impl->devices[index].IsConnected();
}

bool OpenVRContext::DeviceConnectionChanged(size_t index) const
{
    return impl->devices[index].ConnectionChanged();
}

bool OpenVRContext::IsNewDeviceConnection(size_t index) const
{
    return impl->devices[index].IsNewConnection();
}

size_t OpenVRContext::GetDeviceAxisCount(size_t index) const
{
    return impl->devices[index].GetAxisCount();
}

InputAxis* OpenVRContext::GetDeviceAxis(size_t index, size_t axisIndex)
{
    return impl->devices[index].GetAxis(axisIndex);
}

size_t OpenVRContext::GetDeviceButtonCount(size_t index) const
{
    return impl->devices[index].GetButtonCount();
}

InputButton* OpenVRContext::GetDeviceButton(size_t index, size_t buttonIndex)
{
    return impl->devices[index].GetButton(buttonIndex);
}

size_t OpenVRContext::GetLocatorCount(size_t index) const
{
    return impl->devices[index].GetLocatorCount();
}

InputLocator* OpenVRContext::GetLocator(size_t index, size_t locatorIndex)
{
    return impl->devices[index].GetLocator(locatorIndex);
}

void OpenVRContext::AddHapticFeedback(size_t index, const HapticFeedbackSettings* forces, size_t count)
{
    impl->devices[index].AddHapticFeedback(forces, count);
}

void OpenVRContext::StopHapticFeedback(size_t index)
{
    impl->devices[index].StopHapticFeedback();
}

#endif
