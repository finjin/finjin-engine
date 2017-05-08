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

#include "VulkanGpuContextImpl.hpp"
#include "finjin/common/Convert.hpp"
#include "finjin/common/DebugLog.hpp"
#include "finjin/common/PNGReader.hpp"
#include "finjin/common/WrappedFileReader.hpp"
#include "finjin/engine/ASTCReader.hpp"
#include "finjin/engine/KTXReader.hpp"
#include "finjin/engine/OSWindow.hpp"
#include "finjin/engine/StandardAssetDocumentPropertyValues.hpp"
#include "VulkanSystem.hpp"
#include "VulkanSystemImpl.hpp"
#include "VulkanUtilities.hpp"

using namespace Finjin::Engine;


//Macros------------------------------------------------------------------------
#define DEFAULT_COLOR_FORMAT VK_FORMAT_B8G8R8A8_UNORM

#define TEXTURE_SAMPLER_BINDING_ID 0
#define TEXTURES_START_BINDING_ID 1


//Local types-------------------------------------------------------------------
enum class AttachmentIndex
{
    COLOR,
    DEPTH,

    COUNT
};

enum class TextureBindingIndex
{
    DEFAULT_SAMPLER,
    TEXTURES_START,

    COUNT
};


//Implementation----------------------------------------------------------------

//VulkanGpuContextImpl::InternalSettings
VulkanGpuContextImpl::InternalSettings::InternalSettings()
{
    this->contextImpl = nullptr;
}

void VulkanGpuContextImpl::InternalSettings::ParseSettings(const ByteBufferReader& configFileBuffer, const Path& assetFilePath, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    ConfigDocumentReader reader;
    if (reader.Start(configFileBuffer) != nullptr)
    {
        DefaultBufferPackingRules<GpuInputFormatStruct> packingRules(1, sizeof(float), 1);

        GpuInputFormatStruct::Create
            (
            this->inputFormats,
            this->contextImpl->GetAllocator(),
            reader,
            packingRules,
            GpuInputFormatStruct::Defines::NONE,
            error
            );
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to create input formats.");
            return;
        }

        for (auto line = reader.Current(); line != nullptr; line = reader.Next())
        {
            switch (line->GetType())
            {
                case ConfigDocumentLine::Type::KEY_AND_VALUE:
                {
                    if (line->GetDepth() == 0)
                    {
                        Utf8StringView key, value;
                        line->GetKeyAndValue(key, value);

                        if (key == "max-materials")
                            this->contextImpl->settings.maxMaterials = Convert::ToInteger(value.ToString(), this->contextImpl->settings.maxMaterials);
                        else if (key == "max-2d-textures")
                            this->contextImpl->settings.max2DTextures = Convert::ToInteger(value.ToString(), this->contextImpl->settings.max2DTextures);
                        else if (key == "max-cube-textures")
                            this->contextImpl->settings.maxCubeTextures = Convert::ToInteger(value.ToString(), this->contextImpl->settings.maxCubeTextures);
                        else if (key == "max-3d-textures")
                            this->contextImpl->settings.max3DTextures = Convert::ToInteger(value.ToString(), this->contextImpl->settings.max3DTextures);
                        else if (key == "max-shaders")
                            this->contextImpl->settings.maxShaders = Convert::ToInteger(value.ToString(), this->contextImpl->settings.maxShaders);
                        else if (key == "max-meshes")
                            this->contextImpl->settings.maxMeshes = Convert::ToInteger(value.ToString(), this->contextImpl->settings.maxMeshes);
                        else if (key == "max-lights")
                            this->contextImpl->settings.maxLights = Convert::ToInteger(value.ToString(), this->contextImpl->settings.maxLights);
                    }
                    break;
                }
                default: break;
            }
        }
    }
}

void VulkanGpuContextImpl::InternalSettings::Validate(Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    if (this->inputFormats.empty())
    {
        FINJIN_SET_ERROR(error, "No input formats were specified.");
        return;
    }

    if (this->contextImpl->settings.maxShaders == 0)
    {
        FINJIN_SET_ERROR(error, "The max shader count is zero.");
        return;
    }

    if (this->contextImpl->settings.maxMeshes == 0)
    {
        FINJIN_SET_ERROR(error, "The max mesh count is zero.");
        return;
    }

    if (this->contextImpl->settings.maxMaterials == 0)
    {
        FINJIN_SET_ERROR(error, "The max material count is zero.");
        return;
    }

    //maxTextures and maxLights are not checked since it is conceivable that they are not needed
}

GpuInputFormatStruct* VulkanGpuContextImpl::InternalSettings::GetInputFormat(const Utf8String& typeName, const DynamicVector<FinjinVertexElementFormat>& elements)
{
    if (!typeName.empty())
    {
        //Get format with matching name
        for (auto& inputFormat : this->inputFormats)
        {
            if (inputFormat.GetTypeName() == typeName)
                return &inputFormat;
        }
    }
    else
    {
        //Get format with matching elements
        auto count = elements.size();
        for (auto& inputFormat : this->inputFormats)
        {
            if (count == inputFormat.GetElementCount())
            {
                size_t i = 0;
                for (i = 0; i < count; i++)
                {
                    auto inputFormatElement = inputFormat.GetElement(i);
                    if (elements[i].id != inputFormatElement->elementID || elements[i].type != inputFormatElement->type)
                        break;
                }
                if (i == count)
                    return &inputFormat;
            }
        }
    }

    return nullptr;
}

//VulkanGpuContextImpl
VulkanGpuContextImpl::VulkanGpuContextImpl(Allocator* allocator) : AllocatedClass(allocator), settings(allocator), clearColor(0, 0, 0, 0)
{
    this->physicalDevice = VK_NULL_HANDLE;

    this->vk.device = VK_NULL_HANDLE;

    this->surface = VK_NULL_HANDLE;

    this->swapChain = VK_NULL_HANDLE;
    this->presentPossibleSemaphore = VK_NULL_HANDLE;

    this->renderPass = VK_NULL_HANDLE;

    this->sequenceIndex = 0;

    this->nextFrameBufferIndex = 0;

    this->materialMapTypeToGpuElements.insert(StandardAssetDocumentPropertyValues::TypeName::MATERIAL_MAP_DIFFUSE, MaterialMapTypeToGpuElements(VulkanMaterial::MapIndex::DIFFUSE, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_DIFFUSE_TEXTURE_INDEX, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_DIFFUSE_AMOUNT, ShaderFeatureFlag::MAP_DIFFUSE), false);
    this->materialMapTypeToGpuElements.insert(StandardAssetDocumentPropertyValues::TypeName::MATERIAL_MAP_SPECULAR, MaterialMapTypeToGpuElements(VulkanMaterial::MapIndex::SPECULAR, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_SPECULAR_TEXTURE_INDEX, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_SPECULAR_AMOUNT, ShaderFeatureFlag::MAP_SPECULAR), false);
    this->materialMapTypeToGpuElements.insert(StandardAssetDocumentPropertyValues::TypeName::MATERIAL_MAP_REFLECTION, MaterialMapTypeToGpuElements(VulkanMaterial::MapIndex::REFLECTION, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_REFLECTION_TEXTURE_INDEX, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_REFLECTION_AMOUNT, ShaderFeatureFlag::MAP_REFLECTION), false);
    this->materialMapTypeToGpuElements.insert(StandardAssetDocumentPropertyValues::TypeName::MATERIAL_MAP_REFRACTION, MaterialMapTypeToGpuElements(VulkanMaterial::MapIndex::REFRACTION, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_REFRACTION_TEXTURE_INDEX, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_REFRACTION_AMOUNT, ShaderFeatureFlag::MAP_REFRACTION), false);
    this->materialMapTypeToGpuElements.insert(StandardAssetDocumentPropertyValues::TypeName::MATERIAL_MAP_BUMP, MaterialMapTypeToGpuElements(VulkanMaterial::MapIndex::BUMP, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_BUMP_TEXTURE_INDEX, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_BUMP_AMOUNT, ShaderFeatureFlag::MAP_BUMP), false);
    this->materialMapTypeToGpuElements.insert(StandardAssetDocumentPropertyValues::TypeName::MATERIAL_MAP_HEIGHT, MaterialMapTypeToGpuElements(VulkanMaterial::MapIndex::HEIGHT, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_HEIGHT_TEXTURE_INDEX, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_HEIGHT_AMOUNT, ShaderFeatureFlag::MAP_HEIGHT), false);
    this->materialMapTypeToGpuElements.insert(StandardAssetDocumentPropertyValues::TypeName::MATERIAL_MAP_SELF_ILLUMINATION, MaterialMapTypeToGpuElements(VulkanMaterial::MapIndex::SELF_ILLUMINATION, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_SELF_ILLUMINATION_TEXTURE_INDEX, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_SELF_ILLUMINATION_AMOUNT, ShaderFeatureFlag::MAP_SELF_ILLUMINATION), false);
    this->materialMapTypeToGpuElements.insert(StandardAssetDocumentPropertyValues::TypeName::MATERIAL_MAP_OPACITY, MaterialMapTypeToGpuElements(VulkanMaterial::MapIndex::OPACITY, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_OPACITY_TEXTURE_INDEX, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_OPACITY_AMOUNT, ShaderFeatureFlag::MAP_OPACITY), false);
    this->materialMapTypeToGpuElements.insert(StandardAssetDocumentPropertyValues::TypeName::MATERIAL_MAP_ENVIRONMENT, MaterialMapTypeToGpuElements(VulkanMaterial::MapIndex::ENVIRONMENT, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_ENVIRONMENT_TEXTURE_INDEX, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_ENVIRONMENT_AMOUNT, ShaderFeatureFlag::MAP_ENVIRONMENT), false);
    this->materialMapTypeToGpuElements.insert(StandardAssetDocumentPropertyValues::TypeName::MATERIAL_MAP_SHININESS, MaterialMapTypeToGpuElements(VulkanMaterial::MapIndex::SHININESS, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_SHININESS_TEXTURE_INDEX, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_SHININESS_AMOUNT, ShaderFeatureFlag::MAP_SHININESS), false);
}

void VulkanGpuContextImpl::Create(const VulkanGpuContextSettings& settings, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    assert(settings.osWindow != nullptr);
    assert(settings.assetFileReader != nullptr);

    //Copy settings
    this->settings = settings;

    if (!this->readBuffer.Create(this->settings.tempReadBufferSize, GetAllocator()))
    {
        FINJIN_SET_ERROR(error, "Failed to create read buffer.");
        return;
    }

    //Create device
    CreateDevice(error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to find a suitable Vulkan device.");
        return;
    }

    //Create other objects
    CreateDeviceSupportObjects(error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to create support objects.");
        return;
    }

    //Renderers
    CreateRenderers(error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to create renderers.");
        return;
    }

    this->settings.osWindow->AddWindowEventListener(this);
}

void VulkanGpuContextImpl::Destroy()
{
    //Wait for the device to go idle
    if (this->vk.device != VK_NULL_HANDLE)
        this->vk.DeviceWaitIdle(this->vk.device);

    this->settings.osWindow->RemoveWindowEventListener(this);

    if (this->vk.device != VK_NULL_HANDLE)
    {
        this->staticMeshRenderer.Destroy();

        this->assetResources.Destroy(this->vk, this->settings.deviceAllocationCallbacks);

        //Destroy screen size dependent resources
        DestroyScreenSizeDependentResources(false);

        //Destroy frame buffer resources
        for (auto& frameBuffer : this->frameBuffers)
        {
            if (frameBuffer.renderingCompleteSemaphore != VK_NULL_HANDLE)
            {
                this->vk.DestroySemaphore(this->vk.device, frameBuffer.renderingCompleteSemaphore, this->settings.deviceAllocationCallbacks);
                frameBuffer.renderingCompleteSemaphore = VK_NULL_HANDLE;
            }

            if (frameBuffer.renderingCompleteFence != VK_NULL_HANDLE)
            {
                this->vk.DestroyFence(this->vk.device, frameBuffer.renderingCompleteFence, this->settings.deviceAllocationCallbacks);
                frameBuffer.renderingCompleteFence = VK_NULL_HANDLE;
            }

            frameBuffer.graphicsCommandBuffers.Destroy();

            if (frameBuffer.graphicsCommandPool != VK_NULL_HANDLE)
            {
                this->vk.DestroyCommandPool(this->vk.device, frameBuffer.graphicsCommandPool, this->settings.deviceAllocationCallbacks);
                frameBuffer.graphicsCommandPool = VK_NULL_HANDLE;
            }
        }
        this->frameStages.clear();

        //Destroy texture objects
        if (this->textureResources.defaultSampler != VK_NULL_HANDLE)
        {
            this->vk.DestroySampler(this->vk.device, this->textureResources.defaultSampler, this->settings.deviceAllocationCallbacks);
            this->textureResources.defaultSampler = VK_NULL_HANDLE;
        }

        if (this->textureResources.texturesDescriptorSetLayout != VK_NULL_HANDLE)
        {
            this->vk.DestroyDescriptorSetLayout(this->vk.device, this->textureResources.texturesDescriptorSetLayout, this->settings.deviceAllocationCallbacks);
            this->textureResources.texturesDescriptorSetLayout = VK_NULL_HANDLE;
        }

        if (this->textureResources.textureDescriptorPool != VK_NULL_HANDLE)
        {
            this->vk.DestroyDescriptorPool(this->vk.device, this->textureResources.textureDescriptorPool, this->settings.deviceAllocationCallbacks);
            this->textureResources.textureDescriptorPool = VK_NULL_HANDLE;
            this->textureResources.textureDescriptorSet = VK_NULL_HANDLE; //Freed with pool
        }

        //Destroy temp objects
        this->tempGraphicsWork.commandBuffer = VK_NULL_HANDLE;

        if (this->tempGraphicsWork.commandPool != VK_NULL_HANDLE)
        {
            this->vk.DestroyCommandPool(this->vk.device, this->tempGraphicsWork.commandPool, this->settings.deviceAllocationCallbacks);
            this->tempGraphicsWork.commandPool = VK_NULL_HANDLE;
        }

        if (this->tempGraphicsWork.fence != VK_NULL_HANDLE)
        {
            this->vk.DestroyFence(this->vk.device, this->tempGraphicsWork.fence, this->settings.deviceAllocationCallbacks);
            this->tempGraphicsWork.fence = VK_NULL_HANDLE;
        }

        if (this->presentPossibleSemaphore != VK_NULL_HANDLE)
        {
            this->vk.DestroySemaphore(this->vk.device, this->presentPossibleSemaphore, this->settings.deviceAllocationCallbacks);
            this->presentPossibleSemaphore = VK_NULL_HANDLE;
        }

        //Destroy device
        this->vk.ShutdownDevice(this->settings.deviceAllocationCallbacks);
    }

    //Destroy surface
    if (this->surface != VK_NULL_HANDLE)
    {
        this->vulkanSystem->GetImpl()->vk.DestroySurfaceKHR(this->vulkanSystem->GetImpl()->vk.instance, this->surface, this->settings.deviceAllocationCallbacks);
        this->surface = VK_NULL_HANDLE;
    }
}

