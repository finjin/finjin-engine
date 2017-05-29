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
#include "ApplicationViewport.hpp"
#include "finjin/common/ByteBuffer.hpp"
#include "finjin/common/DebugLog.hpp"
#include "finjin/common/JobSystem.hpp"
#include "finjin/common/StaticVector.hpp"
#include "finjin/engine/OSWindow.hpp"
#include "ApplicationViewportsController.hpp"
#include "PlatformCapabilities.hpp"

using namespace Finjin::Engine;


//Local types-------------------------------------------------------------------
struct ApplicationViewport::Impl : public AllocatedClass
{
    Impl(Allocator* allocator) : AllocatedClass(allocator)
    {
    }

    std::unique_ptr<ApplicationViewportDelegate> windowDelegate;

    std::unique_ptr<OSWindow> osWindow;

    AssetPathSelector applicationAssetFileSelectorWithSubsystems;
    EnumArray<AssetClass, AssetClass::COUNT, AssetClassFileReader> assetClassFileReaders;

    std::unique_ptr<InputContext> inputContext;
    std::unique_ptr<SoundContext> soundContext;
    std::unique_ptr<GpuContext> gpuContext;
#if FINJIN_TARGET_VR_SYSTEM != FINJIN_TARGET_VR_SYSTEM_NONE
    std::unique_ptr<VRContext> vrContext;
#endif

    bool isMain;
    bool windowResized;
    bool toggleFullScreenRequested;
    bool closeRequested;
    bool exitApplicationRequested;
    bool finishResizeTargetsRequested;

    ErrorState errorState;

    //Processing pipeline-------------------------------
    int targetFramesPerSecond;

    HighResolutionClock frameTimeClock;
    HighResolutionTimeStamp frameTimeStamp;
    int64_t leftoverFrameTimeNanoseconds;

    size_t jobProcessingPipelineSize; //See ApplicationViewportDescription::jobProcessingPipelineSize
    size_t renderTickCount; //Used to trigger when the rendering starts. When this value is >= jobProcessingPipelineSize, rendering starts
    std::atomic_size_t jobPipelineStageUpdateIndex; //Index where update jobs will added
    std::atomic_size_t jobPipelineStageRenderIndex; //Index where render jobs will be completed. This will lag jobPipelineStageUpdateIndex by jobProcessingPipelineSize-1 steps
    struct Stage
    {
        Stage()
        {
            this->hasFrame = false;
        }

        JobPipelineStage jobPipelineStage;
        std::unique_ptr<ApplicationViewportUpdateContext> updateContext;
        std::unique_ptr<ApplicationViewportRenderContext> renderContext;
        bool hasFrame;
    };
    StaticVector<Stage, EngineConstants::MAX_FRAME_STAGES> stages;
};


//Implementation----------------------------------------------------------------
ApplicationViewport::ApplicationViewport(Allocator* allocator) :
    AllocatedClass(allocator),
    impl(AllocatedClass::New<Impl>(allocator, FINJIN_CALLER_ARGUMENTS))
{
    Init();
}

void ApplicationViewport::Init()
{
    impl->isMain = false;
    impl->windowResized = false;
    impl->toggleFullScreenRequested = false;
    impl->closeRequested = false;
    impl->exitApplicationRequested = false;
    impl->finishResizeTargetsRequested = false;

    impl->targetFramesPerSecond = 1000;

    impl->jobProcessingPipelineSize = 1;
    ResetUpdateAndRenderingCounters();
}

void ApplicationViewport::ConfigureJobPipeline(size_t renderBuffering, size_t pipelineSize)
{
    impl->jobProcessingPipelineSize = renderBuffering;

    impl->stages.resize(pipelineSize);
    for (size_t stageIndex = 0; stageIndex < impl->stages.size(); stageIndex++)
        impl->stages[stageIndex].jobPipelineStage.index = stageIndex;
}

size_t ApplicationViewport::GetJobPipelineSize() const
{
    return impl->stages.size();
}

void ApplicationViewport::SetJobPipelineStageData
    (
    size_t index,
    std::unique_ptr<ApplicationViewportUpdateContext>&& updateContext,
    std::unique_ptr<ApplicationViewportRenderContext>&& renderContext
    )
{
    auto& stage = impl->stages[index];

    updateContext->jobPipelineStage = &stage.jobPipelineStage;
    stage.updateContext = std::move(updateContext);

    renderContext->jobPipelineStage = &stage.jobPipelineStage;
    stage.renderContext = std::move(renderContext);
}

