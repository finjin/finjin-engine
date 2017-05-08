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
#include "PVRReader.hpp"
#include "finjin/common/ByteOrder.hpp"

using namespace Finjin::Engine;


//Local functions---------------------------------------------------------------
static void SwapHeaderBytes(PVRReader::Header& header)
{
    SwapBytes(header.version);
    SwapBytes(header.flags);
    SwapBytes(header.pixelFormat);
    SwapBytes(header.colorSpace);
    SwapBytes(header.channelType);
    SwapBytes(header.height);
    SwapBytes(header.width);
    SwapBytes(header.depth);
    SwapBytes(header.surfaceCount);
    SwapBytes(header.faceCount);
    SwapBytes(header.mipMapCount);
    SwapBytes(header.metadataSize);
}


//Implementation----------------------------------------------------------------

//PVRReader::Header
bool PVRReader::Header::HasPixelFormatValue() const
{
    return (this->pixelFormat & 0xffffffff00000000) ? false : true;
}

PVRReader::PixelFormat PVRReader::Header::GetPixelFormatValue() const
{
    //Valid if HasPixelFormatValue() == true
    if (HasPixelFormatValue())
        return static_cast<PixelFormat>(this->pixelFormat);
    else
        return PixelFormat::COUNT;
}

const char* PVRReader::Header::GetPixelFormatString() const
{
    switch (GetPixelFormatValue())
    {
        case PixelFormat::PVRTC_2BPP_RGB: return "PVRTC_2BPP_RGB";
        case PixelFormat::PVRTC_2BPP_RGBA: return "PVRTC_2BPP_RGBA";
        case PixelFormat::PVRTC_4BPP_RGB: return "PVRTC_4BPP_RGB";
        case PixelFormat::PVRTC_4BPP_RGBA: return "PVRTC_4BPP_RGBA";
        case PixelFormat::PVRTC_II_2BPP: return "PVRTC_II_2BPP";
        case PixelFormat::PVRTC_II_4BPP: return "PVRTC_II_4BPP";
        case PixelFormat::ETC1: return "ETC1";
        //case PixelFormat::DXT1: return "DXT1";
        case PixelFormat::DXT2: return "DXT2";
        //case PixelFormat::DXT3: return "DXT3";
        case PixelFormat::DXT4: return "DXT4";
        //case PixelFormat::DXT5: return "DXT5";
        case PixelFormat::BC1: return "BC1";
        case PixelFormat::BC2: return "BC2";
        case PixelFormat::BC3: return "BC3";
        case PixelFormat::BC4: return "BC4";
        case PixelFormat::BC5: return "BC5";
        case PixelFormat::BC6: return "BC6";
        case PixelFormat::BC7: return "BC7";
        case PixelFormat::UYVY: return "UYVY";
        case PixelFormat::YUY2: return "YUY2";
        case PixelFormat::BW1bpp: return "BW1bpp";
        case PixelFormat::R9G9B9E5_SHARED_EXPONENT: return "R9G9B9E5_SHARED_EXPONENT";
        case PixelFormat::RGBG8888: return "RGBG8888";
        case PixelFormat::GRGB8888: return "GRGB8888";
        case PixelFormat::ETC2_RGB: return "ETC2_RGB";
        case PixelFormat::ETC2_RGBA: return "ETC2_RGBA";
        case PixelFormat::ETC2_RGB_A1: return "ETC2_RGB_A1";
        case PixelFormat::EAC_R11: return "EAC_R11";
        case PixelFormat::EAC_RG11: return "EAC_RG11";
        case PixelFormat::ASTC_4x4: return "ASTC_4x4";
        case PixelFormat::ASTC_5x4: return "ASTC_5x4";
        case PixelFormat::ASTC_5x5: return "ASTC_5x5";
        case PixelFormat::ASTC_6x5: return "ASTC_6x5";
        case PixelFormat::ASTC_6x6: return "ASTC_6x6";
        case PixelFormat::ASTC_8x5: return "ASTC_8x5";
        case PixelFormat::ASTC_8x6: return "ASTC_8x6";
        case PixelFormat::ASTC_8x8: return "ASTC_8x8";
        case PixelFormat::ASTC_10x5: return "ASTC_10x5";
        case PixelFormat::ASTC_10x6: return "ASTC_10x6";
        case PixelFormat::ASTC_10x8: return "ASTC_10x8";
        case PixelFormat::ASTC_10x10: return "ASTC_10x10";
        case PixelFormat::ASTC_12x10: return "ASTC_12x10";
        case PixelFormat::ASTC_12x12: return "ASTC_12x12";
        case PixelFormat::ASTC_3x3x3: return "ASTC_3x3x3";
        case PixelFormat::ASTC_4x3x3: return "ASTC_4x3x3";
        case PixelFormat::ASTC_4x4x3: return "ASTC_4x4x3";
        case PixelFormat::ASTC_4x4x4: return "ASTC_4x4x4";
        case PixelFormat::ASTC_5x4x4: return "ASTC_5x4x4";
        case PixelFormat::ASTC_5x5x4: return "ASTC_5x5x4";
        case PixelFormat::ASTC_5x5x5: return "ASTC_5x5x5";
        case PixelFormat::ASTC_6x5x5: return "ASTC_6x5x5";
        case PixelFormat::ASTC_6x6x5: return "ASTC_6x6x5";
        case PixelFormat::ASTC_6x6x6: return "ASTC_6x6x6";
        default: return "<unknown>";
    }
}