void VulkanGpuContextImpl::CreateDevice(Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    this->vulkanSystem->GetImpl()->GetDeviceExtensionNames(this->internalSettings.extensions);

    this->physicalDevice = this->vulkanSystem->GetImpl()->GetBestVulkanPhysicalDevice(this->settings.gpuID, this->physicalDeviceDescription, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to find a suitable Vulkan physical device.");
        return;
    }

    //Construct the surface description--------------------------
    VkResult result;
#if FINJIN_TARGET_PLATFORM_IS_WINDOWS
    VkWin32SurfaceCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    createInfo.hinstance = (HINSTANCE)this->settings.applicationHandle;
    createInfo.hwnd = (HWND)this->settings.osWindow->GetWindowHandle();
    result = this->vulkanSystem->GetImpl()->vk.CreateWin32SurfaceKHR(this->vulkanSystem->GetImpl()->vk.instance, &createInfo, this->settings.deviceAllocationCallbacks, &this->surface);
#elif FINJIN_TARGET_PLATFORM_IS_ANDROID
    VkAndroidSurfaceCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR;
    createInfo.window = (ANativeWindow*)this->settings.osWindow->GetWindowHandle();
    result = this->vulkanSystem->GetImpl()->vk.CreateAndroidSurfaceKHR(this->vulkanSystem->GetImpl()->vk.instance, &createInfo, this->settings.deviceAllocationCallbacks, &this->surface);
#elif FINJIN_TARGET_PLATFORM_IS_LINUX
    VkXcbSurfaceCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
    createInfo.connection = this->settings.osWindow->GetConnection();
    createInfo.window = this->settings.osWindow->GetWindowHandle();
    result = this->vulkanSystem->GetImpl()->vk.CreateXcbSurfaceKHR(this->vulkanSystem->GetImpl()->vk.instance, &createInfo, this->settings.deviceAllocationCallbacks, &this->surface);
#else
    #error Implement this!
#endif
    if (FINJIN_CHECK_VKRESULT_FAILED(result))
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create window/surface binding. Vulkan result: %1%", VulkanResult::ToString(result)));
        return;
    }

    //Get surface capabilities----------------------------------------
    result = this->vulkanSystem->GetImpl()->vk.GetPhysicalDeviceSurfaceCapabilitiesKHR(this->physicalDevice, this->surface, &this->surfaceCapabilities);
    if (FINJIN_CHECK_VKRESULT_FAILED(result))
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to get surface capabilities. Vulkan result: %1%", VulkanResult::ToString(result)));
        return;
    }

    //Ensure frame count is valid----------------------------------------
    //this->surfaceCapabilities.minImageCount is typically 2, this->surfaceCapabilities.maxImageCount is typically between 3 and 8
    FINJIN_DEBUG_LOG_INFO("Minimum image count '%1%'. Maximum image count '%2%'.", this->surfaceCapabilities.minImageCount, this->surfaceCapabilities.maxImageCount);
    this->settings.frameBufferCount.actual = this->settings.frameBufferCount.requested;
    if (this->settings.frameBufferCount.actual < this->surfaceCapabilities.minImageCount)
        this->settings.frameBufferCount.actual = this->surfaceCapabilities.minImageCount;
    if (this->surfaceCapabilities.maxImageCount > 0 && this->settings.frameBufferCount.actual > this->surfaceCapabilities.maxImageCount)
        this->settings.frameBufferCount.actual = this->surfaceCapabilities.maxImageCount;
    this->settings.jobProcessingPipelineSize = GpuContextCommonSettings::CalculateJobPipelineSize(this->settings.frameBufferCount.actual);

    //Enumerate presentation modes----------------------------------------
    this->presentModes.Enumerate(this->physicalDevice, this->surface, this->vulkanSystem->GetImpl()->vk, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to enumerate presentation modes.");
        return;
    }

    //Enumerate various queues--------------------------
    this->queueFamilyIndexes.Enumerate(this->physicalDevice, this->surface, this->vulkanSystem->GetImpl()->vk, this->physicalDeviceDescription.queueProperties, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to enumerate queue family indexes.");
        return;
    }

    //Enumerate device surface formats--------------------------
    this->surfaceFormats.Enumerate(this->physicalDevice, this->surface, this->vulkanSystem->GetImpl()->vk, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to enumerate surface formats.");
        return;
    }

    //Determine color format/space-----------------------------
    {
        auto surfaceFormatFoundAt = this->surfaceFormats.FindFormat(this->settings.colorFormat.requested, this->settings.colorSpace.requested);
        if (surfaceFormatFoundAt != this->surfaceFormats.end())
        {
            //Requested color format and color space are valid
            this->settings.colorFormat.actual = surfaceFormatFoundAt->format;
            this->settings.colorSpace.actual = surfaceFormatFoundAt->colorSpace;
        }
        else
        {
            surfaceFormatFoundAt = this->surfaceFormats.FindFormat(this->settings.colorFormat.requested);
            if (surfaceFormatFoundAt != this->surfaceFormats.end())
            {
                //Requested color format is valid
                this->settings.colorFormat.actual = surfaceFormatFoundAt->format;
                this->settings.colorSpace.actual = surfaceFormatFoundAt->colorSpace;
            }
            else
            {
                if (this->surfaceFormats.size() == 1 && this->surfaceFormats[0].format == VK_FORMAT_UNDEFINED)
                {
                    //The surface has no preferred format
                    this->settings.colorFormat.actual = DEFAULT_COLOR_FORMAT;
                    this->settings.colorSpace.actual = this->surfaceFormats[0].colorSpace;
                }
                else
                {
                    //Use preferred format
                    this->settings.colorFormat.actual = this->surfaceFormats[0].format;
                    this->settings.colorSpace.actual = this->surfaceFormats[0].colorSpace;
                }
            }
        }
    }

    //Determine depth/stencil format-------------------------
    {
        //Build up list of formats to check, putting the requested one first
        StaticVector<VkFormat, 10> formats;
        if (this->settings.depthStencilFormat.requested != VK_FORMAT_UNDEFINED)
        {
            if (!this->settings.stencilRequired || VulkanUtilities::IsDepthStencilFormat(this->settings.depthStencilFormat.requested))
                formats.push_back(this->settings.depthStencilFormat.requested);
        }
        if (this->settings.stencilRequired)
        {
            formats.push_back(VK_FORMAT_D32_SFLOAT_S8_UINT);
            formats.push_back(VK_FORMAT_D24_UNORM_S8_UINT);
            formats.push_back(VK_FORMAT_D16_UNORM_S8_UINT);
        }
        else
        {
            formats.push_back(VK_FORMAT_D32_SFLOAT);
            formats.push_back(VK_FORMAT_D32_SFLOAT_S8_UINT);
            formats.push_back(VK_FORMAT_D24_UNORM_S8_UINT);
            formats.push_back(VK_FORMAT_D16_UNORM_S8_UINT);
            formats.push_back(VK_FORMAT_D16_UNORM);
        }

        //Get first supported format
        for (auto format : formats)
        {
            VkFormatProperties formatProperties;
            this->vulkanSystem->GetImpl()->vk.GetPhysicalDeviceFormatProperties(this->physicalDevice, format, &formatProperties);
            if ((formatProperties.linearTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) ||
                (formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT))
            {
                this->settings.depthStencilFormat.actual = format;
                break;
            }
        }

        if (this->settings.depthStencilFormat.actual == VK_FORMAT_UNDEFINED)
        {
            FINJIN_SET_ERROR(error, "Failed to find a supported depth/stencil format.");
            return;
        }
    }

    //Create device--------------------------------------

    //Describe desired queues
    StaticVector<VkDeviceQueueCreateInfo, VulkanQueueFamilyIndexes::MAX_VALID_COUNT> queueInfos;

    const float queuePriorities[] = { 0.0f }; //Just one queue so only one priority, which doesn't matter
    const int queuePriorityCount = FINJIN_COUNT_OF(queuePriorities);
    queueInfos.push_back(VulkanDeviceQueueCreateInfo(this->queueFamilyIndexes.graphics, queuePriorityCount, queuePriorities));
    if (this->queueFamilyIndexes.IsComputeValid())
        queueInfos.push_back(VulkanDeviceQueueCreateInfo(this->queueFamilyIndexes.compute, queuePriorityCount, queuePriorities));
    if (this->queueFamilyIndexes.IsTransferValid())
        queueInfos.push_back(VulkanDeviceQueueCreateInfo(this->queueFamilyIndexes.transfer, queuePriorityCount, queuePriorities));
    if (this->queueFamilyIndexes.IsSparseBindingValid())
        queueInfos.push_back(VulkanDeviceQueueCreateInfo(this->queueFamilyIndexes.sparseBinding, queuePriorityCount, queuePriorities));

    //Create
    VulkanDeviceCreateInfo deviceInfo;
    deviceInfo.queueCreateInfoCount = static_cast<uint32_t>(queueInfos.size());
    deviceInfo.pQueueCreateInfos = queueInfos.data();
    deviceInfo.enabledExtensionCount = static_cast<uint32_t>(this->internalSettings.extensions.size());
    deviceInfo.ppEnabledExtensionNames = this->internalSettings.extensions.data();
    VkDevice device;
    result = this->vulkanSystem->GetImpl()->vk.CreateDevice(this->physicalDevice, &deviceInfo, this->settings.deviceAllocationCallbacks, &device);
    if (FINJIN_CHECK_VKRESULT_FAILED(result))
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create device. Vulkan result: %1%", VulkanResult::ToString(result)));
        return;
    }

    this->vk.InitializeDevice(device, this->vulkanSystem->GetImpl()->vk, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to load Vulkan library device functions. Vulkan result: %1%", VulkanResult::ToString(result)));
        return;
    }

    //Get desired queues
    GetPrimaryQueues(error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to get all primary queues.");
        return;
    }

    //Create temp graphics command pool, command buffer, and fence----------------------------------
    {
        //Pool
        VulkanCommandPoolCreateInfo commandPoolInfo;
        commandPoolInfo.queueFamilyIndex = this->queueFamilyIndexes.graphics;
        commandPoolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        result = this->vk.CreateCommandPool(this->vk.device, &commandPoolInfo, this->settings.deviceAllocationCallbacks, &this->tempGraphicsWork.commandPool);
        if (FINJIN_CHECK_VKRESULT_FAILED(result))
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create command pool. Vulkan result: %1%", VulkanResult::ToString(result)));
            return;
        }
    }
    {
        //Buffer
        VulkanCommandBufferAllocateInfo commandBufferInfo;
        commandBufferInfo.commandPool = this->tempGraphicsWork.commandPool;
        commandBufferInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        commandBufferInfo.commandBufferCount = 1;
        result = this->vk.AllocateCommandBuffers(this->vk.device, &commandBufferInfo, &this->tempGraphicsWork.commandBuffer);
        if (FINJIN_CHECK_VKRESULT_FAILED(result))
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create command buffer. Vulkan result: %1%", VulkanResult::ToString(result)));
            return;
        }
    }
    {
        //Fence
        VulkanFenceCreateInfo tempFenceInfo;
        result = this->vk.CreateFence(this->vk.device, &tempFenceInfo, this->settings.deviceAllocationCallbacks, &this->tempGraphicsWork.fence);
        if (FINJIN_CHECK_VKRESULT_FAILED(result))
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create fence. Vulkan result: %1%", VulkanResult::ToString(result)));
            return;
        }
    }

    //Create pipeline specific objects------------------------------------
    this->frameStages.resize(this->settings.jobProcessingPipelineSize);
    for (size_t stageIndex = 0; stageIndex < this->frameStages.size(); stageIndex++)
    {
        auto& frameStage = this->frameStages[stageIndex];
        frameStage.index = stageIndex;
    }

    {
        VulkanSemaphoreCreateInfo semaphoreCreateInfo;
        result = this->vk.CreateSemaphore(this->vk.device, &semaphoreCreateInfo, this->settings.deviceAllocationCallbacks, &this->presentPossibleSemaphore);
        if (FINJIN_CHECK_VKRESULT_FAILED(result))
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create rendering possible semaphore. Vulkan result: %1%", VulkanResult::ToString(result)));
            return;
        }
    }
}

