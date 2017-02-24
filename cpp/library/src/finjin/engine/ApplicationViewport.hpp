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


//Includes---------------------------------------------------------------------
#include "finjin/engine/ApplicationViewportDelegate.hpp"


//Classes----------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;
    
    class OSWindow;

    class ApplicationViewport : public AllocatedClass
    {
        friend class Application;
        friend class ApplicationViewportsController;

    public:
        ApplicationViewport(Allocator* allocator);
        virtual ~ApplicationViewport();

        ApplicationViewportDelegate* GetDelegate();
        void SetDelegate(std::unique_ptr<ApplicationViewportDelegate>&& del);

        OSWindow* GetOSWindow();
        
        bool IsMain() const;
        void SetMain(bool value);

        bool HasFocus() const;

    #if FINJIN_TARGET_VR_SYSTEM != FINJIN_TARGET_VR_SYSTEM_NONE
        VRContext* DetachVRContext();
        VRContext* GetVRContext();
        void SetVRContext(std::unique_ptr<VRContext>&& context);
    #endif
        
        InputContext* DetachInputContext();
        InputContext* GetInputContext();
        void SetInputContext(std::unique_ptr<InputContext>&& context);

        SoundContext* DetachSoundContext();
        SoundContext* GetSoundContext();
        void SetSoundContext(std::unique_ptr<SoundContext>&& context);
        
        GpuContext* DetachGpuContext();
        GpuContext* GetGpuContext();
        void SetGpuContext(std::unique_ptr<GpuContext>&& context);

        void CreateAssetClassFileReaders(AssetFileReader& assetFileReader, const AssetPathSelector& applicationAssetFileSelector, Error& error);
        EnumValues<AssetClass, AssetClass::COUNT, AssetClassFileReader>& GetAssetClassFileReaders();

        bool NotifyWindowResized();
        
        void RequestFullScreenToggle();
        
        bool CloseRequested() const;
        void RequestClose();

        bool ExitApplicationRequested() const;
        void RequestApplicationExit();

        bool NeedsToModifyRenderTarget() const;
        void ApplyModifyRenderTarget(Error& error);

        void ApplyRenderTargetsResize(Error& error);

        void ApplyFullScreenToggle(Error& error);
        void ApplyFullScreenToggle(bool needsExclusiveToggle, Error& error);

        void OnTick(JobSystem& jobSystem, Error& error);
        void FinishWork(bool continueRendering);
        
        int Update(JobSystem& jobSystem, Error& error);
        void FinishFrame(bool continueRendering, size_t presentSyncIntervalOverride, Error& error);

        virtual bool StartResizeTargets(Error& error);
        virtual void FinishResizeTargets(Error& error);
        
    private:
        void Init();
        
        void SetOSWindow(std::unique_ptr<OSWindow>&& osWindow);

        void ConfigureJobPipeline(size_t renderBuffering, size_t pipelineSize);
        size_t GetJobPipelineSize() const;
        void SetJobPipelineStageData
            (
            size_t index, 
            std::unique_ptr<ApplicationViewportUpdateContext>&& updateContext, 
            std::unique_ptr<ApplicationViewportRenderContext>&& renderContext
            );
        
        void ResetUpdateAndRenderingCounters();

    private:
        struct Impl;
        std::unique_ptr<Impl> impl;
    };

} }
