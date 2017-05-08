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

#include "finjin/common/Convert.hpp"
#include "finjin/common/DebugLog.hpp"
#include "finjin/common/Hash.hpp"
#include "finjin/common/StaticUnorderedMap.hpp"
#include "VulkanUtilities.hpp"

using namespace Finjin::Engine;


//Macros------------------------------------------------------------------------
#define MAKE_ASTC_BLOCK_DIMENSION_CONSTANT(x, y, z) (x + y * 256 + z * 65535)


//Implementation----------------------------------------------------------------
bool VulkanUtilities::CheckVkResultFailed(VkResult result)
{
    if (result != VK_SUCCESS)
    {
        FINJIN_DEBUG_LOG_INFO("Result indicates a failure. Vulkan result: %1%", VulkanResult::ToString(result));
        assert(0 && "Result indicates a failure.");
        return true;
    }
    else
        return false;
}

bool VulkanUtilities::IsDepthStencilFormat(VkFormat format)
{
    return
        format == VK_FORMAT_D16_UNORM_S8_UINT ||
        format == VK_FORMAT_D24_UNORM_S8_UINT ||
        format == VK_FORMAT_D32_SFLOAT_S8_UINT
        ;
}

bool VulkanUtilities::IsBCFormat(VkFormat format)
{
    switch (format)
    {
        case VK_FORMAT_BC1_RGB_UNORM_BLOCK:
        case VK_FORMAT_BC1_RGB_SRGB_BLOCK:
        case VK_FORMAT_BC1_RGBA_UNORM_BLOCK:
        case VK_FORMAT_BC1_RGBA_SRGB_BLOCK:
        case VK_FORMAT_BC2_UNORM_BLOCK:
        case VK_FORMAT_BC2_SRGB_BLOCK:
        case VK_FORMAT_BC3_UNORM_BLOCK:
        case VK_FORMAT_BC3_SRGB_BLOCK:
        case VK_FORMAT_BC4_UNORM_BLOCK:
        case VK_FORMAT_BC4_SNORM_BLOCK:
        case VK_FORMAT_BC5_UNORM_BLOCK:
        case VK_FORMAT_BC5_SNORM_BLOCK:
        case VK_FORMAT_BC6H_UFLOAT_BLOCK:
        case VK_FORMAT_BC6H_SFLOAT_BLOCK:
        case VK_FORMAT_BC7_UNORM_BLOCK:
        case VK_FORMAT_BC7_SRGB_BLOCK: return true;
        default: return false;
    }
}

bool VulkanUtilities::IsETC2Format(VkFormat format)
{
    switch (format)
    {
        case VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK:
        case VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK:
        case VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK:
        case VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK:
        case VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK:
        case VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK: return true;
        default: return false;
    }
}

bool VulkanUtilities::IsASTCFormat(VkFormat format)
{
    switch (format)
    {
        case VK_FORMAT_ASTC_4x4_UNORM_BLOCK:
        case VK_FORMAT_ASTC_4x4_SRGB_BLOCK:
        case VK_FORMAT_ASTC_5x4_UNORM_BLOCK:
        case VK_FORMAT_ASTC_5x4_SRGB_BLOCK:
        case VK_FORMAT_ASTC_5x5_UNORM_BLOCK:
        case VK_FORMAT_ASTC_5x5_SRGB_BLOCK:
        case VK_FORMAT_ASTC_6x5_UNORM_BLOCK:
        case VK_FORMAT_ASTC_6x5_SRGB_BLOCK:
        case VK_FORMAT_ASTC_6x6_UNORM_BLOCK:
        case VK_FORMAT_ASTC_6x6_SRGB_BLOCK:
        case VK_FORMAT_ASTC_8x5_UNORM_BLOCK:
        case VK_FORMAT_ASTC_8x5_SRGB_BLOCK:
        case VK_FORMAT_ASTC_8x6_UNORM_BLOCK:
        case VK_FORMAT_ASTC_8x6_SRGB_BLOCK:
        case VK_FORMAT_ASTC_8x8_UNORM_BLOCK:
        case VK_FORMAT_ASTC_8x8_SRGB_BLOCK:
        case VK_FORMAT_ASTC_10x5_UNORM_BLOCK:
        case VK_FORMAT_ASTC_10x5_SRGB_BLOCK:
        case VK_FORMAT_ASTC_10x6_UNORM_BLOCK:
        case VK_FORMAT_ASTC_10x6_SRGB_BLOCK:
        case VK_FORMAT_ASTC_10x8_UNORM_BLOCK:
        case VK_FORMAT_ASTC_10x8_SRGB_BLOCK:
        case VK_FORMAT_ASTC_10x10_UNORM_BLOCK:
        case VK_FORMAT_ASTC_10x10_SRGB_BLOCK:
        case VK_FORMAT_ASTC_12x10_UNORM_BLOCK:
        case VK_FORMAT_ASTC_12x10_SRGB_BLOCK:
        case VK_FORMAT_ASTC_12x12_UNORM_BLOCK:
        case VK_FORMAT_ASTC_12x12_SRGB_BLOCK: return true;
        default: return false;
    }
}

