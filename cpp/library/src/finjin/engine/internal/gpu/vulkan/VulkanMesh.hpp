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
#include "finjin/common/Error.hpp"
#include "finjin/common/Math.hpp"
#include "finjin/engine/FinjinSceneAssets.hpp"
#include "VulkanBufferView.hpp"
#include "VulkanGpuBuffer.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    class VulkanMesh
    {
    public:
        struct Submesh
        {
            Submesh(VulkanMesh* mesh = nullptr);

            VulkanVertexBufferView GetVertexBufferView() const;
            VulkanIndexBufferView GetIndexBufferView() const;

            GpuInputFormatStruct* GetInputFormat();

            VulkanMesh* mesh;

            uint32_t vertexBufferIndex;
            uint32_t startIndexLocation;
            uint32_t indexCount;
            uint32_t baseVertexLocation;
            uint32_t vertexCount;

            VulkanGpuBuffer vertexBuffer;
            VulkanGpuBuffer indexBuffer;

            VkPrimitiveTopology primitiveType;
        };

        VulkanMesh(Allocator* allocator);

        void Destroy(VulkanDeviceFunctions& vk, VkAllocationCallbacks* allocationCallbacks);

        void HandleCreationFailure(VulkanDeviceFunctions& vk, VkAllocationCallbacks* allocationCallbacks);
        void ReleaseUploaders(VulkanDeviceFunctions& vk, VkAllocationCallbacks* allocationCallbacks);

        VulkanVertexBufferView GetSharedVertexBufferView(uint32_t vertexBufferIndex) const;
        VulkanIndexBufferView GetSharedIndexBufferView() const;

        bool IsResidentOnGpu() const;
        void UpdateResidentOnGpuStatus(VulkanDeviceFunctions& vk, VkAllocationCallbacks* allocationCallbacks);

    public:
        VulkanGpuBuffer sharedIndexBuffer;
        DynamicVector<VulkanGpuBuffer> sharedVertexBuffers;

        DynamicVector<Submesh> submeshes;

        size_t isResidentCountdown;
        VulkanMesh* waitingToBeResidentNext;
    };

} }
