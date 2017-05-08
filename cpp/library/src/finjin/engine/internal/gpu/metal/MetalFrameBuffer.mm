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

    this->commandQueue = nullptr;
}

void MetalFrameBuffer::SetIndex(size_t index)
{
    this->index = index;

    this->staticMeshRendererFrameState.index = index;
}

id<MTLCommandBuffer> MetalFrameBuffer::NewGraphicsCommandBuffer()
{
    if (this->commandBuffersToExecute.full())
        return nullptr;

    auto result = [this->commandQueue commandBufferWithUnretainedReferences];
    assert(result != nullptr);

    this->commandBuffersToExecute.push_back(result);
    return result;
}

id<MTLCommandBuffer> MetalFrameBuffer::GetCurrentGraphicsCommandBuffer()
{
    return !this->commandBuffersToExecute.empty() ? this->commandBuffersToExecute.back() : nullptr;
}

void MetalFrameBuffer::CommitCommandBuffers()
{
    for (auto buffer : this->commandBuffersToExecute)
        [buffer commit];
}

void MetalFrameBuffer::WaitForCommandBuffers()
{
    for (auto& buffer : this->commandBuffersToExecute)
        [buffer waitUntilCompleted];
}

void MetalFrameBuffer::ResetCommandBuffers()
{
    for (auto& buffer : this->commandBuffersToExecute)
        buffer = nullptr;
    this->commandBuffersToExecute.clear();
}

#endif