VkFormat VulkanUtilities::ASTCBlockSizeToVulkanFormat(uint8_t xDim, uint8_t yDim, uint8_t zDim, ASTCCompressionSubmode submode)
{
    //Note: There is currently no standard Vulkan format that uses the 'zDim', so it is ignored for now

    switch (MAKE_ASTC_BLOCK_DIMENSION_CONSTANT(xDim, yDim, zDim))
    {
        case MAKE_ASTC_BLOCK_DIMENSION_CONSTANT(4, 4, 1): return submode == ASTCCompressionSubmode::LINEAR ? VK_FORMAT_ASTC_4x4_UNORM_BLOCK : VK_FORMAT_ASTC_4x4_SRGB_BLOCK;
        case MAKE_ASTC_BLOCK_DIMENSION_CONSTANT(5, 4, 1): return submode == ASTCCompressionSubmode::LINEAR ? VK_FORMAT_ASTC_5x4_UNORM_BLOCK : VK_FORMAT_ASTC_5x4_SRGB_BLOCK;
        case MAKE_ASTC_BLOCK_DIMENSION_CONSTANT(5, 5, 1): return submode == ASTCCompressionSubmode::LINEAR ? VK_FORMAT_ASTC_5x5_UNORM_BLOCK : VK_FORMAT_ASTC_5x5_SRGB_BLOCK;
        case MAKE_ASTC_BLOCK_DIMENSION_CONSTANT(6, 5, 1): return submode == ASTCCompressionSubmode::LINEAR ? VK_FORMAT_ASTC_6x5_UNORM_BLOCK : VK_FORMAT_ASTC_6x5_SRGB_BLOCK;
        case MAKE_ASTC_BLOCK_DIMENSION_CONSTANT(6, 6, 1): return submode == ASTCCompressionSubmode::LINEAR ? VK_FORMAT_ASTC_6x6_UNORM_BLOCK : VK_FORMAT_ASTC_6x6_SRGB_BLOCK;
        case MAKE_ASTC_BLOCK_DIMENSION_CONSTANT(8, 5, 1): return submode == ASTCCompressionSubmode::LINEAR ? VK_FORMAT_ASTC_8x5_UNORM_BLOCK : VK_FORMAT_ASTC_8x5_SRGB_BLOCK;
        case MAKE_ASTC_BLOCK_DIMENSION_CONSTANT(8, 6, 1): return submode == ASTCCompressionSubmode::LINEAR ? VK_FORMAT_ASTC_8x6_UNORM_BLOCK : VK_FORMAT_ASTC_8x6_SRGB_BLOCK;
        case MAKE_ASTC_BLOCK_DIMENSION_CONSTANT(8, 8, 1): return submode == ASTCCompressionSubmode::LINEAR ? VK_FORMAT_ASTC_8x8_UNORM_BLOCK : VK_FORMAT_ASTC_8x8_SRGB_BLOCK;
        case MAKE_ASTC_BLOCK_DIMENSION_CONSTANT(10, 5, 1): return submode == ASTCCompressionSubmode::LINEAR ? VK_FORMAT_ASTC_10x5_UNORM_BLOCK : VK_FORMAT_ASTC_10x5_SRGB_BLOCK;
        case MAKE_ASTC_BLOCK_DIMENSION_CONSTANT(10, 6, 1): return submode == ASTCCompressionSubmode::LINEAR ? VK_FORMAT_ASTC_10x6_UNORM_BLOCK : VK_FORMAT_ASTC_10x6_SRGB_BLOCK;
        case MAKE_ASTC_BLOCK_DIMENSION_CONSTANT(10, 8, 1): return submode == ASTCCompressionSubmode::LINEAR ? VK_FORMAT_ASTC_10x8_UNORM_BLOCK : VK_FORMAT_ASTC_10x8_SRGB_BLOCK;
        case MAKE_ASTC_BLOCK_DIMENSION_CONSTANT(10, 10, 1): return submode == ASTCCompressionSubmode::LINEAR ? VK_FORMAT_ASTC_10x10_UNORM_BLOCK : VK_FORMAT_ASTC_10x10_SRGB_BLOCK;
        case MAKE_ASTC_BLOCK_DIMENSION_CONSTANT(12, 10, 1): return submode == ASTCCompressionSubmode::LINEAR ? VK_FORMAT_ASTC_12x10_UNORM_BLOCK : VK_FORMAT_ASTC_12x10_SRGB_BLOCK;
        case MAKE_ASTC_BLOCK_DIMENSION_CONSTANT(12, 12, 1): return submode == ASTCCompressionSubmode::LINEAR ? VK_FORMAT_ASTC_12x12_UNORM_BLOCK : VK_FORMAT_ASTC_12x12_SRGB_BLOCK;
        default: return VK_FORMAT_UNDEFINED;
    }
}

