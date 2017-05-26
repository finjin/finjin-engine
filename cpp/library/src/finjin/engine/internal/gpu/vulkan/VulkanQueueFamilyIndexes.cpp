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

#include "VulkanQueueFamilyIndexes.hpp"
#include "VulkanUtilities.hpp"

using namespace Finjin::Engine;


//Implementation----------------------------------------------------------------
VulkanQueueFamilyIndexes::VulkanQueueFamilyIndexes()
{
    Reset();
}

void VulkanQueueFamilyIndexes::Reset()
{
    this->graphics = UINT32_MAX;
    this->compute = UINT32_MAX;
    this->transfer = UINT32_MAX;
    this->sparseBinding = UINT32_MAX;
    this->present = UINT32_MAX;
}

void VulkanQueueFamilyIndexes::Enumerate(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, VulkanInstanceFunctions& vk, VulkanQueueFamilyProperties& queueProperties, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    Reset();

    StaticVector<VkBool32, VulkanQueueFamilyProperties::MAX_ITEMS> supportsPresent;
    supportsPresent.resize(queueProperties.size());
    for (size_t queueIndex = 0; queueIndex < queueProperties.size(); queueIndex++)
    {
        if (queueProperties[queueIndex].queueCount > 0)
        {
            vk.GetPhysicalDeviceSurfaceSupportKHR(physicalDevice, static_cast<uint32_t>(queueIndex), surface, &supportsPresent[queueIndex]);

            if (queueProperties[queueIndex].queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                if (this->graphics == UINT32_MAX)
                    this->graphics = static_cast<uint32_t>(queueIndex);

                if (this->present == UINT32_MAX && supportsPresent[queueIndex])
                    this->present = static_cast<uint32_t>(queueIndex);
            }
            else if (queueProperties[queueIndex].queueFlags & VK_QUEUE_COMPUTE_BIT)
            {
                if (this->compute == UINT32_MAX)
                    this->compute = static_cast<uint32_t>(queueIndex);
            }
            else if (queueProperties[queueIndex].queueFlags & VK_QUEUE_TRANSFER_BIT)
            {
                if (this->transfer == UINT32_MAX)
                    this->transfer = static_cast<uint32_t>(queueIndex);
            }
        }
    }

    //Initialize present queue index if it hasn't already been initialized
    if (this->present == UINT32_MAX)
    {
        for (size_t queueIndex = 0; queueIndex < queueProperties.size(); queueIndex++)
        {
            if (supportsPresent[queueIndex])
            {
                this->present = static_cast<uint32_t>(queueIndex);
                break;
            }
        }
    }

    if (this->graphics == UINT32_MAX)
    {
        FINJIN_SET_ERROR(error, "Could not find graphics queue.");
        return;
    }
    if (this->present == UINT32_MAX)
    {
        FINJIN_SET_ERROR(error, "Could not find present queue.");
        return;
    }
}

size_t VulkanQueueFamilyIndexes::GetValidCount() const
{
    size_t count = 0;
    count += IsGraphicsValid() ? 1 : 0;
    count += IsComputeValid() ? 1 : 0;
    count += IsTransferValid() ? 1 : 0;
    count += IsSparseBindingValid() ? 1 : 0;
    count += IsPresentValid() ? 1 : 0;
    return count;
}

StaticVector<VkDeviceQueueCreateInfo, VulkanQueueFamilyIndexes::MAX_VALID_COUNT> VulkanQueueFamilyIndexes::GetCreateInfo(const float* queuePriorities, size_t queuePriorityCount)
{
    StaticVector<VkDeviceQueueCreateInfo, MAX_VALID_COUNT> queueInfos;

    if (IsGraphicsValid())
        queueInfos.push_back(VulkanDeviceQueueCreateInfo(this->graphics, static_cast<uint32_t>(queuePriorityCount), queuePriorities));
    if (IsComputeValid())
        queueInfos.push_back(VulkanDeviceQueueCreateInfo(this->compute, static_cast<uint32_t>(queuePriorityCount), queuePriorities));
    if (IsTransferValid())
        queueInfos.push_back(VulkanDeviceQueueCreateInfo(this->transfer, static_cast<uint32_t>(queuePriorityCount), queuePriorities));
    if (IsSparseBindingValid())
        queueInfos.push_back(VulkanDeviceQueueCreateInfo(this->sparseBinding, static_cast<uint32_t>(queuePriorityCount), queuePriorities));

    return queueInfos;
}

#endif
