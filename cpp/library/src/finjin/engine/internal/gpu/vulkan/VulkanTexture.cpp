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

#include "VulkanTexture.hpp"
#include "VulkanUtilities.hpp"

using namespace Finjin::Engine;


//Implementation----------------------------------------------------------------
VulkanTexture::VulkanTexture(Allocator* allocator) : name(allocator)
{
    Reset();
}

void VulkanTexture::CreateFromPNG
    (
    PNGReader& reader,
    const void* bytes,
    size_t byteCount,
    VulkanInstanceFunctions& instanceVk,
    VkPhysicalDevice physicalDevice,
    VulkanDeviceFunctions& deviceVk,
    VkAllocationCallbacks* allocationCallbacks,
    const VulkanGpuDescription& gpuDescription,
    uint32_t* queueFamilyIndices,
    size_t queueFamilyIndexCount,
    Error& error
    )
{
    FINJIN_ERROR_METHOD_START(error);

    //Determine pixel format
    VkFormat format = VK_FORMAT_UNDEFINED;
    auto pngPixelFormat = reader.GetPixelFormat();
    if (pngPixelFormat == PNGReader::PixelFormat::RGBA)
    {
        if (reader.GetBytesPerChannel() == 1)
        {
            if (reader.IsSRGB())
                format = VK_FORMAT_R8G8B8A8_SRGB;
            else
                format = VK_FORMAT_R8G8B8A8_UNORM;
        }
        else if (reader.GetBytesPerChannel() == 2)
            format = VK_FORMAT_R16G16B16A16_UNORM;
        else
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Unsupported RGBA bytes per component '%1%'.", reader.GetBytesPerChannel()));
            return;
        }
    }
    else if (pngPixelFormat == PNGReader::PixelFormat::GRAY)
    {
        if (reader.GetBytesPerChannel() == 1)
            format = VK_FORMAT_R8_UNORM;
        else if (reader.GetBytesPerChannel() == 2)
            format = VK_FORMAT_R16_UNORM;
        else
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Unsupported gray bytes per component '%1%'.", reader.GetBytesPerChannel()));
            return;
        }
    }
    else if (pngPixelFormat == PNGReader::PixelFormat::GA)
    {
        if (reader.GetBytesPerChannel() == 1)
            format = VK_FORMAT_R8G8_UNORM;
        else if (reader.GetBytesPerChannel() == 2)
            format = VK_FORMAT_R16G16_UNORM;
        else
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Unsupported gray/alpha bytes per component '%1%'.", reader.GetBytesPerChannel()));
            return;
        }
    }
    else
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Invalid PNG pixel format '%1%'.", pngPixelFormat));
        return;
    }

    //Create
    CreateFromImage
        (
        format,
        VK_IMAGE_TYPE_2D,
        reader.GetWidth(),
        reader.GetHeight(),
        1,
        false,
        queueFamilyIndices,
        queueFamilyIndexCount,
        bytes,
        byteCount,
        reader.GetBytesPerRow(),
        instanceVk,
        physicalDevice,
        deviceVk,
        allocationCallbacks,
        gpuDescription,
        error
        );
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to create single mip.");
        return;
    }
}

void VulkanTexture::CreateFromASTC
    (
    ASTCReader& reader,
    const void* bytes,
    size_t byteCount,
    VulkanInstanceFunctions& instanceVk,
    VkPhysicalDevice physicalDevice,
    VulkanDeviceFunctions& deviceVk,
    VkAllocationCallbacks* allocationCallbacks,
    const VulkanGpuDescription& gpuDescription,
    uint32_t* queueFamilyIndices,
    size_t queueFamilyIndexCount,
    Error& error
    )
{
    FINJIN_ERROR_METHOD_START(error);

    auto& header = reader.GetHeader();

    auto& gpuFeatures = gpuDescription.features;
    if (!gpuFeatures.textureCompressionASTC_LDR)
    {
        FINJIN_SET_ERROR(error, "ASTC compression not supported.");
        return;
    }

    //Determine pixel format
    auto submode = ASTCCompressionSubmode::SRGB;
    auto format = VulkanUtilities::ASTCBlockSizeToVulkanFormat(header.blockdim_x, header.blockdim_y, header.blockdim_z, submode);
    if (format = VK_FORMAT_UNDEFINED)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Invalid KTX pixel format '%1%'.", format));
        return;
    }

    //Determine image type
    VkImageType imageType;
    if (header.blockdim_z == 1)
    {
        if (header.blockdim_y == 1)
            imageType = VK_IMAGE_TYPE_1D;
        else
            imageType = VK_IMAGE_TYPE_2D;
    }
    else
        imageType = VK_IMAGE_TYPE_3D;

    //Create
    CreateFromImage
        (
        format,
        imageType,
        header.GetWidth(),
        header.GetHeight(),
        header.GetDepth(),
        false,
        queueFamilyIndices,
        queueFamilyIndexCount,
        bytes,
        byteCount,
        0,
        instanceVk,
        physicalDevice,
        deviceVk,
        allocationCallbacks,
        gpuDescription,
        error
        );
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to create single mip.");
        return;
    }
}