VkFormat VulkanUtilities::GLInternalFormatToVulkanFormat(uint32_t glInternalFormat)
{
    switch (glInternalFormat)
    {
        //8 bits per component
        case GL_R8: return VK_FORMAT_R8_UNORM;
        case GL_R8_SNORM: return VK_FORMAT_R8_SNORM;
        case GL_R8UI: return VK_FORMAT_R8_UINT;
        case GL_R8I: return VK_FORMAT_R8_SINT;
        case GL_SR8: return VK_FORMAT_R8_SRGB;

        case GL_RG8: return VK_FORMAT_R8G8_UNORM;
        case GL_RG8_SNORM: return VK_FORMAT_R8G8_SNORM;
        case GL_RG8UI: return VK_FORMAT_R8G8_UINT;
        case GL_RG8I: return VK_FORMAT_R8G8_SINT;
        case GL_SRG8: return VK_FORMAT_R8G8_SRGB;

        case GL_RGB8: return VK_FORMAT_R8G8B8_UNORM;
        case GL_RGB8_SNORM: return VK_FORMAT_R8G8B8_SNORM;
        case GL_RGB8UI: return VK_FORMAT_R8G8B8_UINT;
        case GL_RGB8I: return VK_FORMAT_R8G8B8_SINT;
        case GL_SRGB8: return VK_FORMAT_R8G8B8_SRGB;

        case GL_RGBA8: return VK_FORMAT_R8G8B8A8_UNORM;
        case GL_RGBA8_SNORM: return VK_FORMAT_R8G8B8A8_SNORM;
        case GL_RGBA8UI: return VK_FORMAT_R8G8B8A8_UINT;
        case GL_RGBA8I: return VK_FORMAT_R8G8B8A8_SINT;
        case GL_SRGB8_ALPHA8: return VK_FORMAT_R8G8B8A8_SRGB;

        //16 bits per component
        case GL_R16: return VK_FORMAT_R16_UNORM;
        case GL_R16_SNORM: return VK_FORMAT_R16_SNORM;
        case GL_R16UI: return VK_FORMAT_R16_UINT;
        case GL_R16I: return VK_FORMAT_R16_SINT;
        case GL_R16F: return VK_FORMAT_R16_SFLOAT;

        case GL_RG16: return VK_FORMAT_R16G16_UNORM;
        case GL_RG16_SNORM: return VK_FORMAT_R16G16_SNORM;
        case GL_RG16UI: return VK_FORMAT_R16G16_UINT;
        case GL_RG16I: return VK_FORMAT_R16G16_SINT;
        case GL_RG16F: return VK_FORMAT_R16G16_SFLOAT;

        case GL_RGB16: return VK_FORMAT_R16G16B16_UNORM;
        case GL_RGB16_SNORM: return VK_FORMAT_R16G16B16_SNORM;
        case GL_RGB16UI: return VK_FORMAT_R16G16B16_UINT;
        case GL_RGB16I: return VK_FORMAT_R16G16B16_SINT;
        case GL_RGB16F: return VK_FORMAT_R16G16B16_SFLOAT;

        case GL_RGBA16: return VK_FORMAT_R16G16B16A16_UNORM;
        case GL_RGBA16_SNORM: return VK_FORMAT_R16G16B16A16_SNORM;
        case GL_RGBA16UI: return VK_FORMAT_R16G16B16A16_UINT;
        case GL_RGBA16I: return VK_FORMAT_R16G16B16A16_SINT;
        case GL_RGBA16F: return VK_FORMAT_R16G16B16A16_SFLOAT;

        //32 bits per component
        case GL_R32UI: return VK_FORMAT_R32_UINT;
        case GL_R32I: return VK_FORMAT_R32_SINT;
        case GL_R32F: return VK_FORMAT_R32_SFLOAT;

        case GL_RG32UI: return VK_FORMAT_R32G32_UINT;
        case GL_RG32I: return VK_FORMAT_R32G32_SINT;
        case GL_RG32F: return VK_FORMAT_R32G32_SFLOAT;

        case GL_RGB32UI: return VK_FORMAT_R32G32B32_UINT;
        case GL_RGB32I: return VK_FORMAT_R32G32B32_SINT;
        case GL_RGB32F: return VK_FORMAT_R32G32B32_SFLOAT;

        case GL_RGBA32UI: return VK_FORMAT_R32G32B32A32_UINT;
        case GL_RGBA32I: return VK_FORMAT_R32G32B32A32_SINT;
        case GL_RGBA32F: return VK_FORMAT_R32G32B32A32_SFLOAT;

        //Packed
        case GL_RGB565: return VK_FORMAT_R5G6B5_UNORM_PACK16;
        //case GL_RGB10: return VK_FORMAT_A2R10G10B10_UNORM_PACK32; //Leading alpha in Vulkan format is a problem
        case GL_RGBA4: return VK_FORMAT_R4G4B4A4_UNORM_PACK16;
        case GL_RGB5_A1: return VK_FORMAT_R5G5B5A1_UNORM_PACK16;
        //case GL_RGB10_A2: return VK_FORMAT_A2R10G10B10_UNORM_PACK32; //Needs to be reversed
        //case GL_RGB10_A2UI: return VK_FORMAT_A2R10G10B10_UINT_PACK32; //Needs to be reversed
        //case GL_R11F_G11F_B10F: return VK_FORMAT_B10G11R11_UFLOAT_PACK32; //Needs to be reversed
        //case GL_RGB9_E5: return VK_FORMAT_E5B9G9R9_UFLOAT_PACK32; //Needs to be reversed

        //ETC
        case GL_COMPRESSED_RGB8_ETC2: return VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK;
        case GL_COMPRESSED_SRGB8_ETC2: return VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK;
        case GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2: return VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK;
        case GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2: return VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK;

        case GL_COMPRESSED_RGBA8_ETC2_EAC: return VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK;
        case GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC: return VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK;

        //EAC
        case GL_COMPRESSED_R11_EAC: return VK_FORMAT_EAC_R11_UNORM_BLOCK;
        case GL_COMPRESSED_SIGNED_R11_EAC: return VK_FORMAT_EAC_R11_SNORM_BLOCK;

        case GL_COMPRESSED_RG11_EAC: return VK_FORMAT_EAC_R11G11_UNORM_BLOCK;
        case GL_COMPRESSED_SIGNED_RG11_EAC: return VK_FORMAT_EAC_R11G11_SNORM_BLOCK;

        //ASTC
        case GL_COMPRESSED_RGBA_ASTC_4x4_KHR: return VK_FORMAT_ASTC_4x4_UNORM_BLOCK;
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR: return VK_FORMAT_ASTC_4x4_SRGB_BLOCK;

        case GL_COMPRESSED_RGBA_ASTC_5x4_KHR: return VK_FORMAT_ASTC_5x4_UNORM_BLOCK;
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR: return VK_FORMAT_ASTC_5x4_SRGB_BLOCK;

        case GL_COMPRESSED_RGBA_ASTC_5x5_KHR: return VK_FORMAT_ASTC_5x5_UNORM_BLOCK;
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR: return VK_FORMAT_ASTC_5x5_SRGB_BLOCK;

        case GL_COMPRESSED_RGBA_ASTC_6x5_KHR: return VK_FORMAT_ASTC_6x5_UNORM_BLOCK;
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR: return VK_FORMAT_ASTC_6x5_SRGB_BLOCK;

        case GL_COMPRESSED_RGBA_ASTC_6x6_KHR: return VK_FORMAT_ASTC_6x6_UNORM_BLOCK;
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR: return VK_FORMAT_ASTC_6x6_SRGB_BLOCK;

        case GL_COMPRESSED_RGBA_ASTC_8x5_KHR: return VK_FORMAT_ASTC_8x5_UNORM_BLOCK;
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR: return VK_FORMAT_ASTC_8x5_SRGB_BLOCK;

        case GL_COMPRESSED_RGBA_ASTC_8x6_KHR: return VK_FORMAT_ASTC_8x6_UNORM_BLOCK;
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR: return VK_FORMAT_ASTC_8x6_SRGB_BLOCK;

        case GL_COMPRESSED_RGBA_ASTC_8x8_KHR: return VK_FORMAT_ASTC_8x8_UNORM_BLOCK;
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR: return VK_FORMAT_ASTC_8x8_SRGB_BLOCK;

        case GL_COMPRESSED_RGBA_ASTC_10x5_KHR: return VK_FORMAT_ASTC_10x5_UNORM_BLOCK;
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR: return VK_FORMAT_ASTC_10x5_SRGB_BLOCK;

        case GL_COMPRESSED_RGBA_ASTC_10x6_KHR: return VK_FORMAT_ASTC_10x6_UNORM_BLOCK;
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR: return VK_FORMAT_ASTC_10x6_SRGB_BLOCK;

        case GL_COMPRESSED_RGBA_ASTC_10x8_KHR: return VK_FORMAT_ASTC_10x8_UNORM_BLOCK;
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR: return VK_FORMAT_ASTC_10x8_SRGB_BLOCK;

        case GL_COMPRESSED_RGBA_ASTC_10x10_KHR: return VK_FORMAT_ASTC_10x10_UNORM_BLOCK;
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR: return VK_FORMAT_ASTC_10x10_SRGB_BLOCK;

        case GL_COMPRESSED_RGBA_ASTC_12x10_KHR: return VK_FORMAT_ASTC_12x10_UNORM_BLOCK;
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR: return VK_FORMAT_ASTC_12x10_SRGB_BLOCK;

        case GL_COMPRESSED_RGBA_ASTC_12x12_KHR: return VK_FORMAT_ASTC_12x12_UNORM_BLOCK;
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR: return VK_FORMAT_ASTC_12x12_SRGB_BLOCK;

        default: return VK_FORMAT_UNDEFINED;
    }
}

