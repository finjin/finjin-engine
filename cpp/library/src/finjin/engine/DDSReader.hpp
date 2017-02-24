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


//Includes---------------------------------------------------------------------
#include "finjin/common/AllocatedVector.hpp"
#include "finjin/common/Error.hpp"
#if FINJIN_TARGET_GPU_SYSTEM == FINJIN_TARGET_GPU_SYSTEM_D3D12
    #include "finjin/common/ByteBuffer.hpp"
    #include <wtypes.h>
    #include <wrl.h>
    #include <dxgi1_4.h>
    #include <d3d12.h>
#endif


//Macros-----------------------------------------------------------------------
#define FINJIN_DDS_MAGIC FINJIN_FOURCC('D', 'D', 'S', ' ') //0x20534444 
#define FINJIN_DDS_FOURCC 0x00000004


//Classes----------------------------------------------------------------------
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

        #if FINJIN_TARGET_GPU_SYSTEM == FINJIN_TARGET_GPU_SYSTEM_D3D12
            DXGI_FORMAT GetDXGIFormat() const;
        #endif
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
        #if FINJIN_TARGET_GPU_SYSTEM == FINJIN_TARGET_GPU_SYSTEM_D3D12
            DXGI_FORMAT dxgiFormat;
        #else
            uint32_t dxgiFormat;
        #endif
            uint32_t resourceDimension;
            uint32_t miscFlag; // see D3D11_RESOURCE_MISC_FLAG
            uint32_t arraySize;
            uint32_t miscFlags2;

            bool IsCube() const;
        };
    #pragma pack(pop)

        DDSReader();

        enum class ReadHeaderResult
        {
            SUCCESS,
            FAILED_TO_READ_MAGIC_VALUE,
            INVALID_MAGIC_VALUE,
            INVALID_HEADER_SIZE,
            INVALID_PIXEL_FORMAT_SIZE,
            NO_DXT10_HEADER
        };
        ReadHeaderResult ReadHeader(const void* bytes, size_t byteCount);
        Utf8String GetReadHeaderResultString(ReadHeaderResult result) const;
        
        void ReadHeader(const void* bytes, size_t byteCount, Error& error);
        
        const Header& GetHeader() const;
        const DX10HeaderExtension* GetHeaderDX10Extension() const;

    private:        
        Header header;
        DX10HeaderExtension headerDX10;
        bool isHeaderDX10;
    };

} }