void VulkanGpuContextImpl::CreateDeviceSupportObjects(Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    //Set more selector settings
    if (this->physicalDeviceDescription.features.textureCompressionASTC_LDR && AnySet(this->settings.availableAssetTextureFormats & GpuAssetTextureFormats::ASTC))
        this->settings.initialAssetFileSelector.Set(AssetPathComponent::GPU_PREFERRED_TEXTURE_FORMAT, "astc");
    else if (this->physicalDeviceDescription.features.textureCompressionETC2 && AnySet(this->settings.availableAssetTextureFormats & GpuAssetTextureFormats::ETC2))
        this->settings.initialAssetFileSelector.Set(AssetPathComponent::GPU_PREFERRED_TEXTURE_FORMAT, "etc2");
    else if (this->physicalDeviceDescription.features.textureCompressionBC && AnySet(this->settings.availableAssetTextureFormats & GpuAssetTextureFormats::BC))
        this->settings.initialAssetFileSelector.Set(AssetPathComponent::GPU_PREFERRED_TEXTURE_FORMAT, "bc");
    this->settings.initialAssetFileSelector.Set(AssetPathComponent::GPU_MODEL, this->physicalDeviceDescription.properties.deviceName);

    //Set up asset readers
    this->settingsAssetClassFileReader.Create(*this->settings.assetFileReader, this->settings.initialAssetFileSelector, AssetClass::SETTINGS, GetAllocator(), error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to create settings asset file reader.");
        return;
    }

    this->shaderAssetClassFileReader.Create(*this->settings.assetFileReader, this->settings.initialAssetFileSelector, AssetClass::SHADER, GetAllocator(), error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to create shader asset file reader.");
        return;
    }

    //Read internal settings
    this->internalSettings.contextImpl = this;
    this->settingsAssetClassFileReader.ReadAndParseSettingsFiles(this->internalSettings, this->readBuffer, this->settings.contextSettingsFileNames, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to read internal settings.");
        return;
    }
    this->internalSettings.Validate(error);
    if (error)
    {
        FINJIN_SET_ERROR_NO_MESSAGE(error);
        return;
    }

    VulkanInputFormat::CreateUnorderedMap(this->assetResources.inputFormatsByNameHash, GetAllocator(), this->internalSettings.inputFormats, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to create input formats.");
        return;
    }

    auto maxTextures = this->settings.max2DTextures + this->settings.maxCubeTextures + this->settings.max3DTextures;
    if (!this->assetResources.texturesByNameHash.Create(maxTextures, maxTextures, GetAllocator(), GetAllocator()))
    {
        FINJIN_SET_ERROR(error, "Failed to create textures lookup.");
        return;
    }

    for (auto& shadersByNameHash : this->assetResources.shadersByShaderTypeAndNameHash)
    {
        if (!shadersByNameHash.Create(this->settings.maxShaders, this->settings.maxShaders, GetAllocator(), GetAllocator()))
        {
            FINJIN_SET_ERROR(error, "Failed to create shaders lookup.");
            return;
        }
    }

    if (!this->assetResources.materialsByNameHash.Create(this->settings.maxMaterials, this->settings.maxMaterials, GetAllocator(), GetAllocator()))
    {
        FINJIN_SET_ERROR(error, "Failed to create materials lookup.");
        return;
    }

    if (!this->assetResources.meshesByNameHash.Create(this->settings.maxMeshes, this->settings.maxMeshes, GetAllocator(), GetAllocator()))
    {
        FINJIN_SET_ERROR(error, "Failed to allocate mesh lookup.");
        return;
    }

    if (!this->lights.Create(this->settings.maxLights, GetAllocator(), GetAllocator()))
    {
        FINJIN_SET_ERROR(error, "Failed to allocate light lookup.");
        return;
    }
    for (size_t i = 0; i < this->lights.items.size(); i++)
        this->lights.items[i].gpuBufferIndex = i;

    //Create texture samplers
    {
        VkSamplerCreateInfo samplerCreateInfo = {};
        samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerCreateInfo.magFilter = VK_FILTER_LINEAR;
        samplerCreateInfo.minFilter = VK_FILTER_LINEAR;
        samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
        samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
        samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
        samplerCreateInfo.mipLodBias = 0.0;
        samplerCreateInfo.anisotropyEnable = VK_FALSE;
        samplerCreateInfo.maxAnisotropy = 1.0f;
        samplerCreateInfo.compareOp = VK_COMPARE_OP_NEVER;
        samplerCreateInfo.minLod = 0.0;
        samplerCreateInfo.maxLod = 0.0; //FLT_MAX
        samplerCreateInfo.compareEnable = VK_FALSE;

        //Create sampler
        auto result = this->vk.CreateSampler(this->vk.device, &samplerCreateInfo, this->settings.deviceAllocationCallbacks, &this->textureResources.defaultSampler);
        if (result != VK_SUCCESS)
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create default texture sampler. Vulkan error: %1%", VulkanResult::ToString(result)));
            return;
        }
    }

    //Create other texture resources
    {
        size_t textureOffset = 0;

        this->assetResources.textureOffsetsByDimension[TextureDimension::DIMENSION_2D].SetOffsetAndCount(textureOffset, this->settings.max2DTextures);
        textureOffset += this->settings.max2DTextures;

        this->assetResources.textureOffsetsByDimension[TextureDimension::DIMENSION_CUBE].SetOffsetAndCount(textureOffset, this->settings.maxCubeTextures);
        textureOffset += this->settings.maxCubeTextures;

        this->assetResources.textureOffsetsByDimension[TextureDimension::DIMENSION_3D].SetOffsetAndCount(textureOffset, this->settings.max3DTextures);
        textureOffset += this->settings.max3DTextures;

        //Texture descriptor set layout
        EnumArray<TextureBindingIndex, TextureBindingIndex::COUNT, VkDescriptorSetLayoutBinding> texturesDescriptorSetLayoutBinding;
        {
            FINJIN_ZERO_ITEM(texturesDescriptorSetLayoutBinding);

            texturesDescriptorSetLayoutBinding[TextureBindingIndex::DEFAULT_SAMPLER].binding = TEXTURE_SAMPLER_BINDING_ID;
            texturesDescriptorSetLayoutBinding[TextureBindingIndex::DEFAULT_SAMPLER].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
            texturesDescriptorSetLayoutBinding[TextureBindingIndex::DEFAULT_SAMPLER].descriptorCount = 1;
            texturesDescriptorSetLayoutBinding[TextureBindingIndex::DEFAULT_SAMPLER].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

            texturesDescriptorSetLayoutBinding[TextureBindingIndex::TEXTURES_START].binding = TEXTURES_START_BINDING_ID;
            texturesDescriptorSetLayoutBinding[TextureBindingIndex::TEXTURES_START].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
            texturesDescriptorSetLayoutBinding[TextureBindingIndex::TEXTURES_START].descriptorCount = static_cast<uint32_t>(textureOffset);
            texturesDescriptorSetLayoutBinding[TextureBindingIndex::TEXTURES_START].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        }

        VulkanDescriptorSetLayoutCreateInfo texturesDescriptorSetCreate(texturesDescriptorSetLayoutBinding.data(), texturesDescriptorSetLayoutBinding.size());
        auto result = this->vk.CreateDescriptorSetLayout(this->vk.device, &texturesDescriptorSetCreate, this->settings.deviceAllocationCallbacks, &this->textureResources.texturesDescriptorSetLayout);
        if (result != VK_SUCCESS)
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create texture descriptor set layout. Vulkan error: %1%", VulkanResult::ToString(result)));
            return;
        }

        //Texture descriptor set pool
        EnumArray<TextureBindingIndex, TextureBindingIndex::COUNT, VkDescriptorPoolSize> textureDescriptorSetPoolSize;
        {
            FINJIN_ZERO_ITEM(textureDescriptorSetPoolSize);

            textureDescriptorSetPoolSize[TextureBindingIndex::DEFAULT_SAMPLER].type = VK_DESCRIPTOR_TYPE_SAMPLER;
            textureDescriptorSetPoolSize[TextureBindingIndex::DEFAULT_SAMPLER].descriptorCount = 1;

            textureDescriptorSetPoolSize[TextureBindingIndex::TEXTURES_START].type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
            textureDescriptorSetPoolSize[TextureBindingIndex::TEXTURES_START].descriptorCount = static_cast<uint32_t>(textureOffset);
        }
        VulkanDescriptorPoolCreateInfo descriptorPoolCreateInfo(textureDescriptorSetPoolSize.data(), textureDescriptorSetPoolSize.size());
        descriptorPoolCreateInfo.maxSets = 1; //Only one set will be created, with the previously specified 'textureOffset' entries in it
        result = this->vk.CreateDescriptorPool(this->vk.device, &descriptorPoolCreateInfo, this->settings.deviceAllocationCallbacks, &this->textureResources.textureDescriptorPool);
        if (result != VK_SUCCESS)
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create texture descriptor set pool. Vulkan error: %1%", VulkanResult::ToString(result)));
            return;
        }

        //Texture descriptor set
        VulkanDescriptorSetAllocateInfo textureDescriptorSetAllocateInfo(this->textureResources.textureDescriptorPool);
        textureDescriptorSetAllocateInfo.descriptorSetCount = 1;
        textureDescriptorSetAllocateInfo.pSetLayouts = &this->textureResources.texturesDescriptorSetLayout;
        result = this->vk.AllocateDescriptorSets(this->vk.device, &textureDescriptorSetAllocateInfo, &this->textureResources.textureDescriptorSet);
        if (result != VK_SUCCESS)
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to allocate texture descriptor set. Vulkan result: %1%", VulkanResult::ToString(result)));
            return;
        }

        //Set sampler into descriptor set
        {
            VkDescriptorImageInfo descriptorSamplerInfo = {};
            descriptorSamplerInfo.sampler = this->textureResources.defaultSampler;

            VulkanWriteDescriptorSet samplerWriteDescriptorSet;
            samplerWriteDescriptorSet.dstSet = this->textureResources.textureDescriptorSet;
            samplerWriteDescriptorSet.dstBinding = TEXTURE_SAMPLER_BINDING_ID;
            samplerWriteDescriptorSet.descriptorCount = 1;
            samplerWriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
            samplerWriteDescriptorSet.pImageInfo = &descriptorSamplerInfo;

            this->vk.UpdateDescriptorSets(this->vk.device, 1, &samplerWriteDescriptorSet, 0, nullptr);
        }
    }

    CreateScreenSizeDependentResources(error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to create GPU core assets.");
        return;
    }
}

void VulkanGpuContextImpl::CreateRenderers(Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    //Static mesh renderer
    {
        //Settings
        VulkanStaticMeshRenderer::Settings staticMeshRendererSettings;
        staticMeshRendererSettings.contextImpl = this;
        for (auto fileName : { "gpu-context-static-mesh-renderer-descriptor-set-layouts.cfg", "gpu-context-static-mesh-renderer-pipeline-layouts.cfg", "gpu-context-static-mesh-renderer-render-states.cfg" })
        {
            this->settings.staticMeshRendererSettingsFileNames.AddLocalFile(fileName, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to add '%1%' GPU context static mesh renderer settings file.", fileName));
                return;
            }
        }
        this->settingsAssetClassFileReader.ReadAndParseSettingsFiles(staticMeshRendererSettings, this->readBuffer, this->settings.staticMeshRendererSettingsFileNames, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to read static mesh renderer settings.");
            return;
        }

        //Renderer
        this->staticMeshRenderer.Create(std::move(staticMeshRendererSettings), error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to create static mesh renderer.");
            return;
        }
    }
}

VulkanFrameStage& VulkanGpuContextImpl::StartFrameStage(size_t index, SimpleTimeDelta elapsedTime, SimpleTimeCounter totalElapsedTime)
{
    auto& frameStage = this->frameStages[index];

    //Hold onto current frame buffer index
    auto frameBufferIndex = this->nextFrameBufferIndex;

    //Calculate next frame buffer index
    this->nextFrameBufferIndex = (this->nextFrameBufferIndex + 1) % this->frameBuffers.size();

    if (frameStage.sequenceIndex == this->sequenceIndex - this->settings.jobProcessingPipelineSize)
    {
        auto& frameBuffer = this->frameBuffers[frameStage.frameBufferIndex];

        //Wait for previous submitted work to be finished
        auto result = this->vk.WaitForFences(this->vk.device, 1, &frameBuffer.renderingCompleteFence, VK_TRUE, UINT64_MAX);
        if (FINJIN_CHECK_VKRESULT_FAILED(result))
            FINJIN_DEBUG_LOG_INFO("Failed to wait for swap chain fence. Vulkan result: %1%", VulkanResult::ToString(result));

        //Reset fence to unsignaled state
        frameBuffer.ResetFences(this);
    }

    frameStage.frameBufferIndex = frameBufferIndex;

    frameStage.elapsedTime = elapsedTime;

    frameStage.totalElapsedTime = totalElapsedTime;

    frameStage.sequenceIndex = this->sequenceIndex++;

    return frameStage;
}

