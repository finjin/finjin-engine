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
#include "MetalIncludes.hpp"
#include "MetalStaticMeshRendererFrameState.hpp"
#include "MetalRenderTarget.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    class MetalFrameBuffer
    {
    public:
        MetalFrameBuffer();

        void SetIndex(size_t index);

        id<MTLCommandBuffer> NewGraphicsCommandBuffer();
        id<MTLCommandBuffer> GetCurrentGraphicsCommandBuffer();
        void CommitCommandBuffers();
        void WaitForCommandBuffers();
        void ResetCommandBuffers();

    public:
        size_t index;
        MetalRenderTarget renderTarget;

        id<MTLCommandQueue> commandQueue;
        StaticVector<id<MTLCommandBuffer>, 20> commandBuffersToExecute;

        MetalStaticMeshRendererFrameState staticMeshRendererFrameState;
    };

} }
