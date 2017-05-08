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

#if FINJIN_TARGET_GPU_SYSTEM == FINJIN_TARGET_GPU_SYSTEM_VULKAN

#include "VulkanPresentModes.hpp"

using namespace Finjin::Engine;


//Macros------------------------------------------------------------------------
#if FINJIN_TARGET_PLATFORM_IS_ANDROID
    //I've observed some unpredictable behavior on Android/Shield, with too many back buffers being returned, and AcquireNextImageKHR not waiting to return, as it should
    //This behavior seems to be related to https://vulkan.lunarg.com/issue/home?limit=10;q=;mine=false;org=false;khronos=false;lunarg=false;indie=false;status=new,open
    #define ALLOW_MAILBOX 0
#else
    #define ALLOW_MAILBOX 1
#endif


//Implementation----------------------------------------------------------------
void VulkanPresentModes::Enumerate(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, VulkanInstanceFunctions& vk, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    clear();

    uint32_t presentModeCount;
    auto result = vk.GetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);
    if (FINJIN_CHECK_VKRESULT_FAILED(result))
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to get surface present mode count. Vulkan result: %1%", VulkanResult::ToString(result)));
        return;
    }

    presentModeCount = static_cast<uint32_t>(resize(presentModeCount));
    result = vk.GetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, data());
    if (FINJIN_CHECK_VKRESULT_FAILED(result))
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to get surface present modes. Vulkan result: %1%", VulkanResult::ToString(result)));
        return;
    }
}

VkPresentModeKHR VulkanPresentModes::GetBest(GpuSwapChainPresentMode swapChainPresentMode) const
{
    //Try to get the ideal
    switch (swapChainPresentMode)
    {
        case GpuSwapChainPresentMode::FULL_VSYNC:
        {
        #if ALLOW_MAILBOX
            if (contains(VK_PRESENT_MODE_MAILBOX_KHR))
                return VK_PRESENT_MODE_MAILBOX_KHR;
        #endif
            if (contains(VK_PRESENT_MODE_FIFO_KHR))
                return VK_PRESENT_MODE_FIFO_KHR;
            break;
        }
        case GpuSwapChainPresentMode::ADAPTIVE_VSYNC:
        {
            if (contains(VK_PRESENT_MODE_FIFO_RELAXED_KHR))
                return VK_PRESENT_MODE_FIFO_RELAXED_KHR;
            break;
        }
        case GpuSwapChainPresentMode::IMMEDIATE:
        {
            if (contains(VK_PRESENT_MODE_IMMEDIATE_KHR))
                return VK_PRESENT_MODE_IMMEDIATE_KHR;
            break;
        }
    }

    //Ideal failed. Try to get next best

    //Fully vsync'ed first
#if ALLOW_MAILBOX
    if (contains(VK_PRESENT_MODE_MAILBOX_KHR))
        return VK_PRESENT_MODE_MAILBOX_KHR;
#endif
    if (contains(VK_PRESENT_MODE_FIFO_KHR))
        return VK_PRESENT_MODE_FIFO_KHR;

    //Partially vsync'ed next
    if (contains(VK_PRESENT_MODE_FIFO_RELAXED_KHR))
        return VK_PRESENT_MODE_FIFO_RELAXED_KHR;

    //Unlikely to get this far, but if so, no vsync
    return VK_PRESENT_MODE_IMMEDIATE_KHR;
}

#endif
