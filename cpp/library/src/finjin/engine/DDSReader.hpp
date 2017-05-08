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
#include "finjin/common/ByteBuffer.hpp"
#include "finjin/common/DynamicVector.hpp"
#include "finjin/common/Error.hpp"
#include "finjin/engine/DXGIIncludes.hpp"
#include "finjin/engine/TextureDimension.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    class DDSReader
    {
    public:
        enum class AlphaMode
        {
            UNKNOWN_ALPHA = 0,
            STRAIGHT_ALPHA = 1,
            PREMULTIPLIED_ALPHA = 2,
            OPAQUE_ALPHA = 3,
            CUSTOM_ALPHA = 4
        };

    #pragma pack(push, 1)
        struct PixelFormat
        {
            uint32_t size;
            uint32_t flags;
            uint32_t fourCC;
            uint32_t RGBBitCount;
            uint32_t RBitMask;
            uint32_t GBitMask;
            uint32_t BBitMask;
            uint32_t ABitMask;

            DXGI_FORMAT GetDXGIFormat() const;
        };

        struct Header
        {
            uint32_t size;
            uint32_t flags;
            uint32_t height;
            uint32_t width;
            uint32_t pitchOrLinearSize;
            uint32_t depth;  //If IsVolumeTexture() returns true
            uint32_t mipMapCount;
            uint32_t reserved1[11];
            PixelFormat ddspf;
            uint32_t caps;
            uint32_t caps2;
            uint32_t caps3;
            uint32_t caps4;
            uint32_t reserved2;

            AlphaMode GetAlphaMode() const;
            bool IsVolume() const;
            bool IsCube() const;
            bool IsCubeAllFaces() const;
            bool HasWidth() const;
            bool HasHeight() const;
        };

        struct DX10HeaderExtension
        {
            uint32_t dxgiFormat;
            uint32_t resourceDimension;
            uint32_t miscFlag; //See D3D11_RESOURCE_MISC_FLAG
            uint32_t arraySize;
            uint32_t miscFlags2;

            bool IsCube() const;
        };
    #pragma pack(pop)

        DDSReader();

        enum class ReadHeaderResult
        {
            SUCCESS,
            FAILED_TO_READ_SIGNATURE,
            INVALID_SIGNATURE,
            FAILED_TO_READ_HEADER,
            IMAGE_BYTE_SWAPPING_REQUIRED,
            INVALID_HEADER_SIZE,
            INVALID_PIXEL_FORMAT_SIZE,
            FAILED_TO_READ_DXT10_HEADER
        };
        ReadHeaderResult ReadHeader(ByteBufferReader& reader);
        Utf8String GetReadHeaderResultString(ReadHeaderResult result) const;

        void ReadHeader(ByteBufferReader& reader, Error& error);

        const Header& GetHeader() const;
        const DX10HeaderExtension* GetHeaderDX10Extension() const;

        DXGI_FORMAT GetDXGIFormat() const;

        bool IsCube() const;
        bool IsArray() const;

        TextureDimension GetDimension() const;

        bool IsImageByteSwapped() const;

    private:
        Header header;
        DX10HeaderExtension headerDX10;
        bool isHeaderDX10;
        bool swapBytes;
    };

} }
