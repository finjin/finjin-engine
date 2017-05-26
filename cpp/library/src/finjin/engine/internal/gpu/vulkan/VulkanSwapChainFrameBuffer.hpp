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
#include "VulkanUtilities.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    class VulkanSwapChainFrameBuffer
    {
    public:
        enum { MAX_SWAP_CHAIN_IMAGES = 16 }; //A large number, more than necessary

        VulkanSwapChainFrameBuffer();

        void SetImageAndCreateView(VulkanDeviceFunctions& vk, VkAllocationCallbacks* allocationCallbacks, VkImage image, VkFormat colorFormat, Error& error);
        void CreateFrameBuffer(VulkanDeviceFunctions& vk, VkAllocationCallbacks* allocationCallbacks, VulkanFramebufferCreateInfo& frameBufferCreateInfo, size_t colorViewIndex, Error& error);
        void Destroy(VulkanDeviceFunctions& vk, VkAllocationCallbacks* allocationCallbacks);

    public:
        VkImage image;
        VkImageView view;
        VkFramebuffer vkframeBuffer;
    };

} }