void VulkanGpuContextImpl::FinishBackFrameBufferRender(VulkanFrameStage& frameStage, bool continueRendering, bool modifyingRenderTarget, size_t presentSyncIntervalOverride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto& frameBuffer = this->frameBuffers[frameStage.frameBufferIndex];

    if (!continueRendering || modifyingRenderTarget)
    {
        frameBuffer.ResetCommandBuffers();

        return;
    }

    uint32_t frameBufferIndex;
    auto result = this->vk.AcquireNextImageKHR(this->vk.device, this->swapChain, UINT64_MAX, this->presentPossibleSemaphore, VK_NULL_HANDLE, &frameBufferIndex);
    switch (result)
    {
        case VK_SUCCESS: //Fall through
        case VK_SUBOPTIMAL_KHR:
        {
            break;
        }
        case VK_ERROR_OUT_OF_DATE_KHR:
        {
            //This should not occur
            FINJIN_DEBUG_LOG_INFO("Out of date swap chain frame.");
            assert(0 && "Out of date swap chain frame.");
            break;
        }
        default:
        {
            FINJIN_DEBUG_LOG_INFO("Failed to get swap chain frame. Vulkan result: %1%", VulkanResult::ToString(result));
            assert(0 && "Failed to get swap chain frame.");
            break;
        }
    }

    if (frameBufferIndex != frameBuffer.index)
    {
        FINJIN_DEBUG_LOG_INFO("Frame buffer indexes do not match.");
        assert(0 && "Frame buffer indexes do not match.");
    }

    VulkanSubmitInfo submitInfo;
    VkPipelineStageFlags pipelineStageFlags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    submitInfo.pWaitDstStageMask = &pipelineStageFlags;
    submitInfo.commandBufferCount = static_cast<uint32_t>(frameBuffer.graphicsCommandBuffers.size());
    submitInfo.pCommandBuffers = frameBuffer.graphicsCommandBuffers.data();
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &this->presentPossibleSemaphore;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &frameBuffer.renderingCompleteSemaphore;

    result = this->vk.QueueSubmit(this->primaryQueues.graphics, 1, &submitInfo, frameBuffer.renderingCompleteFence);
    if (FINJIN_CHECK_VKRESULT_FAILED(result))
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to submit primary queue. Vulkan result: %1%", VulkanResult::ToString(result)));
        return;
    }

    frameBuffer.ResetCommandBuffers();

    VulkanPresentInfoKHR presentInfo;
    presentInfo.swapchainCount = 1;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &frameBuffer.renderingCompleteSemaphore;
    presentInfo.pSwapchains = &this->swapChain;
    presentInfo.pImageIndices = &frameBufferIndex;

    result = this->vk.QueuePresentKHR(this->primaryQueues.present, &presentInfo);
    if (FINJIN_CHECK_VKRESULT_FAILED(result))
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to present back buffer. Vulkan result: %1%", VulkanResult::ToString(result)));
        return;
    }
}

VulkanRenderTarget* VulkanGpuContextImpl::GetRenderTarget(VulkanFrameStage& frameStage, const Utf8String& name)
{
    auto& frameBuffer = this->frameBuffers[frameStage.frameBufferIndex];

    if (name.empty())
        return &frameBuffer.renderTarget;

    return nullptr;
}

void VulkanGpuContextImpl::StartGraphicsCommandList(VulkanFrameStage& frameStage, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto& frameBuffer = this->frameBuffers[frameStage.frameBufferIndex];

    //Clear command pool if it's the first command buffer
    auto isFirst = frameBuffer.commandBuffersToExecute.empty();
    if (isFirst)
    {
        auto result = this->vk.ResetCommandPool(this->vk.device, frameBuffer.graphicsCommandPool, VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT);
        if (FINJIN_CHECK_VKRESULT_FAILED(result))
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to reset command buffer. Vulkan result: %1%", VulkanResult::ToString(result)));
            return;
        }
    }

    //Clear command buffer
    auto commandBuffer = frameBuffer.NewGraphicsCommandBuffer();
    if (commandBuffer == VK_NULL_HANDLE)
    {
        FINJIN_SET_ERROR(error, "Failed to get an available graphics command buffer.");
        return;
    }

    //Start command buffer
    VulkanCommandBufferBeginInfo commandBeginInfo(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
    auto result = this->vk.BeginCommandBuffer(commandBuffer, &commandBeginInfo);
    if (FINJIN_CHECK_VKRESULT_FAILED(result))
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to start command buffer for population. Vulkan result: %1%", VulkanResult::ToString(result)));
        return;
    }
}

void VulkanGpuContextImpl::FinishGraphicsCommandList(VulkanFrameStage& frameStage, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto& frameBuffer = this->frameBuffers[frameStage.frameBufferIndex];

    //End command buffer
    auto commandBuffer = frameBuffer.GetCurrentGraphicsCommandBuffer();
    auto result = this->vk.EndCommandBuffer(commandBuffer);
    if (FINJIN_CHECK_VKRESULT_FAILED(result))
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to close populated command buffer. Vulkan result: %1%", VulkanResult::ToString(result)));
        return;
    }
}

void VulkanGpuContextImpl::Execute(VulkanFrameStage& frameStage, GpuEvents& events, GpuCommands& commands, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto& frameBuffer = this->frameBuffers[frameStage.frameBufferIndex];

    VulkanRenderTarget* lastRenderTarget = nullptr;

    for (auto& baseCommand : commands)
    {
        switch (baseCommand.type)
        {
            case GpuCommand::Type::NOTIFY:
            {
                if (baseCommand.eventInfo.HasEventID())
                {
                    if (!events.push_back())
                    {
                        FINJIN_SET_ERROR(error, "Failed to record 'notify' GPU event.");
                        return;
                    }

                    auto& event = events.back();
                    event.type = GpuEvent::Type::NOTIFY;
                    event.eventInfo = baseCommand.eventInfo;
                }

                break;
            }
            case GpuCommand::Type::START_GRAPHICS_COMMAND_LIST:
            {
                StartGraphicsCommandList(frameStage, error);
                if (error)
                {
                    FINJIN_SET_ERROR(error, "Failed to start command list.");
                    return;
                }
                break;
            }
            case GpuCommand::Type::FINISH_GRAPHICS_COMMAND_LIST:
            {
                FinishGraphicsCommandList(frameStage, error);
                if (error)
                {
                    FINJIN_SET_ERROR(error, "Failed to finish command list.");
                    return;
                }
                break;
            }
            case GpuCommand::Type::EXECUTE_GRAPHICS_COMMAND_LISTS:
            {
                assert(0 && "Not yet supported or needed");
                //frameBuffer.ExecutePrimaryGraphicsCommandBuffer(this, this->primaryQueues.graphics);
                break;
            }
            case GpuCommand::Type::START_RENDER_TARGET:
            {
                auto& command = static_cast<StartRenderTargetGpuCommand&>(baseCommand);

                auto renderTarget = GetRenderTarget(frameStage, command.renderTargetName);
                StaticVector<VulkanRenderTarget*, EngineConstants::MAX_RENDER_TARGET_DEPENDENCIES> dependentRenderTargets;
                for (auto& renderTargetName : command.dependentRenderTargetNames)
                {
                    dependentRenderTargets.push_back(GetRenderTarget(frameStage, renderTargetName));
                    if (dependentRenderTargets.back() == nullptr)
                    {
                        FINJIN_SET_ERROR(error, "Failed to look up render target.");
                        return;
                    }
                }
                StartRenderTarget(frameStage, renderTarget, dependentRenderTargets, error);
                if (error)
                {
                    FINJIN_SET_ERROR(error, "Failed to start render target.");
                    return;
                }

                lastRenderTarget = renderTarget;

                break;
            }
            case GpuCommand::Type::FINISH_RENDER_TARGET:
            {
                auto& command = static_cast<FinishRenderTargetGpuCommand&>(baseCommand);

                if (lastRenderTarget == nullptr)
                {
                    FINJIN_SET_ERROR(error, "Failed to finish render target. There was no render target to finish.");
                    return;
                }

                if (lastRenderTarget == GetRenderTarget(frameStage, Utf8String::Empty()))
                {
                    //Main render target finished
                    auto commandBuffer = frameBuffer.GetCurrentGraphicsCommandBuffer();
                    assert(commandBuffer != nullptr);

                    auto& frameBuffer = this->frameBuffers[frameStage.frameBufferIndex];
                    this->staticMeshRenderer.UpdatePassAndMaterialConstants(frameBuffer.staticMeshRendererFrameState, frameStage.elapsedTime, frameStage.totalElapsedTime);
                    this->staticMeshRenderer.RenderQueued(frameBuffer.staticMeshRendererFrameState, commandBuffer);
                }

                FinishRenderTarget(frameStage, error);
                if (error)
                {
                    FINJIN_SET_ERROR(error, "Failed to finish command list");
                    return;
                }

                lastRenderTarget = nullptr;

                break;
            }
            case GpuCommand::Type::CREATE_MESH:
            {
                auto& command = static_cast<CreateMeshGpuCommand&>(baseCommand);

                auto meshAsset = command.asset;
                auto commandBuffer = frameBuffer.GetCurrentGraphicsCommandBuffer();
                assert(commandBuffer != nullptr);
                UploadMesh(commandBuffer, meshAsset, error);
                if (error)
                {
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to upload mesh '%1%'.", meshAsset->name));
                    return;
                }

                if (command.eventInfo.HasEventID())
                {
                    if (!events.push_back())
                    {
                        FINJIN_SET_ERROR(error, "Failed to record 'upload mesh' GPU event.");
                        return;
                    }

                    auto& event = events.back();
                    event.type = GpuEvent::Type::CREATE_MESH_FINISHED;
                    event.meshResource = meshAsset->gpuMesh;
                    event.eventInfo = command.eventInfo;
                }

                break;
            }
            case GpuCommand::Type::CREATE_TEXTURE:
            {
                auto& command = static_cast<CreateTextureGpuCommand&>(baseCommand);

                auto textureAsset = command.asset;
                auto commandBuffer = frameBuffer.GetCurrentGraphicsCommandBuffer();
                assert(commandBuffer != nullptr);
                UploadTexture(commandBuffer, textureAsset);
                if (error)
                {
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to upload texture '%1%'.", textureAsset->name));
                    return;
                }

                if (command.eventInfo.HasEventID())
                {
                    if (!events.push_back())
                    {
                        FINJIN_SET_ERROR(error, "Failed to record 'upload texture' GPU event.");
                        return;
                    }

                    auto& event = events.back();
                    event.type = GpuEvent::Type::CREATE_TEXTURE_FINISHED;
                    event.textureResource = textureAsset->gpuTexture;
                    event.eventInfo = command.eventInfo;
                }

                break;
            }
            case GpuCommand::Type::CREATE_MATERIAL:
            {
                auto& command = static_cast<CreateMaterialGpuCommand&>(baseCommand);

                auto materialAsset = command.asset;
                auto commandBuffer = frameBuffer.GetCurrentGraphicsCommandBuffer();
                assert(commandBuffer != nullptr);
                auto materialResource = CreateMaterial(commandBuffer, materialAsset, error);
                if (error)
                {
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to upload material '%1%'.", materialAsset->name));
                    return;
                }

                if (command.eventInfo.HasEventID())
                {
                    if (!events.push_back())
                    {
                        FINJIN_SET_ERROR(error, "Failed to record 'upload material' GPU event.");
                        return;
                    }

                    auto& event = events.back();
                    event.type = GpuEvent::Type::CREATE_MATERIAL_FINISHED;
                    event.materialResource = materialResource;
                    event.eventInfo = command.eventInfo;
                }

                break;
            }
            case GpuCommand::Type::SET_CLEAR_COLOR:
            {
                auto& command = static_cast<SetClearColorGpuCommand&>(baseCommand);

                this->clearColor = command.clearColor;

                if (command.eventInfo.HasEventID())
                {
                    if (!events.push_back())
                    {
                        FINJIN_SET_ERROR(error, "Failed to record 'set clear color' GPU event.");
                        return;
                    }

                    auto& event = events.back();
                    event.type = GpuEvent::Type::SET_CLEAR_COLOR_FINISHED;
                    event.eventInfo = command.eventInfo;
                }

                break;
            }
            case GpuCommand::Type::SET_CAMERA:
            {
                auto& command = static_cast<SetCameraGpuCommand&>(baseCommand);

                this->camera = command.camera;

                if (command.eventInfo.HasEventID())
                {
                    if (!events.push_back())
                    {
                        FINJIN_SET_ERROR(error, "Failed to record 'set camera' GPU event.");
                        return;
                    }

                    auto& event = events.back();
                    event.type = GpuEvent::Type::SET_CAMERA_FINISHED;
                    event.eventInfo = command.eventInfo;
                }

                break;
            }
            case GpuCommand::Type::RENDER_ENTITY:
            {
                auto& command = static_cast<RenderEntityGpuCommand&>(baseCommand);

                auto& frameBuffer = this->frameBuffers[frameStage.frameBufferIndex];

                this->staticMeshRenderer.RenderEntity(frameBuffer.staticMeshRendererFrameState, command.sceneNodeState, command.entity, command.shaderFeatureFlags, command.sortedLights, command.ambientLight);

                if (command.eventInfo.HasEventID())
                {
                    if (!events.push_back())
                    {
                        FINJIN_SET_ERROR(error, "Failed to record 'render entity' GPU event.");
                        return;
                    }

                    auto& event = events.back();
                    event.type = GpuEvent::Type::RENDER_ENTITY_FINISHED;
                    event.eventInfo = command.eventInfo;
                }

                break;
            }
        }
    }
}

VulkanFrameBuffer& VulkanGpuContextImpl::GetFrameBuffer(size_t index)
{
    return this->frameBuffers[index];
}

void VulkanGpuContextImpl::FlushGpu()
{
    if (this->vk.device != VK_NULL_HANDLE)
        this->vk.DeviceWaitIdle(this->vk.device);
}

