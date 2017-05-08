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

#include "VulkanGpuDescription.hpp"

using namespace Finjin::Engine;


//Local functions---------------------------------------------------------------
static int GetPhysicalDeviceTypePriority(VkPhysicalDeviceType deviceType)
{
    switch (deviceType)
    {
        case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU: return 4;
        case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU: return 3;
        case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU: return 2;
        case VK_PHYSICAL_DEVICE_TYPE_CPU: return 1;
        default: return 0;
    }
}

static bool MemoryHeapHasProperty(const VkPhysicalDeviceMemoryProperties& memoryProperties, uint32_t heapIndex, VkMemoryPropertyFlagBits propertyBits, VkMemoryPropertyFlagBits notPropertyBits = (VkMemoryPropertyFlagBits)0)
{
    for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++)
    {
        if (memoryProperties.memoryTypes[i].heapIndex == heapIndex &&
            (memoryProperties.memoryTypes[i].propertyFlags & propertyBits) == propertyBits &&
            (memoryProperties.memoryTypes[i].propertyFlags & notPropertyBits) == 0)
        {
            return true;
        }
    }

    return false;
}


//Implementation----------------------------------------------------------------

//VulkanGpuDescription
VulkanGpuDescription::VulkanGpuDescription()
{
    this->physicalDeviceIndex = 0;
    FINJIN_ZERO_ITEM(this->properties);
    FINJIN_ZERO_ITEM(this->features);
    FINJIN_ZERO_ITEM(this->memoryProperties);
}

const char* VulkanGpuDescription::GetGpuID() const
{
    return this->properties.deviceName;
}

void VulkanGpuDescription::GetApiVersion(uint32_t& major, uint32_t& minor, uint32_t& patch) const
{
    major = VK_VERSION_MAJOR(this->properties.apiVersion);
    minor = VK_VERSION_MINOR(this->properties.apiVersion);
    patch = VK_VERSION_PATCH(this->properties.apiVersion);
}

VkDeviceSize VulkanGpuDescription::GetMemorySize(VkMemoryPropertyFlagBits propertyBits, VkMemoryPropertyFlagBits notPropertyBits) const
{
    VkDeviceSize size = 0;

    for (uint32_t heapIndex = 0; heapIndex < this->memoryProperties.memoryHeapCount; heapIndex++)
    {
        if (MemoryHeapHasProperty(this->memoryProperties, heapIndex, propertyBits, notPropertyBits))
            size += this->memoryProperties.memoryHeaps[heapIndex].size;
    }

    return size;
}

bool VulkanGpuDescription::GetMemoryTypeIndexFromProperties(uint32_t typeBits, VkFlags propertyRequirementsBits, uint32_t& resultTypeIndex) const
{
    //Search memory properties to find first index with those properties
    for (uint32_t typeIndex = 0; typeBits != 0 && typeIndex < VK_MAX_MEMORY_TYPES; typeIndex++)
    {
        if (typeBits & 1)
        {
            //Type is available
            if ((this->memoryProperties.memoryTypes[typeIndex].propertyFlags & propertyRequirementsBits) == propertyRequirementsBits)
            {
                //Matches specified requirements
                resultTypeIndex = typeIndex;
                return true;
            }
        }
        typeBits >>= 1;
    }

    return false;
}

uint32_t VulkanGpuDescription::GetMaxMipMapCount() const
{
    auto& limits = this->properties.limits;
    auto maxDimension = std::max(std::max(limits.maxImageDimension1D, limits.maxImageDimension2D), limits.maxImageDimension3D);
    return RoundToInt(log2(maxDimension)) + 1;
}

VkDeviceSize VulkanGpuDescription::GetDedicatedDeviceMemorySize() const
{
    //Device memory accessible by GPU
    return GetMemorySize(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
}

VkDeviceSize VulkanGpuDescription::GetDedicatedSystemMemorySize() const
{
    //Host memory accessible by GPU (allocated when computer starts up)

    //It doesn't appear to be possible to determine this from the standard flags provided by Vulkan,
    //though it may be possible using the vkGetPhysicalDeviceExternalBufferPropertiesKHX extension
    return 0; //Returning 0 has the effect of rating host GPU memory the same as device GPU memory
}

VkDeviceSize VulkanGpuDescription::GetTotalDedicatedMemorySize() const
{
    return GetDedicatedDeviceMemorySize() + GetDedicatedSystemMemorySize();
}

//VulkanHardwareGpuDescriptions
const VulkanHardwareGpuDescription* VulkanHardwareGpuDescriptions::GetByGpuID(const Utf8String& gpuID) const
{
    for (const auto& desc : *this)
    {
        if (gpuID == desc.properties.deviceName)
            return &desc;
    }

    return nullptr;
}

const VulkanHardwareGpuDescription* VulkanHardwareGpuDescriptions::GetByDeviceType(VkPhysicalDeviceType deviceType) const
{
    for (const auto& desc : *this)
    {
        if (desc.properties.deviceType == deviceType)
            return &desc;
    }

    return nullptr;
}

void VulkanHardwareGpuDescriptions::SortBestToWorst()
{
    //Order by device type, then the various GPU memory types/sizes
    std::sort(begin(), end(), [](const VulkanHardwareGpuDescription& a, const VulkanHardwareGpuDescription& b)
    {
        if (GetPhysicalDeviceTypePriority(a.properties.deviceType) != GetPhysicalDeviceTypePriority(b.properties.deviceType))
            return GetPhysicalDeviceTypePriority(a.properties.deviceType) > GetPhysicalDeviceTypePriority(b.properties.deviceType);
        if (a.GetDedicatedDeviceMemorySize() != b.GetDedicatedDeviceMemorySize())
            return a.GetDedicatedDeviceMemorySize() > b.GetDedicatedDeviceMemorySize();
        if (a.GetDedicatedSystemMemorySize() != b.GetDedicatedSystemMemorySize())
            return a.GetDedicatedSystemMemorySize() < b.GetDedicatedSystemMemorySize();
        return a.GetTotalDedicatedMemorySize() > b.GetTotalDedicatedMemorySize();
    });
}

//VulkanSoftwareGpuDescriptions
const VulkanHardwareGpuDescription* VulkanSoftwareGpuDescriptions::GetByGpuID(const Utf8String& gpuID) const
{
    for (auto& desc : *this)
    {
        if (gpuID == desc.properties.deviceName)
            return &desc;
    }

    return nullptr;
}

#endif
