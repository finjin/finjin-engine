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
#define TEXTURE_SAMPLER_BINDING_ID 0
#define TEXTURES_START_BINDING_ID 1

#define FULL_SCREEN_QUAD_TEXTURE_SAMPLER_BINDING_ID 0
#define FULL_SCREEN_QUAD_TEXTURES_START_BINDING_ID 1


//Local types-------------------------------------------------------------------
enum class SceneRenderAttachmentIndex
{
    COLOR,
    DEPTH,

    COUNT
};

enum class FullScreenQuadAttachmentIndex
{
    COLOR,

    COUNT
};

enum class TextureBindingIndex
{
    DEFAULT_SAMPLER,
    TEXTURES_START,

    COUNT
};

enum class FullScreenQuadBindingIndex
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
                            this->contextImpl->settings.maxMaterials = Convert::ToInteger(value, this->contextImpl->settings.maxMaterials);
                        else if (key == "max-2d-textures")
                            this->contextImpl->settings.max2DTextures = Convert::ToInteger(value, this->contextImpl->settings.max2DTextures);
                        else if (key == "max-cube-textures")
                            this->contextImpl->settings.maxCubeTextures = Convert::ToInteger(value, this->contextImpl->settings.maxCubeTextures);
                        else if (key == "max-3d-textures")
                            this->contextImpl->settings.max3DTextures = Convert::ToInteger(value, this->contextImpl->settings.max3DTextures);
                        else if (key == "max-shaders")
                            this->contextImpl->settings.maxShaders = Convert::ToInteger(value, this->contextImpl->settings.maxShaders);
                        else if (key == "max-meshes")
                            this->contextImpl->settings.maxMeshes = Convert::ToInteger(value, this->contextImpl->settings.maxMeshes);
                        else if (key == "max-lights")
                            this->contextImpl->settings.maxLights = Convert::ToInteger(value, this->contextImpl->settings.maxLights);
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
                size_t elementIndex = 0;
                for (elementIndex = 0; elementIndex < count; elementIndex++)
                {
                    auto inputFormatElement = inputFormat.GetElement(elementIndex);
                    if (elements[elementIndex].id != inputFormatElement->elementID || elements[elementIndex].type != inputFormatElement->type)
                        break;
                }
                if (elementIndex == count)
                    return &inputFormat;
            }
        }
    }

    return nullptr;
}