void VulkanGpuContextImpl::CreateScreenSizeDependentResources(Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    UpdateCachedFrameBufferSize();

    //Swap chain--------------------------------
    {
        //Clear command pool
        auto result = this->vk.ResetCommandPool(this->vk.device, this->tempGraphicsWork.commandPool, VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT);
        if (FINJIN_CHECK_VKRESULT_FAILED(result))
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to reset command buffer. Vulkan result: %1%", VulkanResult::ToString(result)));
            return;
        }

        //Execute begin
        VulkanCommandBufferBeginInfo commandBeginInfo(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
        result = this->vk.BeginCommandBuffer(this->tempGraphicsWork.commandBuffer, &commandBeginInfo);
        if (FINJIN_CHECK_VKRESULT_FAILED(result))
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to execute command buffer begin. Vulkan result: %1%", VulkanResult::ToString(result)));
            return;
        }

        //Frame resources
        CreateSwapChain(this->tempGraphicsWork.commandBuffer, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to initialize swap chain.");
            return;
        }

        //Depth stencil
        CreateDepthBuffer(this->tempGraphicsWork.commandBuffer, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to initialize depth buffer.");
            return;
        }

        result = this->vk.EndCommandBuffer(this->tempGraphicsWork.commandBuffer);
        if (FINJIN_CHECK_VKRESULT_FAILED(result))
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to execute end command buffer. Vulkan result: %1%", VulkanResult::ToString(result)));
            return;
        }

        {
            this->vk.ResetFences(this->vk.device, 1, &this->tempGraphicsWork.fence);

            //Submit command buffer for execution
            VulkanSubmitInfo submitInfo;
            VkPipelineStageFlags pipelineStageFlags = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
            submitInfo.pWaitDstStageMask = &pipelineStageFlags;
            submitInfo.commandBufferCount = 1;
            submitInfo.pCommandBuffers = &this->tempGraphicsWork.commandBuffer;
            result = this->vk.QueueSubmit(this->primaryQueues.graphics, 1, &submitInfo, this->tempGraphicsWork.fence);
            if (FINJIN_CHECK_VKRESULT_FAILED(result))
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to submit queue for submit/wait. Vulkan result: %1%", VulkanResult::ToString(result)));
                return;
            }

            //Wait for submitted command buffer to finish
            auto timeoutNanoseconds = this->settings.maxFenceTimeout.ToNanoseconds();
            int timeoutCount = 0;
            do
            {
                result = this->vk.WaitForFences(this->vk.device, 1, &this->tempGraphicsWork.fence, VK_TRUE, timeoutNanoseconds);
                if (result == VK_TIMEOUT)
                    timeoutCount++;
            } while (result == VK_TIMEOUT && timeoutCount < 10);

            if (FINJIN_CHECK_VKRESULT_FAILED(result))
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to wait for fence for submit/wait. Vulkan result: %1%", VulkanResult::ToString(result)));
                return;
            }
        }
    }

    //Need attachments for render target and depth buffer
    {
        EnumArray<AttachmentIndex, AttachmentIndex::COUNT, VkAttachmentDescription> attachments;
        FINJIN_ZERO_ITEM(attachments);

        attachments[AttachmentIndex::COLOR].format = this->settings.colorFormat.actual;
        attachments[AttachmentIndex::COLOR].samples = VK_SAMPLE_COUNT_1_BIT;
        attachments[AttachmentIndex::COLOR].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        attachments[AttachmentIndex::COLOR].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        attachments[AttachmentIndex::COLOR].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attachments[AttachmentIndex::COLOR].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        attachments[AttachmentIndex::COLOR].initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        attachments[AttachmentIndex::COLOR].finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        attachments[AttachmentIndex::DEPTH].format = this->settings.depthStencilFormat.actual;
        attachments[AttachmentIndex::DEPTH].samples = VK_SAMPLE_COUNT_1_BIT;
        attachments[AttachmentIndex::DEPTH].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        attachments[AttachmentIndex::DEPTH].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        attachments[AttachmentIndex::DEPTH].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
        attachments[AttachmentIndex::DEPTH].stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
        attachments[AttachmentIndex::DEPTH].initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        attachments[AttachmentIndex::DEPTH].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VulkanAttachmentReference colorAttachmentReference(0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
        VulkanAttachmentReference depthAttachmentReference(1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

        VkSubpassDescription subpass = {};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentReference;
        subpass.pDepthStencilAttachment = &depthAttachmentReference;

        std::array<VkSubpassDependency, 2> dependencies;
        FINJIN_ZERO_ITEM(dependencies);

        //First dependency at the start of the renderpass
        //The transition from final to initial layout
        dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
        dependencies[0].dstSubpass = 0;
        dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
        dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
        dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

        //Second dependency at the end the renderpass
        //The transition from the initial to the final layout
        dependencies[1].srcSubpass = 0;
        dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
        dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
        dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        dependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
        dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

        VulkanRenderPassCreateInfo renderPassCreateInfo;
        renderPassCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        renderPassCreateInfo.pAttachments = attachments.data();
        renderPassCreateInfo.subpassCount = 1;
        renderPassCreateInfo.pSubpasses = &subpass;
        renderPassCreateInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
        renderPassCreateInfo.pDependencies = dependencies.data();

        auto result = this->vk.CreateRenderPass(this->vk.device, &renderPassCreateInfo, this->settings.deviceAllocationCallbacks, &this->renderPass);
        if (FINJIN_CHECK_VKRESULT_FAILED(result))
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create render pass. Vulkan result: %1%", VulkanResult::ToString(result)));
            return;
        }
    }

    //------------------------------------
    {
        auto frameBufferSize = this->renderingPixelBounds;
        frameBufferSize.LimitClientSize(this->physicalDeviceDescription.properties.limits.maxFramebufferWidth, this->physicalDeviceDescription.properties.limits.maxFramebufferHeight);

        EnumArray<AttachmentIndex, AttachmentIndex::COUNT, VkImageView> attachments;
        attachments[AttachmentIndex::DEPTH] = this->depth.view;

        VulkanFramebufferCreateInfo frameBufferCreateInfo;
        frameBufferCreateInfo.renderPass = this->renderPass;
        frameBufferCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        frameBufferCreateInfo.pAttachments = attachments.data();
        frameBufferCreateInfo.width = static_cast<uint32_t>(frameBufferSize.GetClientWidth());
        frameBufferCreateInfo.height = static_cast<uint32_t>(frameBufferSize.GetClientHeight());
        frameBufferCreateInfo.layers = 1;

        for (size_t bufferIndex = 0; bufferIndex < this->frameBuffers.size(); bufferIndex++)
        {
            auto& frameBuffer = this->frameBuffers[bufferIndex];

            attachments[AttachmentIndex::COLOR] = frameBuffer.renderTarget.view;

            auto result = this->vk.CreateFramebuffer(this->vk.device, &frameBufferCreateInfo, this->settings.deviceAllocationCallbacks, &frameBuffer.frameBuffer);
            if (FINJIN_CHECK_VKRESULT_FAILED(result))
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create frame buffer. Vulkan result: %1%", VulkanResult::ToString(result)));
                return;
            }
        }
    }
}

void VulkanGpuContextImpl::DestroyScreenSizeDependentResources(bool resizing)
{
    if (this->renderPass != VK_NULL_HANDLE)
    {
        this->vk.DestroyRenderPass(this->vk.device, this->renderPass, this->settings.deviceAllocationCallbacks);
        this->renderPass = VK_NULL_HANDLE;
    }

    //Destroy depth
    DestroyImageView(this->depth.view, this->depth.image, this->depth.mem);

    //Destroy swap chain
    for (auto& frameBuffer : this->frameBuffers)
    {
        if (frameBuffer.frameBuffer != VK_NULL_HANDLE)
        {
            this->vk.DestroyFramebuffer(this->vk.device, frameBuffer.frameBuffer, this->settings.deviceAllocationCallbacks);
            frameBuffer.frameBuffer = VK_NULL_HANDLE;
        }

        DestroyImageView(frameBuffer.renderTarget.view);
        frameBuffer.renderTarget.image = VK_NULL_HANDLE; //The image should not be destroyed since it is owned by the swap chain
    }

    DestroySwapChain(this->swapChain);

    this->nextFrameBufferIndex = 0;
    this->sequenceIndex = 0;
}

void VulkanGpuContextImpl::WindowResized(OSWindow* osWindow)
{
    UpdateCachedFrameBufferSize();
}

VkPhysicalDevice VulkanGpuContextImpl::GetPhysicalDevice()
{
    return this->physicalDevice;
}

VkDevice VulkanGpuContextImpl::GetDevice()
{
    return this->vk.device;
}

VkAllocationCallbacks* VulkanGpuContextImpl::GetAllocationCallbacks()
{
    return this->settings.deviceAllocationCallbacks;
}

VkQueue VulkanGpuContextImpl::GetGraphicsQueue()
{
    return this->primaryQueues.graphics;
}

void VulkanGpuContextImpl::CreateSwapChain(VkCommandBuffer theGraphicsCommandBuffer, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    //Create the swap chain------------------------------------------------------------
    VkFormatProperties colorFormatProperties;
    this->vulkanSystem->GetImpl()->vk.GetPhysicalDeviceFormatProperties(this->physicalDevice, this->settings.colorFormat.actual, &colorFormatProperties);

    auto oldSwapChain = this->swapChain;
    this->swapChain = VK_NULL_HANDLE;

    VulkanSwapchainCreateInfoKHR swapChainInfo;
    swapChainInfo.surface = this->surface;
    swapChainInfo.minImageCount = static_cast<uint32_t>(this->settings.frameBufferCount.actual);
    swapChainInfo.imageFormat = this->settings.colorFormat.actual;
    swapChainInfo.imageExtent.width = static_cast<uint32_t>(this->renderingPixelBounds.GetClientWidth());
    swapChainInfo.imageExtent.height = static_cast<uint32_t>(this->renderingPixelBounds.GetClientHeight());
    if (this->surfaceCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
        swapChainInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    else
        swapChainInfo.preTransform = this->surfaceCapabilities.currentTransform;
    swapChainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    for (auto compositeAlphaFlag : { VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR, VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR, VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR, VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR })
    {
        if (this->surfaceCapabilities.supportedCompositeAlpha & compositeAlphaFlag)
        {
            swapChainInfo.compositeAlpha = compositeAlphaFlag;
            break;
        }
    }
    swapChainInfo.imageArrayLayers = 1;
    swapChainInfo.presentMode = this->presentModes.GetBest(this->settings.presentMode);
    swapChainInfo.oldSwapchain = oldSwapChain;
    swapChainInfo.clipped = VK_TRUE;
    swapChainInfo.imageColorSpace = this->settings.colorSpace.actual;
    swapChainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    if (colorFormatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_BLIT_DST_BIT)
        swapChainInfo.imageUsage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    swapChainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;

    auto result = this->vk.CreateSwapchainKHR(this->vk.device, &swapChainInfo, this->settings.deviceAllocationCallbacks, &this->swapChain);
    for (auto& frameBuffer : this->frameBuffers)
    {
        DestroyImageView(frameBuffer.renderTarget.view);
        frameBuffer.renderTarget.image = VK_NULL_HANDLE; //The image should not be destroyed since it is owned by the swap chain
    }
    DestroySwapChain(oldSwapChain);
    if (FINJIN_CHECK_VKRESULT_FAILED(result))
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create swap chain. Vulkan result: %1%", VulkanResult::ToString(result)));
        return;
    }

    //Get the swap chain images and create views onto those images------------------------------------------
    uint32_t swapChainImageCount = 0;
    result = this->vk.GetSwapchainImagesKHR(this->vk.device, this->swapChain, &swapChainImageCount, nullptr);
    if (FINJIN_CHECK_VKRESULT_FAILED(result))
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to get swap chain image count. Vulkan result: %1%", VulkanResult::ToString(result)));
        return;
    }
    if (swapChainImageCount != this->settings.frameBufferCount.actual)
    {
        //This occurs on NVidia Shield hardware. It returns 1 more than requested. Bug?
        FINJIN_DEBUG_LOG_INFO("Obtained %1% images but wanted %2%. Continuing...", swapChainImageCount, this->settings.frameBufferCount.actual);

        DestroySwapChain(this->swapChain);

        swapChainInfo.minImageCount--;

        this->vk.CreateSwapchainKHR(this->vk.device, &swapChainInfo, this->settings.deviceAllocationCallbacks, &this->swapChain);
        result = this->vk.GetSwapchainImagesKHR(this->vk.device, this->swapChain, &swapChainImageCount, nullptr);
        if (swapChainImageCount != this->settings.frameBufferCount.actual)
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Obtained %1% images but wanted %2%.", swapChainImageCount, this->settings.frameBufferCount.actual));
            return;
        }

        FINJIN_DEBUG_LOG_INFO("Retried and obtained %1% images as requested.", swapChainImageCount);

        this->settings.frameBufferCount.actual = swapChainImageCount;
    }

    StaticVector<VkImage, EngineConstants::MAX_FRAME_BUFFERS> swapChainImages;
    swapChainImageCount = static_cast<uint32_t>(swapChainImages.resize(swapChainImageCount));
    result = this->vk.GetSwapchainImagesKHR(this->vk.device, this->swapChain, &swapChainImageCount, swapChainImages.data());
    if (FINJIN_CHECK_VKRESULT_FAILED(result))
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to get swap chain image count. Vulkan result: %1%", VulkanResult::ToString(result)));
        return;
    }

    //Frames-------------------------------------------------------------------
    if (this->frameBuffers.empty())
    {
        this->frameBuffers.resize(swapChainImageCount);
        for (size_t bufferIndex = 0; bufferIndex < this->frameBuffers.size(); bufferIndex++)
        {
            auto& frameBuffer = this->frameBuffers[bufferIndex];

            frameBuffer.SetIndex(bufferIndex);

            //Create fence---------------------------------
            VulkanFenceCreateInfo fenceInfo;
            fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
            result = this->vk.CreateFence(this->vk.device, &fenceInfo, this->settings.deviceAllocationCallbacks, &frameBuffer.renderingCompleteFence);
            if (FINJIN_CHECK_VKRESULT_FAILED(result))
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create fence. Vulkan result: %1%", VulkanResult::ToString(result)));
                return;
            }

            //Create command pool--------------------------
            VulkanCommandPoolCreateInfo commandPoolInfo;
            commandPoolInfo.queueFamilyIndex = this->queueFamilyIndexes.graphics;
            commandPoolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
            result = this->vk.CreateCommandPool(this->vk.device, &commandPoolInfo, this->settings.deviceAllocationCallbacks, &frameBuffer.graphicsCommandPool);
            if (FINJIN_CHECK_VKRESULT_FAILED(result))
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create command pool. Vulkan result: %1%", VulkanResult::ToString(result)));
                return;
            }

            //Create command buffers-----------------------------
            if (!frameBuffer.commandBuffersToExecute.Create(this->settings.maxGpuCommandListsPerStage, GetAllocator(), nullptr))
            {
                FINJIN_SET_ERROR(error, "Failed to create 'all command buffers to execute' collection.");
                return;
            }

            if (!frameBuffer.graphicsCommandBuffers.Create(this->settings.maxGpuCommandListsPerStage, GetAllocator(), nullptr))
            {
                FINJIN_SET_ERROR(error, "Failed to create graphics command buffers collection.");
                return;
            }
            VulkanCommandBufferAllocateInfo commandBufferInfo;
            commandBufferInfo.commandPool = frameBuffer.graphicsCommandPool;
            commandBufferInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            commandBufferInfo.commandBufferCount = static_cast<uint32_t>(frameBuffer.graphicsCommandBuffers.size());
            result = this->vk.AllocateCommandBuffers(this->vk.device, &commandBufferInfo, frameBuffer.graphicsCommandBuffers.data());
            if (FINJIN_CHECK_VKRESULT_FAILED(result))
            {
                frameBuffer.graphicsCommandBuffers.Destroy(); //Destroy buffers collection since it doesn't contain anything valid

                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create command buffer. Vulkan result: %1%", VulkanResult::ToString(result)));
                return;
            }
            frameBuffer.ResetCommandBuffers();

            //Create rendering complete semaphore--------------------
            VulkanSemaphoreCreateInfo semaphoreCreateInfo;
            result = this->vk.CreateSemaphore(this->vk.device, &semaphoreCreateInfo, this->settings.deviceAllocationCallbacks, &frameBuffer.renderingCompleteSemaphore);
            if (FINJIN_CHECK_VKRESULT_FAILED(result))
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create rendering complete semaphore. Vulkan result: %1%", VulkanResult::ToString(result)));
                return;
            }
        }
    }

    for (size_t bufferIndex = 0; bufferIndex < this->frameBuffers.size(); bufferIndex++)
    {
        auto& frameBuffer = this->frameBuffers[bufferIndex];

        frameBuffer.renderTarget.image = swapChainImages[bufferIndex];

        VulkanImageViewCreateInfo colorImageViewInfo;
        colorImageViewInfo.format = this->settings.colorFormat.actual;
        colorImageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        colorImageViewInfo.subresourceRange.baseMipLevel = 0;
        colorImageViewInfo.subresourceRange.levelCount = 1;
        colorImageViewInfo.subresourceRange.baseArrayLayer = 0;
        colorImageViewInfo.subresourceRange.layerCount = 1;
        colorImageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        colorImageViewInfo.image = frameBuffer.renderTarget.image;
        result = this->vk.CreateImageView(this->vk.device, &colorImageViewInfo, this->settings.deviceAllocationCallbacks, &frameBuffer.renderTarget.view);
        if (FINJIN_CHECK_VKRESULT_FAILED(result))
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create swap chain image view. Vulkan result: %1%", VulkanResult::ToString(result)));
            return;
        }
    }
}

