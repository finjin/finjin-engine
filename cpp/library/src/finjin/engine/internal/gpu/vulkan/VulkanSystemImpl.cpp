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

#include "VulkanSystemImpl.hpp"
#if FINJIN_TARGET_PLATFORM_IS_WINDOWS
    #include "finjin/common/WindowsUtilities.hpp"
#endif
#include "VulkanUtilities.hpp"

using namespace Finjin::Engine;


//Implementation----------------------------------------------------------------
VulkanSystemImpl::VulkanSystemImpl(Allocator* allocator) : AllocatedClass(allocator), settings(allocator)
{
    this->systemAllocationCallbacks = nullptr;
}

bool VulkanSystemImpl::IsDebugEnabled() const
{
    return this->settings.enableDebug;
}

VkPhysicalDevice VulkanSystemImpl::GetBestVulkanPhysicalDevice(Utf8String& gpuID, VulkanGpuDescription& gpuDescription, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    const VulkanGpuDescription* foundGpuDescription = nullptr;

    //Find by GPU ID
    if (!gpuID.empty())
    {
        foundGpuDescription = this->hardwareGpuDescriptions.GetByGpuID(gpuID);
        if (foundGpuDescription == nullptr)
            foundGpuDescription = this->softwareGpuDescriptions.GetByGpuID(gpuID);
    }

    //Find take first (best) device
    if (foundGpuDescription == nullptr)
    {
        if (!this->hardwareGpuDescriptions.empty())
            foundGpuDescription = &this->hardwareGpuDescriptions[0];
        else if (!this->softwareGpuDescriptions.empty())
            foundGpuDescription = &this->softwareGpuDescriptions[0];
    }

    if (foundGpuDescription == nullptr)
    {
        FINJIN_SET_ERROR(error, "Failed to get best Vulkan physical device.");
        return nullptr;
    }
    else
    {
        if (gpuID.assign(foundGpuDescription->GetGpuID()).HasError())
        {
            FINJIN_SET_ERROR(error, "Failed to assign GPU ID.");
            return nullptr;
        }
        gpuDescription = *foundGpuDescription;
        return this->physicalDevices[foundGpuDescription->physicalDeviceIndex];
    }
}

void VulkanSystemImpl::EnumeratePhysicalDevices(Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    VkResult result;

    //Get all physical devices----------------------------------------
    uint32_t physicalDeviceCount = 0;
    do
    {
        result = this->vk.EnumeratePhysicalDevices(this->vk.instance, &physicalDeviceCount, nullptr);
        if (result == VK_INCOMPLETE)
        {
            //Do nothing
        }
        else if (FINJIN_CHECK_VKRESULT_FAILED(result))
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to get Vulkan physical device count. Vulkan result: %1%", VulkanResult::ToString(result)));
            return;
        }
    } while (result == VK_INCOMPLETE);

    if (physicalDeviceCount == 0)
    {
        FINJIN_SET_ERROR(error, "Failed to detect any Vulkan physical devices.");
        return;
    }

    physicalDeviceCount = static_cast<uint32_t>(this->physicalDevices.resize(physicalDeviceCount));
    do
    {
        result = this->vk.EnumeratePhysicalDevices(this->vk.instance, &physicalDeviceCount, this->physicalDevices.data());
        if (result == VK_INCOMPLETE)
        {
            //Do nothing
        }
        else if (FINJIN_CHECK_VKRESULT_FAILED(result))
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to get Vulkan physical devices. Vulkan result: %1%", VulkanResult::ToString(result)));
            return;
        }
    } while (result == VK_INCOMPLETE);

    //Get more info--------------------------------------------------------------
    size_t physicalDeviceIndex = 0;

    this->hardwareGpuDescriptions.resize(this->physicalDevices.size());
    size_t hardwareAdapterCount = 0;
    for (auto physicalDevice : this->physicalDevices)
    {
        VulkanGpuDescription* gpuDescription = nullptr;

        VkPhysicalDeviceProperties physicalDeviceProperties;
        this->vk.GetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);
        if (physicalDeviceProperties.deviceType != VK_PHYSICAL_DEVICE_TYPE_CPU)
        {
            //Hardware
            if (hardwareAdapterCount < this->hardwareGpuDescriptions.size())
                gpuDescription = &this->hardwareGpuDescriptions[hardwareAdapterCount++];
        }
        else if (this->softwareGpuDescriptions.empty())
        {
            //Software
            this->softwareGpuDescriptions.resize(1);
            gpuDescription = &this->softwareGpuDescriptions[0];
        }

        if (gpuDescription != nullptr)
        {
            gpuDescription->physicalDeviceIndex = physicalDeviceIndex;

            gpuDescription->properties = physicalDeviceProperties;
            this->vk.GetPhysicalDeviceFeatures(physicalDevice, &gpuDescription->features);
            this->vk.GetPhysicalDeviceMemoryProperties(physicalDevice, &gpuDescription->memoryProperties);

            uint32_t queueCount = 0;
            this->vk.GetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueCount, nullptr);
            gpuDescription->queueProperties.resize(queueCount);
            this->vk.GetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueCount, gpuDescription->queueProperties.data());
        }

        physicalDeviceIndex++;
    }
    this->hardwareGpuDescriptions.resize(hardwareAdapterCount);
    this->hardwareGpuDescriptions.SortBestToWorst();
}

void VulkanSystemImpl::GetInstanceExtensionNames(VulkanNamePointers& names)
{
    names.push_back(VK_KHR_SURFACE_EXTENSION_NAME);

#if defined(VK_USE_PLATFORM_WIN32_KHR)
    names.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_XCB_KHR)
    names.push_back(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_ANDROID_KHR)
    names.push_back(VK_KHR_ANDROID_SURFACE_EXTENSION_NAME);