//VulkanGpuContextImpl
VulkanGpuContextImpl::VulkanGpuContextImpl(Allocator* allocator) :
    AllocatedClass(allocator),
    GpuContextCommonImpl(allocator),
    settings(allocator),
    materialMapTypeToElements
        (
        StandardAssetDocumentPropertyValues::TypeName::MATERIAL_MAP_DIFFUSE, GpuMaterialMapIndexToConstantBufferElements<VulkanMaterial::MapIndex>(VulkanMaterial::MapIndex::DIFFUSE, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_DIFFUSE_TEXTURE_INDEX, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_DIFFUSE_AMOUNT, ShaderFeatureFlag::MAP_DIFFUSE),
        StandardAssetDocumentPropertyValues::TypeName::MATERIAL_MAP_SPECULAR, GpuMaterialMapIndexToConstantBufferElements<VulkanMaterial::MapIndex>(VulkanMaterial::MapIndex::SPECULAR, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_SPECULAR_TEXTURE_INDEX, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_SPECULAR_AMOUNT, ShaderFeatureFlag::MAP_SPECULAR),
        StandardAssetDocumentPropertyValues::TypeName::MATERIAL_MAP_REFLECTION, GpuMaterialMapIndexToConstantBufferElements<VulkanMaterial::MapIndex>(VulkanMaterial::MapIndex::REFLECTION, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_REFLECTION_TEXTURE_INDEX, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_REFLECTION_AMOUNT, ShaderFeatureFlag::MAP_REFLECTION),
        StandardAssetDocumentPropertyValues::TypeName::MATERIAL_MAP_REFRACTION, GpuMaterialMapIndexToConstantBufferElements<VulkanMaterial::MapIndex>(VulkanMaterial::MapIndex::REFRACTION, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_REFRACTION_TEXTURE_INDEX, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_REFRACTION_AMOUNT, ShaderFeatureFlag::MAP_REFRACTION),
        StandardAssetDocumentPropertyValues::TypeName::MATERIAL_MAP_BUMP, GpuMaterialMapIndexToConstantBufferElements<VulkanMaterial::MapIndex>(VulkanMaterial::MapIndex::BUMP, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_BUMP_TEXTURE_INDEX, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_BUMP_AMOUNT, ShaderFeatureFlag::MAP_BUMP),
        StandardAssetDocumentPropertyValues::TypeName::MATERIAL_MAP_HEIGHT, GpuMaterialMapIndexToConstantBufferElements<VulkanMaterial::MapIndex>(VulkanMaterial::MapIndex::HEIGHT, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_HEIGHT_TEXTURE_INDEX, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_HEIGHT_AMOUNT, ShaderFeatureFlag::MAP_HEIGHT),
        StandardAssetDocumentPropertyValues::TypeName::MATERIAL_MAP_SELF_ILLUMINATION, GpuMaterialMapIndexToConstantBufferElements<VulkanMaterial::MapIndex>(VulkanMaterial::MapIndex::SELF_ILLUMINATION, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_SELF_ILLUMINATION_TEXTURE_INDEX, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_SELF_ILLUMINATION_AMOUNT, ShaderFeatureFlag::MAP_SELF_ILLUMINATION),
        StandardAssetDocumentPropertyValues::TypeName::MATERIAL_MAP_OPACITY, GpuMaterialMapIndexToConstantBufferElements<VulkanMaterial::MapIndex>(VulkanMaterial::MapIndex::OPACITY, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_OPACITY_TEXTURE_INDEX, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_OPACITY_AMOUNT, ShaderFeatureFlag::MAP_OPACITY),
        StandardAssetDocumentPropertyValues::TypeName::MATERIAL_MAP_ENVIRONMENT, GpuMaterialMapIndexToConstantBufferElements<VulkanMaterial::MapIndex>(VulkanMaterial::MapIndex::ENVIRONMENT, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_ENVIRONMENT_TEXTURE_INDEX, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_ENVIRONMENT_AMOUNT, ShaderFeatureFlag::MAP_ENVIRONMENT),
        StandardAssetDocumentPropertyValues::TypeName::MATERIAL_MAP_SHININESS, GpuMaterialMapIndexToConstantBufferElements<VulkanMaterial::MapIndex>(VulkanMaterial::MapIndex::SHININESS, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_SHININESS_TEXTURE_INDEX, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_SHININESS_AMOUNT, ShaderFeatureFlag::MAP_SHININESS)
        ),
    commonResources(allocator)
{
    this->physicalDevice = VK_NULL_HANDLE;

    this->surface = VK_NULL_HANDLE;

    this->swapChain = VK_NULL_HANDLE;

    this->sceneRenderPass = VK_NULL_HANDLE;
    this->toFullScreenQuadRenderPass = VK_NULL_HANDLE;

    this->presentCompleteSemaphore = VK_NULL_HANDLE;

    this->frameBufferScreenCaptureColorFormatBytesPerPixel = 0;
    this->frameBufferScreenCapturePixelFormat = ScreenCapturePixelFormat::NONE;
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

    //Destroy device and dependent objects
    if (this->vk.device != VK_NULL_HANDLE)
    {
        for (auto& swapChainFrameBuffer : this->swapChainFrameBuffers)
            swapChainFrameBuffer.Destroy(this->vk, this->settings.deviceAllocationCallbacks);

        for (auto& frameBuffer : this->frameBuffers)
            frameBuffer.Destroy(this->vk, this->settings.deviceAllocationCallbacks);

        for (auto& frameStage : this->frameStages)
            frameStage.Destroy(this->vk, this->settings.deviceAllocationCallbacks);

        this->depthStencilRenderTarget.Destroy(this->vk, this->settings.deviceAllocationCallbacks);

        this->staticMeshRenderer.Destroy();

        this->assetResources.Destroy(this->vk, this->settings.deviceAllocationCallbacks);
        this->commonResources.Destroy(this->vk, this->settings.deviceAllocationCallbacks);

        this->tempGraphicsWork.Destroy(this->vk, this->settings.deviceAllocationCallbacks);

        if (this->sceneRenderPass != VK_NULL_HANDLE)
        {
            this->vk.DestroyRenderPass(this->vk.device, this->sceneRenderPass, this->settings.deviceAllocationCallbacks);
            this->sceneRenderPass = VK_NULL_HANDLE;
        }

        if (this->toFullScreenQuadRenderPass != VK_NULL_HANDLE)
        {
            this->vk.DestroyRenderPass(this->vk.device, this->toFullScreenQuadRenderPass, this->settings.deviceAllocationCallbacks);
            this->toFullScreenQuadRenderPass = VK_NULL_HANDLE;
        }

        if (this->presentCompleteSemaphore != VK_NULL_HANDLE)
        {
            this->vk.DestroySemaphore(this->vk.device, this->presentCompleteSemaphore, this->settings.deviceAllocationCallbacks);
            this->presentCompleteSemaphore = VK_NULL_HANDLE;
        }

        if (this->swapChain != VK_NULL_HANDLE)
        {
            this->vk.DestroySwapchainKHR(this->vk.device, this->swapChain, this->settings.deviceAllocationCallbacks);
            this->swapChain = VK_NULL_HANDLE;
        }

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
    if (!this->surfaceFormats.GetBestColorFormatAndSpace(this->settings.colorFormat, this->settings.colorSpace, VK_FORMAT_B8G8R8A8_UNORM))
    {
        FINJIN_SET_ERROR(error, "Failed to determine supported color format and color space.");
        return;
    }
    switch (this->settings.colorFormat.actual)
    {
        case VK_FORMAT_R8G8B8A8_UNORM: this->frameBufferScreenCapturePixelFormat = ScreenCapturePixelFormat::RGBA8_UNORM; this->frameBufferScreenCaptureColorFormatBytesPerPixel = 4; break;
        case VK_FORMAT_R8G8B8A8_SRGB: this->frameBufferScreenCapturePixelFormat = ScreenCapturePixelFormat::RGBA8_UNORM_SRGB; this->frameBufferScreenCaptureColorFormatBytesPerPixel = 4; break;
        case VK_FORMAT_B8G8R8A8_UNORM: this->frameBufferScreenCapturePixelFormat = ScreenCapturePixelFormat::BGRA8_UNORM; this->frameBufferScreenCaptureColorFormatBytesPerPixel = 4; break;
        case VK_FORMAT_B8G8R8A8_SRGB: this->frameBufferScreenCapturePixelFormat = ScreenCapturePixelFormat::BGRA8_UNORM_SRGB; this->frameBufferScreenCaptureColorFormatBytesPerPixel = 4; break;
        case VK_FORMAT_R16G16B16A16_SFLOAT: this->frameBufferScreenCapturePixelFormat = ScreenCapturePixelFormat::RGBA16_FLOAT; this->frameBufferScreenCaptureColorFormatBytesPerPixel = 8; break;
        default: assert(0); this->frameBufferScreenCapturePixelFormat = ScreenCapturePixelFormat::NONE; break;
    }
    this->settings.screenCaptureFrequency.actual = this->settings.screenCaptureFrequency.requested;

    //Determine depth/stencil format-------------------------
    if (!VulkanUtilities::GetBestDepthStencilFormat(this->settings.depthStencilFormat, this->settings.stencilRequired, this->vulkanSystem->GetImpl()->vk, this->physicalDevice))
    {
        FINJIN_SET_ERROR(error, "Failed to determine supported depth/stencil format.");
        return;
    }

    //Create device--------------------------------------
    const float queuePriorities[] = { 0.0f }; //Just one queue of each type needed so only one priority, which doesn't matter
    auto queueInfos = this->queueFamilyIndexes.GetCreateInfo(queuePriorities, FINJIN_COUNT_OF(queuePriorities));

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
    this->primaryQueues.Create(this->vk, this->queueFamilyIndexes, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to get all primary queues.");
        return;
    }

    VulkanSemaphoreCreateInfo semaphoreCreateInfo;
    result = this->vk.CreateSemaphore(this->vk.device, &semaphoreCreateInfo, this->settings.deviceAllocationCallbacks, &this->presentCompleteSemaphore);
    if (FINJIN_CHECK_VKRESULT_FAILED(result))
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create rendering complete semaphore. Vulkan result: %1%", VulkanResult::ToString(result)));
        return;
    }

    //Create temp work objects----------------------------------
    this->tempGraphicsWork.Create(this->vk, this->settings.deviceAllocationCallbacks, this->primaryQueues.graphics,  this->queueFamilyIndexes.graphics, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to create temporary work objects.");
        return;
    }

    //Create pipeline specific objects------------------------------------
    this->frameStages.resize(this->settings.jobProcessingPipelineSize);
    for (size_t frameStageIndex = 0; frameStageIndex < this->frameStages.size(); frameStageIndex++)
    {
        auto& frameStage = this->frameStages[frameStageIndex];

        frameStage.SetIndex(frameStageIndex);
    }

    //Determine some settings-------------------------------------------
    UpdatedCachedWindowSize();
    auto maxRenderTargetSize = this->settings.renderTargetSize.EvaluateMax(nullptr, &this->windowPixelBounds);

    this->tempGraphicsWork.Start(this->vk, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to start work for creating resources.");
        return;
    }

    this->frameBuffers.resize(this->settings.frameBufferCount.actual);
    for (size_t frameBufferIndex = 0; frameBufferIndex < this->frameBuffers.size(); frameBufferIndex++)
    {
        auto& frameBuffer = this->frameBuffers[frameBufferIndex];

        frameBuffer.SetIndex(frameBufferIndex);

        frameBuffer.CreateCommandBuffers(this->vk, this->settings.deviceAllocationCallbacks, this->queueFamilyIndexes, this->settings.maxGpuCommandListsPerStage, GetAllocator(), error);
        if (error)
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create command buffer objects for frame buffer %1%.", frameBufferIndex));
            return;
        }

        if (this->settings.renderTargetSize.GetType() == GpuRenderTargetSizeType::EXPLICIT_SIZE)
        {
            //Create color buffer
            frameBuffer.renderTarget.CreateColor
                (
                this->physicalDevice,
                this->physicalDeviceDescription,
                this->vulkanSystem->GetImpl()->vk,
                this->vk,
                this->settings.deviceAllocationCallbacks,
                this->tempGraphicsWork.commandBuffer,
                maxRenderTargetSize[0],
                maxRenderTargetSize[1],
                this->settings.colorFormat.actual,
                this->settings.multisampleCount.actual,
                false,
                error
                );
            if (error)
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create color render target for frame buffer %1%.", frameBufferIndex));
                return;
            }

            //Create screen capture buffer
            if (this->settings.screenCaptureFrequency.actual != ScreenCaptureFrequency::NEVER)
            {
                auto byteCount = maxRenderTargetSize[0] * maxRenderTargetSize[1] * this->frameBufferScreenCaptureColorFormatBytesPerPixel;
                frameBuffer.CreateScreenCaptureBuffer(this->vk, this->settings.deviceAllocationCallbacks, this->physicalDeviceDescription, byteCount, false, error);
                if (error)
                {
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create screen capture buffer for frame buffer %1%.", frameBufferIndex));
                    return;
                }
            }
        }
    }

    if (this->settings.renderTargetSize.GetType() == GpuRenderTargetSizeType::EXPLICIT_SIZE &&
        this->settings.depthStencilFormat.actual != VK_FORMAT_UNDEFINED)
    {
        this->depthStencilRenderTarget.CreateDepthStencil
            (
            this->physicalDevice,
            this->physicalDeviceDescription,
            this->vulkanSystem->GetImpl()->vk,
            this->vk,
            this->settings.deviceAllocationCallbacks,
            this->tempGraphicsWork.commandBuffer,
            maxRenderTargetSize[0],
            maxRenderTargetSize[1],
            this->settings.depthStencilFormat.actual,
            this->settings.multisampleCount.actual,
            false,
            error
            );
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to create depth/stencil buffer.");
            return;
        }
    }

    this->tempGraphicsWork.Finish(this->vk, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to finish work for creating resources.");
        return;
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

    if (!this->commonResources.lights.Create(this->settings.maxLights, GetAllocator(), GetAllocator()))
    {
        FINJIN_SET_ERROR(error, "Failed to allocate light lookup.");
        return;
    }
    for (size_t lightIndex = 0; lightIndex < this->commonResources.lights.items.size(); lightIndex++)
        this->commonResources.lights.items[lightIndex].gpuBufferIndex = lightIndex;

    //Create pipeline cache
    {
        VulkanPipelineCacheCreateInfo pipelineCacheCreateInfo; //No special setup needed
        auto result = this->vk.CreatePipelineCache(this->vk.device, &pipelineCacheCreateInfo, this->settings.deviceAllocationCallbacks, &this->commonResources.pipelineCache);
        if (result != VK_SUCCESS)
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create pipeline cache. Vulkan error: %1%", VulkanResult::ToString(result)));
            return;
        }
    }

    //Create texture samplers
    {
        //Create sampler
        VulkanSamplerCreateInfo samplerCreateInfo(VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT);
        auto result = this->vk.CreateSampler(this->vk.device, &samplerCreateInfo, this->settings.deviceAllocationCallbacks, &this->commonResources.defaultSampler);
        if (result != VK_SUCCESS)
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create default texture sampler. Vulkan error: %1%", VulkanResult::ToString(result)));
            return;
        }
    }

    //Create other texture resources
    {
        size_t textureOffset = this->settings.frameBufferCount.actual; //Reserve the first off screen buffers

        this->assetResources.textureOffsetsByDimension[TextureDimension::DIMENSION_2D].SetOffsetAndCount(textureOffset, this->settings.max2DTextures);
        textureOffset += this->settings.max2DTextures;

        this->assetResources.textureOffsetsByDimension[TextureDimension::DIMENSION_CUBE].SetOffsetAndCount(textureOffset, this->settings.maxCubeTextures);
        textureOffset += this->settings.maxCubeTextures;

        this->assetResources.textureOffsetsByDimension[TextureDimension::DIMENSION_3D].SetOffsetAndCount(textureOffset, this->settings.max3DTextures);
        textureOffset += this->settings.max3DTextures;

        //Texture descriptor set layout and pool
        {
            VulkanDescriptorSetLayoutBuilder texturesDescriptorSetLayoutBuilder;
            texturesDescriptorSetLayoutBuilder.AddBinding(TextureBindingIndex::DEFAULT_SAMPLER, TEXTURE_SAMPLER_BINDING_ID, VK_DESCRIPTOR_TYPE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
            texturesDescriptorSetLayoutBuilder.AddBinding(TextureBindingIndex::TEXTURES_START, TEXTURES_START_BINDING_ID, VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, textureOffset - this->settings.frameBufferCount.actual, VK_SHADER_STAGE_FRAGMENT_BIT);

            //Layout
            texturesDescriptorSetLayoutBuilder.CreateDescriptorSetLayout(&this->commonResources.texturesDescriptorSetLayout, this->vk, this->settings.deviceAllocationCallbacks, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to create texture descriptor set layout.");
                return;
            }

            //Pool
            texturesDescriptorSetLayoutBuilder.CreateDescriptorPool(&this->commonResources.textureDescriptorPool, 1, this->vk, this->settings.deviceAllocationCallbacks, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to create texture descriptor pool.");
                return;
            }
        }

        //Texture descriptor set
        VulkanDescriptorSetAllocateInfo textureDescriptorSetAllocateInfo(this->commonResources.textureDescriptorPool);
        textureDescriptorSetAllocateInfo.descriptorSetCount = 1;
        textureDescriptorSetAllocateInfo.pSetLayouts = &this->commonResources.texturesDescriptorSetLayout;
        auto result = this->vk.AllocateDescriptorSets(this->vk.device, &textureDescriptorSetAllocateInfo, &this->commonResources.textureDescriptorSet);
        if (result != VK_SUCCESS)
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to allocate texture descriptor set. Vulkan result: %1%", VulkanResult::ToString(result)));
            return;
        }

        //Set sampler into descriptor set
        {
            VkDescriptorImageInfo descriptorSamplerInfo = {};
            descriptorSamplerInfo.sampler = this->commonResources.defaultSampler;

            VulkanWriteDescriptorSet samplerWriteDescriptorSet;
            samplerWriteDescriptorSet.dstSet = this->commonResources.textureDescriptorSet;
            samplerWriteDescriptorSet.dstBinding = TEXTURE_SAMPLER_BINDING_ID;
            samplerWriteDescriptorSet.descriptorCount = 1;
            samplerWriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
            samplerWriteDescriptorSet.pImageInfo = &descriptorSamplerInfo;
            this->vk.UpdateDescriptorSets(this->vk.device, 1, &samplerWriteDescriptorSet, 0, nullptr);
        }
    }

    //Need attachments for render target and depth buffer
    {
        EnumArray<SceneRenderAttachmentIndex, SceneRenderAttachmentIndex::COUNT, VkAttachmentDescription> attachmentDescriptions;
        FINJIN_ZERO_ITEM(attachmentDescriptions);

        //Color attachment
        {
            auto& attachmentDescription = attachmentDescriptions[SceneRenderAttachmentIndex::COLOR];
            attachmentDescription.format = this->settings.colorFormat.actual;
            attachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
            attachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            attachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            attachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            attachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            attachmentDescription.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        }

        //Depth/stencil attachment
        {
            auto& attachmentDescription = attachmentDescriptions[SceneRenderAttachmentIndex::DEPTH];
            attachmentDescription.format = this->settings.depthStencilFormat.actual;
            attachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
            attachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            attachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            attachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
            attachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
            attachmentDescription.initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        }

        VulkanAttachmentReference colorAttachmentReference((uint32_t)SceneRenderAttachmentIndex::COLOR, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
        VulkanAttachmentReference depthAttachmentReference((uint32_t)SceneRenderAttachmentIndex::DEPTH, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

        VkSubpassDescription subpass = {};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentReference;
        subpass.pDepthStencilAttachment = &depthAttachmentReference;

        std::array<VkSubpassDependency, 2> dependencies;
        {
            FINJIN_ZERO_ITEM(dependencies);

            //First dependency at the start of the renderpass
            //The transition from final to initial layout
            {
                auto& dependency = dependencies[0];
                dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
                dependency.dstSubpass = 0;
                dependency.srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
                dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
                dependency.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
                dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
                dependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
            }

            //Second dependency at the end the renderpass
            //The transition from the initial to the final layout
            {
                auto& dependency = dependencies[1];
                dependency.srcSubpass = 0;
                dependency.dstSubpass = VK_SUBPASS_EXTERNAL;
                dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
                dependency.dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
                dependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
                dependency.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
                dependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
            }
        }

        VulkanRenderPassCreateInfo renderPassCreateInfo;
        renderPassCreateInfo.attachmentCount = static_cast<uint32_t>(attachmentDescriptions.size());
        renderPassCreateInfo.pAttachments = attachmentDescriptions.data();
        renderPassCreateInfo.subpassCount = 1;
        renderPassCreateInfo.pSubpasses = &subpass;
        renderPassCreateInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
        renderPassCreateInfo.pDependencies = dependencies.data();

        auto result = this->vk.CreateRenderPass(this->vk.device, &renderPassCreateInfo, this->settings.deviceAllocationCallbacks, &this->sceneRenderPass);
        if (FINJIN_CHECK_VKRESULT_FAILED(result))
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create scene render pass. Vulkan result: %1%", VulkanResult::ToString(result)));
            return;
        }
    }

    {
        EnumArray<FullScreenQuadAttachmentIndex, FullScreenQuadAttachmentIndex::COUNT, VkAttachmentDescription> attachmentDescriptions;
        FINJIN_ZERO_ITEM(attachmentDescriptions);

        //Color attachment
        {
            auto& attachmentDescription = attachmentDescriptions[SceneRenderAttachmentIndex::COLOR];
            attachmentDescription.format = this->settings.colorFormat.actual;
            attachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
            attachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            attachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            attachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            attachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            attachmentDescription.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        }

        VulkanAttachmentReference colorAttachmentReference((uint32_t)SceneRenderAttachmentIndex::COLOR, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

        VkSubpassDescription subpass = {};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentReference;

        std::array<VkSubpassDependency, 2> dependencies;
        {
            FINJIN_ZERO_ITEM(dependencies);

            //First dependency at the start of the renderpass
            //The transition from final to initial layout
            {
                auto& dependency = dependencies[0];
                dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
                dependency.dstSubpass = 0;
                dependency.srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
                dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
                dependency.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
                dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
                dependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
            }

            //Second dependency at the end the renderpass
            //The transition from the initial to the final layout
            {
                auto& dependency = dependencies[1];
                dependency.srcSubpass = 0;
                dependency.dstSubpass = VK_SUBPASS_EXTERNAL;
                dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
                dependency.dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
                dependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
                dependency.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
                dependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
            }
        }

        VulkanRenderPassCreateInfo renderPassCreateInfo;
        renderPassCreateInfo.attachmentCount = static_cast<uint32_t>(attachmentDescriptions.size());
        renderPassCreateInfo.pAttachments = attachmentDescriptions.data();
        renderPassCreateInfo.subpassCount = 1;
        renderPassCreateInfo.pSubpasses = &subpass;
        renderPassCreateInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
        renderPassCreateInfo.pDependencies = dependencies.data();

        auto result = this->vk.CreateRenderPass(this->vk.device, &renderPassCreateInfo, this->settings.deviceAllocationCallbacks, &this->toFullScreenQuadRenderPass);
        if (FINJIN_CHECK_VKRESULT_FAILED(result))
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create full screen quad render pass. Vulkan result: %1%", VulkanResult::ToString(result)));
            return;
        }
    }

    //Create full screen shader resources
    {
        //Read shaders
        size_t shaderFileIndex = 0;
        for (auto shaderFileName : { "common-shaders-fullscreen-quad.vert.spv", "common-shaders-fullscreen-quad.frag.spv" })
        {
            this->workingAssetReference.ForUriString(shaderFileName, this->workingUri, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to parse common shaders asset reference.");
                return;
            }

            auto readResult = this->shaderAssetClassFileReader.ReadAsset(this->readBuffer, this->workingAssetReference);
            if (readResult != FileOperationResult::SUCCESS)
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read shader file '%1%'.", shaderFileName));
                return;
            }

            this->commonResources.fullScreenShaders[shaderFileIndex].Create(this->vk, this->settings.deviceAllocationCallbacks, GetAllocator(), shaderFileName, this->readBuffer.data(), this->readBuffer.size(), true, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create shader for file '%1%'.", shaderFileName));
                return;
            }

            shaderFileIndex++;
        }

        //Full screen descriptor set layout and pool
        {
            VulkanDescriptorSetLayoutBuilder texturesDescriptorSetLayoutBuilder;
            texturesDescriptorSetLayoutBuilder.AddBinding(FullScreenQuadBindingIndex::DEFAULT_SAMPLER, FULL_SCREEN_QUAD_TEXTURE_SAMPLER_BINDING_ID, VK_DESCRIPTOR_TYPE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
            texturesDescriptorSetLayoutBuilder.AddBinding(FullScreenQuadBindingIndex::TEXTURES_START, FULL_SCREEN_QUAD_TEXTURES_START_BINDING_ID, VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 60, VK_SHADER_STAGE_FRAGMENT_BIT);

            //Layout
            texturesDescriptorSetLayoutBuilder.CreateDescriptorSetLayout(&this->commonResources.fullScreenQuadDescriptorSetLayout, this->vk, this->settings.deviceAllocationCallbacks, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to create full screen descriptor set layout.");
                return;
            }

            //Pool
            texturesDescriptorSetLayoutBuilder.CreateDescriptorPool(&this->commonResources.fullScreenQuadDescriptorPool, 1, this->vk, this->settings.deviceAllocationCallbacks, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to create full screen descriptor pool.");
                return;
            }
        }

        //Full screen descriptor set
        {
            VulkanDescriptorSetAllocateInfo fullScreenQuadDescriptorSetAllocateInfo(this->commonResources.fullScreenQuadDescriptorPool);
            fullScreenQuadDescriptorSetAllocateInfo.descriptorSetCount = 1;
            fullScreenQuadDescriptorSetAllocateInfo.pSetLayouts = &this->commonResources.fullScreenQuadDescriptorSetLayout;
            auto result = this->vk.AllocateDescriptorSets(this->vk.device, &fullScreenQuadDescriptorSetAllocateInfo, &this->commonResources.fullScreenQuadDescriptorSet);
            if (result != VK_SUCCESS)
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to allocate full screen quad descriptor set. Vulkan result: %1%", VulkanResult::ToString(result)));
                return;
            }
        }

        //Full screen pipeline layout
        {
            VulkanPipelineLayoutCreateInfo pipelineLayoutCreateInfo;
            pipelineLayoutCreateInfo.setLayoutCount = 1;
            pipelineLayoutCreateInfo.pSetLayouts = &this->commonResources.fullScreenQuadDescriptorSetLayout;
            auto result = this->vk.CreatePipelineLayout(this->vk.device, &pipelineLayoutCreateInfo, this->settings.deviceAllocationCallbacks, &this->commonResources.fullScreenQuadPipelineLayout);
            if (result != VK_SUCCESS)
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create full screen quad pipeline layout. Vulkan result: %1%", VulkanResult::ToString(result)));
                return;
            }
        }

        //Create pipeline
        {
            VulkanGraphicsPipelineCreateInfoBuilder graphicsPipelineCreateInfoBuilder;
            graphicsPipelineCreateInfoBuilder.SetDefaults();

            {
                auto& rasterizationState = graphicsPipelineCreateInfoBuilder.UseRasterizationState();
                rasterizationState.cullMode = VK_CULL_MODE_NONE;
            }
            {
                auto& depthStencilState = graphicsPipelineCreateInfoBuilder.UseDepthStencilState();
                depthStencilState.depthTestEnable = VK_FALSE;
                depthStencilState.depthWriteEnable = VK_FALSE;
            }
            {
                auto& inputAssemblyState = graphicsPipelineCreateInfoBuilder.UseInputAssemblyState();
                inputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
            }

            size_t shaderIndex = 0;
            for (auto shaderStageFlag : { VK_SHADER_STAGE_VERTEX_BIT, VK_SHADER_STAGE_FRAGMENT_BIT })
            {
                auto& shaderStage = graphicsPipelineCreateInfoBuilder.AddShaderStage();
                shaderStage.pName = FINJIN_VULKAN_DEFAULT_SHADER_ENTRY_POINT_NAME;
                shaderStage.module = this->commonResources.fullScreenShaders[shaderIndex].shaderModule;
                shaderStage.stage = shaderStageFlag;
                shaderIndex++;
            }

            graphicsPipelineCreateInfoBuilder.graphicsPipelineCreateInfo.layout = this->commonResources.fullScreenQuadPipelineLayout;
            graphicsPipelineCreateInfoBuilder.graphicsPipelineCreateInfo.renderPass = this->toFullScreenQuadRenderPass;

            graphicsPipelineCreateInfoBuilder.CreatePipeline(&this->commonResources.fullScreenQuadPipeline, this->commonResources.pipelineCache, this->vk, this->settings.deviceAllocationCallbacks, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to create full screen pipeline");
                return;
            }
        }
    }

    if (this->settings.renderTargetSize.GetType() == GpuRenderTargetSizeType::EXPLICIT_SIZE)
    {
        CreateNonSwapChainFrameBufferObjects(error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to create non-swap chain frame buffer objects.");
            return;
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

    auto frameBufferIndex = this->currentFrameBufferIndex++ % this->frameBuffers.size();

    //Wait for previous frame to complete
    auto& frameBuffer = this->frameBuffers[frameBufferIndex];
    frameBuffer.WaitForNotInUse();
    frameStage.frameBufferIndex = frameBufferIndex;

    //Update state
    frameStage.elapsedTime = elapsedTime;
    frameStage.totalElapsedTime = totalElapsedTime;
    frameStage.sequenceIndex = this->sequenceIndex++;

    return frameStage;
}

void VulkanGpuContextImpl::PresentFrameStage(VulkanFrameStage& frameStage, RenderStatus renderStatus, size_t presentSyncIntervalOverride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto& frameBuffer = this->frameBuffers[frameStage.frameBufferIndex];

    if (renderStatus.SuccessRequired())
    {
        //Get swap chain buffer
        uint32_t swapChainFrameBufferIndex;
        auto result = this->vk.AcquireNextImageKHR(this->vk.device, this->swapChain, UINT64_MAX, this->presentCompleteSemaphore, VK_NULL_HANDLE, &swapChainFrameBufferIndex);
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
                frameBuffer.ResetCommandBuffers();

                FINJIN_DEBUG_LOG_INFO("Out of date swap chain frame.");
                assert(0 && "Out of date swap chain frame.");
                break;
            }
            default:
            {
                frameBuffer.ResetCommandBuffers();

                FINJIN_DEBUG_LOG_INFO("Failed to get swap chain frame. Vulkan result: %1%", VulkanResult::ToString(result));
                assert(0 && "Failed to get swap chain frame.");
                break;
            }
        }

        auto& swapChainFrameBuffer = this->swapChainFrameBuffers[swapChainFrameBufferIndex];

        {
            EnumArray<FullScreenQuadAttachmentIndex, FullScreenQuadAttachmentIndex::COUNT, VkClearValue> clearValues;
            {
                auto& clearColor = this->clearColor;

                auto& colorClearValue = clearValues[FullScreenQuadAttachmentIndex::COLOR];
                colorClearValue.color.float32[0] = clearColor(0);
                colorClearValue.color.float32[1] = clearColor(1);
                colorClearValue.color.float32[2] = clearColor(2);
                colorClearValue.color.float32[3] = clearColor(3);
            }

            VulkanRenderPassBeginInfo renderPassBeginInfo;
            renderPassBeginInfo.renderPass = this->toFullScreenQuadRenderPass;
            renderPassBeginInfo.framebuffer = swapChainFrameBuffer.vkframeBuffer;
            renderPassBeginInfo.renderArea = this->windowScissorRect;
            renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
            renderPassBeginInfo.pClearValues = clearValues.data();

            //Encode commands to draw the off screen render target onto a full screen quad on the back buffer----------------
            auto commandBufferHandle = frameBuffer.NewGraphicsCommandBuffer(this->vk, error);
            if (error)
            {
                frameBuffer.ResetCommandBuffers();

                FINJIN_SET_ERROR(error, "Failed to get command buffer.");
                return;
            }

            auto commandBuffer = commandBufferHandle->commandBuffer;

            this->vk.CmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

            this->vk.CmdSetViewport(commandBuffer, 0, 1, &this->windowViewport);

            this->vk.CmdSetScissor(commandBuffer, 0, 1, &this->windowScissorRect);

            this->vk.CmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, this->commonResources.fullScreenQuadPipeline);
            this->vk.CmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, this->commonResources.fullScreenQuadPipelineLayout, 0, 1, &this->commonResources.textureDescriptorSet, 0, nullptr);
            {
                uint32_t pushConstantOffsetInBytes = 0;

                MathMatrix4Values vulkanMatrix;
                GetColumnOrderMatrixData(vulkanMatrix, this->commonResources.clipInvertMatrix);
                this->vk.CmdPushConstants(commandBuffer, this->commonResources.fullScreenQuadPipelineLayout, VK_SHADER_STAGE_ALL, pushConstantOffsetInBytes, sizeof(vulkanMatrix), &vulkanMatrix);
                pushConstantOffsetInBytes += sizeof(vulkanMatrix);

                uint32_t textureIndex[4] = { static_cast<uint32_t>(frameBuffer.index), 0, 0, 0 };
                this->vk.CmdPushConstants(commandBuffer, this->commonResources.fullScreenQuadPipelineLayout, VK_SHADER_STAGE_ALL, pushConstantOffsetInBytes, sizeof(textureIndex), textureIndex);
                pushConstantOffsetInBytes += sizeof(textureIndex);

                float multiplyColor[4] = { 1, 1, 1, 1 };
                this->vk.CmdPushConstants(commandBuffer, this->commonResources.fullScreenQuadPipelineLayout, VK_SHADER_STAGE_ALL, pushConstantOffsetInBytes, sizeof(multiplyColor), multiplyColor);
                pushConstantOffsetInBytes += sizeof(multiplyColor);

                float addColor[4] = { 0, 0, 0, 0 };
                this->vk.CmdPushConstants(commandBuffer, this->commonResources.fullScreenQuadPipelineLayout, VK_SHADER_STAGE_ALL, pushConstantOffsetInBytes, sizeof(addColor), addColor);
                pushConstantOffsetInBytes += sizeof(addColor);
            }

            this->vk.CmdDraw(commandBuffer, 4, 1, 0, 0);

            this->vk.CmdEndRenderPass(commandBuffer);

            if ((this->settings.screenCaptureFrequency.actual == ScreenCaptureFrequency::EVERY_FRAME ||
                (this->settings.screenCaptureFrequency.actual == ScreenCaptureFrequency::ON_REQUEST && frameBuffer.screenCaptureRequested)) &&
                frameBuffer.screenCaptureBuffer.mappedMemory != nullptr)
            {
                VkBufferImageCopy bufferImageCopy = {};
                bufferImageCopy.bufferRowLength = 0;// this->renderTargetSize[0] * this->frameBufferScreenCaptureColorFormatBytesPerPixel;
                bufferImageCopy.bufferImageHeight = 0;// this->renderTargetSize[1];
                bufferImageCopy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                bufferImageCopy.imageSubresource.baseArrayLayer = 0;
                bufferImageCopy.imageSubresource.layerCount = 1;
                bufferImageCopy.imageSubresource.mipLevel = 0;
                bufferImageCopy.imageExtent.width = this->renderTargetSize[0];
                bufferImageCopy.imageExtent.height = this->renderTargetSize[1];
                bufferImageCopy.imageExtent.depth = 1;
                this->vk.CmdCopyImageToBuffer(commandBuffer, frameBuffer.renderTarget.colorOutputs[0].image, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, frameBuffer.screenCaptureBuffer.buffer, 1, &bufferImageCopy);
                
                frameBuffer.screenCaptureSize = this->renderTargetSize;
            }
            frameBuffer.screenCaptureRequested = false;

            auto result = this->vk.EndCommandBuffer(commandBuffer);
            if (FINJIN_CHECK_VKRESULT_FAILED(result))
            {
                frameBuffer.ResetCommandBuffers();

                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to close command buffer. Vulkan result: %1%", VulkanResult::ToString(result)));
                return;
            }

            frameBuffer.SubmitCommandBuffers(this->vk, this->primaryQueues.graphics, this->presentCompleteSemaphore, frameBuffer.renderingCompleteSemaphore, error);
            if (error)
            {
                frameBuffer.ResetCommandBuffers();
                assert(0);
            }
        }

        VulkanPresentInfoKHR presentInfo;
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &this->swapChain;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = &frameBuffer.renderingCompleteSemaphore;
        presentInfo.pImageIndices = &swapChainFrameBufferIndex;

        result = this->vk.QueuePresentKHR(this->primaryQueues.present, &presentInfo);
        frameBuffer.ResetCommandBuffers();
        if (FINJIN_CHECK_VKRESULT_FAILED(result))
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to present back buffer. Vulkan result: %1%", VulkanResult::ToString(result)));
            return;
        }
    }
    else
    {
        //No rendering required
        frameBuffer.WaitForCommandBuffers(this->vk);
        frameBuffer.ResetCommandBuffers();
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
            case GpuCommand::Type::START_GRAPHICS_COMMANDS:
            {
                StartGraphicsCommands(frameStage, error);
                if (error)
                {
                    FINJIN_SET_ERROR(error, "Failed to start command list.");
                    return;
                }
                break;
            }
            case GpuCommand::Type::FINISH_GRAPHICS_COMMANDS:
            {
                FinishGraphicsCommands(frameStage, error);
                if (error)
                {
                    FINJIN_SET_ERROR(error, "Failed to finish command list.");
                    return;
                }
                break;
            }
            case GpuCommand::Type::EXECUTE_GRAPHICS_COMMANDS:
            {
                frameBuffer.SubmitCommandBuffers(this->vk, this->primaryQueues.graphics, VK_NULL_HANDLE, VK_NULL_HANDLE, error);
                if (error)
                {
                    FINJIN_SET_ERROR(error, "Failed to submit primary queue.");
                    return;
                }
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

                if (lastRenderTarget == GetRenderTarget(frameStage, Utf8String::GetEmpty()))
                {
                    //Main render target finished
                    auto commandBufferHandle = frameBuffer.GetCurrentGraphicsCommandBuffer();
                    assert(commandBufferHandle != nullptr);

                    auto& frameBuffer = this->frameBuffers[frameStage.frameBufferIndex];
                    this->staticMeshRenderer.UpdatePassAndMaterialConstants(frameStage.staticMeshRendererFrameState, frameStage.elapsedTime, frameStage.totalElapsedTime);
                    this->staticMeshRenderer.RenderQueued(frameStage.staticMeshRendererFrameState, commandBufferHandle->commandBuffer);
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
                auto commandBufferHandle = frameBuffer.GetCurrentGraphicsCommandBuffer();
                assert(commandBufferHandle != nullptr);
                UploadMesh(commandBufferHandle->commandBuffer, meshAsset, error);
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
                auto commandBufferHandle = frameBuffer.GetCurrentGraphicsCommandBuffer();
                assert(commandBufferHandle != nullptr);
                UploadTexture(commandBufferHandle->commandBuffer, textureAsset);
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
                auto commandBufferHandle = frameBuffer.GetCurrentGraphicsCommandBuffer();
                assert(commandBufferHandle != nullptr);
                auto materialResource = CreateMaterial(commandBufferHandle->commandBuffer, materialAsset, error);
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

                this->staticMeshRenderer.RenderEntity(frameStage.staticMeshRendererFrameState, command.sceneNodeState, command.entity, command.shaderFeatureFlags, command.sortedLights, command.ambientLight);

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
            case GpuCommand::Type::CAPTURE_SCREEN:
            {
                auto& command = static_cast<CaptureScreenGpuCommand&>(baseCommand);

                auto& frameBuffer = this->frameBuffers[frameStage.frameBufferIndex];
                frameBuffer.screenCaptureRequested = true;

                if (command.eventInfo.HasEventID())
                {
                    if (!events.push_back())
                    {
                        FINJIN_SET_ERROR(error, "Failed to record 'capture screen' GPU event.");
                        return;
                    }

                    auto& event = events.back();
                    event.type = GpuEvent::Type::CAPTURE_SCREEN_REQUESTED;
                    event.eventInfo = command.eventInfo;
                }

                break;
            }
        }
    }
}

