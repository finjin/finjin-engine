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
#include "finjin/common/Allocator.hpp"
#include "finjin/common/ByteBuffer.hpp"
#include "finjin/common/DynamicVector.hpp"
#include "finjin/common/Error.hpp"
#include "finjin/common/StaticVector.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    //http://cdn.imgtec.com/sdk-documentation/PVR+File+Format.Specification.pdf
    class PVRReader
    {
    public:
        enum class PixelFormat : uint64_t
        {
            PVRTC_2BPP_RGB = 0,
            PVRTC_2BPP_RGBA = 1,
            PVRTC_4BPP_RGB = 2,
            PVRTC_4BPP_RGBA = 3,
            PVRTC_II_2BPP = 4,
            PVRTC_II_4BPP = 5,
            ETC1 = 6,
            //DXT1 = 7, //Same as BC1
            DXT2 = 8,
            //DXT3 = 9, //Same as BC2
            DXT4 = 10,
            //DXT5 = 11, //Same as BC3
            BC1 = 7,
            BC2 = 9,
            BC3 = 11,
            BC4 = 12,
            BC5 = 13,
            BC6 = 14,
            BC7 = 15,
            UYVY = 16,
            YUY2 = 17,
            BW1bpp = 18,
            R9G9B9E5_SHARED_EXPONENT = 19,
            RGBG8888 = 20,
            GRGB8888 = 21,
            ETC2_RGB = 22,
            ETC2_RGBA = 23,
            ETC2_RGB_A1 = 24,
            EAC_R11 = 25,
            EAC_RG11 = 26,
            ASTC_4x4 = 27,
            ASTC_5x4 = 28,
            ASTC_5x5 = 29,
            ASTC_6x5 = 30,
            ASTC_6x6 = 31,
            ASTC_8x5 = 32,
            ASTC_8x6 = 33,
            ASTC_8x8 = 34,
            ASTC_10x5 = 35,
            ASTC_10x6 = 36,
            ASTC_10x8 = 37,
            ASTC_10x10 = 38,
            ASTC_12x10 = 39,
            ASTC_12x12 = 40,
            ASTC_3x3x3 = 41,
            ASTC_4x3x3 = 42,
            ASTC_4x4x3 = 43,
            ASTC_4x4x4 = 44,
            ASTC_5x4x4 = 45,
            ASTC_5x5x4 = 46,
            ASTC_5x5x5 = 47,
            ASTC_6x5x5 = 48,
            ASTC_6x6x5 = 49,
            ASTC_6x6x6 = 50,

            COUNT = 51
        };

        enum class ColorSpace : uint32_t
        {
            LINEAR_RGB = 0,
            SRGB = 1
        };

        enum class ChannelType : uint32_t
        {
            UNSIGNED_BYTE_NORMALIZED = 0,
            SIGNED_BYTE_NORMALIZED = 1,
            UNSIGNED_BYTE = 2,
            SIGNED_BYTE = 3,
            UNSIGNED_SHORT_NORMALIZED = 4,
            SIGNED_SHORT_NORMALIZED = 5,
            UNSIGNED_SHORT = 6,
            SIGNED_SHORT = 7,
            UNSIGNED_INTEGER_NORMALIZED = 8,
            SIGNED_INTEGER_NORMALIZED = 9,
            UNSIGNED_INTEGER = 10,
            SIGNED_INTEGER = 11,
            FLOAT_32 = 12
        };

        struct PixelFormatChannels
        {
            PixelFormatChannels()
            {
                FINJIN_ZERO_ITEM(*this);
            }

            bool Add(uint8_t name, uint8_t bitsPerChannel)
            {
                if (name != 0 && !this->channelNames.full())
                {
                    this->channelNames.push_back(name);
                    this->bitsPerChannel.push_back(bitsPerChannel);
                    return true;
                }
                else
                    return false;
            }

            bool HasAlpha() const
            {
                return this->channelNames.contains('a');
            }

            uint32_t GetBitsPerPixel() const
            {
                uint32_t total = 0;
                for (auto bits : this->bitsPerChannel)
                    total += bits;
                return total;
            }

            uint32_t GetRoundedBytesPerPixel() const
            {
                return (GetBitsPerPixel() + 7) / 8;
            }

            StaticVector<uint8_t, 4> channelNames; //Valid values: 'r', 'g', 'b', 'a'
            StaticVector<uint8_t, 4> bitsPerChannel;
        };

    #pragma pack(push, 1)
        struct Header
        {
            Header()
            {
                FINJIN_ZERO_ITEM(*this);
            }

            uint32_t version;
            uint32_t flags;
            uint64_t pixelFormat;
            uint32_t colorSpace;
            uint32_t channelType;
            uint32_t height;
            uint32_t width;
            uint32_t depth;
            uint32_t surfaceCount;
            uint32_t faceCount;
            uint32_t mipMapCount;
            uint32_t metadataSize;

            bool HasPixelFormatValue() const;
            PixelFormat GetPixelFormatValue() const;
            const char* GetPixelFormatString() const;
            PixelFormatChannels GetPixelFormatChannels() const;
            ColorSpace GetColorSpace() const;
            ChannelType GetChannelType() const;
            bool IsCompressed() const;
            bool HasAlpha() const;
            bool IsAlphaPremultiplied() const;
            bool HasMipMaps() const;
            bool IsVolumeTexture() const;
        };
    #pragma pack(pop)

        struct HeaderExtension
        {
            HeaderExtension()
            {
                Reset();
            }

            void Reset()
            {
                this->normalMapScale = 0;
                FINJIN_ZERO_ITEM(this->normalMapChannels);
                FINJIN_ZERO_ITEM(this->cubeMapFaceOrder);
                FINJIN_ZERO_ITEM(this->textureOrientation);
                FINJIN_ZERO_ITEM(this->border);

                this->atlasRectangles.clear();
            }

            bool HasBorder() const;
            bool IsCubeMap() const;
            bool IsNormalMap() const;

            float normalMapScale;
            uint8_t normalMapChannels[4];

            uint8_t cubeMapFaceOrder[6];

            uint8_t textureOrientation[3];

            uint32_t border[3];

            struct AtlasRectangle
            {
                AtlasRectangle() : x(0), y(0), width(0), height(0) {}
                AtlasRectangle(uint32_t _x, uint32_t _y, uint32_t _width, uint32_t _height) : x(_x), y(_y), width(_width), height(_height) {}

                uint32_t x, y, width, height;
            };
            DynamicVector<AtlasRectangle> atlasRectangles;
        };

        PVRReader();

        enum class ReadHeaderResult
        {
            SUCCESS,
            FAILED_TO_READ_SIGNATURE,
            INVALID_SIGNATURE,
            FAILED_TO_READ_HEADER,
            BAD_METADATA_FOURCC,
            BAD_METADATA_KEY,
            BAD_METADATA_SIZE,
            FAILED_TO_COUNT_ATLAS_RECTANGLES,
            FAILED_TO_ALLOCATE_ATLAS_RECTANGLES,
            FAILED_TO_READ_ATLAS_RECTANGLE,
            FAILED_TO_ADD_ATLAS_RECTANGLE,
            FAILED_TO_READ_METADATA_NORMAL_MAP,
            FAILED_TO_READ_METADATA_CUBE_MAP_FACE_ORDER,
            FAILED_TO_READ_METADATA_TEXTURE_ORIENTATION,
            FAILED_TO_READ_METADATA_BORDER_SIZE,
            IMAGE_BYTE_SWAPPING_REQUIRED
        };
        ReadHeaderResult ReadHeader(ByteBufferReader& reader, Allocator* allocator);
        Utf8String GetReadHeaderResultString(ReadHeaderResult result) const;

        void ReadHeader(ByteBufferReader& reader, Allocator* allocator, Error& error);

        const Header& GetHeader() const;
        const HeaderExtension& GetHeaderExtension() const;

        bool IsImageByteSwapped() const;

        //Return: true to continue reading mips, false to stop
        //Parameters: reader, mip level, surface index, face index, depth index, width, height, mip image size in bytes, mip image bytes
        using ImageFunction = std::function<bool(const PVRReader*, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, const void*)>;

        void ReadImages(ByteBufferReader& reader, ImageFunction imageFunction, Error& error);

    private:
        Header header;
        HeaderExtension headerExtension;
        bool swapBytes;
    };

} }
