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
#include "VulkanIncludes.hpp"
#include "VulkanGpuContext.hpp"
#include "VulkanGpuDescription.hpp"
#include "VulkanLayerProperties.hpp"
#include "VulkanNamePointers.hpp"
#include "VulkanSystemSettings.hpp"
#include "finjin/common/LibraryLoader.hpp"
#include "finjin/common/StaticVector.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    class VulkanSystemImpl : public AllocatedClass
    {
    public:
        VulkanSystemImpl(Allocator* allocator);

        bool IsDebugEnabled() const;

        VkPhysicalDevice GetBestVulkanPhysicalDevice(Utf8String& gpuID, VulkanGpuDescription& gpuDescription, Error& error);

        void EnumeratePhysicalDevices(Error& error);

        void GetInstanceExtensionNames(VulkanNamePointers& names);
        void GetInstanceLayerNames(VulkanNamePointers& names, VulkanNamePointers& extensions);

        void GetDeviceExtensionNames(VulkanNamePointers& names);

        void GetLayerNamesAndExtensions(VulkanNamePointers& names, VulkanNamePointers& extensions, const char* desiredDebugLayers[], size_t desiredDebugLayersCount);

        PFN_vkGetInstanceProcAddr LoadVulkan(Error& error);
        void UnloadVulkan();

    public:
        VulkanSystemSettings settings;
        VulkanLayerProperties layerProperties;
        VulkanHardwareGpuDescriptions hardwareGpuDescriptions;
        VulkanSoftwareGpuDescriptions softwareGpuDescriptions;

        LibraryLoader vulkanLibraryLoader;

        VkAllocationCallbacks* systemAllocationCallbacks; //Not currently set to anything

        struct VulkanFunctions : VulkanGlobalFunctions, VulkanInstanceFunctions
        {
        };
        VulkanFunctions vk;

        StaticVector<VkPhysicalDevice, EngineConstants::MAX_GPU_ADAPTERS> physicalDevices;

        StaticVector<VulkanGpuContext*, EngineConstants::MAX_APPLICATION_VIEWPORTS> contexts;
    };

} }