VkShaderStageFlagBits VulkanUtilities::ParseShaderStages(const Utf8StringView& value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    const size_t count = 8;
    static StaticUnorderedMap<size_t, VkShaderStageFlagBits, count, FINJIN_OVERSIZE_FULL_STATIC_MAP_BUCKET_COUNT(count), MapPairConstructNone<size_t, VkShaderStageFlagBits>, PassthroughHash> lookup
        (
        Utf8String::Hash("vertex"), VK_SHADER_STAGE_VERTEX_BIT,
        Utf8String::Hash("tessellation-control"), VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT,
        Utf8String::Hash("tessellation-evaluation"), VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT,
        Utf8String::Hash("geometry"), VK_SHADER_STAGE_GEOMETRY_BIT,
        Utf8String::Hash("fragment"), VK_SHADER_STAGE_FRAGMENT_BIT,
        Utf8String::Hash("compute"), VK_SHADER_STAGE_COMPUTE_BIT,
        Utf8String::Hash("all-graphics"), VK_SHADER_STAGE_ALL_GRAPHICS,
        Utf8String::Hash("all"), VK_SHADER_STAGE_ALL
        );

    Utf8StringHash hash;
    VkShaderStageFlagBits stageFlags = (VkShaderStageFlagBits)0;
    auto splitResult = Split(value, ' ', [&hash, &stageFlags](Utf8StringView& value)
    {
        auto foundAt = lookup.find(hash(value));
        if (foundAt != lookup.end())
            stageFlags = static_cast<VkShaderStageFlagBits>(stageFlags | foundAt->second);
        else
            return ValueOrError<bool>::CreateError();

        return ValueOrError<bool>(true);
    });
    if (splitResult.HasError())
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to parse shader stages from '%1%'. Invalid value encountered.", value.ToString()));
        return (VkShaderStageFlagBits)0;
    }

    return stageFlags;
}