PVRReader::PixelFormatChannels PVRReader::Header::GetPixelFormatChannels() const
{
    PixelFormatChannels channels;
    if (HasPixelFormatValue())
    {
        auto pixelFormat = GetPixelFormatValue();
        switch (pixelFormat)
        {
            case PixelFormat::RGBG8888:
            {
                channels.Add('r', 8);
                channels.Add('g', 8);
                channels.Add('b', 8);
                channels.Add('g', 8);
                break;
            }
            case PixelFormat::GRGB8888:
            {
                channels.Add('g', 8);
                channels.Add('r', 8);
                channels.Add('g', 8);
                channels.Add('b', 8);
                break;
            }
            default: break;
        }
    }
    else
    {
        auto bitRate = (this->pixelFormat & 0xffffffff00000000) >> 32;
        auto channelOrder = this->pixelFormat & 0x00000000ffffffff;

        uint32_t bitMask = 0xff000000;
        uint32_t shiftCount = 24;

        for (int i = 0; i < 4; i++)
        {
            auto bitsPerChannel = static_cast<uint8_t>((bitRate & bitMask) >> shiftCount);
            auto channelName = static_cast<uint8_t>((channelOrder & bitMask) >> shiftCount);

            if (!channels.Add(channelName, bitsPerChannel))
                break;

            bitMask >>= 8;
            shiftCount -= 8;
        }
    }
    return channels;
}

PVRReader::ColorSpace PVRReader::Header::GetColorSpace() const
{
    return static_cast<ColorSpace>(this->colorSpace);
}

PVRReader::ChannelType PVRReader::Header::GetChannelType() const
{
    return static_cast<ChannelType>(this->channelType);
}

bool PVRReader::Header::IsCompressed() const
{
    if (HasPixelFormatValue())
    {
        auto pixelFormat = GetPixelFormatValue();
        switch (pixelFormat)
        {
            case PixelFormat::RGBG8888:
            case PixelFormat::GRGB8888: return false;
            default: return true;
        }
    }
    else
        return false;
}

bool PVRReader::Header::HasAlpha() const
{
    if (HasPixelFormatValue())
    {
        auto pixelFormat = GetPixelFormatValue();
        switch (pixelFormat)
        {
            case PixelFormat::PVRTC_2BPP_RGBA:
            case PixelFormat::PVRTC_4BPP_RGBA:
            case PixelFormat::ETC2_RGBA:
            case PixelFormat::ETC2_RGB_A1: return true;
            default: return false;
        }
    }
    else
    {
        return GetPixelFormatChannels().HasAlpha();
    }
}

bool PVRReader::Header::IsAlphaPremultiplied() const
{
    return this->flags & 0x02 ? true : false;
}

bool PVRReader::Header::HasMipMaps() const
{
    return this->mipMapCount > 0;
}

