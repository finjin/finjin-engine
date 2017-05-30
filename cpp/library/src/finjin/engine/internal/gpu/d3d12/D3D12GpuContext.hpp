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
#include "finjin/common/AllocatedClass.hpp"
#include "finjin/common/Chrono.hpp"
#include "finjin/common/Error.hpp"
#include "finjin/common/OperationStatus.hpp"
#include "finjin/engine/AssetCreationCapability.hpp"
#include "D3D12GpuContextSettings.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    class D3D12System;
    class D3D12GpuContextImpl;
    class D3D12FrameStage;

    class D3D12GpuContext : public AllocatedClass
    {
    public:
        D3D12GpuContext(Allocator* allocator, D3D12System* d3dSystem);
        ~D3D12GpuContext();

        using Settings = D3D12GpuContextSettings;
        using JobPipelineStage = D3D12FrameStage;

        void Create(const Settings& settings, Error& error);
        void Destroy();

        void GetSelectorComponents(AssetPathSelector& result);

        const Settings& GetSettings() const;

        size_t GetExternalAssetFileExtensions(StaticVector<Utf8String, EngineConstants::MAX_EXTERNAL_ASSET_FILE_EXTENSIONS>& extensions, AssetClass assetClass, Error& error);
        AssetCreationCapability GetAssetCreationCapabilities(AssetClass assetClass) const;

        ScreenCaptureCapability GetScreenCaptureCapabilities() const;
        ScreenCaptureResult GetScreenCapture(ScreenCapture& screenCapture, JobPipelineStage& frameStage);

        bool ToggleFullScreenExclusive(Error& error);
        bool StartResizeTargets(bool minimized, Error& error);
        void FinishResizeTargets(Error& error);

        void CreateMeshFromMainThread(FinjinMesh& mesh, Error& error);
        void CreateTextureFromMainThread(FinjinTexture& texture, Error& error);
        void CreateMaterialFromMainThread(FinjinMaterial& material, Error& error); //Will always generate an error
        void CreateLightFromMainThread(FinjinSceneObjectLight& light, Error& error);

        JobPipelineStage& GetFrameStage(size_t index);

        JobPipelineStage& StartFrameStage(size_t index, SimpleTimeDelta elapsedTime, SimpleTimeCounter totalElapsedTime);
        void Execute(JobPipelineStage& frameStage, GpuEvents& events, GpuCommands& commands, Error& error);
        void FinishFrameStage(JobPipelineStage& frameStage);
        void PresentFrameStage(JobPipelineStage& frameStage, const RenderStatus& renderStatus, size_t presentSyncIntervalOverride, Error& error);

        void FlushGpu();

        D3D12GpuContextImpl* GetImpl();

    private:
        std::unique_ptr<D3D12GpuContextImpl> impl;
    };

} }