void VulkanGpuContextImpl::FlushGpu()
{
    if (this->vk.device != VK_NULL_HANDLE)
        this->vk.DeviceWaitIdle(this->vk.device);
}

void VulkanGpuContextImpl::CreateScreenSizeDependentResources(Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    //Swap chain
    CreateSwapChain(error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to create swap chain.");
        return;
    }

    if (this->settings.renderTargetSize.GetType() == GpuRenderTargetSizeType::WINDOW_SIZE)
    {
        auto maxRenderTargetSize = this->settings.renderTargetSize.EvaluateMax(nullptr, &this->windowPixelBounds);

        this->tempGraphicsWork.Start(this->vk, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to start work for creating resources.");
            return;
        }

        for (size_t frameBufferIndex = 0; frameBufferIndex < this->frameBuffers.size(); frameBufferIndex++)
        {
            auto& frameBuffer = this->frameBuffers[frameBufferIndex];

            //Color buffer
            frameBuffer.renderTarget.CreateColor
                (
                this->physicalDevice,
                this->physicalDeviceDescription,
                this->vulkanSystem->GetImpl()->vk,
                this->vk,
                this->settings.deviceAllocationCallbacks,
                this->tempGraphicsWork.commandBuffer,
                maxRenderTargetSize[0],
                maxRenderTargetSize[1],
                this->settings.colorFormat.actual,
                this->settings.multisampleCount.actual,
                true,
                error
                );
            if (error)
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create color render target for frame buffer %1%.", frameBufferIndex));
                return;
            }

            //Screen capture buffer
            if (this->settings.screenCaptureFrequency.actual != ScreenCaptureFrequency::NEVER)
            {
                auto byteCount = maxRenderTargetSize[0] * maxRenderTargetSize[1] * this->frameBufferScreenCaptureColorFormatBytesPerPixel;
                frameBuffer.CreateScreenCaptureBuffer(this->vk, this->settings.deviceAllocationCallbacks, this->physicalDeviceDescription, byteCount, true, error);
                if (error)
                {
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create screen capture buffer for frame buffer %1%.", frameBufferIndex));
                    return;
                }
            }
        }

        //Depth buffer
        if (this->settings.depthStencilFormat.actual != VK_FORMAT_UNDEFINED)
        {
            this->depthStencilRenderTarget.CreateDepthStencil
                (
                this->physicalDevice,
                this->physicalDeviceDescription,
                this->vulkanSystem->GetImpl()->vk,
                this->vk,
                this->settings.deviceAllocationCallbacks,
                this->tempGraphicsWork.commandBuffer,
                maxRenderTargetSize[0],
                maxRenderTargetSize[1],
                this->settings.depthStencilFormat.actual,
                this->settings.multisampleCount.actual,
                true,
                error
                );
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to create depth/stencil buffer.");
                return;
            }
        }

        this->tempGraphicsWork.Finish(this->vk, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to finish work for creating resources.");
            return;
        }

        CreateNonSwapChainFrameBufferObjects(error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to create non-swap chain frame buffer objects.");
            return;
        }
    }

    CreateSwapChainFrameBufferObjects(error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to create swap chain frame buffer objects.");
        return;
    }
}