bool PVRReader::Header::IsVolumeTexture() const
{
    return this->depth > 0;
}

//PVRReader::HeaderExtension
bool PVRReader::HeaderExtension::HasBorder() const
{
    for (int i = 0; i < FINJIN_COUNT_OF(this->border); i++)
    {
        if (this->border[i] != 0)
            return true;
    }
    return false;
}

bool PVRReader::HeaderExtension::IsCubeMap() const
{
    for (int i = 0; i < FINJIN_COUNT_OF(this->cubeMapFaceOrder); i++)
    {
        if (this->cubeMapFaceOrder[i] != 0)
            return true;
    }
    return false;
}

bool PVRReader::HeaderExtension::IsNormalMap() const
{
    for (int i = 0; i < FINJIN_COUNT_OF(this->normalMapChannels); i++)
    {
        if (this->normalMapChannels[i] != 0)
            return true;
    }
    return false;
}

//PVRReader
PVRReader::PVRReader()
{
    this->swapBytes = false;
}

PVRReader::ReadHeaderResult PVRReader::ReadHeader(ByteBufferReader& reader, Allocator* allocator)
{
    const uint32_t SIGNATURE = 0x03525650;
    const uint32_t SIGNATURE_REVERSE = 0x50565203;

    if (reader.size_left() < sizeof(uint32_t))
        return ReadHeaderResult::FAILED_TO_READ_SIGNATURE;

    //Check signature. Note that checking the signature does not modify the reader
    auto& signature = *reinterpret_cast<const uint32_t*>(reader.data_left());
    if (signature != SIGNATURE && signature != SIGNATURE_REVERSE)
        return ReadHeaderResult::INVALID_SIGNATURE;

    //Read header
    if (reader.Read(&this->header, sizeof(Header)) < sizeof(Header))
        return ReadHeaderResult::FAILED_TO_READ_HEADER;

    this->swapBytes = this->header.version == SIGNATURE_REVERSE;
    if (this->swapBytes)
        SwapHeaderBytes(header);

    this->headerExtension.Reset();

    auto metadataSizeLeft = this->header.metadataSize;
    while (metadataSizeLeft > 0)
    {
        uint32_t fourCC;
        if (reader.ReadAndSwap(&fourCC, sizeof(fourCC), this->swapBytes) != sizeof(fourCC))
            return ReadHeaderResult::BAD_METADATA_FOURCC;
        metadataSizeLeft -= sizeof(fourCC);

        uint32_t key;
        if (reader.ReadAndSwap(&key, sizeof(key), this->swapBytes) != sizeof(key))
            return ReadHeaderResult::BAD_METADATA_KEY;
        metadataSizeLeft -= sizeof(key);

        uint32_t dataSize;
        if (reader.ReadAndSwap(&dataSize, sizeof(dataSize), this->swapBytes) != sizeof(dataSize))
            return ReadHeaderResult::BAD_METADATA_SIZE;
        metadataSizeLeft -= sizeof(dataSize);

        auto handled = false;

        switch (fourCC)
        {
            case FINJIN_FOURCC('P', 'V', 'R', 3):
            {
                switch (key)
                {
                    case 0:
                    {
                        if ((dataSize % (sizeof(uint32_t) * 4)) != 0)
                            return ReadHeaderResult::FAILED_TO_COUNT_ATLAS_RECTANGLES;
                        size_t rectangleCount = dataSize / (sizeof(uint32_t) * 4);

                        if (rectangleCount > 0)
                        {
                            if (this->headerExtension.atlasRectangles.max_size() < rectangleCount)
                            {
                                if (!this->headerExtension.atlasRectangles.CreateEmpty(rectangleCount, allocator))
                                    return ReadHeaderResult::FAILED_TO_ALLOCATE_ATLAS_RECTANGLES;
                            }

                            uint32_t x, y, width, height;
                            while (dataSize > 0)
                            {
                                reader.ReadAndSwap(&x, sizeof(x), this->swapBytes);
                                reader.ReadAndSwap(&y, sizeof(y), this->swapBytes);
                                reader.ReadAndSwap(&width, sizeof(width), this->swapBytes);
                                if (reader.ReadAndSwap(&height, sizeof(height), this->swapBytes) < sizeof(height))
                                    return ReadHeaderResult::FAILED_TO_READ_ATLAS_RECTANGLE;

                                if (this->headerExtension.atlasRectangles.push_back(HeaderExtension::AtlasRectangle(x, y, width, height)).HasErrorOrValue(false))
                                    return ReadHeaderResult::FAILED_TO_ADD_ATLAS_RECTANGLE;

                                metadataSizeLeft -= sizeof(uint32_t) * 4;
                                dataSize -= sizeof(uint32_t) * 4;
                            }
                        }

                        handled = true;

                        break;
                    }
                    case 1:
                    {
                        reader.ReadAndSwap(&this->headerExtension.normalMapScale, sizeof(this->headerExtension.normalMapScale), this->swapBytes);

                        if (reader.Read(&this->headerExtension.normalMapChannels, sizeof(this->headerExtension.normalMapChannels)) < sizeof(this->headerExtension.normalMapChannels))
                            return ReadHeaderResult::FAILED_TO_READ_METADATA_NORMAL_MAP;

                        metadataSizeLeft -= sizeof(this->headerExtension.normalMapScale) + sizeof(this->headerExtension.normalMapChannels);
                        dataSize -= sizeof(this->headerExtension.normalMapScale) + sizeof(this->headerExtension.normalMapChannels);

                        handled = true;

                        break;
                    }
                    case 2:
                    {
                        if (reader.Read(&this->headerExtension.cubeMapFaceOrder, sizeof(this->headerExtension.cubeMapFaceOrder)) < sizeof(this->headerExtension.cubeMapFaceOrder))
                            return ReadHeaderResult::FAILED_TO_READ_METADATA_CUBE_MAP_FACE_ORDER;

                        metadataSizeLeft -= sizeof(this->headerExtension.cubeMapFaceOrder);
                        dataSize -= sizeof(this->headerExtension.cubeMapFaceOrder);

                        handled = true;

                        break;
                    }
                    case 3:
                    {
                        if (reader.Read(&this->headerExtension.textureOrientation, sizeof(this->headerExtension.textureOrientation)) < sizeof(this->headerExtension.textureOrientation))
                            return ReadHeaderResult::FAILED_TO_READ_METADATA_TEXTURE_ORIENTATION;

                        metadataSizeLeft -= sizeof(this->headerExtension.textureOrientation);
                        dataSize -= sizeof(this->headerExtension.textureOrientation);

                        handled = true;

                        break;
                    }
                    case 4:
                    {
                        for (int i = 0; i < FINJIN_COUNT_OF(this->headerExtension.border); i++)
                        {
                            if (reader.ReadAndSwap(&this->headerExtension.border[i], sizeof(this->headerExtension.border[i]), this->swapBytes) < sizeof(this->headerExtension.border[i]))
                                return ReadHeaderResult::FAILED_TO_READ_METADATA_BORDER_SIZE;
                        }

                        metadataSizeLeft -= sizeof(this->headerExtension.border);
                        dataSize -= sizeof(this->headerExtension.border);

                        handled = true;

                        break;
                    }
                    case 5:
                    {
                        //Skip, as per the PVR file format documentation
                        break;
                    }
                }
                break;
            }
        }

        if (!handled)
        {
            metadataSizeLeft -= dataSize;
            reader.Skip(dataSize);
        }
    }


    if (this->swapBytes)
    {
        //Requiring byte swapping is probably an error, but the caller might be able to handle it.
        return ReadHeaderResult::IMAGE_BYTE_SWAPPING_REQUIRED;
    }

    return ReadHeaderResult::SUCCESS;
}