void ApplicationViewport::ResetUpdateAndRenderingCounters()
{
    impl->frameTimeStamp = impl->frameTimeClock.Now();
    impl->leftoverFrameTimeNanoseconds = 0;

    impl->renderTickCount = 0;
    impl->jobPipelineStageUpdateIndex = 0;
    impl->jobPipelineStageRenderIndex = 0;
}

ApplicationViewport::~ApplicationViewport()
{
    if (impl->osWindow != nullptr)
        impl->osWindow->Destroy();
}

ApplicationViewportDelegate* ApplicationViewport::GetDelegate()
{
    return impl->windowDelegate.get();
}

void ApplicationViewport::SetDelegate(std::unique_ptr<ApplicationViewportDelegate>&& del)
{
    impl->windowDelegate = std::move(del);
}

OSWindow* ApplicationViewport::GetOSWindow()
{
    return impl->osWindow.get();
}

void ApplicationViewport::SetOSWindow(std::unique_ptr<OSWindow>&& osWindow)
{
    impl->osWindow = std::move(osWindow);
}

bool ApplicationViewport::IsMain() const
{
    return impl->isMain;
}

void ApplicationViewport::SetMain(bool value)
{
    impl->isMain = value;
}

bool ApplicationViewport::HasFocus() const
{
    return impl->osWindow->HasFocus();
}

#if FINJIN_TARGET_VR_SYSTEM != FINJIN_TARGET_VR_SYSTEM_NONE
VRContext* ApplicationViewport::DetachVRContext()
{
    auto result = impl->vrContext.release();
    impl->vrContext = nullptr;
    return result;
}

VRContext* ApplicationViewport::GetVRContext()
{
    return impl->vrContext.get();
}

void ApplicationViewport::SetVRContext(std::unique_ptr<VRContext>&& context)
{
    impl->vrContext = std::move(context);
}

#endif

InputContext* ApplicationViewport::DetachInputContext()
{
    auto result = impl->inputContext.release();
    impl->inputContext = nullptr;
    return result;
}

InputContext* ApplicationViewport::GetInputContext()
{
    return impl->inputContext.get();
}

void ApplicationViewport::SetInputContext(std::unique_ptr<InputContext>&& context)
{
    impl->inputContext = std::move(context);
}

SoundContext* ApplicationViewport::DetachSoundContext()
{
    auto result = impl->soundContext.release();
    impl->soundContext = nullptr;
    return result;
}

SoundContext* ApplicationViewport::GetSoundContext()
{
    return impl->soundContext.get();
}

void ApplicationViewport::SetSoundContext(std::unique_ptr<SoundContext>&& context)
{
    impl->soundContext = std::move(context);
}

GpuContext* ApplicationViewport::DetachGpuContext()
{
    auto result = impl->gpuContext.release();
    impl->gpuContext = nullptr;
    return result;
}

GpuContext* ApplicationViewport::GetGpuContext()
{
    return impl->gpuContext.get();
}

void ApplicationViewport::SetGpuContext(std::unique_ptr<GpuContext>&& context)
{
    impl->gpuContext = std::move(context);
}

void ApplicationViewport::CreateAssetClassFileReaders(AssetFileReader& assetFileReader, const AssetPathSelector& applicationAssetFileSelector, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    if (!impl->applicationAssetFileSelectorWithSubsystems.Create(GetAllocator()))
    {
        FINJIN_SET_ERROR(error, "Failed to create asset file selector.");
        return;
    }
    impl->applicationAssetFileSelectorWithSubsystems = applicationAssetFileSelector;

    assert(impl->gpuContext != nullptr);
    impl->gpuContext->GetSelectorComponents(impl->applicationAssetFileSelectorWithSubsystems);

    assert(impl->soundContext != nullptr);
    impl->soundContext->GetSelectorComponents(impl->applicationAssetFileSelectorWithSubsystems);

    assert(impl->inputContext != nullptr);
    impl->inputContext->GetSelectorComponents(impl->applicationAssetFileSelectorWithSubsystems);

#if FINJIN_TARGET_VR_SYSTEM != FINJIN_TARGET_VR_SYSTEM_NONE
    if (impl->vrContext != nullptr)
        impl->vrContext->GetSelectorComponents(impl->applicationAssetFileSelectorWithSubsystems);
#endif

    for (size_t assetClass = 0; assetClass < (size_t)AssetClass::COUNT; assetClass++)
    {
        impl->assetClassFileReaders[assetClass].Create(assetFileReader, impl->applicationAssetFileSelectorWithSubsystems, static_cast<AssetClass>(assetClass), GetAllocator(), error);
        if (error)
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create asset class reader for asset class '%1%'.", AssetClassUtilities::ToString(assetClass)));
            return;
        }
    }
}

