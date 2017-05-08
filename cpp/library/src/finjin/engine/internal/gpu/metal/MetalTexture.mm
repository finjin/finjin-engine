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

#include "MetalTexture.hpp"
#include "MetalUtilities.hpp"

using namespace Finjin::Engine;


//Implementation----------------------------------------------------------------
MetalTexture::MetalTexture(Allocator* allocator) : name(allocator)
{
    this->resource = nullptr;
    this->resourceDescriptor = nullptr;
    this->textureIndex = 0;
    this->textureDimension = TextureDimension::DIMENSION_2D;
}

void MetalTexture::CreateFromPNG
    (
    const PNGReader& reader,
    id<MTLDevice> device,
    const void* bytes,
    size_t byteCount,
    size_t maxDimension,
    Error& error
    )
{
    FINJIN_ERROR_METHOD_START(error);

    //Determine pixel format
    MTLPixelFormat format = MTLPixelFormatInvalid;
    auto pngPixelFormat = reader.GetPixelFormat();
    if (pngPixelFormat == PNGReader::PixelFormat::RGBA)
    {
        if (reader.GetBytesPerChannel() == 1)
        {
            if (reader.IsSRGB())
                format = MTLPixelFormatRGBA8Unorm_sRGB;
            else
                format = MTLPixelFormatRGBA8Unorm;
        }
        else if (reader.GetBytesPerChannel() == 2)
            format = MTLPixelFormatRGBA16Unorm;
        else
        {
            FINJIN_SET_ERROR(error, Utf8StringFormatter::Format("Unsupported RGBA bytes per component '%1%'.", reader.GetBytesPerChannel()));
            return;
        }
    }
    else if (pngPixelFormat == PNGReader::PixelFormat::GRAY)
    {
        if (reader.GetBytesPerChannel() == 1)
        {
        #if FINJIN_TARGET_PLATFORM == FINJIN_TARGET_PLATFORM_MACOS
            format = MTLPixelFormatR8Unorm;
        #else
            if (reader.IsSRGB())
                format = MTLPixelFormatR8Unorm_sRGB;
            else
                format = MTLPixelFormatR8Unorm;
        #endif
        }
        else if (reader.GetBytesPerChannel() == 2)
            format = MTLPixelFormatR16Unorm;
        else
        {
            FINJIN_SET_ERROR(error, Utf8StringFormatter::Format("Unsupported gray bytes per component '%1%'.", reader.GetBytesPerChannel()));
            return;
        }
    }
    else if (pngPixelFormat == PNGReader::PixelFormat::GA)
    {
        if (reader.GetBytesPerChannel() == 1)
        {
        #if FINJIN_TARGET_PLATFORM == FINJIN_TARGET_PLATFORM_MACOS
            format = MTLPixelFormatRG8Unorm;
        #else
            if (reader.IsSRGB())
                format = MTLPixelFormatRG8Unorm_sRGB;
            else
                format = MTLPixelFormatRG8Unorm;
        #endif
        }
        else if (reader.GetBytesPerChannel() == 2)
            format = MTLPixelFormatRG16Unorm;
        else
        {
            FINJIN_SET_ERROR(error, Utf8StringFormatter::Format("Unsupported gray/alpha bytes per component '%1%'.", reader.GetBytesPerChannel()));
            return;
        }
    }
    else
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Invalid PNG pixel format '%1%'.", pngPixelFormat));
        return;
    }

    this->resourceDescriptor = [[MTLTextureDescriptor alloc] init];
    this->resourceDescriptor.textureType = MTLTextureType2D;
    this->resourceDescriptor.pixelFormat = format;
    this->resourceDescriptor.width = reader.GetWidth();
    this->resourceDescriptor.height = reader.GetHeight();
    this->resourceDescriptor.depth = 1;
    this->resourceDescriptor.mipmapLevelCount = 1;
    this->resourceDescriptor.arrayLength = 1;
    this->resourceDescriptor.usage = MTLTextureUsageShaderRead;
    this->resource = [device newTextureWithDescriptor:this->resourceDescriptor];
    if (this->resource == nullptr)
    {
        FINJIN_SET_ERROR(error, "Failed to create single mip.");
        return;
    }

    auto region = MTLRegionMake2D(0, 0, this->resourceDescriptor.width, this->resourceDescriptor.height);
    [this->resource replaceRegion:region mipmapLevel:0 withBytes:bytes bytesPerRow:reader.GetBytesPerRow()];
}

void MetalTexture::Destroy()
{
    this->resource = nullptr;
    this->resourceDescriptor = nullptr;

    this->name.Destroy();
}

void MetalTexture::HandleCreationFailure()
{
    Destroy();
}

#endif
