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
#include "finjin/common/Convert.hpp"
#include "finjin/common/DebugLog.hpp"
#include "finjin/common/DynamicVector.hpp"
#include "finjin/common/Math.hpp"
#include "finjin/common/StaticVector.hpp"
#include "finjin/engine/InputComponents.hpp"
#include "OpenVRDeviceImpl.hpp"
#include "OpenVRSystem.hpp"

#if FINJIN_TARGET_GPU_SYSTEM == FINJIN_TARGET_GPU_SYSTEM_D3D12
    #include "finjin/engine/internal/gpu/d3d12/D3D12FrameBuffer.hpp"
    #include "finjin/engine/internal/gpu/d3d12/D3D12GpuContext.hpp"
    #include "finjin/engine/internal/gpu/d3d12/D3D12GpuContextImpl.hpp"
#elif FINJIN_TARGET_GPU_SYSTEM == FINJIN_TARGET_GPU_SYSTEM_VULKAN
    #include "finjin/engine/internal/gpu/vulkan/VulkanFrameBuffer.hpp"
    #include "finjin/engine/internal/gpu/vulkan/VulkanGpuContextImpl.hpp"
    #include "finjin/engine/internal/gpu/vulkan/VulkanSystem.hpp"
    #include "finjin/engine/internal/gpu/vulkan/VulkanSystemImpl.hpp"
#endif

using namespace Finjin::Engine;


//Macros------------------------------------------------------------------------
#define HAPTIC_PULSE_LENGTH_MICROSECONDS 3000


//Local functions---------------------------------------------------------------
static void GetBounds(vr::VRTextureBounds_t& leftBounds, vr::VRTextureBounds_t& rightBounds, size_t textureCount)
{
    if (textureCount == 1)
    {
        //Each eye takes up a half of the texture
        leftBounds.uMin = 0;
        leftBounds.uMax = .5f;
        leftBounds.vMin = 0;
        leftBounds.vMax = 1;

        rightBounds.uMin = .5f;
        rightBounds.uMax = 1.0f;
        rightBounds.vMin = 0;
        rightBounds.vMax = 1;
    }
    else
    {
        //Each eye has its own texture
        leftBounds.uMin = 0;
        leftBounds.uMax = 1;
        leftBounds.vMin = 0;
        leftBounds.vMax = 1;

        rightBounds = leftBounds;
    }
}

template <typename T>
static void GetPoseOrientationMatrix(T& m, const vr::TrackedDevicePose_t& pose)
{
    m(0, 0) = pose.mDeviceToAbsoluteTracking.m[0][0];
    m(0, 1) = pose.mDeviceToAbsoluteTracking.m[0][1];
    m(0, 2) = pose.mDeviceToAbsoluteTracking.m[0][2];

    m(1, 0) = pose.mDeviceToAbsoluteTracking.m[1][0];
    m(1, 1) = pose.mDeviceToAbsoluteTracking.m[1][1];
    m(1, 2) = pose.mDeviceToAbsoluteTracking.m[1][2];

    m(2, 0) = pose.mDeviceToAbsoluteTracking.m[2][0];
    m(2, 1) = pose.mDeviceToAbsoluteTracking.m[2][1];
    m(2, 2) = pose.mDeviceToAbsoluteTracking.m[2][2];
}

static void GetPoseMatrix4(MathMatrix4& m, const vr::TrackedDevicePose_t& pose)
{
    m.setIdentity();
    GetPoseOrientationMatrix(m, pose);

    auto translation = MathAffineTransform(MathTranslation(pose.mDeviceToAbsoluteTracking.m[0][3], pose.mDeviceToAbsoluteTracking.m[1][3], pose.mDeviceToAbsoluteTracking.m[2][3]));

    m = (translation * m).matrix();
}

static void GetInversePoseMatrix4(MathMatrix4& m, const vr::TrackedDevicePose_t& pose)
{
    MathMatrix4 worldMatrix;
    GetPoseMatrix4(worldMatrix, pose);
    m = worldMatrix.inverse();
}

