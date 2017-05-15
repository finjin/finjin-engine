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

#if FINJIN_TARGET_GPU_SYSTEM == FINJIN_TARGET_GPU_SYSTEM_METAL

#include "MetalGpuContext.hpp"
#include "finjin/common/MemorySize.hpp"
#include "MetalGpuContextImpl.hpp"
#include "MetalSystem.hpp"
#include "MetalSystemImpl.hpp"
#include "OSWindow.hpp"
#include "OSWindowImpl.hpp"

using namespace Finjin::Engine;


//Implementation----------------------------------------------------------------
MetalGpuContext::MetalGpuContext(Allocator* allocator, MetalSystem* metalSystem) :
    AllocatedClass(allocator),
    impl(AllocatedClass::New<MetalGpuContextImpl>(allocator, FINJIN_CALLER_ARGUMENTS))
{
    impl->metalSystem = metalSystem;
}

MetalGpuContext::~MetalGpuContext()
{
}

void MetalGpuContext::Create(const Settings& settings, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    FINJIN_ENGINE_CHECK_IMPL_NOT_NULL(impl, error);

    impl->Create(settings, error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

void MetalGpuContext::Destroy()
{
    impl->Destroy();
}

void MetalGpuContext::GetSelectorComponents(AssetPathSelector& result)
{
    result.Set(impl->settings.initialAssetFileSelector, FINJIN_GPU_CONTEXT_ASSET_PATH_COMPONENTS);
}

const MetalGpuContext::Settings& MetalGpuContext::GetSettings() const
{
    return impl->settings;
}

size_t MetalGpuContext::GetExternalAssetFileExtensions(StaticVector<Utf8String, EngineConstants::MAX_EXTERNAL_ASSET_FILE_EXTENSIONS>& extensions, AssetClass assetClass, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    size_t count = 0;
    
    switch (assetClass)
    {
        case AssetClass::SHADER:
        {
            for (auto ext : { "metallib" })
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
            for (auto ext : { "png" })
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
        default: break;
    }

    return count;
}

AssetCreationCapability MetalGpuContext::GetAssetCreationCapabilities(AssetClass assetClass) const
{
    return AssetCreationCapability::MAIN_THREAD;
}

bool MetalGpuContext::ToggleFullScreenExclusive(Error& error)
{
    return false;
}

bool MetalGpuContext::StartResizeTargets(bool minimized, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    impl->DestroyScreenSizeDependentResources(true);

    return true;
}

void MetalGpuContext::FinishResizeTargets(Error& error)
{
    impl->CreateScreenSizeDependentResources(error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to create screen size dependent resources.");
        return;
    }
}

void MetalGpuContext::CreateMeshFromMainThread(FinjinMesh& mesh, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    impl->CreateMeshFromMainThread(&mesh, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create mesh '%1%'.", mesh.name));
        return;
    }
}

void MetalGpuContext::CreateTextureFromMainThread(FinjinTexture& texture, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    impl->CreateTextureFromMainThread(&texture, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create texture '%1%'.", texture.name));
        return;
    }
}

void MetalGpuContext::CreateMaterialFromMainThread(FinjinMaterial& material, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    impl->CreateMaterialFromMainThread(&material, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create material '%1%'.", material.name));
        return;
    }
}

void MetalGpuContext::CreateLightFromMainThread(FinjinSceneObjectLight& light, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    impl->CreateLightFromMainThread(&light, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create light '%1%'.", light.name));
        return;
    }
}

MetalGpuContext::JobPipelineStage& MetalGpuContext::GetFrameStage(size_t index)
{
    return impl->frameStages[index];
}

MetalGpuContext::JobPipelineStage& MetalGpuContext::StartFrameStage(size_t index, SimpleTimeDelta elapsedTime, SimpleTimeCounter totalElapsedTime)
{
    return impl->StartFrameStage(index, elapsedTime, totalElapsedTime);
}

void MetalGpuContext::StartBackFrameBufferRender(JobPipelineStage& frameStage)
{
    //auto& frameBuffer = impl->frameBuffers[frameStage.frameBufferIndex];
}

void MetalGpuContext::Execute(JobPipelineStage& frameStage, GpuEvents& events, GpuCommands& commands, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    impl->Execute(frameStage, events, commands, error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

void MetalGpuContext::FinishFrameStage(JobPipelineStage& frameStage)
{
    //std::cout << "VulkanGpuContext::FinishFrameStage()" << std::endl;
}

void MetalGpuContext::FinishBackFrameBufferRender(JobPipelineStage& frameStage, bool continueRendering, bool modifyingRenderTarget, size_t presentSyncIntervalOverride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    impl->FinishBackFrameBufferRender(frameStage, continueRendering, modifyingRenderTarget, presentSyncIntervalOverride, error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

void MetalGpuContext::FlushGpu()
{
    impl->FlushGpu();
}

MetalGpuContextImpl* MetalGpuContext::GetImpl()
{
    return impl.get();
}

#endif