EnumArray<AssetClass, AssetClass::COUNT, AssetClassFileReader>& ApplicationViewport::GetAssetClassFileReaders()
{
    return impl->assetClassFileReaders;
}

bool ApplicationViewport::NotifyWindowResized()
{
    FINJIN_DEBUG_LOG_INFO("ApplicationViewport::NotifyWindowResized()");

    if (!impl->osWindow->GetWindowSize().IsApplyingToWindow())
    {
        FINJIN_DEBUG_LOG_INFO("  Resize accepted");

        impl->windowResized = true;

        return true;
    }
    else
    {
        FINJIN_DEBUG_LOG_INFO("  The window was applying a change - resize ignored");

        return false;
    }
}

void ApplicationViewport::RequestFullScreenToggle()
{
    impl->toggleFullScreenRequested = true;
}

bool ApplicationViewport::CloseRequested() const
{
    return impl->closeRequested;
}

void ApplicationViewport::RequestClose()
{
    impl->closeRequested = true;
}

bool ApplicationViewport::ExitApplicationRequested() const
{
    return impl->exitApplicationRequested;
}

void ApplicationViewport::RequestApplicationExit()
{
    impl->exitApplicationRequested = true;
}

bool ApplicationViewport::NeedsToModifyRenderTarget() const
{
    return impl->windowResized || impl->toggleFullScreenRequested || impl->finishResizeTargetsRequested;
}

void ApplicationViewport::ApplyModifyRenderTarget(Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    assert(NeedsToModifyRenderTarget());

    ResetUpdateAndRenderingCounters();

    if (impl->windowResized || impl->finishResizeTargetsRequested)
        ApplyRenderTargetsResize(error);
    else if (impl->toggleFullScreenRequested)
        ApplyFullScreenToggle(error);

    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to apply render target size modification.");
        return;
    }
}

void ApplicationViewport::ApplyRenderTargetsResize(Error& error)
{
    FINJIN_DEBUG_LOG_INFO("ApplicationViewport::ApplyRenderTargetsResize");

    FINJIN_ERROR_METHOD_START(error);

    impl->windowResized = false;

    if (impl->finishResizeTargetsRequested)
    {
        impl->finishResizeTargetsRequested = false;

        FinishResizeTargets(error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to finish resize swap chain render targets.");
            return;
        }
    }
    else
    {
        //Start resize
        impl->finishResizeTargetsRequested = StartResizeTargets(error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to start resize swap chain render targets.");
            return;
        }
    }
}

void ApplicationViewport::ApplyFullScreenToggle(Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto nextStateRequiresFullScreenExclusiveToggle = GetOSWindow()->GetWindowSize().GetFullScreenState() == WindowSizeState::EXCLUSIVE_FULLSCREEN;
    ApplyFullScreenToggle(nextStateRequiresFullScreenExclusiveToggle, error);
    if (error)
    {
        FINJIN_SET_ERROR_NO_MESSAGE(error);
    }
}

void ApplicationViewport::ApplyFullScreenToggle(bool needsExclusiveToggle, Error& error)
{
    FINJIN_DEBUG_LOG_INFO("ApplicationViewport::ApplyFullScreenToggle(%1%)", (int)needsExclusiveToggle);

    FINJIN_ERROR_METHOD_START(error);

    //Clear toggle flag
    impl->toggleFullScreenRequested = false;

    //Have the device toggle its full screen state if necessary
    if (needsExclusiveToggle)
    {
        impl->gpuContext->ToggleFullScreenExclusive(error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to toggle exclusive full screen mode.");
            return;
        }
    }

    //Change to next state
    FINJIN_DEBUG_LOG_INFO("  Changing to next");
    GetOSWindow()->GetWindowSize().Next();
    FINJIN_DEBUG_LOG_INFO("  Changed to next");

    //Resize targets based on new current state
    if (!PlatformCapabilities::GetInstance().hasDelayedWindowSizeChangeNotification)
    {
        impl->finishResizeTargetsRequested = StartResizeTargets(error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to start resize swap chain render targets.");
            return;
        }
    }
    else
    {
        //A window size change will come in later and trigger a resize
        FINJIN_DEBUG_LOG_INFO("  A window size change will come later");
    }
}