static void GetLocatorFromTrackingPose(InputLocator& locator, const vr::TrackedDevicePose_t& pose)
{
    GetPoseOrientationMatrix(locator.orientationMatrix, pose);

    MathVector3 position(pose.mDeviceToAbsoluteTracking.m[0][3], pose.mDeviceToAbsoluteTracking.m[1][3], pose.mDeviceToAbsoluteTracking.m[2][3]);
    locator.position.SetMeters(position);

    MathVector3 velocity(pose.vVelocity.v[0], pose.vVelocity.v[1], pose.vVelocity.v[2]);
    locator.velocity.SetMeters(velocity);
}

static VRContextInitializationStatus RecoverableVRErrorToInitializationStatus(vr::HmdError vrError)
{
    switch (vrError)
    {
        case vr::VRInitError_Init_HmdNotFound: return VRContextInitializationStatus::NO_HMD_FOUND;
        case vr::VRInitError_Init_NotInitialized: return VRContextInitializationStatus::RUNTIME_NOT_INITIALIZED;
        case vr::VRInitError_Init_InitCanceledByUser: return VRContextInitializationStatus::CANCELED_BY_USER;
        case vr::VRInitError_Init_AnotherAppLaunching: return VRContextInitializationStatus::ANOTHER_APP_LAUNCHING;
    }

    return VRContextInitializationStatus::NONE;
}

static bool IsRecoverableVRError(vr::HmdError vrError)
{
    return RecoverableVRErrorToInitializationStatus(vrError) != VRContextInitializationStatus::NONE;
}


//Local types-------------------------------------------------------------------
struct OpenVRContext::Impl : public AllocatedClass
{
    struct DistortionVertex
    {
        MathVector2 position;
        MathVector2 texCoordRed;
        MathVector2 texCoordGreen;
        MathVector2 texCoordBlue;
    };

    struct DistortionMesh
    {
        DynamicVector<DistortionVertex> verts;
        DynamicVector<uint16_t> indices;
    };

    Impl(Allocator* allocator, OpenVRContext* context) : AllocatedClass(allocator), settings(allocator)
    {
        this->context = context;

        this->hasRuntime = false;

        this->ivrSystem = nullptr;

        this->initializationStatus = VRContextInitializationStatus::NONE;
    }

    void FinishInitialization(Error& error)
    {
        FINJIN_ERROR_METHOD_START(error);

        if (!vr::VRCompositor())
        {
            FINJIN_SET_ERROR(error, "Failed to create compositor.");
            return;
        }

        CreateDistortionMesh(error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to create distortion mesh.");
            return;
        }

        //Handle already connected devices
        for (vr::TrackedDeviceIndex_t deviceIndex = 0; deviceIndex < vr::k_unMaxTrackedDeviceCount; deviceIndex++)
        {
            auto& device = this->devices[deviceIndex];
            auto deviceImpl = device.GetImpl();

            //Set device index
            deviceImpl->deviceIndex = deviceIndex;

            //Get device initial state
            if (this->ivrSystem->GetControllerState(deviceIndex, &deviceImpl->controllerState, sizeof(deviceImpl->controllerState)))
            {
                GetDeviceInfo(device);
                if (device.IsConnected() && this->settings.addDeviceHandler != nullptr)
                    this->settings.addDeviceHandler(this->context, deviceIndex);
            }
        }

        this->initializationStatus = VRContextInitializationStatus::INITIALIZED;
    }

