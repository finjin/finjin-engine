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

#include "VulkanLayerProperties.hpp"
#include "VulkanUtilities.hpp"

using namespace Finjin::Engine;


//Implementation----------------------------------------------------------------
void VulkanLayerProperties::Create(VulkanGlobalFunctions& vulkanGlobalFunctions, Allocator* allocator, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    Destroy();

    VkResult result;

    uint32_t layerCount;
    do
    {
        result = vulkanGlobalFunctions.EnumerateInstanceLayerProperties(&layerCount, nullptr);
        if (result == VK_INCOMPLETE)
        {
            //Do nothing
        }
        else if (FINJIN_CHECK_VKRESULT_FAILED(result))
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to get Vulkan layer property count. Vulkan result: %1%", VulkanResult::ToString(result)));
            return;
        }
    } while (result == VK_INCOMPLETE);

    if (layerCount == 0)
        return;

    StaticVector<VkLayerProperties, 32> tempLayerProperties;
    tempLayerProperties.resize(layerCount);
    do
    {
        result = vulkanGlobalFunctions.EnumerateInstanceLayerProperties(&layerCount, tempLayerProperties.data());
        if (result == VK_INCOMPLETE)
        {
            //Do nothing
        }
        else if (FINJIN_CHECK_VKRESULT_FAILED(result))
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to get Vulkan layer properties. Vulkan result: %1%", VulkanResult::ToString(result)));
            return;
        }
    } while (result == VK_INCOMPLETE);

    //Now gather the extension list for each instance layer
    if (!CreateEmpty(layerCount, allocator))
    {
        FINJIN_SET_ERROR(error, "Failed to allocate layer properties array.");
        return;
    }
    for (uint32_t layerIndex = 0; layerIndex < layerCount; layerIndex++)
    {
        push_back();
        auto& layerExtensions = back();

        layerExtensions.properties = tempLayerProperties[layerIndex];

        EnumerateExtensions(vulkanGlobalFunctions, layerExtensions, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to enumerate Vulkan layer extensions.");
            return;
        }
    }
}

void VulkanLayerProperties::EnumerateExtensions(VulkanGlobalFunctions& vulkanGlobalFunctions, VulkanLayerPropertyExtensions& layerExtensions, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    layerExtensions.extensions.clear();

    VkResult result;

    uint32_t instanceExtensionCount;
    do
    {
        result = vulkanGlobalFunctions.EnumerateInstanceExtensionProperties(layerExtensions.properties.layerName, &instanceExtensionCount, nullptr);
        if (result == VK_INCOMPLETE)
        {
            //Do nothing
        }
        else if (FINJIN_CHECK_VKRESULT_FAILED(result))
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to get Vulkan layer extension count. Vulkan result: %1%", VulkanResult::ToString(result)));
            return;
        }
    } while (result == VK_INCOMPLETE);

    if (instanceExtensionCount == 0)
        return;

    layerExtensions.extensions.resize(instanceExtensionCount);
    do
    {
        result = vulkanGlobalFunctions.EnumerateInstanceExtensionProperties(layerExtensions.properties.layerName, &instanceExtensionCount, layerExtensions.extensions.data());
        if (result == VK_INCOMPLETE)
        {
            //Do nothing
        }
        else if (FINJIN_CHECK_VKRESULT_FAILED(result))
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to get Vulkan layer extensions. Vulkan result: %1%", VulkanResult::ToString(result)));
            return;
        }
    } while (result == VK_INCOMPLETE);
}

const VulkanLayerPropertyExtensions* VulkanLayerProperties::GetByName(const Utf8String& layerName) const
{
    for (auto& item : *this)
    {
        if (layerName == item.properties.layerName)
            return &item;
    }

    return nullptr;
}

#endif