void ApplicationViewport::OnTick(JobSystem& jobSystem, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    if (NeedsToModifyRenderTarget())
    {
        FinishWork(RenderStatus(true, true));

        ApplyModifyRenderTarget(error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to apply render target modifications.");
            return;
        }
    }
    else
    {
        //Perform an update
        auto stepCount = Update(jobSystem, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Error while updating frame.");
            return;
        }

        if (stepCount > 0)
        {
            //A frame was updated

            //Render previous frame if buffering requirement has been met
            if (++impl->renderTickCount >= impl->jobProcessingPipelineSize)
            {
                FinishFrame(RenderStatus(true, false), -1, error);
                if (error)
                {
                    FINJIN_SET_ERROR(error, "Error while finishing frame.");
                    return;
                }
            }
        }
    }
}

void ApplicationViewport::FinishWork(RenderStatus renderStatus)
{
    if (!impl->stages.empty())
    {
        while ((impl->jobPipelineStageRenderIndex % impl->stages.size()) != (impl->jobPipelineStageUpdateIndex % impl->stages.size()))
        {
            FINJIN_DECLARE_ERROR(error);
            FinishFrame(renderStatus, 0, error);
        }
    }

    if (impl->gpuContext != nullptr)
        impl->gpuContext->FlushGpu();

    ResetUpdateAndRenderingCounters();
}

int ApplicationViewport::Update(JobSystem& jobSystem, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto now = impl->frameTimeClock.Now();
    auto elapsedTimeDuration = now - impl->frameTimeStamp;
    impl->frameTimeStamp = now;

    int stepCount = 0;

    impl->leftoverFrameTimeNanoseconds += elapsedTimeDuration.ToNanoseconds();
    int64_t targetNanosecondsPerFrame = 1000000000ll / static_cast<int64_t>(impl->targetFramesPerSecond);
    if (impl->leftoverFrameTimeNanoseconds >= targetNanosecondsPerFrame)
    {
        stepCount = static_cast<int>(impl->leftoverFrameTimeNanoseconds / targetNanosecondsPerFrame);
        impl->leftoverFrameTimeNanoseconds = impl->leftoverFrameTimeNanoseconds % targetNanosecondsPerFrame;

        auto stageIndex = impl->jobPipelineStageUpdateIndex++ % impl->stages.size();
        auto& stage = impl->stages[stageIndex];
        //auto& jobPipelineStage = stage.jobPipelineStage;
        auto& updateContext = *stage.updateContext.get();
        updateContext.elapsedTime = NanosecondsToSeconds<SimpleTimeDelta>(stepCount * targetNanosecondsPerFrame);
        updateContext.fixedSteps = stepCount;
        updateContext.fixedStepTime = NanosecondsToSeconds<SimpleTimeDelta>(targetNanosecondsPerFrame);

        updateContext.ClearCommands();

        auto updateResult = impl->windowDelegate->Update(updateContext, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Application viewport delegate failed.");
        }

        stage.hasFrame = updateResult == ApplicationViewportDelegate::UpdateResult::STARTED_FRAME;
    }

    return stepCount;
}

void ApplicationViewport::FinishFrame(RenderStatus renderStatus, size_t presentSyncIntervalOverride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto stageIndex = impl->jobPipelineStageRenderIndex++ % impl->stages.size();
    auto& stage = impl->stages[stageIndex];
    //auto& jobPipelineStage = stage.jobPipelineStage;
    auto& renderContext = *stage.renderContext.get();

    renderContext.renderStatus = renderStatus;
    renderContext.presentSyncIntervalOverride = presentSyncIntervalOverride;

    if (stage.hasFrame)
    {
        impl->windowDelegate->FinishFrame(renderContext, error);
        if (error)
            FINJIN_SET_ERROR(error, "Failed to finish frame.");

        stage.hasFrame = false;
    }
}

bool ApplicationViewport::StartResizeTargets(Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto result = impl->gpuContext->StartResizeTargets(impl->osWindow->IsMinimized(), error);
    if (error)
        FINJIN_SET_ERROR(error, "Failed to start resizing targets.");

    return result;
}

void ApplicationViewport::FinishResizeTargets(Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    impl->gpuContext->FinishResizeTargets(error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to finish resizing targets.");
        return;
    }
}
