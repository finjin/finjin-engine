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
#include "finjin/common/DomainInformation.hpp"
#include "finjin/common/StandardPaths.hpp"
#include "finjin/common/UserInformation.hpp"
#include "finjin/engine/AssetClass.hpp"
#include "finjin/engine/AssetReadQueue.hpp"
#include "finjin/engine/FinjinSceneReader.hpp"
#include "finjin/engine/GpuContext.hpp"
#include "finjin/engine/InputContext.hpp"
#include "finjin/engine/JobPipelineStage.hpp"
#include "finjin/engine/SoundContext.hpp"
#if FINJIN_TARGET_VR_SYSTEM != FINJIN_TARGET_VR_SYSTEM_NONE
    #include "finjin/engine/VRContext.hpp"
#endif


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    class ApplicationViewport;

    class ApplicationViewportUpdateContext : public AllocatedClass
    {
    public:
        ApplicationViewportUpdateContext(Allocator* allocator);
        ~ApplicationViewportUpdateContext();

        virtual bool IsMain() const = 0;
        virtual bool HasFocus() const = 0;

        virtual void RequestFullScreenToggle() = 0;
        virtual void RequestClose() = 0;
        virtual void RequestApplicationExit() = 0;

        void ClearCommands();

        void ClearEvents();

        void StartPoll();
        void FinishPoll();

        void Execute(GpuContext::JobPipelineStage& frameStage, Error& error);

    public:
    #if FINJIN_TARGET_VR_SYSTEM != FINJIN_TARGET_VR_SYSTEM_NONE
        VRContext* vrContext; //May be null
    #endif
        InputContext* inputContext;
        SoundContext* soundContext;
        GpuContext* gpuContext;

        SimpleTimeDelta elapsedTime;
        int fixedSteps;
        SimpleTimeDelta fixedStepTime;
        JobPipelineStage* jobPipelineStage;
        JobSystem* jobSystem;
        AssetReadQueue* assetReadQueue;
        EnumArray<AssetClass, AssetClass::COUNT, AssetClassFileReader>* assetClassFileReaders;
        StandardPaths* standardPaths;
        UserInformation* userInfo;
        DomainInformation* domainInfo;

        NewAssetsResult<FinjinScene> newScenes;
        NewAssetsResult<FinjinSound> newSounds;
        NewAssetsResult<FinjinMesh> newMeshes;
        NewAssetsResult<FinjinTexture> newTextures;
        NewAssetsResult<FinjinMaterial> newMaterials;

    #if FINJIN_TARGET_VR_SYSTEM != FINJIN_TARGET_VR_SYSTEM_NONE
        VREvents vrEvents;
        VRCommands vrCommands;
    #endif

        InputEvents inputEvents;
        InputCommands inputCommands;

        SoundEvents soundEvents;
        SoundCommands soundCommands;

        GpuEvents gpuEvents;
        GpuCommands gpuCommands;
    };

    class ApplicationViewportRenderContext : public AllocatedClass
    {
    public:
        ApplicationViewportRenderContext(Allocator* allocator);
        ~ApplicationViewportRenderContext();

        virtual bool IsMain() const = 0;
        virtual bool HasFocus() const = 0;

    public:
    #if FINJIN_TARGET_VR_SYSTEM != FINJIN_TARGET_VR_SYSTEM_NONE
        VRContext* vrContext;
    #endif
        InputContext* inputContext;
        SoundContext* soundContext;
        GpuContext* gpuContext;

        JobPipelineStage* jobPipelineStage;
        RenderStatus renderStatus;
        size_t presentSyncIntervalOverride;
    };

    class ApplicationViewportDelegate : public AllocatedClass
    {
    public:
        ApplicationViewportDelegate(Allocator* allocator) : AllocatedClass(allocator) {}
        virtual ~ApplicationViewportDelegate() {}

        enum class UpdateResult
        {
            LOGIC_ONLY,
            STARTED_FRAME
        };

        virtual UpdateResult Update(ApplicationViewportUpdateContext& updateContext, Error& error) { return UpdateResult::LOGIC_ONLY; }
        virtual void FinishFrame(ApplicationViewportRenderContext& renderContext, Error& error) {}

        virtual void OnApplicationViewportClosing(ApplicationViewport* appViewport) {}
        virtual void OnApplicationViewportResized(ApplicationViewport* appViewport) {}
        virtual void OnApplicationViewportLostFocus(ApplicationViewport* appViewport) {}
        virtual void OnApplicationViewportGainedFocus(ApplicationViewport* appViewport) {}
    };

} }