Utf8String PVRReader::GetReadHeaderResultString(ReadHeaderResult result) const
{
    switch (result)
    {
        case ReadHeaderResult::FAILED_TO_READ_SIGNATURE: return "Failed to read PVR signature/version.";
        case ReadHeaderResult::INVALID_SIGNATURE: return "Invalid PVR signature/version.";
        case ReadHeaderResult::FAILED_TO_READ_HEADER: return "Failed to read header.";
        case ReadHeaderResult::BAD_METADATA_FOURCC: return "Failed to read metadata four CC.";
        case ReadHeaderResult::BAD_METADATA_KEY: return "Failed to read metadata key.";
        case ReadHeaderResult::BAD_METADATA_SIZE: return "Failed to read metadata data size.";
        case ReadHeaderResult::FAILED_TO_COUNT_ATLAS_RECTANGLES: return "Failed to count metadata atlas rectangles.";
        case ReadHeaderResult::FAILED_TO_ALLOCATE_ATLAS_RECTANGLES: return "Failed to allocate metadata atlas rectangels";
        case ReadHeaderResult::FAILED_TO_READ_ATLAS_RECTANGLE: return "Failed to read metadata atlas rectangle.";
        case ReadHeaderResult::FAILED_TO_ADD_ATLAS_RECTANGLE: return "Failed to add metadata atlas rectangle.";
        case ReadHeaderResult::FAILED_TO_READ_METADATA_NORMAL_MAP: return "Failed to read metadata normal map.";
        case ReadHeaderResult::FAILED_TO_READ_METADATA_CUBE_MAP_FACE_ORDER: return "Failed to read metadata cube map face order.";
        case ReadHeaderResult::FAILED_TO_READ_METADATA_TEXTURE_ORIENTATION: return "Failed to read metadata texture orientation.";
        case ReadHeaderResult::FAILED_TO_READ_METADATA_BORDER_SIZE: return "Failed to read metadata border size.";
        case ReadHeaderResult::IMAGE_BYTE_SWAPPING_REQUIRED: return "Image byte swapping required.";
        default: return Utf8String::Empty();
    }
}