void VulkanUtilities::ParsePushConstantRangeOffsetAndSize(VkPushConstantRange& result, const Utf8StringView& value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    size_t valueIndex = 0;
    auto splitResult = Split(value, ' ', [&result, &valueIndex](Utf8StringView& item)
    {
        if (valueIndex > 1)
            return ValueOrError<bool>::CreateError();

        if (valueIndex == 0)
            result.offset = Convert::ToInteger(item.ToString(), result.offset);
        else
            result.size = Convert::ToInteger(item.ToString(), result.size);
        valueIndex++;

        return ValueOrError<bool>(true);
    });
    if (splitResult.HasError())
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to push constant range from '%1%'. More than two elements encountered.", value.ToString()));
}

VkImageViewType VulkanUtilities::GetBestImageViewType(VkImageType imageType, bool isArray)
{
    switch (imageType)
    {
        case VK_IMAGE_TYPE_1D: return isArray ? VK_IMAGE_VIEW_TYPE_1D_ARRAY : VK_IMAGE_VIEW_TYPE_1D;
        case VK_IMAGE_TYPE_2D: return isArray ? VK_IMAGE_VIEW_TYPE_2D_ARRAY : VK_IMAGE_VIEW_TYPE_2D;
        case VK_IMAGE_TYPE_3D: return VK_IMAGE_VIEW_TYPE_3D;
        default: return VK_IMAGE_VIEW_TYPE_MAX_ENUM;
    }
}