void VulkanGpuContextImpl::CreateDepthBuffer(VkCommandBuffer theGraphicsCommandBuffer, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    VulkanImageCreateInfo imageCreateInfo;

    VkFormatProperties formatProperties;
    this->vulkanSystem->GetImpl()->vk.GetPhysicalDeviceFormatProperties(this->physicalDevice, this->settings.depthStencilFormat.actual, &formatProperties);
    if (formatProperties.linearTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
        imageCreateInfo.tiling = VK_IMAGE_TILING_LINEAR;
    else if (formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
        imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;

    //Fill in remaining image info
    imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
    imageCreateInfo.format = this->settings.depthStencilFormat.actual;
    imageCreateInfo.extent.width = static_cast<uint32_t>(this->renderingPixelBounds.GetClientWidth());
    imageCreateInfo.extent.height = static_cast<uint32_t>(this->renderingPixelBounds.GetClientHeight());
    imageCreateInfo.extent.depth = 1;
    imageCreateInfo.mipLevels = 1;
    imageCreateInfo.arrayLayers = 1;
    imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageCreateInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

    //Create
    auto result = this->vk.CreateImage(this->vk.device, &imageCreateInfo, this->settings.deviceAllocationCallbacks, &this->depth.image);
    if (FINJIN_CHECK_VKRESULT_FAILED(result))
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create depth image. Vulkan result: %1%", VulkanResult::ToString(result)));
        return;
    }

    //Allocate memory for the image----------------------------------
    VkMemoryRequirements memReqs;
    this->vk.GetImageMemoryRequirements(this->vk.device, this->depth.image, &memReqs);

    //Use the memory properties to determine the type of memory required
    VulkanMemoryAllocateInfo memAllocInfo;
    memAllocInfo.allocationSize = memReqs.size;
    if (!this->physicalDeviceDescription.GetMemoryTypeIndexFromProperties(memReqs.memoryTypeBits, 0, memAllocInfo.memoryTypeIndex))
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to determine depth memory type. Vulkan result: %1%", VulkanResult::ToString(result)));
        return;
    }

    result = this->vk.AllocateMemory(this->vk.device, &memAllocInfo, this->settings.deviceAllocationCallbacks, &this->depth.mem);
    if (FINJIN_CHECK_VKRESULT_FAILED(result))
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to allocate depth memory. Vulkan result: %1%", VulkanResult::ToString(result)));
        return;
    }

    //Bind image memory------------------------------------
    result = this->vk.BindImageMemory(this->vk.device, this->depth.image, this->depth.mem, 0);
    if (FINJIN_CHECK_VKRESULT_FAILED(result))
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to bind depth memory. Vulkan result: %1%", VulkanResult::ToString(result)));
        return;
    }

    //Create depth image view------------------------------
    VulkanImageViewCreateInfo imageViewInfo;
    imageViewInfo.image = this->depth.image;
    imageViewInfo.format = this->settings.depthStencilFormat.actual;
    imageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    if (VulkanUtilities::IsDepthStencilFormat(this->settings.depthStencilFormat.actual))
        imageViewInfo.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
    imageViewInfo.subresourceRange.baseMipLevel = 0;
    imageViewInfo.subresourceRange.levelCount = 1;
    imageViewInfo.subresourceRange.baseArrayLayer = 0;
    imageViewInfo.subresourceRange.layerCount = 1;
    imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    VulkanUtilities::SetImageLayout(this->vk, theGraphicsCommandBuffer, this->depth.image, imageViewInfo.subresourceRange.aspectMask, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

    result = this->vk.CreateImageView(this->vk.device, &imageViewInfo, this->settings.deviceAllocationCallbacks, &this->depth.view);
    if (FINJIN_CHECK_VKRESULT_FAILED(result))
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create depth image view. Vulkan result: %1%", VulkanResult::ToString(result)));
        return;
    }
}

void VulkanGpuContextImpl::GetPrimaryQueues(Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    if (this->queueFamilyIndexes.IsGraphicsValid())
    {
        this->vk.GetDeviceQueue(this->vk.device, this->queueFamilyIndexes.graphics, 0, &this->primaryQueues.graphics);
        if (this->primaryQueues.graphics == VK_NULL_HANDLE)
        {
            FINJIN_SET_ERROR(error, "Failed to get graphics queue.");
            return;
        }
    }

    if (this->queueFamilyIndexes.IsComputeValid())
    {
        this->vk.GetDeviceQueue(this->vk.device, this->queueFamilyIndexes.compute, 0, &this->primaryQueues.compute);
        if (this->primaryQueues.compute == VK_NULL_HANDLE)
        {
            FINJIN_SET_ERROR(error, "Failed to get compute queue.");
            return;
        }
    }

    if (this->queueFamilyIndexes.IsTransferValid())
    {
        this->vk.GetDeviceQueue(this->vk.device, this->queueFamilyIndexes.transfer, 0, &this->primaryQueues.transfer);
        if (this->primaryQueues.transfer == VK_NULL_HANDLE)
        {
            FINJIN_SET_ERROR(error, "Failed to get transfer queue.");
            return;
        }
    }

    if (this->queueFamilyIndexes.IsSparseBindingValid())
    {
        this->vk.GetDeviceQueue(this->vk.device, this->queueFamilyIndexes.sparseBinding, 0, &this->primaryQueues.sparseBinding);
        if (this->primaryQueues.sparseBinding == VK_NULL_HANDLE)
        {
            FINJIN_SET_ERROR(error, "Failed to get sparse binding queue.");
            return;
        }
    }

    this->primaryQueues.SetPresent(this->queueFamilyIndexes);
}

void VulkanGpuContextImpl::DestroyImageView(VkImageView& view, VkImage& image, VkDeviceMemory& mem)
{
    if (view != VK_NULL_HANDLE)
    {
        this->vk.DestroyImageView(this->vk.device, view, this->settings.deviceAllocationCallbacks);
        view = VK_NULL_HANDLE;
    }

    if (image != VK_NULL_HANDLE)
    {
        this->vk.DestroyImage(this->vk.device, image, this->settings.deviceAllocationCallbacks);
        image = VK_NULL_HANDLE;
    }

    if (mem != VK_NULL_HANDLE)
    {
        this->vk.FreeMemory(this->vk.device, mem, this->settings.deviceAllocationCallbacks);
        mem = VK_NULL_HANDLE;
    }
}

void VulkanGpuContextImpl::DestroyImageView(VkImageView& view, VkImage& image)
{
    if (view != VK_NULL_HANDLE)
    {
        this->vk.DestroyImageView(this->vk.device, view, this->settings.deviceAllocationCallbacks);
        view = VK_NULL_HANDLE;
    }

    if (image != VK_NULL_HANDLE)
    {
        this->vk.DestroyImage(this->vk.device, image, this->settings.deviceAllocationCallbacks);
        image = VK_NULL_HANDLE;
    }
}

void VulkanGpuContextImpl::DestroyImageView(VkImageView& view)
{
    if (view != VK_NULL_HANDLE)
    {
        this->vk.DestroyImageView(this->vk.device, view, this->settings.deviceAllocationCallbacks);
        view = VK_NULL_HANDLE;
    }
}

void VulkanGpuContextImpl::DestroySwapChain(VkSwapchainKHR& swapChain)
{
    if (swapChain != VK_NULL_HANDLE)
    {
        this->vk.DestroySwapchainKHR(this->vk.device, swapChain, this->settings.deviceAllocationCallbacks);
        swapChain = VK_NULL_HANDLE;
    }
}

void VulkanGpuContextImpl::UpdateResourceGpuResidencyStatus()
{
    std::unique_lock<FiberSpinLock> thisLock(this->createLock);

    {
        VulkanMesh* previous = nullptr;
        for (auto resource = this->assetResources.waitingToBeResidentMeshesHead; resource != nullptr; resource = resource->waitingToBeResidentNext)
        {
            //Remove from 'waiting to be resident' list
            resource->UpdateResidentOnGpuStatus(this->vk, this->settings.deviceAllocationCallbacks);
            if (resource->IsResidentOnGpu())
            {
                if (previous == nullptr)
                    this->assetResources.waitingToBeResidentMeshesHead = resource->waitingToBeResidentNext;
                else
                    previous->waitingToBeResidentNext = resource->waitingToBeResidentNext;
            }
            previous = resource;
        }
    }

    {
        VulkanTexture* previous = nullptr;
        for (auto resource = this->assetResources.waitingToBeResidentTexturesHead; resource != nullptr; resource = resource->waitingToBeResidentNext)
        {
            resource->UpdateResidentOnGpuStatus(this->vk, this->settings.deviceAllocationCallbacks);
            if (resource->IsResidentOnGpu())
            {
                //Remove from 'waiting to be resident' list
                if (previous == nullptr)
                    this->assetResources.waitingToBeResidentTexturesHead = resource->waitingToBeResidentNext;
                else
                    previous->waitingToBeResidentNext = resource->waitingToBeResidentNext;
            }
            previous = resource;
        }
    }
}