void VulkanTexture::CreateFromKTX
    (
    KTXReader& reader,
    const void* bytes,
    size_t byteCount,
    VulkanInstanceFunctions& instanceVk,
    VkPhysicalDevice physicalDevice,
    VulkanDeviceFunctions& deviceVk,
    VkAllocationCallbacks* allocationCallbacks,
    const VulkanGpuDescription& gpuDescription,
    size_t maxDimension,
    uint32_t* queueFamilyIndices,
    size_t queueFamilyIndexCount,
    Error& error
    )
{
    FINJIN_ERROR_METHOD_START(error);

    VulkanImageCreateInfo imageCreateInfo;
    imageCreateInfo.format = VulkanUtilities::GLInternalFormatToVulkanFormat(reader.GetHeader().glInternalFormat);
    if (imageCreateInfo.format == VK_FORMAT_UNDEFINED)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Invalid KTX pixel format '%1%'.", reader.GetHeader().glInternalFormat));
        return;
    }

    if (VulkanUtilities::IsASTCFormat(imageCreateInfo.format) && !gpuDescription.features.textureCompressionASTC_LDR)
    {
        FINJIN_SET_ERROR(error, "ASTC compression not supported.");
        return;
    }
    else if (VulkanUtilities::IsETC2Format(imageCreateInfo.format) && !gpuDescription.features.textureCompressionETC2)
    {
        FINJIN_SET_ERROR(error, "ETC2 compression not supported.");
        return;
    }
    else if (VulkanUtilities::IsBCFormat(imageCreateInfo.format) && !gpuDescription.features.textureCompressionBC)
    {
        FINJIN_SET_ERROR(error, "BC compression not supported.");
        return;
    }

    //TODO: Implement this. Since ASTC is only supported by Tegra hardware, I'll have to wait until I move back to the mobile development and testing

    ByteBufferReader byteBuffer(bytes, byteCount);
    reader.ReadImages(byteBuffer, [maxDimension](const KTXReader* reader, uint32_t mipLevel, uint32_t arrayItemIndex, uint32_t faceIndex, uint32_t depthIndex, uint32_t width, uint32_t height, uint32_t imageByteCount, const void* imageBytes)
    {
        if (maxDimension == 0 || (width <= maxDimension && height <= maxDimension))
        {
            //std::cout << "KTX image width: " << width << ", height: " << height << std::endl;
        }
        return true;
    }, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to read all image levels.");
        return;
    }
}

void VulkanTexture::Destroy(VulkanDeviceFunctions& vk, VkAllocationCallbacks* allocationCallbacks)
{
    this->resource.Destroy(vk, allocationCallbacks);
    this->uploadResource.Destroy(vk, allocationCallbacks);

    this->textureData.Destroy();

    this->name.Destroy();

    Reset();
}

void VulkanTexture::HandleCreationFailure(VulkanDeviceFunctions& vk, VkAllocationCallbacks* allocationCallbacks)
{
    Destroy(vk, allocationCallbacks);
}

void VulkanTexture::ReleaseUploaders(VulkanDeviceFunctions& vk, VkAllocationCallbacks* allocationCallbacks)
{
    this->uploadResource.Destroy(vk, allocationCallbacks);
    this->textureData.Destroy();
}

bool VulkanTexture::IsResidentOnGpu() const
{
    return this->isResidentCountdown == 0;
}

void VulkanTexture::UpdateResidentOnGpuStatus(VulkanDeviceFunctions& vk, VkAllocationCallbacks* allocationCallbacks)
{
    if (this->isResidentCountdown > 0)
    {
        this->isResidentCountdown--;

        if (IsResidentOnGpu())
        {
            //Became resident
            ReleaseUploaders(vk, allocationCallbacks);
        }
    }
}