    bool IsInitializationFinished()
    {
        return this->initializationStatus == VRContextInitializationStatus::INITIALIZED;
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

    std::array<uint32_t, 2> GetPreferredRenderTargetDimensions()
    {
        std::array<uint32_t, 2> result;
        if (this->ivrSystem != nullptr)
            this->ivrSystem->GetRecommendedRenderTargetSize(&result[0], &result[1]);
        return result;
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

            auto vrError = vr::VRRenderModels()->LoadRenderModel_Async(modelName.c_str(), &renderModel);
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
        for (size_t modelIndex = 0; modelIndex < this->loadingRenderModels.size();)
        {
            auto& loadingRenderModel = this->loadingRenderModels[modelIndex];
            if (loadingRenderModel.model->diffuseTextureId == textureID)
            {
                UpdateDeviceRenderModels(loadingRenderModel.name, loadingRenderModel.model, texture);
                this->loadingRenderModels.erase(this->loadingRenderModels.begin() + modelIndex);
            }
            else
                modelIndex++;
        }
    }

    void UpdateAsyncResources()
    {
        for (size_t modelIndex = 0; modelIndex < this->loadingRenderModels.size();)
        {
            auto& loadingRenderModel = this->loadingRenderModels[modelIndex];

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
                this->loadingRenderModels.erase(this->loadingRenderModels.begin() + modelIndex);
            }
            else if (vrError != vr::VRRenderModelError_Loading)
            {
                FINJIN_DEBUG_LOG_ERROR("Failed to continue loading VR render model '%1%'.", loadingRenderModel.name);
                this->loadingRenderModels.erase(this->loadingRenderModels.begin() + modelIndex);
            }
            else
                modelIndex++;
        }

        for (size_t textureIndex = 0; textureIndex < this->loadingRenderModelTextures.size();)
        {
            auto& loadingRenderModelTexture = this->loadingRenderModelTextures[textureIndex];

            auto vrError = vr::VRRenderModels()->LoadTexture_Async(loadingRenderModelTexture.textureID, &loadingRenderModelTexture.texture);
            if (vrError == vr::VRRenderModelError_None)
            {
                //Texture finished loading

                //Add to "loaded" vector
                this->loadedRenderModelTextures.push_back(loadingRenderModelTexture);

                //Update devices that need the model that is now finished (due to the texture being finished)
                UpdateDeviceRenderModelsForTexture(loadingRenderModelTexture.textureID, loadingRenderModelTexture.texture);

                //Remove from "loading" vector
                this->loadingRenderModelTextures.erase(this->loadingRenderModelTextures.begin() + textureIndex);
            }
            else if (vrError != vr::VRRenderModelError_Loading)
            {
                FINJIN_DEBUG_LOG_ERROR("Failed to continue loading VR render model texture '%1%'.", loadingRenderModelTexture.textureID);
                this->loadingRenderModelTextures.erase(this->loadingRenderModelTextures.begin() + textureIndex);
            }
            else
                textureIndex++;
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

    void CreateDistortionMesh(Error& error)
    {
        FINJIN_ERROR_METHOD_START(error);

        uint16_t lensGridSegmentCountHorz = 43;
        uint16_t lensGridSegmentCountVert = 43;

        float w = (float)(1.0 / float(lensGridSegmentCountHorz - 1));
        float h = (float)(1.0 / float(lensGridSegmentCountVert - 1));

        float u, v = 0;

        auto& verts = this->distortionMesh.verts;
        if (!verts.CreateEmpty(lensGridSegmentCountVert * lensGridSegmentCountHorz * 2, GetAllocator()))
        {
            FINJIN_SET_ERROR(error, "Failed to allocate vertices.");
            return;
        }

        DistortionVertex vert;

        //Left eye distortion vertices
        float xOffset = -1;
        for (int y = 0; y < lensGridSegmentCountVert; y++)
        {
            for (int x = 0; x < lensGridSegmentCountHorz; x++)
            {
                u = x * w;
                v = 1 - y * h;

                vert.position = MathVector2(xOffset + u, -1 + 2 * y*h);

                vr::DistortionCoordinates_t dc0;
                if (!this->ivrSystem->ComputeDistortion(vr::Eye_Left, u, v, &dc0))
                {
                    FINJIN_SET_ERROR(error, "Failed to compute distortion coordinates for left eye.");
                    return;
                }

                vert.texCoordRed = MathVector2(dc0.rfRed[0], 1 - dc0.rfRed[1]);
                vert.texCoordGreen = MathVector2(dc0.rfGreen[0], 1 - dc0.rfGreen[1]);
                vert.texCoordBlue = MathVector2(dc0.rfBlue[0], 1 - dc0.rfBlue[1]);

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

                vert.position = MathVector2(xOffset + u, -1 + 2 * y*h);

                vr::DistortionCoordinates_t dc0;
                if (!this->ivrSystem->ComputeDistortion(vr::Eye_Right, u, v, &dc0))
                {
                    FINJIN_SET_ERROR(error, "Failed to compute distortion coordinates for right eye.");
                    return;
                }

                vert.texCoordRed = MathVector2(dc0.rfRed[0], 1 - dc0.rfRed[1]);
                vert.texCoordGreen = MathVector2(dc0.rfGreen[0], 1 - dc0.rfGreen[1]);
                vert.texCoordBlue = MathVector2(dc0.rfBlue[0], 1 - dc0.rfBlue[1]);

                verts.push_back(vert);
            }
        }

        auto& indices = this->distortionMesh.indices;
        if (!indices.CreateEmpty((lensGridSegmentCountVert - 1) * (lensGridSegmentCountHorz - 1) * 6 * 2, GetAllocator()))
        {
            FINJIN_SET_ERROR(error, "Failed to allocate indices.");
            return;
        }

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

    bool hasRuntime;

    vr::IVRSystem* ivrSystem;

    VRContextInitializationStatus initializationStatus;

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


//Implementation----------------------------------------------------------------
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

    impl->settings = settings;

    //Detect runtime
    impl->hasRuntime = vr::VR_IsRuntimeInstalled();
    if (!impl->hasRuntime)
    {
        //No runtime drivers installed. This isn't an error condition
        return;
    }

    if (!vr::VR_IsHmdPresent())
    {
        //No headset. This isn't an error condition.
        return;
    }

    //Initialize
    auto vrError = vr::VRInitError_None;
    impl->ivrSystem = vr::VR_Init(&vrError, vr::VRApplication_Scene);
    if (IsRecoverableVRError(vrError))
        impl->initializationStatus = RecoverableVRErrorToInitializationStatus(vrError);
    else if (vrError != vr::VRInitError_None)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to initialize VR runtime: '%1%'", vr::VR_GetVRInitErrorAsEnglishDescription(vrError)));
        return;
    }

    if (vrError == vr::VRInitError_None)
    {
        impl->FinishInitialization(error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to finish initialization.");
            return;
        }
    }
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

void OpenVRContext::GetSelectorComponents(AssetPathSelector& result)
{
}

const OpenVRContext::Settings& OpenVRContext::GetSettings() const
{
    return impl->settings;
}

VRContextInitializationStatus OpenVRContext::GetInitializationStatus() const
{
    return impl->initializationStatus;
}

VRContextInitializationStatus OpenVRContext::TryInitialization(Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    if (!impl->IsInitializationFinished())
    {
        auto vrError = vr::VRInitError_None;
        impl->ivrSystem = vr::VR_Init(&vrError, vr::VRApplication_Scene);
        if (IsRecoverableVRError(vrError))
            impl->initializationStatus = RecoverableVRErrorToInitializationStatus(vrError);
        else if (vrError != vr::VRInitError_None)
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to initialize VR runtime: '%1%'", vr::VR_GetVRInitErrorAsEnglishDescription(vrError)));
            return impl->initializationStatus;
        }

        if (vrError == vr::VRInitError_None)
        {
            impl->FinishInitialization(error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to finish initialization.");
                return impl->initializationStatus;
            }
        }
    }

    return impl->initializationStatus;
}

std::array<uint32_t, 2> OpenVRContext::GetPreferredRenderTargetDimensions()
{
    std::array<uint32_t, 2> result{ 0, 0 };
    if (impl->IsInitializationFinished())
        result = impl->GetPreferredRenderTargetDimensions();
    return result;
}

void OpenVRContext::UpdateInputDevices(SimpleTimeDelta elapsedTime)
{
    if (!impl->IsInitializationFinished())
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
            switch (deviceImpl->vrDeviceClass)
            {
                case vr::TrackedDeviceClass_HMD:
                {
                    impl->ivrSystem->GetControllerStateWithPose(vr::TrackingUniverseStanding, deviceIndex, &deviceImpl->controllerState, sizeof(deviceImpl->controllerState), &deviceImpl->controllerPoseState);

                    if (deviceImpl->controllerPoseState.bPoseIsValid && !deviceImpl->locators.empty())
                    {
                        auto& locator = deviceImpl->locators[0];

                        GetLocatorFromTrackingPose(locator, deviceImpl->controllerPoseState);
                    }

                    break;
                }
                case vr::TrackedDeviceClass_Controller:
                {
                    impl->ivrSystem->GetControllerStateWithPose(vr::TrackingUniverseStanding, deviceIndex, &deviceImpl->controllerState, sizeof(deviceImpl->controllerState), &deviceImpl->controllerPoseState);

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

                        GetLocatorFromTrackingPose(locator, deviceImpl->controllerPoseState);
                    }

                    if (deviceImpl->forceFeedback.IsActive())
                    {
                        impl->SetForces(deviceImpl);

                        deviceImpl->forceFeedback.Update(elapsedTime);
                    }
                    break;
                }
            }

            if (device.IsNewConnection() && impl->settings.addDeviceHandler != nullptr)
                impl->settings.addDeviceHandler(this, deviceIndex);
        }
    }

    //Update async resource requests
    impl->UpdateAsyncResources();
}