void PVRReader::ReadHeader(ByteBufferReader& reader, Allocator* allocator, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto result = ReadHeader(reader, allocator);
    if (result != ReadHeaderResult::SUCCESS)
        FINJIN_SET_ERROR(error, GetReadHeaderResultString(result));
}

const PVRReader::Header& PVRReader::GetHeader() const
{
    return this->header;
}

const PVRReader::HeaderExtension& PVRReader::GetHeaderExtension() const
{
    return this->headerExtension;
}

bool PVRReader::IsImageByteSwapped() const
{
    return this->swapBytes;
}

void PVRReader::ReadImages(ByteBufferReader& reader, ImageFunction imageFunction, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    if (imageFunction == nullptr)
    {
        FINJIN_SET_ERROR(error, "No mip level function callback was specified.");
        return;
    }

    auto pixels = reader.data_left();

    auto width = this->header.width;
    auto height = this->header.height;
    auto depth = this->header.depth;

    if (!this->header.IsCompressed())
    {
        //Uncompressed

        auto bytesPerPixel = this->header.GetPixelFormatChannels().GetRoundedBytesPerPixel();

        for (uint32_t mipLevel = 0; mipLevel < 1 + this->header.mipMapCount; mipLevel++)
        {
            auto imageSizeBytes = width * height * bytesPerPixel;

            for (uint32_t surfaceIndex = 0; surfaceIndex < this->header.surfaceCount; surfaceIndex++)
            {
                for (uint32_t faceIndex = 0; faceIndex < this->header.faceCount; faceIndex++)
                {
                    for (uint32_t depthIndex = 0; depthIndex < depth; depthIndex++)
                    {
                        if (!imageFunction(this, mipLevel, surfaceIndex, faceIndex, depthIndex, width, height, imageSizeBytes, pixels))
                            break;

                        pixels += imageSizeBytes;
                    }
                }
            }

            //Move to next level
            width /= 2;
            height /= 2;
            depth /= 2;
        }
    }
    else
    {
        //Compressed

        assert(0 && "Compressed images not yet supported.");
        FINJIN_SET_ERROR(error, "Compressed images not yet supported.");

        /*for each MIP-Map Level in MIP-Map Count
            for each Surface in Num. Surfaces
                for each Face in Num. Faces
                    for each Region by aligned Depth (Based_On_PixelFormat)
                        for each Region by aligned Height (Based_On_PixelFormat)
                            for each Region by aligned Width (Based_On_PixelFormat)
                                Byte data[Size_Based_On_PixelFormat] */
    }
}
