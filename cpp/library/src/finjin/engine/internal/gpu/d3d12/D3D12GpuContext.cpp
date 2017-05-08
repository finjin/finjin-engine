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

#if FINJIN_TARGET_GPU_SYSTEM == FINJIN_TARGET_GPU_SYSTEM_D3D12

#include "D3D12GpuContext.hpp"
#include "finjin/common/DebugLog.hpp"
#include "finjin/common/Math.hpp"
#include "finjin/common/MemorySize.hpp"
#include "finjin/engine/OSWindow.hpp"
#include "D3D12GpuContextImpl.hpp"
#include "D3D12System.hpp"
#include "D3D12SystemImpl.hpp"

using namespace Finjin::Engine;


//Implementation----------------------------------------------------------------
D3D12GpuContext::D3D12GpuContext(Allocator* allocator, D3D12System* d3dSystem) :
    AllocatedClass(allocator),
    impl(AllocatedClass::New<D3D12GpuContextImpl>(allocator, FINJIN_CALLER_ARGUMENTS))
{
    impl->d3dSystem = d3dSystem;
}

D3D12GpuContext::~D3D12GpuContext()
{
}

void D3D12GpuContext::Create(const Settings& settings, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    FINJIN_ENGINE_CHECK_IMPL_NOT_NULL(impl, error);

    impl->Create(settings, error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

void D3D12GpuContext::Destroy()
{
    impl->Destroy();
}

void D3D12GpuContext::GetSelectorComponents(AssetPathSelector& result)
{
    result.Set
        (
        impl->settings.initialAssetFileSelector,
        AssetPathComponent::GPU_FEATURE_LEVEL,
        AssetPathComponent::GPU_SHADER_MODEL,
        AssetPathComponent::GPU_PREFERRED_TEXTURE_FORMAT,
        AssetPathComponent::GPU_MODEL
        );
}

const D3D12GpuContext::Settings& D3D12GpuContext::GetSettings() const
{
    return impl->settings;
}

size_t D3D12GpuContext::GetExternalAssetFileExtensions(StaticVector<Utf8String, EngineConstants::MAX_EXTERNAL_ASSET_FILE_EXTENSIONS>& extensions, AssetClass assetClass, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    switch (assetClass)
    {
        case AssetClass::SHADER:
        {
            size_t count = 0;
            for (auto ext : { "cso" })
            {
                if (extensions.push_back(ext).HasErrorOrValue(false))
                {
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to add '%1%' extension.", ext));
                    return count;
                }
                count++;
            }
            return count;
        }
        case AssetClass::TEXTURE:
        {
            size_t count = 0;
            for (auto ext : { "dds", "png" })
            {
                if (extensions.push_back(ext).HasErrorOrValue(false))
                {
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to add '%1%' extension.", ext));
                    return count;
                }
                count++;
            }
            return count;
        }
    }

    return 0;
}

AssetCreationCapability D3D12GpuContext::GetAssetCreationCapabilities(AssetClass assetClass) const
{
    switch (assetClass)
    {
        case AssetClass::MATERIAL: return AssetCreationCapability::FRAME_THREAD;
        case AssetClass::MESH: return AssetCreationCapability::TWO_STEP;
        case AssetClass::TEXTURE: return AssetCreationCapability::TWO_STEP;
        default: return AssetCreationCapability::MAIN_THREAD;
    }
}

bool D3D12GpuContext::ToggleFullScreenExclusive(Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto result = impl->ToggleFullScreenExclusive(error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
    return result;
}

bool D3D12GpuContext::StartResizeTargets(bool minimized, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto result = impl->StartResizeTargets(minimized, error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
    return result;
}

void D3D12GpuContext::FinishResizeTargets(Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    impl->FinishResizeTargets(error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

void D3D12GpuContext::CreateMeshFromMainThread(FinjinMesh& mesh, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    impl->CreateMeshFromMainThread(&mesh, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create mesh '%1%'.", mesh.name));
        return;
    }
}

void D3D12GpuContext::CreateTextureFromMainThread(FinjinTexture& texture, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    impl->CreateTextureFromMainThread(&texture, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create texture '%1%'.", texture.name));
        return;
    }
}

void D3D12GpuContext::CreateMaterialFromMainThread(FinjinMaterial& material, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    FINJIN_SET_ERROR(error, "Creating materials on main thread not allowed.");
}

void D3D12GpuContext::CreateLightFromMainThread(FinjinSceneObjectLight& light, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    impl->CreateLightFromMainThread(&light, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create light '%1%'.", light.name));
        return;
    }
}

D3D12GpuContext::JobPipelineStage& D3D12GpuContext::GetFrameStage(size_t index)
{
    return impl->frameStages[index];
}

D3D12GpuContext::JobPipelineStage& D3D12GpuContext::StartFrameStage(size_t index, SimpleTimeDelta elapsedTime, SimpleTimeCounter totalElapsedTime)
{
    return impl->StartFrameStage(index, elapsedTime, totalElapsedTime);
}

void D3D12GpuContext::StartBackFrameBufferRender(JobPipelineStage& frameStage)
{
    //auto& frameBuffer = impl->frameBuffers[frameStage.frameBufferIndex];
}

void D3D12GpuContext::Execute(JobPipelineStage& frameStage, GpuEvents& events, GpuCommands& commands, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    impl->Execute(frameStage, events, commands, error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

void D3D12GpuContext::FinishFrameStage(JobPipelineStage& frameStage)
{
    //auto& frameBuffer = impl->frameBuffers[frameStage.frameBufferIndex];

    impl->UpdateResourceGpuResidencyStatus();
}

void D3D12GpuContext::FinishBackFrameBufferRender(JobPipelineStage& frameStage, bool continueRendering, bool modifyingRenderTarget, size_t presentSyncIntervalOverride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    impl->FinishBackFrameBufferRender(frameStage, continueRendering, modifyingRenderTarget, presentSyncIntervalOverride, error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

void D3D12GpuContext::FlushGpu()
{
    impl->FlushGpu();
}

#endif