void OpenVRContext::GetHeadsetViewRenderMatrix(MathMatrix4& viewMatrix)
{
    auto vrError = vr::VRCompositor()->WaitGetPoses(impl->deviceRenderPoseStates.data(), impl->deviceRenderPoseStates.size(), nullptr, 0);
    assert(vrError == vr::VRCompositorError_None);

    auto& pose = impl->deviceRenderPoseStates[vr::k_unTrackedDeviceIndex_Hmd];
    if (pose.bPoseIsValid)
    {
        GetInversePoseMatrix4(viewMatrix, pose);
    }
    else
    {
        viewMatrix.setIdentity();
        assert(0);
    }
}

void OpenVRContext::SubmitEyeTextures(void* gpuContextImpl, void* gpuFrameBuffer)
{
    vr::Texture_t leftEyeTexture, rightEyeTexture;

    vr::VRTextureBounds_t leftBounds, rightBounds;

#if FINJIN_TARGET_GPU_SYSTEM == FINJIN_TARGET_GPU_SYSTEM_D3D12
    auto d3d12GpuContextImpl = static_cast<D3D12GpuContextImpl*>(gpuContextImpl);
    auto d3d12FrameBuffer = static_cast<D3D12FrameBuffer*>(gpuFrameBuffer);

    vr::D3D12TextureData_t d3d12LeftEyeTexture;
    d3d12LeftEyeTexture.m_pCommandQueue = d3d12GpuContextImpl->graphicsCommandQueue.Get();
    d3d12LeftEyeTexture.m_pResource = d3d12FrameBuffer->renderTarget.colorOutputs[0].resource.Get();
    d3d12LeftEyeTexture.m_nNodeMask = d3d12FrameBuffer->renderTarget.colorOutputs[0].nodeMask;

    auto d3d12RightEyeTexture = d3d12LeftEyeTexture;
    if (d3d12FrameBuffer->renderTarget.colorOutputs.size() > 1)
    {
        d3d12RightEyeTexture.m_pResource = d3d12FrameBuffer->renderTarget.colorOutputs[1].resource.Get();
        d3d12RightEyeTexture.m_nNodeMask = d3d12FrameBuffer->renderTarget.colorOutputs[1].nodeMask;
    }

    leftEyeTexture.eType = vr::TextureType_DirectX12;
    leftEyeTexture.handle = &d3d12LeftEyeTexture;
    leftEyeTexture.eColorSpace = vr::ColorSpace_Auto;

    rightEyeTexture = leftEyeTexture;
    rightEyeTexture.handle = &d3d12RightEyeTexture;

    GetBounds(leftBounds, rightBounds, d3d12FrameBuffer->renderTarget.colorOutputs.size());
#elif FINJIN_TARGET_GPU_SYSTEM == FINJIN_TARGET_GPU_SYSTEM_VULKAN
    auto vulkanGpuContextImpl = static_cast<VulkanGpuContextImpl*>(gpuContextImpl);
    auto vulkanGpuSystem = vulkanGpuContextImpl->vulkanSystem;
    auto vulkanGpuSystemImpl = vulkanGpuSystem->GetImpl();
    auto vulkanFrameBuffer = static_cast<VulkanFrameBuffer*>(gpuFrameBuffer);

    vr::VRVulkanTextureData_t vulkanLeftEyeTexture;
    vulkanLeftEyeTexture.m_nImage = reinterpret_cast<uint64_t>(vulkanFrameBuffer->renderTarget.colorOutputs[0].image);
    vulkanLeftEyeTexture.m_pDevice = vulkanGpuContextImpl->vk.device;
    vulkanLeftEyeTexture.m_pPhysicalDevice = vulkanGpuContextImpl->physicalDevice;
    vulkanLeftEyeTexture.m_pInstance = vulkanGpuSystemImpl->vk.instance;
    vulkanLeftEyeTexture.m_pQueue = vulkanGpuContextImpl->primaryQueues.graphics;
    vulkanLeftEyeTexture.m_nQueueFamilyIndex = vulkanGpuContextImpl->queueFamilyIndexes.graphics;
    vulkanLeftEyeTexture.m_nWidth = vulkanGpuContextImpl->renderTargetSize[0];
    vulkanLeftEyeTexture.m_nHeight = vulkanGpuContextImpl->renderTargetSize[1];
    vulkanLeftEyeTexture.m_nFormat = vulkanGpuContextImpl->settings.colorFormat.actual;
    vulkanLeftEyeTexture.m_nSampleCount = vulkanGpuContextImpl->settings.multisampleCount.actual;

    auto vulkanRightEyeTexture = vulkanLeftEyeTexture;
    if (vulkanFrameBuffer->renderTarget.colorOutputs.size() > 1)
        vulkanRightEyeTexture.m_nImage = reinterpret_cast<uint64_t>(vulkanFrameBuffer->renderTarget.colorOutputs[1].image);

    leftEyeTexture.eType = vr::TextureType_Vulkan;
    leftEyeTexture.handle = &vulkanLeftEyeTexture;
    leftEyeTexture.eColorSpace = vr::ColorSpace_Auto;

    rightEyeTexture = leftEyeTexture;
    rightEyeTexture.handle = &vulkanRightEyeTexture;

    GetBounds(leftBounds, rightBounds, vulkanFrameBuffer->renderTarget.colorOutputs.size());
#endif

    auto submitFlags = vr::Submit_Default; //Submit_LensDistortionAlreadyApplied; //For the future
    auto vrError = vr::VRCompositor()->Submit(vr::Eye_Left, &leftEyeTexture, &leftBounds, submitFlags);
    assert(vrError == vr::VRCompositorError_None);
    vrError = vr::VRCompositor()->Submit(vr::Eye_Right, &rightEyeTexture, &rightBounds, submitFlags);
    assert(vrError == vr::VRCompositorError_None);
}

void OpenVRContext::OnPresentFinish()
{
    vr::VRCompositor()->PostPresentHandoff();
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
