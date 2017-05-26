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

#include "MetalFrameBuffer.hpp"

using namespace Finjin::Engine;


//Implementation----------------------------------------------------------------
MetalFrameBuffer::MetalFrameBuffer()
{
    this->index = 0;
    this->commandBufferWaitIndex = 0;
    this->commandBufferCommitIndex = 0;

    this->commandQueue = nullptr;
}

void MetalFrameBuffer::SetIndex(size_t index)
{
    this->index = index;
}

void MetalFrameBuffer::Destroy()
{
    ResetCommandBuffers();

    this->freeCommandBuffers.Destroy();
    this->commandBuffersToExecute.Destroy();

    this->renderTarget.Destroy();

    this->commandQueue = nullptr;
}

void MetalFrameBuffer::CreateCommandBuffers(size_t maxGpuCommandListsPerStage, Allocator* allocator, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    if (!this->freeCommandBuffers.CreateEmpty(maxGpuCommandListsPerStage, allocator, nullptr))
    {
        FINJIN_SET_ERROR(error, "Failed to create free command buffers collection");
        return;
    }

    if (!this->commandBuffersToExecute.CreateEmpty(maxGpuCommandListsPerStage, allocator, nullptr))
    {
        FINJIN_SET_ERROR(error, "Failed to create execute command buffers collection.");
        return;
    }
}

id<MTLCommandBuffer> MetalFrameBuffer::NewGraphicsCommandBuffer()
{
    if (!this->freeCommandBuffers.push_back())
        return nullptr;

    auto commandBuffer = this->freeCommandBuffers.back();

    this->commandBuffersToExecute.push_back(commandBuffer);

    return commandBuffer;
}

id<MTLCommandBuffer> MetalFrameBuffer::GetCurrentGraphicsCommandBuffer()
{
    return !this->commandBuffersToExecute.empty() ? this->commandBuffersToExecute.back() : nullptr;
}

void MetalFrameBuffer::CommitCommandBuffers()
{
    if (this->commandBufferCommitIndex < this->commandBuffersToExecute.size())
    {
        for (; this->commandBufferCommitIndex < this->commandBuffersToExecute.size(); this->commandBufferCommitIndex++)
        {
            auto commandBuffer = this->commandBuffersToExecute[this->commandBufferCommitIndex];
            [commandBuffer commit];
        }
    }
}

void MetalFrameBuffer::WaitForCommandBuffers()
{
    for (; this->commandBufferWaitIndex < this->commandBufferCommitIndex; this->commandBufferWaitIndex++)
    {
        auto commandBuffer = this->commandBuffersToExecute[this->commandBufferWaitIndex];
        [commandBuffer waitUntilCompleted];
        commandBuffer = nullptr;
    }
}

void MetalFrameBuffer::ResetCommandBuffers()
{
    for (auto& commandBuffer : this->commandBuffersToExecute)
        commandBuffer = nullptr;
    this->commandBuffersToExecute.clear();

    this->freeCommandBuffers.maximize();
    for (auto& commandBuffer : this->freeCommandBuffers)
        commandBuffer = nullptr;
    this->freeCommandBuffers.clear();

    this->commandBufferWaitIndex = 0;
    this->commandBufferCommitIndex = 0;
}

void MetalFrameBuffer::WaitForNotInUse()
{
    while (!this->commandBuffersToExecute.empty())
    {
        //Spin
    }
}

void MetalFrameBuffer::CreateFreeCommandBuffers()
{
    this->freeCommandBuffers.maximize();
    for (auto& commandBuffer : this->freeCommandBuffers)
    {
        if (commandBuffer == nullptr)
            commandBuffer = [this->commandQueue commandBufferWithUnretainedReferences];
    }
    this->freeCommandBuffers.clear();
}

#endif