void VulkanGpuContextImpl::StartRenderTarget(VulkanFrameStage& frameStage, VulkanRenderTarget* renderTarget, StaticVector<VulkanRenderTarget*, EngineConstants::MAX_RENDER_TARGET_DEPENDENCIES>& dependentRenderTargets, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto& frameBuffer = this->frameBuffers[frameStage.frameBufferIndex];

    auto frameBufferSize = this->renderingPixelBounds;
    frameBufferSize.LimitClientSize(this->physicalDeviceDescription.properties.limits.maxFramebufferWidth, this->physicalDeviceDescription.properties.limits.maxFramebufferHeight);

    //Hold onto render targets for FinishRenderTarget()----------------------
    if (renderTarget == nullptr)
        renderTarget = &frameBuffer.renderTarget;
    frameStage.renderTarget = renderTarget;
    frameStage.dependentRenderTargets = dependentRenderTargets;

    //Get command buffer---------------------
    auto commandBuffer = frameBuffer.GetCurrentGraphicsCommandBuffer();

    //Clear render target and depth stencil----------------------
    EnumArray<AttachmentIndex, AttachmentIndex::COUNT, VkClearValue> clearValues;
    auto& clearColor = renderTarget->clearColor.IsSet() ? renderTarget->clearColor.value : this->clearColor;
    clearValues[AttachmentIndex::COLOR].color.float32[0] = clearColor(0);
    clearValues[AttachmentIndex::COLOR].color.float32[1] = clearColor(1);
    clearValues[AttachmentIndex::COLOR].color.float32[2] = clearColor(2);
    clearValues[AttachmentIndex::COLOR].color.float32[3] = clearColor(3);
    clearValues[AttachmentIndex::DEPTH].depthStencil.depth = 1.0f;
    clearValues[AttachmentIndex::DEPTH].depthStencil.stencil = 0;

    VulkanRenderPassBeginInfo renderPassBeginInfo;
    renderPassBeginInfo.renderPass = this->renderPass;
    renderPassBeginInfo.framebuffer = frameBuffer.frameBuffer;
    renderPassBeginInfo.renderArea.offset.x = 0;
    renderPassBeginInfo.renderArea.offset.y = 0;
    renderPassBeginInfo.renderArea.extent.width = static_cast<uint32_t>(frameBufferSize.GetClientWidth());
    renderPassBeginInfo.renderArea.extent.height = static_cast<uint32_t>(frameBufferSize.GetClientHeight());
    renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassBeginInfo.pClearValues = clearValues.data();

    this->vk.CmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    //Set viewport(s)----------------------
    VkViewport viewport = {};
    if (!renderTarget->viewports.empty())
    {
        viewport.width = renderTarget->viewports[0].width;
        viewport.height = renderTarget->viewports[0].height;
    }
    else
    {
        viewport.width = RoundToFloat(frameBufferSize.GetClientWidth());
        viewport.height = RoundToFloat(frameBufferSize.GetClientHeight());
    }
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    this->vk.CmdSetViewport(commandBuffer, 0, 1, &viewport);

    //Set scissor rectangle(s)----------------------
    VkRect2D scissor = {};
    if (!renderTarget->scissorRects.empty())
    {
        scissor.extent.width = renderTarget->scissorRects[0].width;
        scissor.extent.height = renderTarget->scissorRects[0].height;
    }
    else
    {
        scissor.extent.width = static_cast<uint32_t>(frameBufferSize.GetClientWidth());
        scissor.extent.height = static_cast<uint32_t>(frameBufferSize.GetClientHeight());
    }
    this->vk.CmdSetScissor(commandBuffer, 0, 1, &scissor);
}

void VulkanGpuContextImpl::FinishRenderTarget(VulkanFrameStage& frameStage, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto& frameBuffer = this->frameBuffers[frameStage.frameBufferIndex];

    auto renderTarget = frameStage.renderTarget;

    auto commandBuffer = frameBuffer.GetCurrentGraphicsCommandBuffer();
    this->vk.CmdEndRenderPass(commandBuffer);
}


bool VulkanGpuContextImpl::ResolveMeshRef(FinjinMesh& mesh, const AssetReference& assetRef)
{
    if (mesh.gpuMesh == nullptr)
        return false;

    auto vulkanMesh = static_cast<VulkanMesh*>(mesh.gpuMesh);
    return vulkanMesh->IsResidentOnGpu();
}

bool VulkanGpuContextImpl::ResolveMaterialRef(FinjinMaterial& material, const AssetReference& assetRef)
{
    return material.gpuMaterial != nullptr;
}

bool VulkanGpuContextImpl::ResolveMaterialMaps(FinjinMaterial& material)
{
    auto vulkanMaterial = static_cast<VulkanMaterial*>(material.gpuMaterial);

    if (!vulkanMaterial->isFullyResolved)
    {
        auto isFullyResolved = true; //Assume resolution will succeed
        for (auto& map : vulkanMaterial->finjinMaterial->maps)
        {
            if (map.gpuMaterialMapIndex != (size_t)-1)
            {
                auto& vulkanMap = vulkanMaterial->maps[map.gpuMaterialMapIndex];
                if (vulkanMap.texture == nullptr)
                {
                    vulkanMap.texture = this->assetResources.GetTextureByName(map.textureHandle.assetRef.objectName);
                    if (vulkanMap.texture == nullptr)
                        isFullyResolved = false; //Failed to find the texture
                }
            }
        }
        vulkanMaterial->isFullyResolved = isFullyResolved;
    }

    return vulkanMaterial->isFullyResolved;
}

VulkanTexture* VulkanGpuContextImpl::CreateTextureFromMainThread(FinjinTexture* texture, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    std::unique_lock<FiberSpinLock> thisLock(this->createLock);

    this->assetResources.ValidateTextureForCreation(texture->name, error);
    if (error)
    {
        FINJIN_SET_ERROR_NO_MESSAGE(error);
        return nullptr;
    }

    ByteBufferReader reader(texture->fileBytes);

    auto isKTX = false;
    auto isASTC = false;
    auto isPNG = false;
    auto foundWrappedFormat = false;

    WrappedFileReader wrappedFileReader;
    auto wrappedReadHeaderResult = wrappedFileReader.ReadHeader(reader);
    if (wrappedReadHeaderResult == WrappedFileReader::ReadHeaderResult::SUCCESS)
    {
        if (wrappedFileReader.GetHeader().fileExtension == "ktx")
            isKTX = foundWrappedFormat = true;
        else if (wrappedFileReader.GetHeader().fileExtension == "astc")
            isASTC = foundWrappedFormat = true;
        else if (wrappedFileReader.GetHeader().fileExtension == "png")
            isPNG = foundWrappedFormat = true;
        else
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Invalid texture format '%1%'.", wrappedFileReader.GetHeader().fileExtension));
            return nullptr;
        }
    }
    else if (wrappedReadHeaderResult == WrappedFileReader::ReadHeaderResult::INVALID_SIGNATURE_VALUE)
    {
        //Not a wrapped file
    }
    else
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Unexpected failure while reading file: %1%", wrappedFileReader.GetReadHeaderResultString(wrappedReadHeaderResult)));
        return nullptr;
    }

    auto textureQueueFamilies = &this->queueFamilyIndexes.graphics;
    size_t textureQueueFamilyCount = 1;

    VulkanTexture* vulkanTexture = nullptr;

    if (vulkanTexture == nullptr && (isKTX || !foundWrappedFormat))
    {
        KTXReader ktxReader;
        auto ktxReadHeaderResult = ktxReader.ReadHeader(reader);
        if (ktxReadHeaderResult == KTXReader::ReadHeaderResult::SUCCESS)
        {
            auto addedTexture = this->assetResources.texturesByNameHash.GetOrAdd(texture->name.GetHash());
            if (addedTexture.HasErrorOrValue(nullptr))
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to add texture '%1%' to collection.", texture->name));
                return nullptr;
            }

            vulkanTexture = addedTexture.value;

            if (!vulkanTexture->textureData.Create(reader.data_left(), reader.size_left(), GetAllocator()))
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to allocate memory for texture '%1%'.", texture->name));
                return nullptr;
            }

            vulkanTexture->textureDimension = ktxReader.GetHeader().GetDimension();

            vulkanTexture->CreateFromKTX
                (
                ktxReader,
                vulkanTexture->textureData.data(), //reader.data_left(),
                vulkanTexture->textureData.size(), //reader.size_left(),
                this->vulkanSystem->GetImpl()->vk,
                this->physicalDevice,
                this->vk,
                this->settings.deviceAllocationCallbacks,
                this->physicalDeviceDescription,
                0,
                textureQueueFamilies,
                textureQueueFamilyCount,
                error
                );
            if (error)
            {
                vulkanTexture->HandleCreationFailure(this->vk, this->settings.deviceAllocationCallbacks);
                this->assetResources.texturesByNameHash.remove(texture->name.GetHash());

                FINJIN_SET_ERROR(error, "Failed to create texture for KTX texture file.");
                return nullptr;
            }
        }
        else if (ktxReadHeaderResult == KTXReader::ReadHeaderResult::INVALID_SIGNATURE)
        {
            //Do nothing
        }
        else
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read header for KTX texture: %1%", ktxReader.GetReadHeaderResultString(ktxReadHeaderResult)));
            return nullptr;
        }
    }

    if (vulkanTexture == nullptr && (isASTC || !foundWrappedFormat))
    {
        ASTCReader astcReader;
        auto astcReadHeaderResult = astcReader.ReadHeader(reader);
        if (astcReadHeaderResult == ASTCReader::ReadHeaderResult::SUCCESS)
        {
            auto addedTexture = this->assetResources.texturesByNameHash.GetOrAdd(texture->name.GetHash());
            if (addedTexture.HasErrorOrValue(nullptr))
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to add texture '%1%' to collection.", texture->name));
                return nullptr;
            }

            vulkanTexture = addedTexture.value;

            if (!vulkanTexture->textureData.Create(reader.data_left(), reader.size_left(), GetAllocator()))
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to allocate memory for texture '%1%'.", texture->name));
                return nullptr;
            }

            vulkanTexture->textureDimension = astcReader.GetHeader().GetDimension();

            vulkanTexture->CreateFromASTC
                (
                astcReader,
                vulkanTexture->textureData.data(), //reader.data_left(),
                vulkanTexture->textureData.size(), //reader.size_left(),
                this->vulkanSystem->GetImpl()->vk,
                this->physicalDevice,
                this->vk,
                this->settings.deviceAllocationCallbacks,
                this->physicalDeviceDescription,
                textureQueueFamilies,
                textureQueueFamilyCount,
                error
                );
            if (error)
            {
                vulkanTexture->HandleCreationFailure(this->vk, this->settings.deviceAllocationCallbacks);
                this->assetResources.texturesByNameHash.remove(texture->name.GetHash());

                FINJIN_SET_ERROR(error, "Failed to create texture for ASTC texture file.");
                return nullptr;
            }
        }
        else if (astcReadHeaderResult == ASTCReader::ReadHeaderResult::INVALID_SIGNATURE)
        {
            //Do nothing
        }
        else
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read header for ASTC texture: %1%", astcReader.GetReadHeaderResultString(astcReadHeaderResult)));
            return nullptr;
        }
    }

    if (vulkanTexture == nullptr && (isPNG || !foundWrappedFormat))
    {
        PNGReader pngReader;
        pngReader.SetConvert16bitTo8bit(true);
        pngReader.SetAddAlpha(true);

        auto pngReadResult = pngReader.ReadImage(reader, this->readBuffer);
        if (pngReadResult == PNGReader::ReadResult::SUCCESS)
        {
            auto addedTexture = this->assetResources.texturesByNameHash.GetOrAdd(texture->name.GetHash());
            if (addedTexture.HasErrorOrValue(nullptr))
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to add texture '%1%' to collection.", texture->name));
                return nullptr;
            }

            vulkanTexture = addedTexture.value;

            if (!vulkanTexture->textureData.Create(this->readBuffer.data(), this->readBuffer.size(), GetAllocator()))
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to allocate memory for texture '%1%'.", texture->name));
                return nullptr;
            }

            vulkanTexture->textureDimension = TextureDimension::DIMENSION_2D;

            vulkanTexture->CreateFromPNG
                (
                pngReader,
                vulkanTexture->textureData.data(), //this->readBuffer.data()
                vulkanTexture->textureData.size(), //this->readBuffer.size()
                this->vulkanSystem->GetImpl()->vk,
                this->physicalDevice,
                this->vk,
                this->settings.deviceAllocationCallbacks,
                this->physicalDeviceDescription,
                textureQueueFamilies,
                textureQueueFamilyCount,
                error
                );
            if (error)
            {
                vulkanTexture->HandleCreationFailure(this->vk, this->settings.deviceAllocationCallbacks);
                this->assetResources.texturesByNameHash.remove(texture->name.GetHash());

                FINJIN_SET_ERROR(error, "Failed to create texture for PNG texture file.");
                return nullptr;
            }
        }
        else if (pngReadResult == PNGReader::ReadResult::INVALID_SIGNATURE)
        {
            //Do nothing
        }
        else
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read PNG texture: %1%", pngReader.GetReadResultString(pngReadResult)));
            return nullptr;
        }
    }

    if (vulkanTexture == nullptr)
    {
        FINJIN_SET_ERROR(error, "The texture file contained an invalid magic value/signature.");
        return nullptr;
    }

    auto vulkanTextureIndex = this->assetResources.textureOffsetsByDimension[vulkanTexture->textureDimension].GetCurrentOffset();
    vulkanTexture->textureIndex = vulkanTextureIndex;
    if (vulkanTexture->name.assign(texture->name).HasError())
    {
        vulkanTexture->HandleCreationFailure(this->vk, this->settings.deviceAllocationCallbacks);
        this->assetResources.texturesByNameHash.remove(texture->name.GetHash());

        FINJIN_SET_ERROR(error, "Failed to assign texture name.");
        return nullptr;
    }

    //Set texture into texture descriptor set
    {
        VkDescriptorImageInfo descriptorTextureImageInfo = {};
        descriptorTextureImageInfo.imageView = vulkanTexture->resource.imageView;
        descriptorTextureImageInfo.imageLayout = vulkanTexture->imageLayout;

        VulkanWriteDescriptorSet textureWriteDescriptorSet;
        textureWriteDescriptorSet.dstSet = this->textureResources.textureDescriptorSet;
        textureWriteDescriptorSet.dstBinding = TEXTURES_START_BINDING_ID;
        textureWriteDescriptorSet.dstArrayElement = static_cast<uint32_t>(vulkanTexture->textureIndex);
        textureWriteDescriptorSet.descriptorCount = 1;
        textureWriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        textureWriteDescriptorSet.pImageInfo = &descriptorTextureImageInfo;

        this->vk.UpdateDescriptorSets(this->vk.device, 1, &textureWriteDescriptorSet, 0, nullptr);
    }

    //Finish up
    vulkanTexture->isResidentCountdown = (size_t)-1; //Infinite countdown. Not set properly until UploadTexture() is called
    vulkanTexture->waitingToBeResidentNext = this->assetResources.waitingToBeResidentTexturesHead;
    this->assetResources.waitingToBeResidentTexturesHead = vulkanTexture;

    this->assetResources.textureOffsetsByDimension[vulkanTexture->textureDimension].count++;

    texture->gpuTexture = vulkanTexture;

    return vulkanTexture;
}