#endif

    if (this->settings.enableDebug)
        names.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
}

void VulkanSystemImpl::GetInstanceLayerNames(VulkanNamePointers& names, VulkanNamePointers& extensions)
{
    static const char* desiredDebugLayers[] =
        {
        //This order matters!
        //https://vulkan.lunarg.com/doc/sdk/1.0.49.0/windows/getting_started.html
        //"VK_LAYER_GOOGLE_threading", //Disabled since the latest Vulkan update seems to cause a problem (https://github.com/KhronosGroup/Vulkan-LoaderAndValidationLayers/commit/a00917b8295326a0c0501aefe5ab4c12a501a4c5)
        "VK_LAYER_LUNARG_parameter_validation",
        "VK_LAYER_LUNARG_param_checker",
        "VK_LAYER_LUNARG_device_limits",
        "VK_LAYER_LUNARG_object_tracker",
        "VK_LAYER_LUNARG_image",
        "VK_LAYER_LUNARG_mem_tracker",
        "VK_LAYER_LUNARG_draw_state",
        "VK_LAYER_LUNARG_swapchain",
        "VK_LAYER_GOOGLE_unique_objects"
        };

    if (this->settings.enableDebug)
        GetLayerNamesAndExtensions(names, extensions, desiredDebugLayers, FINJIN_COUNT_OF(desiredDebugLayers));
}

void VulkanSystemImpl::GetDeviceExtensionNames(VulkanNamePointers& names)
{
    names.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
}

void VulkanSystemImpl::GetLayerNamesAndExtensions(VulkanNamePointers& names, VulkanNamePointers& extensions, const char* desiredDebugLayers[], size_t desiredDebugLayersCount)
{
    for (size_t layerIndex = 0; layerIndex < desiredDebugLayersCount; layerIndex++)
    {
        auto foundLayer = this->layerProperties.GetByName(desiredDebugLayers[layerIndex]);
        if (foundLayer != nullptr)
        {
            //Add layer name
            auto added = names.push_back(foundLayer->properties.layerName);
            assert(!added.HasErrorOrValue(false));
            if (added.HasErrorOrValue(false))
                break;

            //Add extensions
            for (auto& ext : foundLayer->extensions)
            {
                if (!extensions.HasName(ext.extensionName))
                {
                    added = extensions.push_back(ext.extensionName);
                    assert(!added.HasErrorOrValue(false));
                }
            }
        }
    }
}

PFN_vkGetInstanceProcAddr VulkanSystemImpl::LoadVulkan(Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

#if FINJIN_TARGET_PLATFORM_IS_WINDOWS
    auto vulkanFileName = "vulkan-1.dll";
    auto vulkanFileNameExtension = ".dll";
#elif FINJIN_TARGET_PLATFORM_IS_LINUX
    auto vulkanFileName = "libvulkan.so";
    auto vulkanFileNameExtension = ".so";
#else
    #error Implement this!
#endif

    StaticVector<Path, 3> paths;

    //Plain file name---------------
    paths.push_back(vulkanFileName);

    //Full path + file name, as specified by the settings---------------
    auto vulkanRuntimePath = this->settings.driverRuntimePath;
    if (!vulkanRuntimePath.empty())
    {
        if ((vulkanRuntimePath /= vulkanFileName).HasError())
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to append Vulkan file name '%1%' to path.", vulkanFileName));
            return nullptr;
        }
        paths.push_back(vulkanRuntimePath);
    }

    //Full path + file name, as specified by the environment---------------
    vulkanRuntimePath.clear();
#if FINJIN_TARGET_PLATFORM_IS_WINDOWS
    if (WindowsUtilities::GetEnv(vulkanRuntimePath, "VULKAN_DLL_PATH").HasError())
    {
        //Environment variable not available. Do nothing
    }
#elif FINJIN_TARGET_PLATFORM_IS_LINUX
    if (vulkanRuntimePath.assign(getenv("VULKAN_SO_PATH")).HasError())
    {
        //Environment variable not available. Do nothing
    }
#endif
    if (!vulkanRuntimePath.empty())
    {
        if (vulkanRuntimePath.IsDirectory())
        {
            //Path is a directory
            if ((vulkanRuntimePath /= vulkanFileName).HasError())
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to append Vulkan file name '%1%' to path.", vulkanFileName));
                return nullptr;
            }
        }
        else if (!vulkanRuntimePath.EndsWith(vulkanFileNameExtension))
        {
            //Assume the path is a file name
            if ((vulkanRuntimePath /= vulkanFileNameExtension).HasError())
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to append Vulkan file extension '%1%' to path.", vulkanFileNameExtension));
                return nullptr;
            }
        }
        paths.push_back(vulkanRuntimePath);
    }

    //Search all paths until the library can be loaded
    for (auto& path : paths)
    {
        if (this->vulkanLibraryLoader.Load(path))
        {
            //Library was loaded

            //Get the instance entry point
            auto getInstanceProc = this->vulkanLibraryLoader.GetProc<PFN_vkGetInstanceProcAddr>("vk_icdGetInstanceProcAddr");
            if (getInstanceProc == nullptr)
                getInstanceProc = this->vulkanLibraryLoader.GetProc<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
            if (getInstanceProc == nullptr)
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to get load proc 'vk_icdGetInstanceProcAddr' or 'vkGetInstanceProcAddr' from Vulkan library at '%1%'.", path));
                return nullptr;
            }

            return getInstanceProc;
        }
    }

    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to load Vulkan library '%1%'.", vulkanFileName));
    return nullptr;
}

void VulkanSystemImpl::UnloadVulkan()
{
    this->vulkanLibraryLoader.Unload();
}

#endif
