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

#include "VulkanSurfaceFormats.hpp"

using namespace Finjin::Engine;


//Implementation----------------------------------------------------------------
VulkanSurfaceFormats::const_iterator VulkanSurfaceFormats::FindFormat(VkFormat format) const
{
    for (auto iter = begin(); iter != end(); ++iter)
    {
        if (iter->format == format)
            return iter;
    }
    return end();
}

VulkanSurfaceFormats::const_iterator VulkanSurfaceFormats::FindFormat(VkFormat format, VkColorSpaceKHR colorSpace) const
{
    for (auto iter = begin(); iter != end(); ++iter)
    {
        if (iter->format == format && iter->colorSpace == colorSpace)
            return iter;
    }
    return end();
}

void VulkanSurfaceFormats::Enumerate(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, VulkanInstanceFunctions& vk, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    uint32_t formatCount;
    auto result = vk.GetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);
    if (FINJIN_CHECK_VKRESULT_FAILED(result))
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to get device surface format count. Vulkan result: %1%", VulkanResult::ToString(result)));
        return;
    }
    if (formatCount == 0)
    {
        FINJIN_SET_ERROR(error, "Failed to get device surface format count. None were returned.");
        return;
    }

    formatCount = static_cast<uint32_t>(resize(formatCount));
    result = vk.GetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, data());
    if (FINJIN_CHECK_VKRESULT_FAILED(result))
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to get device surface formats. Vulkan result: %1%", VulkanResult::ToString(result)));
        return;
    }
    if (formatCount == 0)
    {
        FINJIN_SET_ERROR(error, "Failed to get device surface formats. None were returned.");
        return;
    }
}

#endif
