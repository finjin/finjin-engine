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

#if FINJIN_TARGET_GPU_SYSTEM == FINJIN_TARGET_GPU_SYSTEM_METAL

#include "MetalUtilities.hpp"

using namespace Finjin::Engine;


//Local functions---------------------------------------------------------------
static bool IsFormatAvailable(MetalFeatureSetCapability formatCapabilities)
{
    //The presence of either RESOLVE or MSAA seems to be sufficient
    return AnySet(formatCapabilities & (MetalFeatureSetCapability::RESOLVE | MetalFeatureSetCapability::MSAA));
}

static bool IsSupportedColorFormat(MetalFeatureSet::Capabilities& caps, MTLPixelFormat format)
{
    switch (format)
    {
        case MTLPixelFormatBGRA8Unorm: return IsFormatAvailable(caps.BGRA8Unorm);
        case MTLPixelFormatBGRA8Unorm_sRGB: return IsFormatAvailable(caps.BGRA8Unorm_sRGB);
        case MTLPixelFormatRGBA8Unorm: return IsFormatAvailable(caps.RGBA8Unorm);
        case MTLPixelFormatRGBA8Unorm_sRGB: return IsFormatAvailable(caps.RGBA8Unorm_sRGB);

        default: return false;
    }
}

static bool IsSupportedDepthStencilFormat(MetalFeatureSet::Capabilities& caps, MTLPixelFormat format)
{
    switch (format)
    {
    #if FINJIN_TARGET_PLATFORM == FINJIN_TARGET_PLATFORM_MACOS
        case MTLPixelFormatDepth16Unorm: return IsFormatAvailable(caps.Depth16Unorm);
        case MTLPixelFormatDepth24Unorm_Stencil8: return IsFormatAvailable(caps.Depth24Unorm_Stencil8);
        case MTLPixelFormatX24_Stencil8: return IsFormatAvailable(caps.X24_Stencil8);
    #endif
        case MTLPixelFormatDepth32Float: return IsFormatAvailable(caps.Depth32Float);
        case MTLPixelFormatDepth32Float_Stencil8: return IsFormatAvailable(caps.Depth32Float_Stencil8);
        case MTLPixelFormatX32_Stencil8: return IsFormatAvailable(caps.X32_Stencil8);

        default: return false;
    }
}


//Implementation----------------------------------------------------------------
size_t MetalUtilities::GetUniformBufferAlignedSize(size_t paddedSize, size_t size, size_t count, size_t minBufferOffsetAlignment)
{
    auto byteSize = paddedSize * (count - 1) + size;
    return std::max(byteSize, minBufferOffsetAlignment);
}

bool MetalUtilities::IsDepthStencilFormat(MTLPixelFormat format)
{
    switch (format)
    {
    #if FINJIN_TARGET_PLATFORM == FINJIN_TARGET_PLATFORM_MACOS
        case MTLPixelFormatDepth24Unorm_Stencil8: return true;
        case MTLPixelFormatX24_Stencil8: return true;
    #endif
        case MTLPixelFormatDepth32Float_Stencil8: return true;
        case MTLPixelFormatX32_Stencil8: return true;

        default: return false;
    }
}

bool MetalUtilities::GetBestColorFormat(RequestedValue<uint32_t>& colorFormat32, MetalFeatureSet::Capabilities& caps)
{
    RequestedValue<MTLPixelFormat> colorFormat(static_cast<MTLPixelFormat>(colorFormat32.requested));
    if (GetBestColorFormat(colorFormat, caps))
    {
        colorFormat32.actual = static_cast<uint32_t>(colorFormat.actual);
        return true;
    }
    else
        return false;
}

bool MetalUtilities::GetBestColorFormat(RequestedValue<MTLPixelFormat>& colorFormat, MetalFeatureSet::Capabilities& caps)
{
    colorFormat.actual = MTLPixelFormatInvalid;

    StaticVector<MTLPixelFormat, 10> formats;

    if (colorFormat.requested != MTLPixelFormatInvalid)
        formats.push_back(static_cast<MTLPixelFormat>(colorFormat.requested));

    formats.push_back(MTLPixelFormatBGRA8Unorm);
    formats.push_back(MTLPixelFormatBGRA8Unorm_sRGB);
    formats.push_back(MTLPixelFormatRGBA16Float);

    //Get first supported format
    for (auto format : formats)
    {
        if (IsSupportedColorFormat(caps, format))
        {
            colorFormat.actual = format;
            break;
        }
    }

    return colorFormat.actual != MTLPixelFormatInvalid;
}

bool MetalUtilities::GetBestDepthStencilFormat(RequestedValue<uint32_t>& depthStencilFormat32, bool stencilRequired, MetalFeatureSet::Capabilities& caps)
{
    RequestedValue<MTLPixelFormat> depthStencilFormat(static_cast<MTLPixelFormat>(depthStencilFormat32.requested));
    if (GetBestDepthStencilFormat(depthStencilFormat, stencilRequired, caps))
    {
        depthStencilFormat32.actual = static_cast<uint32_t>(depthStencilFormat.actual);
        return true;
    }
    else
        return false;
}

