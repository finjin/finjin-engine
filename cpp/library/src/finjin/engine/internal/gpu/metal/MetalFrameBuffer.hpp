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
#include "finjin/common/DynamicVector.hpp"
#include "MetalIncludes.hpp"
#include "MetalRenderTarget.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    class MetalFrameBuffer
    {
    public:
        MetalFrameBuffer();

        void SetIndex(size_t index);

        void Destroy();

        void CreateCommandBuffers(size_t maxGpuCommandListsPerStage, Allocator* allocator, Error& error);

        id<MTLCommandBuffer> NewGraphicsCommandBuffer();
        id<MTLCommandBuffer> GetCurrentGraphicsCommandBuffer();
        void CommitCommandBuffers();
        void WaitForCommandBuffers();
        void ResetCommandBuffers();

        void WaitForNotInUse();

        void CreateFreeCommandBuffers();

    public:
        size_t index;
        std::atomic<size_t> commandBufferWaitIndex;
        std::atomic<size_t> commandBufferCommitIndex;

        MetalRenderTarget renderTarget;

        id<MTLCommandQueue> commandQueue;

        DynamicVector<id<MTLCommandBuffer> > freeCommandBuffers;
        DynamicVector<id<MTLCommandBuffer> > commandBuffersToExecute;
    };

} }