void VulkanUtilities::SetImageLayout
    (
    VulkanDeviceFunctions& vk,
    VkCommandBuffer commandBuffer,
    VkImage image,
    VkImageLayout oldImageLayout,
    VkImageLayout newImageLayout,
    VkImageSubresourceRange subresourceRange,
    VkPipelineStageFlags srcStageMask,
    VkPipelineStageFlags dstStageMask
    )
{
    VulkanImageMemoryBarrier imageMemoryBarrier;
    imageMemoryBarrier.oldLayout = oldImageLayout;
    imageMemoryBarrier.newLayout = newImageLayout;
    imageMemoryBarrier.image = image;
    imageMemoryBarrier.subresourceRange = subresourceRange;

    switch (oldImageLayout)
    {
        case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL: imageMemoryBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT; break;
        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL: imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT; break;
        case VK_IMAGE_LAYOUT_PREINITIALIZED: imageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT; break;
        default: break;
    }

    switch (newImageLayout)
    {
        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL: imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT; break;
        case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL: imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT; break;
        case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL: imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT; break;
        case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL: imageMemoryBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT; break;
        case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL: imageMemoryBarrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT; break;
        default: break;
    }

    vk.CmdPipelineBarrier(commandBuffer, srcStageMask, dstStageMask, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);
}

void VulkanUtilities::SetImageLayout
    (
    VulkanDeviceFunctions& vk,
    VkCommandBuffer commandBuffer,
    VkImage image,
    VkImageAspectFlags aspectMask,
    VkImageLayout oldImageLayout,
    VkImageLayout newImageLayout,
    VkPipelineStageFlags srcStageMask,
    VkPipelineStageFlags dstStageMask
    )
{
    //First mip level and layer
    VkImageSubresourceRange subresourceRange = {};
    subresourceRange.aspectMask = aspectMask;
    subresourceRange.levelCount = 1;
    subresourceRange.layerCount = 1;
    SetImageLayout(vk, commandBuffer, image, oldImageLayout, newImageLayout, subresourceRange);
}

#endif
