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
#include "finjin/common/Setting.hpp"
#include "VulkanGpuDescription.hpp"
#include "VulkanUtilities.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    class VulkanRenderTarget
    {
    public:
        VulkanRenderTarget();

        bool IsScreenSizeDependent() const;

        void CreateColor
            (
            VkPhysicalDevice physicalDevice,
            const VulkanGpuDescription& physicalDeviceDescription,
            VulkanInstanceFunctions& instanceVk,
            VulkanDeviceFunctions& deviceVk,
            VkAllocationCallbacks* allocationCallbacks,
            VkCommandBuffer graphicsCommandBuffer,
            size_t width,
            size_t height,
            VkFormat colorFormat,
            size_t multisampleCount,
            bool isScreenSizeDependent,
            size_t outputCount,
            Error& error
            );

        void CreateDepthStencil
            (
            VkPhysicalDevice physicalDevice,
            const VulkanGpuDescription& physicalDeviceDescription,
            VulkanInstanceFunctions& instanceVk,
            VulkanDeviceFunctions& deviceVk,
            VkAllocationCallbacks* allocationCallbacks,
            VkCommandBuffer graphicsCommandBuffer,
            size_t width,
            size_t height,
            VkFormat depthStencilFormat,
            size_t multisampleCount,
            bool isScreenSizeDependent,
            Error& error
            );

        void CreateFrameBuffer(VulkanDeviceFunctions& vk, VkAllocationCallbacks* allocationCallbacks, VulkanFramebufferCreateInfo& frameBufferCreateInfo, size_t colorViewIndex, Error& error);

        void Destroy(VulkanDeviceFunctions& vk, VkAllocationCallbacks* allocationCallbacks);
        void DestroyScreenSizeDependentResources(VulkanDeviceFunctions& vk, VkAllocationCallbacks* allocationCallbacks);

    public:
        struct Resource
        {
            Resource();

            void Destroy(VulkanDeviceFunctions& vk, VkAllocationCallbacks* allocationCallbacks);
            void DestroyScreenSizeDependentResources(VulkanDeviceFunctions& vk, VkAllocationCallbacks* allocationCallbacks);

            bool isImageOwned;
            VkImage image;
            VkDeviceMemory mem;
            VkImageView view;
            bool isScreenSizeDependent;
        };

        StaticVector<Resource, EngineConstants::MAX_RENDER_TARGET_OUTPUTS> colorOutputs;

        Resource depthStencilResource;

        VkFramebuffer vkframeBuffer;

        StaticVector<VkViewport, 1> defaultViewport;
        StaticVector<VkRect2D, 1> defaultScissorRect;

        StaticVector<VkViewport, EngineConstants::MAX_RENDER_TARGET_VIEWPORTS> viewports;
        StaticVector<VkRect2D, EngineConstants::MAX_RENDER_TARGET_VIEWPORTS> scissorRects;

        Setting<MathVector4> clearColor;
    };

} }
