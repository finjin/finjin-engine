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

#if FINJIN_TARGET_GPU_SYSTEM == FINJIN_TARGET_GPU_SYSTEM_VULKAN

#include "VulkanGpuContext.hpp"
#include "finjin/common/DebugLog.hpp"
#include "finjin/engine/OSWindow.hpp"
#include "VulkanGpuContextImpl.hpp"
#include "VulkanSystem.hpp"
#include "VulkanSystemImpl.hpp"
#include "VulkanUtilities.hpp"

using namespace Finjin::Engine;


//Implementation----------------------------------------------------------------
VulkanGpuContext::VulkanGpuContext(Allocator* allocator, VulkanSystem* vulkanSystem) :
    AllocatedClass(allocator),
    impl(AllocatedClass::New<VulkanGpuContextImpl>(allocator, FINJIN_CALLER_ARGUMENTS))
{
    impl->vulkanSystem = vulkanSystem;
}

VulkanGpuContext::~VulkanGpuContext()
{
}

void VulkanGpuContext::Create(const Settings& settings, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    FINJIN_ENGINE_CHECK_IMPL_NOT_NULL(impl, error);

    impl->Create(settings, error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

void VulkanGpuContext::Destroy()
{
    impl->Destroy();
}

void VulkanGpuContext::GetSelectorComponents(AssetPathSelector& result)
{
    result.Set(impl->settings.initialAssetFileSelector, FINJIN_GPU_CONTEXT_ASSET_PATH_COMPONENTS);
}

const VulkanGpuContext::Settings& VulkanGpuContext::GetSettings() const
{
    return impl->settings;
}

size_t VulkanGpuContext::GetExternalAssetFileExtensions(StaticVector<Utf8String, EngineConstants::MAX_EXTERNAL_ASSET_FILE_EXTENSIONS>& extensions, AssetClass assetClass, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    size_t count = 0;

    switch (assetClass)
    {
        case AssetClass::SHADER:
        {
            for (auto ext : { "spv" })
            {
                if (extensions.push_back(ext).HasErrorOrValue(false))
                {
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to add '%1%' extension.", ext));
                    return count;
                }
                count++;
            }
            break;
        }
        case AssetClass::TEXTURE:
        {
            for (auto ext : { "ktx", "astc", "png" })
            {
                if (extensions.push_back(ext).HasErrorOrValue(false))
                {
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to add '%1%' extension.", ext));
                    return count;
                }
                count++;
            }
            break;
        }
    }

    return count;
}

AssetCreationCapability VulkanGpuContext::GetAssetCreationCapabilities(AssetClass assetClass) const
{
    switch (assetClass)
    {
        case AssetClass::MATERIAL: return AssetCreationCapability::FRAME_THREAD;
        case AssetClass::MESH: return AssetCreationCapability::TWO_STEP;
        case AssetClass::TEXTURE: return AssetCreationCapability::TWO_STEP;
        default: return AssetCreationCapability::MAIN_THREAD;
    }
}

ScreenCaptureCapability VulkanGpuContext::GetScreenCaptureCapabilities() const
{
    return ScreenCaptureCapability::GPU_INTERNAL;
}

ScreenCaptureResult VulkanGpuContext::GetScreenCapture(ScreenCapture& screenCapture, JobPipelineStage& frameStage)
{
    return impl->GetScreenCapture(screenCapture, frameStage);
}

bool VulkanGpuContext::ToggleFullScreenExclusive(Error& error)
{
    return false;
}

bool VulkanGpuContext::StartModifyScreenRenderTargets(Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    impl->DestroyScreenSizeDependentResources(true);

    return true;
}

void VulkanGpuContext::FinishModifyScreenRenderTargets(Error& error)
{
    impl->CreateScreenSizeDependentResources(error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to create screen size dependent resources.");
        return;
    }
}

void VulkanGpuContext::CreateMeshFromMainThread(FinjinMesh& mesh, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    impl->CreateMeshFromMainThread(&mesh, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create mesh '%1%'.", mesh.name));
        return;
    }
}

void VulkanGpuContext::CreateTextureFromMainThread(FinjinTexture& texture, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    impl->CreateTextureFromMainThread(&texture, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create texture '%1%'.", texture.name));
        return;
    }
}

void VulkanGpuContext::CreateMaterialFromMainThread(FinjinMaterial& material, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    FINJIN_SET_ERROR(error, "Creating materials on main thread not allowed.");
}

void VulkanGpuContext::CreateLightFromMainThread(FinjinSceneObjectLight& light, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    impl->CreateLightFromMainThread(&light, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create light '%1%'.", light.name));
        return;
    }
}

VulkanGpuContext::JobPipelineStage& VulkanGpuContext::GetFrameStage(size_t index)
{
    return impl->frameStages[index];
}

VulkanGpuContext::JobPipelineStage& VulkanGpuContext::StartFrameStage(size_t index, SimpleTimeDelta elapsedTime, SimpleTimeCounter totalElapsedTime)
{
    return impl->StartFrameStage(index, elapsedTime, totalElapsedTime);
}

void VulkanGpuContext::Execute(JobPipelineStage& frameStage, GpuEvents& events, GpuCommands& commands, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    impl->Execute(frameStage, events, commands, error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

void VulkanGpuContext::FinishFrameStage(JobPipelineStage& frameStage)
{
    impl->UpdateResourceGpuResidencyStatus();
}

void VulkanGpuContext::PresentFrameStage(JobPipelineStage& frameStage, const RenderStatus& renderStatus, size_t presentSyncIntervalOverride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    impl->PresentFrameStage(frameStage, renderStatus, presentSyncIntervalOverride, error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

void VulkanGpuContext::FlushGpu()
{
    impl->FlushGpu();
}

VulkanGpuContextImpl* VulkanGpuContext::GetImpl()
{
    return impl.get();
}

#endif
