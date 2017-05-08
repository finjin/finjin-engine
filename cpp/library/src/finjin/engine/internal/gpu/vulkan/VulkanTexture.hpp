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
#include "finjin/common/ByteBuffer.hpp"
#include "finjin/common/DynamicVector.hpp"
#include "finjin/common/Error.hpp"
#include "finjin/common/PNGReader.hpp"
#include "finjin/engine/ASTCReader.hpp"
#include "finjin/engine/KTXReader.hpp"
#include "finjin/engine/TextureDimension.hpp"
#include "VulkanGpuDescription.hpp"
#include "VulkanMemoryResource.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    class VulkanTexture
    {
    public:
        VulkanTexture(Allocator* allocator);

        void CreateFromPNG
            (
            PNGReader& reader,
            const void* bytes,
            size_t byteCount,
            VulkanInstanceFunctions& instanceVk,
            VkPhysicalDevice physicalDevice,
            VulkanDeviceFunctions& deviceVk,
            VkAllocationCallbacks* allocationCallbacks,
            const VulkanGpuDescription& gpuDescription,
            uint32_t* queueFamilyIndices,
            size_t queueFamilyIndexCount,
            Error& error
            );

        void CreateFromASTC
            (
            ASTCReader& reader,
            const void* bytes,
            size_t byteCount,
            VulkanInstanceFunctions& instanceVk,
            VkPhysicalDevice physicalDevice,
            VulkanDeviceFunctions& deviceVk,
            VkAllocationCallbacks* allocationCallbacks,
            const VulkanGpuDescription& gpuDescription,
            uint32_t* queueFamilyIndices,
            size_t queueFamilyIndexCount,
            Error& error
            );

        void CreateFromKTX
            (
            KTXReader& reader,
            const void* bytes,
            size_t byteCount,
            VulkanInstanceFunctions& instanceVk,
            VkPhysicalDevice physicalDevice,
            VulkanDeviceFunctions& deviceVk,
            VkAllocationCallbacks* allocationCallbacks,
            const VulkanGpuDescription& gpuDescription,
            size_t maxDimension,
            uint32_t* queueFamilyIndices,
            size_t queueFamilyIndexCount,
            Error& error
            );

        void Destroy(VulkanDeviceFunctions& vk, VkAllocationCallbacks* allocationCallbacks);

        void HandleCreationFailure(VulkanDeviceFunctions& vk, VkAllocationCallbacks* allocationCallbacks);
        void ReleaseUploaders(VulkanDeviceFunctions& vk, VkAllocationCallbacks* allocationCallbacks);

        bool IsResidentOnGpu() const;
        void UpdateResidentOnGpuStatus(VulkanDeviceFunctions& vk, VkAllocationCallbacks* allocationCallbacks);

        void Upload(VulkanDeviceFunctions& vk, VkCommandBuffer commandBuffer);

    private:
        void Reset();

        void CreateFromImage
            (
            VkFormat format,
            VkImageType imageType,
            size_t width,
            size_t height,
            size_t depth,
            bool isArray,
            uint32_t* queueFamilyIndices,
            size_t queueFamilyIndexCount,
            const void* bytes,
            size_t byteCount,
            size_t bytesPerRow,
            VulkanInstanceFunctions& instanceVk,
            VkPhysicalDevice physicalDevice,
            VulkanDeviceFunctions& deviceVk,
            VkAllocationCallbacks* allocationCallbacks,
            const VulkanGpuDescription& gpuDescription,
            Error& error
            );

    public:
        VulkanMemoryResource resource;
        VulkanMemoryResource uploadResource;

        VkImageLayout imageLayout;

        ByteBuffer textureData;

        Utf8String name;
        size_t textureIndex;

        TextureDimension textureDimension;

        bool needsUploader;
        StaticVector<VkImageCopy, 16> imageCopyInfos;

        size_t isResidentCountdown;
        VulkanTexture* waitingToBeResidentNext;
        VulkanTexture* waitingToBeTransitionedNext;
    };

} }