void VulkanGpuContextImpl::DestroyScreenSizeDependentResources(bool resizing)
{
    for (auto& swapChainFrameBuffer : this->swapChainFrameBuffers)
        swapChainFrameBuffer.Destroy(this->vk, this->settings.deviceAllocationCallbacks);

    if (this->swapChain != VK_NULL_HANDLE)
    {
        this->vk.DestroySwapchainKHR(this->vk.device, this->swapChain, this->settings.deviceAllocationCallbacks);
        this->swapChain = VK_NULL_HANDLE;
    }

    for (auto& frameBuffer : this->frameBuffers)
        frameBuffer.DestroyScreenSizeDependentResources(this->vk, this->settings.deviceAllocationCallbacks);

    this->depthStencilRenderTarget.DestroyScreenSizeDependentResources(this->vk, this->settings.deviceAllocationCallbacks);

    this->currentFrameBufferIndex = 0;
    this->sequenceIndex = 0;
}

void VulkanGpuContextImpl::WindowResized(OSWindow* osWindow)
{
    UpdatedCachedWindowSize();
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
        textureWriteDescriptorSet.dstSet = this->commonResources.textureDescriptorSet;
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

void* VulkanGpuContextImpl::CreateLightFromMainThread(FinjinSceneObjectLight* light, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    std::unique_lock<FiberSpinLock> thisLock(this->createLock);

    auto vulkanLight = this->commonResources.lights.Use();
    if (vulkanLight == nullptr)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to add light '%1%' to collection.", light->name));
        return nullptr;
    }

    vulkanLight->finjinLight = light;

    light->gpuLight = vulkanLight;

    return vulkanLight;
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

ScreenCaptureResult VulkanGpuContextImpl::GetScreenCapture(ScreenCapture& screenCapture, VulkanFrameStage& frameStage)
{
    auto& frameBuffer = this->frameBuffers[frameStage.frameBufferIndex];

    if (frameBuffer.screenCaptureSize[0] > 0 && frameBuffer.screenCaptureSize[1] > 0 &&
        this->frameBufferScreenCapturePixelFormat != ScreenCapturePixelFormat::NONE)
    {
        screenCapture.image = frameBuffer.screenCaptureBuffer.mappedMemory;
        screenCapture.pixelFormat = this->frameBufferScreenCapturePixelFormat;
        screenCapture.rowStride = frameBuffer.screenCaptureSize[0] * this->frameBufferScreenCaptureColorFormatBytesPerPixel;
        screenCapture.width = frameBuffer.screenCaptureSize[0];
        screenCapture.height = frameBuffer.screenCaptureSize[1];

        frameBuffer.screenCaptureSize[0] = frameBuffer.screenCaptureSize[1] = 0;

        return ScreenCaptureResult::SUCCESS;
    }

    return ScreenCaptureResult::NOT_AVAILABLE;
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
        auto foundAt = this->materialMapTypeToElements.find(map.typeName);
        assert(foundAt != this->materialMapTypeToElements.end());
        if (foundAt != this->materialMapTypeToElements.end())
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

void VulkanGpuContextImpl::UploadTexture(VkCommandBuffer commandBuffer, FinjinTexture* texture)
{
    auto vulkanTexture = static_cast<VulkanTexture*>(texture->gpuTexture);

    vulkanTexture->Upload(this->vk, commandBuffer);

    vulkanTexture->isResidentCountdown = this->settings.frameBufferCount.actual + 1;
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

VulkanRenderTarget* VulkanGpuContextImpl::GetRenderTarget(VulkanFrameStage& frameStage, const Utf8String& name)
{
    auto& frameBuffer = this->frameBuffers[frameStage.frameBufferIndex];

    if (name.empty())
        return &frameBuffer.renderTarget;

    return nullptr;
}

void VulkanGpuContextImpl::StartGraphicsCommands(VulkanFrameStage& frameStage, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto& frameBuffer = this->frameBuffers[frameStage.frameBufferIndex];

    auto commandBufferHandle = frameBuffer.NewGraphicsCommandBuffer(this->vk, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to get a graphics command buffer.");
        return;
    }
}

void VulkanGpuContextImpl::FinishGraphicsCommands(VulkanFrameStage& frameStage, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto& frameBuffer = this->frameBuffers[frameStage.frameBufferIndex];

    //End command buffer
    auto commandBufferHandle = frameBuffer.GetCurrentGraphicsCommandBuffer();
    auto result = this->vk.EndCommandBuffer(commandBufferHandle->commandBuffer);
    if (FINJIN_CHECK_VKRESULT_FAILED(result))
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to close command buffer. Vulkan result: %1%", VulkanResult::ToString(result)));
        return;
    }
}

void VulkanGpuContextImpl::StartRenderTarget(VulkanFrameStage& frameStage, VulkanRenderTarget* renderTarget, StaticVector<VulkanRenderTarget*, EngineConstants::MAX_RENDER_TARGET_DEPENDENCIES>& dependentRenderTargets, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto& frameBuffer = this->frameBuffers[frameStage.frameBufferIndex];

    //Hold onto render targets for FinishRenderTarget()----------------------
    renderTarget->defaultViewport.resize(1);
    renderTarget->defaultViewport[0] = this->renderViewport;

    renderTarget->defaultScissorRect.resize(1);
    renderTarget->defaultScissorRect[0] = this->renderScissorRect;

    frameStage.renderTarget = renderTarget;
    frameStage.dependentRenderTargets = dependentRenderTargets;

    //Get command buffer---------------------
    auto commandBufferHandle = frameBuffer.GetCurrentGraphicsCommandBuffer();
    auto commandBuffer = commandBufferHandle->commandBuffer;

    //Clear render target and depth stencil----------------------
    EnumArray<SceneRenderAttachmentIndex, SceneRenderAttachmentIndex::COUNT, VkClearValue> clearValues;
    {
        auto& clearColor = renderTarget->clearColor.IsSet() ? renderTarget->clearColor.value : this->clearColor;

        auto& colorClearValue = clearValues[SceneRenderAttachmentIndex::COLOR];
        colorClearValue.color.float32[0] = clearColor(0);
        colorClearValue.color.float32[1] = clearColor(1);
        colorClearValue.color.float32[2] = clearColor(2);
        colorClearValue.color.float32[3] = clearColor(3);
    }
    {
        auto& depthClearValue = clearValues[SceneRenderAttachmentIndex::DEPTH];
        depthClearValue.depthStencil.depth = 1.0f;
        depthClearValue.depthStencil.stencil = 0;
    }

    VulkanRenderPassBeginInfo renderPassBeginInfo;
    renderPassBeginInfo.renderPass = this->sceneRenderPass;
    renderPassBeginInfo.framebuffer = renderTarget->vkframeBuffer;
    renderPassBeginInfo.renderArea = this->renderScissorRect;
    renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassBeginInfo.pClearValues = clearValues.data();

    this->vk.CmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    //Set viewport(s)----------------------
    VkViewport* viewports = nullptr;
    size_t viewportCount = 0;
    if (!renderTarget->viewports.empty())
    {
        viewports = renderTarget->viewports.data();
        viewportCount = renderTarget->viewports.size();
    }
    else
    {
        viewports = renderTarget->defaultViewport.data();
        viewportCount = renderTarget->defaultViewport.size();
    }
    this->vk.CmdSetViewport(commandBuffer, 0, static_cast<uint32_t>(viewportCount), viewports);

    //Set scissor rectangle(s)----------------------
    VkRect2D* scissorRects = nullptr;
    size_t scissorRectCount = 0;
    if (!renderTarget->scissorRects.empty())
    {
        scissorRects = renderTarget->scissorRects.data();
        scissorRectCount = renderTarget->scissorRects.size();
    }
    else
    {
        scissorRects = renderTarget->defaultScissorRect.data();
        scissorRectCount = renderTarget->defaultScissorRect.size();
    }
    this->vk.CmdSetScissor(commandBuffer, 0, static_cast<uint32_t>(scissorRectCount), scissorRects);
}

void VulkanGpuContextImpl::FinishRenderTarget(VulkanFrameStage& frameStage, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto& frameBuffer = this->frameBuffers[frameStage.frameBufferIndex];

    //auto renderTarget = frameStage.renderTarget;

    auto commandBufferHandle = frameBuffer.GetCurrentGraphicsCommandBuffer();
    auto commandBuffer = commandBufferHandle->commandBuffer;

    this->vk.CmdEndRenderPass(commandBuffer);
}

VulkanFrameBuffer& VulkanGpuContextImpl::GetFrameBuffer(size_t index)
{
    return this->frameBuffers[index];
}

void VulkanGpuContextImpl::CreateSwapChain(Error& error)
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
    swapChainInfo.imageExtent.width = static_cast<uint32_t>(this->windowPixelBounds.GetClientWidth());
    swapChainInfo.imageExtent.height = static_cast<uint32_t>(this->windowPixelBounds.GetClientHeight());
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
    for (auto& swapChainFrameBuffer : this->swapChainFrameBuffers)
        swapChainFrameBuffer.Destroy(this->vk, this->settings.deviceAllocationCallbacks);
    if (oldSwapChain != VK_NULL_HANDLE)
    {
        this->vk.DestroySwapchainKHR(this->vk.device, oldSwapChain, this->settings.deviceAllocationCallbacks);
        oldSwapChain = VK_NULL_HANDLE;
    }
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

        this->vk.DestroySwapchainKHR(this->vk.device, this->swapChain, this->settings.deviceAllocationCallbacks);
        this->swapChain = VK_NULL_HANDLE;

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

    StaticVector<VkImage, VulkanSwapChainFrameBuffer::MAX_SWAP_CHAIN_IMAGES> swapChainImages;
    swapChainImages.resize(swapChainImageCount);
    result = this->vk.GetSwapchainImagesKHR(this->vk.device, this->swapChain, &swapChainImageCount, swapChainImages.data());
    if (FINJIN_CHECK_VKRESULT_FAILED(result))
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to get swap chain image count. Vulkan result: %1%", VulkanResult::ToString(result)));
        return;
    }

    this->swapChainFrameBuffers.resize(swapChainImageCount);
    for (size_t frameBufferIndex = 0; frameBufferIndex < this->swapChainFrameBuffers.size(); frameBufferIndex++)
    {
        auto& swapChainFrameBuffer = this->swapChainFrameBuffers[frameBufferIndex];

        swapChainFrameBuffer.SetImageAndCreateView(this->vk, this->settings.deviceAllocationCallbacks, swapChainImages[frameBufferIndex], this->settings.colorFormat.actual, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create swap chain frame buffer for frame buffer %1%.", frameBufferIndex));
            return;
        }
    }
}