void VulkanTexture::Upload(VulkanDeviceFunctions& vk, VkCommandBuffer commandBuffer)
{
    if (this->needsUploader)
    {
        //Transition source upload image layout to VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL
        VulkanUtilities::SetImageLayout(vk, commandBuffer, this->uploadResource.image, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_PREINITIALIZED, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT);

        //Transition destination texture image layout to VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
        VulkanUtilities::SetImageLayout(vk, commandBuffer, this->resource.image, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_PREINITIALIZED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT);

        //Put the copy command into the command buffer
        vk.CmdCopyImage(commandBuffer, this->uploadResource.image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, this->resource.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, static_cast<uint32_t>(this->imageCopyInfos.size()), this->imageCopyInfos.data());

        //Transition destination texture image layout VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
        VulkanUtilities::SetImageLayout(vk, commandBuffer, this->resource.image, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, this->imageLayout, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);

        this->needsUploader = false;
    }
    else
    {
        VulkanUtilities::SetImageLayout(vk, commandBuffer, this->resource.image, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_PREINITIALIZED, this->imageLayout, VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
    }
}

void VulkanTexture::Reset()
{
    this->imageLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;
    this->textureIndex = 0;
    this->textureDimension = TextureDimension::DIMENSION_2D;
    this->needsUploader = false;
    this->isResidentCountdown = 0;
    this->waitingToBeResidentNext = nullptr;
    this->waitingToBeTransitionedNext = nullptr;
}

void VulkanTexture::CreateFromImage
    (
    VkFormat format,
    VkImageType imageType,
    size_t width,
    size_t height,
    size_t depth,
    bool isArray,
    uint32_t* queueFamilyIndices,
    size_t queueFamilyIndexCount,
    const void* bytes,
    size_t byteCount,
    size_t bytesPerRow,
    VulkanInstanceFunctions& instanceVk,
    VkPhysicalDevice physicalDevice,
    VulkanDeviceFunctions& deviceVk,
    VkAllocationCallbacks* allocationCallbacks,
    const VulkanGpuDescription& gpuDescription,
    Error& error
    )
{
    FINJIN_ERROR_METHOD_START(error);

    VkFormatProperties formatProps;
    instanceVk.GetPhysicalDeviceFormatProperties(physicalDevice, format, &formatProps);

    VkFormatFeatureFlags extraFeatures = 0;
    VkFormatFeatureFlags allFeatures = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | extraFeatures;
    this->needsUploader = ((formatProps.linearTilingFeatures & allFeatures) != allFeatures) ? true : false;

    VulkanImageCreateInfo imageCreateInfo;
    imageCreateInfo.imageType = imageType;
    imageCreateInfo.format = format;
    imageCreateInfo.extent.width = static_cast<uint32_t>(width);
    imageCreateInfo.extent.height = static_cast<uint32_t>(height);
    imageCreateInfo.extent.depth = static_cast<uint32_t>(depth);
    imageCreateInfo.mipLevels = 1;
    imageCreateInfo.arrayLayers = 1;
    imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageCreateInfo.tiling = VK_IMAGE_TILING_LINEAR;
    imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;
    imageCreateInfo.usage = this->needsUploader ? VK_IMAGE_USAGE_TRANSFER_SRC_BIT : VK_IMAGE_USAGE_SAMPLED_BIT;
    VkImageUsageFlags extraUsages = 0;
    imageCreateInfo.usage |= extraUsages;
    imageCreateInfo.queueFamilyIndexCount = static_cast<uint32_t>(queueFamilyIndexCount);
    imageCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
    imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    //Create a mappable image
    auto& mappableResource = this->needsUploader ? this->uploadResource : this->resource;
    auto result = deviceVk.CreateImage(deviceVk.device, &imageCreateInfo, allocationCallbacks, &mappableResource.image);
    if (result != VK_SUCCESS)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create image for mappable resource. Vulkan result: %1%", VulkanResult::ToString(result)));
        return;
    }

    //Find the memory type that is host mappable
    VkMemoryRequirements memReqs;
    deviceVk.GetImageMemoryRequirements(deviceVk.device, mappableResource.image, &memReqs);

    VulkanMemoryAllocateInfo memAllocInfo;
    memAllocInfo.allocationSize = memReqs.size;
    if (!gpuDescription.GetMemoryTypeIndexFromProperties(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, memAllocInfo.memoryTypeIndex))
    {
        FINJIN_SET_ERROR(error, "Failed to get memory type for mappable resource.");
        return;
    }

    //Allocate memory
    result = deviceVk.AllocateMemory(deviceVk.device, &memAllocInfo, allocationCallbacks, &mappableResource.deviceMemory);
    if (result != VK_SUCCESS)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to allocate memory for mappable resource. Vulkan result: %1%", VulkanResult::ToString(result)));
        return;
    }

    //Bind memory
    result = mappableResource.BindImage(deviceVk);
    if (result != VK_SUCCESS)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to bind image memory for mappable resource. Vulkan result: %1%", VulkanResult::ToString(result)));
        return;
    }

    //Get the subresource layout so we know what the row pitch is
    VkImageSubresource subresource = {};
    subresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    subresource.mipLevel = 0;
    subresource.arrayLayer = 0;

    VkSubresourceLayout subresourceLayout;
    deviceVk.GetImageSubresourceLayout(deviceVk.device, mappableResource.image, &subresource, &subresourceLayout);

    result = mappableResource.Map(deviceVk);
    if (result != VK_SUCCESS)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to map memory mappable resource. Vulkan result: %1%", VulkanResult::ToString(result)));
        return;
    }

    auto destinationBytes = static_cast<uint8_t*>(mappableResource.mappedMemory);
    auto sourceBytes = static_cast<const uint8_t*>(bytes);
    if (bytesPerRow == 0 || bytesPerRow == subresourceLayout.rowPitch)
    {
        FINJIN_COPY_MEMORY(destinationBytes, sourceBytes, byteCount);
    }
    else
    {
        for (size_t row = 0; row < height; row++)
        {
            FINJIN_COPY_MEMORY(destinationBytes, sourceBytes, bytesPerRow);

            destinationBytes += subresourceLayout.rowPitch;
            sourceBytes += bytesPerRow;
        }
    }

    mappableResource.Unmap(deviceVk);

    if (this->needsUploader)
    {
        //The mappable image is an uploader. It needs to be copied to texture

        //Create image
        imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageCreateInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;
        result = deviceVk.CreateImage(deviceVk.device, &imageCreateInfo, allocationCallbacks, &this->resource.image);
        if (result != VK_SUCCESS)
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create image for texture resource. Vulkan result: %1%", VulkanResult::ToString(result)));
            return;
        }

        //Find memory type
        deviceVk.GetImageMemoryRequirements(deviceVk.device, this->resource.image, &memReqs);
        memAllocInfo.allocationSize = memReqs.size;
        if (!gpuDescription.GetMemoryTypeIndexFromProperties(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, memAllocInfo.memoryTypeIndex))
        {
            FINJIN_SET_ERROR(error, "Failed to get memory type for texture resource.");
            return;
        }

        //Allocate memory
        result = deviceVk.AllocateMemory(deviceVk.device, &memAllocInfo, allocationCallbacks, &this->resource.deviceMemory);
        if (result != VK_SUCCESS)
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to allocate memory for texture resource. Vulkan result: %1%", VulkanResult::ToString(result)));
            return;
        }

        //Bind memory
        result = this->resource.BindImage(deviceVk);
        if (result != VK_SUCCESS)
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to bind image memory for texture resource. Vulkan result: %1%", VulkanResult::ToString(result)));
            return;
        }

        this->imageCopyInfos.resize(1);
        auto& imageCopyInfo = this->imageCopyInfos[0];
        FINJIN_ZERO_MEMORY(&imageCopyInfo, sizeof(VkImageCopy));
        imageCopyInfo.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageCopyInfo.srcSubresource.layerCount = 1;
        imageCopyInfo.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageCopyInfo.dstSubresource.layerCount = 1;
        imageCopyInfo.extent.width = static_cast<uint32_t>(width);
        imageCopyInfo.extent.height = static_cast<uint32_t>(height);
        imageCopyInfo.extent.depth = static_cast<uint32_t>(depth);
    }

    this->imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    VulkanImageViewCreateInfo imageViewCreateInfo;
    imageViewCreateInfo.image = this->resource.image;
    imageViewCreateInfo.viewType = VulkanUtilities::GetBestImageViewType(imageType, isArray);
    if (imageViewCreateInfo.viewType == VK_IMAGE_VIEW_TYPE_MAX_ENUM)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to determine appropriate image view type for image type '%1%'.", imageType));
        return;
    }
    imageViewCreateInfo.format = imageCreateInfo.format;
    imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageViewCreateInfo.subresourceRange.levelCount = 1;
    imageViewCreateInfo.subresourceRange.layerCount = 1;

    //Create image view
    result = deviceVk.CreateImageView(deviceVk.device, &imageViewCreateInfo, allocationCallbacks, &this->resource.imageView);
    if (result != VK_SUCCESS)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create image view. Vulkan result: %1%", VulkanResult::ToString(result)));
        return;
    }
}

#endif
