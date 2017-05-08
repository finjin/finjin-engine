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
#include "finjin/common/StaticVector.hpp"
#include "finjin/common/Utf8String.hpp"
#include "VulkanIncludes.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    using VulkanQueueFamilyProperties = StaticVector<VkQueueFamilyProperties, 16>;

    struct VulkanGpuDescription
    {
        VulkanGpuDescription();

        const char* GetGpuID() const;

        void GetApiVersion(uint32_t& major, uint32_t& minor, uint32_t& patch) const;

        VkDeviceSize GetMemorySize(VkMemoryPropertyFlagBits propertyBits, VkMemoryPropertyFlagBits notPropertyBits = (VkMemoryPropertyFlagBits)0) const;

        bool GetMemoryTypeIndexFromProperties(uint32_t typeBits, VkFlags propertyRequirementsBits, uint32_t& resultTypeIndex) const;

        uint32_t GetMaxMipMapCount() const;

        VkDeviceSize GetDedicatedDeviceMemorySize() const;
        VkDeviceSize GetDedicatedSystemMemorySize() const;
        VkDeviceSize GetTotalDedicatedMemorySize() const;

        size_t physicalDeviceIndex;
        VkPhysicalDeviceProperties properties;
        VkPhysicalDeviceFeatures features;
        VkPhysicalDeviceMemoryProperties memoryProperties;
        VulkanQueueFamilyProperties queueProperties;
    };

    using VulkanHardwareGpuDescription = VulkanGpuDescription;
    class VulkanHardwareGpuDescriptions : public StaticVector<VulkanHardwareGpuDescription, EngineConstants::MAX_GPU_ADAPTERS>
    {
    public:
        const VulkanHardwareGpuDescription* GetByGpuID(const Utf8String& gpuID) const;
        const VulkanHardwareGpuDescription* GetByDeviceType(VkPhysicalDeviceType deviceType) const;

        void SortBestToWorst();
    };

    using VulkanSoftwareGpuDescription = VulkanGpuDescription;
    class VulkanSoftwareGpuDescriptions : public StaticVector<VulkanSoftwareGpuDescription, 1>
    {
    public:
        const VulkanHardwareGpuDescription* GetByGpuID(const Utf8String& gpuID) const;
    };

} }