void VulkanGpuContextImpl::UpdatedCachedWindowSize()
{
    this->windowPixelBounds = this->settings.osWindow->GetWindowSize().GetSafeCurrentBounds();
    this->windowPixelBounds.Scale(this->settings.osWindow->GetDisplayDensity());

    this->windowViewport.x = 0;
    this->windowViewport.y = 0;
    this->windowViewport.width = RoundToFloat(this->windowPixelBounds.GetClientWidth());
    this->windowViewport.height = RoundToFloat(this->windowPixelBounds.GetClientHeight());
    this->windowViewport.minDepth = 0;
    this->windowViewport.maxDepth = this->settings.maxDepthValue;

    this->windowScissorRect.offset.x = 0;
    this->windowScissorRect.offset.y = 0;
    this->windowScissorRect.extent.width = this->windowPixelBounds.GetClientWidth();
    this->windowScissorRect.extent.height = this->windowPixelBounds.GetClientHeight();


    this->renderTargetSize = this->settings.renderTargetSize.Evaluate(nullptr, &this->windowPixelBounds);

    this->renderViewport.x = 0;
    this->renderViewport.y = 0;
    this->renderViewport.width = RoundToFloat(this->renderTargetSize[0]);
    this->renderViewport.height = RoundToFloat(this->renderTargetSize[1]);
    this->renderViewport.minDepth = 0;
    this->renderViewport.maxDepth = this->settings.maxDepthValue;

    this->renderScissorRect.offset.x = 0;
    this->renderScissorRect.offset.y = 0;
    this->renderScissorRect.extent.width = this->renderTargetSize[0];
    this->renderScissorRect.extent.height = this->renderTargetSize[1];
}