bool MetalUtilities::GetBestDepthStencilFormat(RequestedValue<MTLPixelFormat>& depthStencilFormat, bool stencilRequired, MetalFeatureSet::Capabilities& caps)
{
    depthStencilFormat.actual = MTLPixelFormatInvalid;

    StaticVector<MTLPixelFormat, 10> formats;

    if (depthStencilFormat.requested != MTLPixelFormatInvalid)
    {
        if (!stencilRequired || IsDepthStencilFormat(static_cast<MTLPixelFormat>(depthStencilFormat.requested)))
            formats.push_back(static_cast<MTLPixelFormat>(depthStencilFormat.requested));
    }

    if (stencilRequired)
    {
    #if FINJIN_TARGET_PLATFORM == FINJIN_TARGET_PLATFORM_MACOS
        formats.push_back(MTLPixelFormatDepth24Unorm_Stencil8);
    #endif
        formats.push_back(MTLPixelFormatDepth32Float_Stencil8);
    }
    else
    {
        formats.push_back(MTLPixelFormatDepth32Float);
    #if FINJIN_TARGET_PLATFORM == FINJIN_TARGET_PLATFORM_MACOS
        formats.push_back(MTLPixelFormatDepth24Unorm_Stencil8);
        formats.push_back(MTLPixelFormatDepth32Float_Stencil8);
        formats.push_back(MTLPixelFormatDepth16Unorm);
    #endif
        formats.push_back(MTLPixelFormatDepth32Float_Stencil8);
    }

    //Get first supported format
    for (auto format : formats)
    {
        if (IsSupportedDepthStencilFormat(caps, format))
        {
            depthStencilFormat.actual = format;
            break;
        }
    }

    return depthStencilFormat.actual != MTLPixelFormatInvalid;
}

size_t MetalUtilities::GetBitsPerPixel(MTLPixelFormat format)
{
    switch (format)
    {
        //Normal 8 bit formats
        case MTLPixelFormatA8Unorm:
        case MTLPixelFormatR8Unorm:
    #if FINJIN_TARGET_PLATFORM == FINJIN_TARGET_PLATFORM_IOS
        case MTLPixelFormatR8Unorm_sRGB:
    #endif

        case MTLPixelFormatR8Snorm:
        case MTLPixelFormatR8Uint:
        case MTLPixelFormatR8Sint: return 8;

        //Normal 16 bit formats
        case MTLPixelFormatR16Unorm:
        case MTLPixelFormatR16Snorm:
        case MTLPixelFormatR16Uint:
        case MTLPixelFormatR16Sint:
        case MTLPixelFormatR16Float:

        case MTLPixelFormatRG8Unorm:
    #if FINJIN_TARGET_PLATFORM == FINJIN_TARGET_PLATFORM_IOS
        case MTLPixelFormatRG8Unorm_sRGB:
    #endif
        case MTLPixelFormatRG8Snorm:
        case MTLPixelFormatRG8Uint:
        case MTLPixelFormatRG8Sint: return 16;

        //Packed 16 bit formats
    #if FINJIN_TARGET_PLATFORM == FINJIN_TARGET_PLATFORM_IOS
        case MTLPixelFormatB5G6R5Unorm:
        case MTLPixelFormatA1BGR5Unorm:
        case MTLPixelFormatABGR4Unorm:
        case MTLPixelFormatBGR5A1Unorm: return 16;
    #endif

        //Normal 32 bit formats
        case MTLPixelFormatR32Uint:
        case MTLPixelFormatR32Sint:
        case MTLPixelFormatR32Float:

        case MTLPixelFormatRG16Unorm:
        case MTLPixelFormatRG16Snorm:
        case MTLPixelFormatRG16Uint:
        case MTLPixelFormatRG16Sint:
        case MTLPixelFormatRG16Float:

        case MTLPixelFormatRGBA8Unorm:
        case MTLPixelFormatRGBA8Unorm_sRGB:
        case MTLPixelFormatRGBA8Snorm:
        case MTLPixelFormatRGBA8Uint:
        case MTLPixelFormatRGBA8Sint:

        case MTLPixelFormatBGRA8Unorm:
        case MTLPixelFormatBGRA8Unorm_sRGB: return 32;

        //Packed 32 bit formats
        case MTLPixelFormatRGB10A2Unorm:
        case MTLPixelFormatRGB10A2Uint:

        case MTLPixelFormatRG11B10Float:
        case MTLPixelFormatRGB9E5Float: return 32;

    #if FINJIN_TARGET_PLATFORM == FINJIN_TARGET_PLATFORM_IOS
        case MTLPixelFormatBGR10_XR:
        case MTLPixelFormatBGR10_XR_sRGB: return 32;
    #endif

        //Normal 64 bit formats
        case MTLPixelFormatRG32Uint:
        case MTLPixelFormatRG32Sint:
        case MTLPixelFormatRG32Float:

        case MTLPixelFormatRGBA16Unorm:
        case MTLPixelFormatRGBA16Snorm:
        case MTLPixelFormatRGBA16Uint:
        case MTLPixelFormatRGBA16Sint:
        case MTLPixelFormatRGBA16Float: return 64;

    #if FINJIN_TARGET_PLATFORM == FINJIN_TARGET_PLATFORM_IOS
        case MTLPixelFormatBGRA10_XR:
        case MTLPixelFormatBGRA10_XR_sRGB: return 64;
    #endif

        //Normal 128 bit formats
        case MTLPixelFormatRGBA32Uint:
        case MTLPixelFormatRGBA32Sint:
        case MTLPixelFormatRGBA32Float: return 128;

        //Compressed formats
        case MTLPixelFormatGBGR422: return 8;
        case MTLPixelFormatBGRG422: return 8;

        //Depth
        case MTLPixelFormatDepth16Unorm: return 16;
        case MTLPixelFormatDepth32Float: return 32;

        //Stencil
        case MTLPixelFormatStencil8: return 8;

        //Depth Stencil
        case MTLPixelFormatDepth24Unorm_Stencil8: return 32;
        case MTLPixelFormatDepth32Float_Stencil8: return 40;

        case MTLPixelFormatX32_Stencil8: return 40;
        case MTLPixelFormatX24_Stencil8: return 32;

        default: return 0;
    }
}

#endif
