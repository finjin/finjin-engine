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

    if (colorFormat.requested == MTLPixelFormatInvalid)
        formats.push_back(static_cast<MTLPixelFormat>(colorFormat.requested));

    formats.push_back(MTLPixelFormatBGRA8Unorm);
    formats.push_back(MTLPixelFormatBGRA8Unorm_sRGB);
    formats.push_back(MTLPixelFormatRGBA8Unorm);
    formats.push_back(MTLPixelFormatRGBA8Unorm_sRGB);

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

#endif