void VulkanGpuContextImpl::CreateSwapChainFrameBufferObjects(Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    EnumArray<FullScreenQuadAttachmentIndex, FullScreenQuadAttachmentIndex::COUNT, VkImageView> imageViewAttachments;

    VulkanFramebufferCreateInfo frameBufferCreateInfo;
    frameBufferCreateInfo.renderPass = this->toFullScreenQuadRenderPass;
    frameBufferCreateInfo.attachmentCount = static_cast<uint32_t>(imageViewAttachments.size());
    frameBufferCreateInfo.pAttachments = imageViewAttachments.data();
    frameBufferCreateInfo.width = static_cast<uint32_t>(this->windowPixelBounds.GetClientWidth());
    frameBufferCreateInfo.height = static_cast<uint32_t>(this->windowPixelBounds.GetClientHeight());
    frameBufferCreateInfo.layers = 1;

    for (size_t frameBufferIndex = 0; frameBufferIndex < this->swapChainFrameBuffers.size(); frameBufferIndex++)
    {
        auto& swapChainFrameBuffer = this->swapChainFrameBuffers[frameBufferIndex];

        swapChainFrameBuffer.CreateFrameBuffer(this->vk, this->settings.deviceAllocationCallbacks, frameBufferCreateInfo, (size_t)FullScreenQuadAttachmentIndex::COLOR, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create frame buffer object for swap chain frame buffer '%1%'.", frameBufferIndex));
            return;
        }
    }
}

