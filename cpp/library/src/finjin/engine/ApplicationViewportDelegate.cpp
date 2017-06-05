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
#include "ApplicationViewportDelegate.hpp"
#include "ApplicationViewport.hpp"

using namespace Finjin::Engine;


//Implementation----------------------------------------------------------------

//ApplicationViewportUpdateContext
ApplicationViewportUpdateContext::ApplicationViewportUpdateContext(Allocator* allocator) : AllocatedClass(allocator)
{
#if FINJIN_TARGET_VR_SYSTEM != FINJIN_TARGET_VR_SYSTEM_NONE
    this->vrContext = nullptr;
#endif
    this->inputContext = nullptr;
    this->soundContext = nullptr;
    this->gpuContext = nullptr;

    this->elapsedTime = 0;
    this->fixedSteps = 0;
    this->fixedStepTime = 0;
    this->jobPipelineStage = nullptr;
    this->jobSystem = nullptr;
    this->assetReadQueue = nullptr;
    this->assetClassFileReaders = nullptr;
    this->standardPaths = nullptr;
    this->userInfo = nullptr;
    this->domainInfo = nullptr;
}

ApplicationViewportUpdateContext::~ApplicationViewportUpdateContext()
{
}

void ApplicationViewportUpdateContext::ClearCommands()
{
    this->newScenes.clear();
    this->newSounds.clear();
    this->newMeshes.clear();
    this->newTextures.clear();
    this->newMaterials.clear();

#if FINJIN_TARGET_VR_SYSTEM != FINJIN_TARGET_VR_SYSTEM_NONE
    this->vrCommands.clear();
#endif
    this->inputCommands.clear();
    this->soundCommands.clear();
    this->gpuCommands.clear();
}

void ApplicationViewportUpdateContext::ClearEvents()
{
#if FINJIN_TARGET_VR_SYSTEM != FINJIN_TARGET_VR_SYSTEM_NONE
    this->vrEvents.clear();
#endif
    this->inputEvents.clear();
    this->soundEvents.clear();
    this->gpuEvents.clear();
}

void ApplicationViewportUpdateContext::StartPoll()
{
    ClearEvents();

    if (HasFocus())
    {
    #if FINJIN_TARGET_VR_SYSTEM != FINJIN_TARGET_VR_SYSTEM_NONE
        if (this->vrContext != nullptr)
            this->vrContext->UpdateInputDevices(this->elapsedTime);
    #endif

        this->inputContext->Update(this->elapsedTime);
    }

    this->soundContext->Update(this->elapsedTime);
}

void ApplicationViewportUpdateContext::FinishPoll()
{
    this->inputContext->HandleDeviceChanges();
}

void ApplicationViewportUpdateContext::Execute(GpuContext::JobPipelineStage& frameStage, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

#if FINJIN_TARGET_VR_SYSTEM != FINJIN_TARGET_VR_SYSTEM_NONE
    this->vrContext->Execute(this->vrEvents, this->vrCommands, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to execute VR commands.");
        return;
    }
#endif

    this->inputContext->Execute(this->inputEvents, this->inputCommands, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to execute input commands.");
        return;
    }

    this->soundContext->Execute(this->soundEvents, this->soundCommands, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to execute sound commands.");
        return;
    }

    this->gpuContext->Execute(frameStage, this->gpuEvents, this->gpuCommands, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to execute GPU commands.");
        return;
    }
}

//ApplicationViewportRenderContext
ApplicationViewportRenderContext::ApplicationViewportRenderContext(Allocator* allocator) : AllocatedClass(allocator)
{
#if FINJIN_TARGET_VR_SYSTEM != FINJIN_TARGET_VR_SYSTEM_NONE
    this->vrContext = nullptr;
#endif
    this->inputContext = nullptr;
    this->soundContext = nullptr;
    this->gpuContext = nullptr;

    this->jobPipelineStage = nullptr;
    this->renderStatus = RenderStatus::GetRenderingRequired();
    this->presentSyncIntervalOverride = 0;
}

ApplicationViewportRenderContext::~ApplicationViewportRenderContext()
{
}
