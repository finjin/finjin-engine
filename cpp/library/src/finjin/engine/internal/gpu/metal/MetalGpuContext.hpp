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
#include "finjin/common/Allocator.hpp"
#include "finjin/common/Chrono.hpp"
#include "finjin/common/Error.hpp"
#include "finjin/common/OperationStatus.hpp"
#include "finjin/engine/AssetCreationCapability.hpp"
#include "MetalGpuContextSettings.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    class MetalSystem;
    class MetalGpuContextImpl;
    class MetalFrameStage;

    class MetalGpuContext : public AllocatedClass
    {
    public:
        MetalGpuContext(Allocator* allocator, MetalSystem* metalSystem);
        ~MetalGpuContext();

        using Settings = MetalGpuContextSettings;
        using JobPipelineStage = MetalFrameStage;

        void Create(const Settings& settings, Error& error);
        void Destroy();

        void GetSelectorComponents(AssetPathSelector& result);

        const Settings& GetSettings() const;

        size_t GetExternalAssetFileExtensions(StaticVector<Utf8String, EngineConstants::MAX_EXTERNAL_ASSET_FILE_EXTENSIONS>& extensions, AssetClass assetClass, Error& error);
        AssetCreationCapability GetAssetCreationCapabilities(AssetClass assetClass) const;

        bool ToggleFullScreenExclusive(Error& error);
        bool StartResizeTargets(bool minimized, Error& error);
        void FinishResizeTargets(Error& error);

        void CreateMeshFromMainThread(FinjinMesh& mesh, Error& error);
        void CreateTextureFromMainThread(FinjinTexture& texture, Error& error);
        void CreateMaterialFromMainThread(FinjinMaterial& material, Error& error); //Will always generate an error
        void CreateLightFromMainThread(FinjinSceneObjectLight& light, Error& error);

        JobPipelineStage& GetFrameStage(size_t index);

        JobPipelineStage& StartFrameStage(size_t index, SimpleTimeDelta elapsedTime, SimpleTimeCounter totalElapsedTime);
        void StartBackFrameBufferRender(JobPipelineStage& frameStage);
        void Execute(JobPipelineStage& frameStage, GpuEvents& events, GpuCommands& commands, Error& error);
        void FinishFrameStage(JobPipelineStage& frameStage);
        void FinishBackFrameBufferRender(JobPipelineStage& frameStage, bool continueRendering, bool modifyingRenderTarget, size_t presentSyncIntervalOverride, Error& error);

        void FlushGpu();

        MetalGpuContextImpl* GetImpl();

    private:
        std::unique_ptr<MetalGpuContextImpl> impl;
    };

} }