void VulkanGpuContextImpl::CreateNonSwapChainFrameBufferObjects(Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    EnumArray<SceneRenderAttachmentIndex, SceneRenderAttachmentIndex::COUNT, VkImageView> imageViewAttachments;

    imageViewAttachments[SceneRenderAttachmentIndex::DEPTH] = this->depthStencilRenderTarget.depthStencilResource.view;

    VulkanFramebufferCreateInfo frameBufferCreateInfo;
    frameBufferCreateInfo.renderPass = this->sceneRenderPass;
    frameBufferCreateInfo.attachmentCount = static_cast<uint32_t>(imageViewAttachments.size());
    frameBufferCreateInfo.pAttachments = imageViewAttachments.data();
    frameBufferCreateInfo.width = this->renderTargetSize[0];
    frameBufferCreateInfo.height = this->renderTargetSize[1];
    frameBufferCreateInfo.layers = 1;

    for (size_t frameBufferIndex = 0; frameBufferIndex < this->frameBuffers.size(); frameBufferIndex++)
    {
        auto& frameBuffer = this->frameBuffers[frameBufferIndex];

        frameBuffer.renderTarget.CreateFrameBuffer(this->vk, this->settings.deviceAllocationCallbacks, frameBufferCreateInfo, (size_t)SceneRenderAttachmentIndex::COLOR, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create frame buffer object for frame buffer render target '%1%'.", frameBufferIndex));
            return;
        }

        //Set texture into texture descriptor set
        {
            VkDescriptorImageInfo descriptorTextureImageInfo = {};
            descriptorTextureImageInfo.imageView = frameBuffer.renderTarget.colorOutputs[0].view;
            descriptorTextureImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

            VulkanWriteDescriptorSet textureWriteDescriptorSet;
            textureWriteDescriptorSet.dstSet = this->commonResources.textureDescriptorSet;
            textureWriteDescriptorSet.dstBinding = TEXTURES_START_BINDING_ID;
            textureWriteDescriptorSet.dstArrayElement = frameBufferIndex;
            textureWriteDescriptorSet.descriptorCount = 1;
            textureWriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
            textureWriteDescriptorSet.pImageInfo = &descriptorTextureImageInfo;
            this->vk.UpdateDescriptorSets(this->vk.device, 1, &textureWriteDescriptorSet, 0, nullptr);
        }
    }
}

#endif