void VulkanGpuContextImpl::UploadTexture(VkCommandBuffer commandBuffer, FinjinTexture* texture)
{
    auto vulkanTexture = static_cast<VulkanTexture*>(texture->gpuTexture);

    vulkanTexture->Upload(this->vk, commandBuffer);

    vulkanTexture->isResidentCountdown = this->settings.frameBufferCount.actual + 1;
}

void* VulkanGpuContextImpl::CreateLightFromMainThread(FinjinSceneObjectLight* light, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    std::unique_lock<FiberSpinLock> thisLock(this->createLock);

    auto vulkanLight = this->lights.Use();
    if (vulkanLight == nullptr)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to add light '%1%' to collection.", light->name));
        return nullptr;
    }

    vulkanLight->finjinLight = light;

    light->gpuLight = vulkanLight;

    return vulkanLight;
}

void* VulkanGpuContextImpl::CreateMaterial(VkCommandBuffer commandBuffer, FinjinMaterial* material, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    this->assetResources.ValidateMaterialForCreation(material->name, error);
    if (error)
    {
        FINJIN_SET_ERROR_NO_MESSAGE(error);
        return nullptr;
    }

    auto vulkanMaterialIndex = this->assetResources.materialsByNameHash.size();

    auto addedMaterial = this->assetResources.materialsByNameHash.GetOrAdd(material->name.GetHash());
    if (addedMaterial.HasErrorOrValue(nullptr))
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to add material '%1%' to collection.", material->name));
        return nullptr;
    }

    auto& vulkanMaterial = *addedMaterial.value;
    vulkanMaterial.finjinMaterial = material;
    vulkanMaterial.gpuBufferIndex = vulkanMaterialIndex;

    material->gpuMaterial = &vulkanMaterial;

    for (auto& map : material->maps)
    {
        auto foundAt = this->materialMapTypeToGpuElements.find(map.typeName);
        assert(foundAt != this->materialMapTypeToGpuElements.end());
        if (foundAt != this->materialMapTypeToGpuElements.end())
        {
            map.gpuMaterialMapIndex = static_cast<size_t>(foundAt->second.gpuMaterialMapIndex);
            map.gpuBufferTextureIndexElementID = static_cast<size_t>(foundAt->second.gpuBufferTextureIndexElementID);
            map.gpuBufferAmountElementID = static_cast<size_t>(foundAt->second.gpuBufferAmountElementID);
            map.gpuMaterialMapFlag = foundAt->second.gpuMaterialMapFlag;

            vulkanMaterial.maps[map.gpuMaterialMapIndex].finjinMap = &map;
            material->gpuMapFlags |= map.gpuMaterialMapFlag;
        }
    }
    ResolveMaterialMaps(*material);

    return &vulkanMaterial;
}

void* VulkanGpuContextImpl::CreateMeshFromMainThread(FinjinMesh* mesh, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    std::unique_lock<FiberSpinLock> thisLock(this->createLock);

    this->assetResources.ValidateMeshForCreation(mesh->name, error);
    if (error)
    {
        FINJIN_SET_ERROR_NO_MESSAGE(error);
        return nullptr;
    }

    auto addedMesh = this->assetResources.meshesByNameHash.GetOrAdd(mesh->name.GetHash());
    if (addedMesh.HasErrorOrValue(nullptr))
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to add mesh '%1%' to collection.", mesh->name));
        return nullptr;
    }

    auto& vulkanMesh = *addedMesh.value;

    vulkanMesh.sharedIndexBuffer.CreateIndexBuffer(this->vk, this->settings.deviceAllocationCallbacks, mesh->indexBuffer, this->physicalDeviceDescription, GetAllocator(), error);
    if (error)
    {
        vulkanMesh.HandleCreationFailure(this->vk, this->settings.deviceAllocationCallbacks);
        this->assetResources.meshesByNameHash.remove(mesh->name.GetHash());

        FINJIN_SET_ERROR(error, "Failed to create main mesh index buffer.");
        return nullptr;
    }

    if (!mesh->vertexBuffers.empty())
    {
        if (!vulkanMesh.sharedVertexBuffers.Create(mesh->vertexBuffers.size(), GetAllocator()))
        {
            vulkanMesh.HandleCreationFailure(this->vk, this->settings.deviceAllocationCallbacks);
            this->assetResources.meshesByNameHash.remove(mesh->name.GetHash());

            FINJIN_SET_ERROR(error, "Failed to create shared vertex buffers collection.");
            return nullptr;
        }

        for (size_t vertexBufferIndex = 0; vertexBufferIndex < mesh->vertexBuffers.size(); vertexBufferIndex++)
        {
            auto inputFormat = this->internalSettings.GetInputFormat(mesh->vertexBuffers[vertexBufferIndex].formatName, mesh->vertexBuffers[vertexBufferIndex].formatElements);
            vulkanMesh.sharedVertexBuffers[vertexBufferIndex].CreateVertexBuffer(this->vk, this->settings.deviceAllocationCallbacks, mesh->vertexBuffers[vertexBufferIndex], inputFormat, this->physicalDeviceDescription, GetAllocator(), error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to create shared vertex buffer.");
                return nullptr;
            }
        }
    }

    if (!vulkanMesh.submeshes.Create(mesh->submeshes.size(), GetAllocator(), &vulkanMesh))
    {
        vulkanMesh.HandleCreationFailure(this->vk, this->settings.deviceAllocationCallbacks);
        this->assetResources.meshesByNameHash.remove(mesh->name.GetHash());

        FINJIN_SET_ERROR(error, "Failed to create submeshes collection.");
        return nullptr;
    }

    for (size_t submeshIndex = 0; submeshIndex < mesh->submeshes.size(); submeshIndex++)
    {
        auto& vulkanSubmesh = vulkanMesh.submeshes[submeshIndex];
        auto& submesh = mesh->submeshes[submeshIndex];

        vulkanSubmesh.vertexBufferIndex = static_cast<uint32_t>(submesh.vertexBufferRange.bufferIndex);
        vulkanSubmesh.startIndexLocation = static_cast<uint32_t>(submesh.indexBufferRange.startIndex);
        vulkanSubmesh.indexCount = static_cast<uint32_t>(submesh.indexBufferRange.count);
        vulkanSubmesh.baseVertexLocation = static_cast<uint32_t>(submesh.vertexBufferRange.startIndex);
        vulkanSubmesh.vertexCount = static_cast<uint32_t>(submesh.vertexBufferRange.count);

        auto inputFormat = this->internalSettings.GetInputFormat(submesh.vertexBuffer.formatName, submesh.vertexBuffer.formatElements);
        vulkanSubmesh.vertexBuffer.CreateVertexBuffer(this->vk, this->settings.deviceAllocationCallbacks, submesh.vertexBuffer, inputFormat, this->physicalDeviceDescription, GetAllocator(), error);
        if (error)
        {
            vulkanMesh.HandleCreationFailure(this->vk, this->settings.deviceAllocationCallbacks);
            this->assetResources.meshesByNameHash.remove(mesh->name.GetHash());

            FINJIN_SET_ERROR(error, "Failed to create submesh vertex buffer.");
            return nullptr;
        }

        vulkanSubmesh.indexBuffer.CreateIndexBuffer(this->vk, this->settings.deviceAllocationCallbacks, submesh.indexBuffer, this->physicalDeviceDescription, GetAllocator(), error);
        if (error)
        {
            vulkanMesh.HandleCreationFailure(this->vk, this->settings.deviceAllocationCallbacks);
            this->assetResources.meshesByNameHash.remove(mesh->name.GetHash());

            FINJIN_SET_ERROR(error, "Failed to create submesh index buffer.");
            return nullptr;
        }
    }

    vulkanMesh.isResidentCountdown = (size_t)-1; //Infinite countdown. Not set properly until UploadMesh() is called
    vulkanMesh.waitingToBeResidentNext = this->assetResources.waitingToBeResidentMeshesHead;
    this->assetResources.waitingToBeResidentMeshesHead = &vulkanMesh;

    mesh->gpuMesh = &vulkanMesh;
    vulkanMesh.finjinMesh = mesh;

    return &vulkanMesh;
}

void VulkanGpuContextImpl::CreateShader(VulkanShaderType shaderType, const Utf8String& name, const uint8_t* bytes, size_t byteCount, bool makeLocalCopy, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    std::unique_lock<FiberSpinLock> thisLock(this->createLock);

    auto isNewResource = this->assetResources.ValidateShaderForCreation(shaderType, name, error);
    if (error)
    {
        FINJIN_SET_ERROR_NO_MESSAGE(error);
        return;
    }
    if (!isNewResource)
        return;

    auto& shadersByNameHash = this->assetResources.shadersByShaderTypeAndNameHash[shaderType];
    auto addedShader = shadersByNameHash.GetOrAdd(name.GetHash());
    if (addedShader.HasErrorOrValue(nullptr))
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to add shader '%1%' to collection.", name));
        return;
    }

    auto& vulkanShader = *addedShader.value;

    vulkanShader.Create(this->vk, this->settings.deviceAllocationCallbacks, GetAllocator(), name, bytes, byteCount, makeLocalCopy, error);
    if (error)
    {
        vulkanShader.HandleCreationFailed(this->vk, this->settings.deviceAllocationCallbacks);
        shadersByNameHash.remove(name.GetHash());

        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create shader '%1%'.", name));
        return;
    }
}

void VulkanGpuContextImpl::CreateShader(VulkanShaderType shaderType, const AssetReference& assetRef, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    std::unique_lock<FiberSpinLock> thisLock(this->createLock);

    auto& name = assetRef.objectName;

    auto isNewResource = this->assetResources.ValidateShaderForCreation(shaderType, name, error);
    if (error)
    {
        FINJIN_SET_ERROR_NO_MESSAGE(error);
        return;
    }
    if (!isNewResource)
        return;

    auto& shadersByNameHash = this->assetResources.shadersByShaderTypeAndNameHash[shaderType];
    auto addedShader = shadersByNameHash.GetOrAdd(name.GetHash());
    if (addedShader.HasErrorOrValue(nullptr))
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to add shader '%1%' to collection.", name));
        return;
    }

    auto& vulkanShader = *addedShader.value;

    auto readResult = this->shaderAssetClassFileReader.ReadAsset(this->readBuffer, assetRef);
    if (readResult == FileOperationResult::SUCCESS)
    {
        vulkanShader.Create(this->vk, this->settings.deviceAllocationCallbacks, GetAllocator(), name, this->readBuffer.data(), this->readBuffer.size(), true, error);
        if (error)
        {
            vulkanShader.HandleCreationFailed(this->vk, this->settings.deviceAllocationCallbacks);
            shadersByNameHash.remove(name.GetHash());

            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create shader '%1%'.", name));
            return;
        }
    }
    else
    {
        vulkanShader.HandleCreationFailed(this->vk, this->settings.deviceAllocationCallbacks);
        shadersByNameHash.remove(name.GetHash());

        if (readResult == FileOperationResult::NOT_FOUND)
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read shader '%1%'. File could not be found.", assetRef.ToUriString()));
        else
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read shader '%1%'. There was an error reading the file.", assetRef.ToUriString()));
    }
}

void VulkanGpuContextImpl::UploadMesh(VkCommandBuffer commandBuffer, FinjinMesh* mesh, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto vulkanMesh = static_cast<VulkanMesh*>(mesh->gpuMesh);

    vulkanMesh->sharedIndexBuffer.Upload(this->vk, commandBuffer);

    for (size_t vertexBufferIndex = 0; vertexBufferIndex < mesh->vertexBuffers.size(); vertexBufferIndex++)
        vulkanMesh->sharedVertexBuffers[vertexBufferIndex].Upload(this->vk, commandBuffer);

    for (size_t submeshIndex = 0; submeshIndex < mesh->submeshes.size(); submeshIndex++)
    {
        auto& vulkanSubmesh = vulkanMesh->submeshes[submeshIndex];

        vulkanSubmesh.vertexBuffer.Upload(this->vk, commandBuffer);
        vulkanSubmesh.indexBuffer.Upload(this->vk, commandBuffer);
    }

    vulkanMesh->isResidentCountdown = this->settings.frameBufferCount.actual + 1;
}

void VulkanGpuContextImpl::UpdateCachedFrameBufferSize()
{
    this->renderingPixelBounds = this->settings.osWindow->GetWindowSize().GetSafeCurrentBounds();
}

#endif
