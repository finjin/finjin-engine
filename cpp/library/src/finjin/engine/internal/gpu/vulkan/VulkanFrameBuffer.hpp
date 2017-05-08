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
#include "finjin/common/StaticVector.hpp"
#include "VulkanStaticMeshRendererFrameState.hpp"
#include "VulkanRenderTarget.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    class VulkanGpuContext;
    class VulkanGpuContextImpl;

    class VulkanFrameBuffer
    {
    public:
        VulkanFrameBuffer();

        void SetIndex(size_t index);

        VkCommandBuffer NewGraphicsCommandBuffer();
        VkCommandBuffer GetCurrentGraphicsCommandBuffer();
        void ResetCommandBuffers();

        void ResetFences(VulkanGpuContextImpl* contextImpl);

    public:
        size_t index;
        VulkanRenderTarget renderTarget;
        VkFramebuffer frameBuffer;

        VkFence renderingCompleteFence;

        VkSemaphore renderingCompleteSemaphore;

        VkCommandPool graphicsCommandPool;
        DynamicVector<VkCommandBuffer> graphicsCommandBuffers;
        DynamicVector<VkCommandBuffer> commandBuffersToExecute;

        VulkanStaticMeshRendererFrameState staticMeshRendererFrameState;
    };

} }
