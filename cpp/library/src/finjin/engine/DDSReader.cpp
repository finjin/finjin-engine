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
#include "DDSReader.hpp"

#if FINJIN_TARGET_GPU_SYSTEM == FINJIN_TARGET_GPU_SYSTEM_D3D12
    #include "finjin/engine/internal/gpu/d3d12/D3D12Utilities.hpp"
    using Microsoft::WRL::ComPtr;
#endif

using namespace Finjin::Common;
using namespace Finjin::Engine;


//Macros-----------------------------------------------------------------------
#define DDS_HEADER_FLAGS_TEXTURE        0x00001007  //DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT  
#define DDS_HEADER_FLAGS_MIPMAP         0x00020000  //DDSD_MIPMAPCOUNT 
#define DDS_HEADER_FLAGS_VOLUME         0x00800000  //DDSD_DEPTH 
#define DDS_HEADER_FLAGS_PITCH          0x00000008  //DDSD_PITCH 
#define DDS_HEADER_FLAGS_LINEARSIZE     0x00080000  //DDSD_LINEARSIZE 

#define DDS_HEIGHT 0x00000002 //DDSD_HEIGHT 
#define DDS_WIDTH  0x00000004 //DDSD_WIDTH 

#define DDS_SURFACE_FLAGS_TEXTURE 0x00001000 //DDSCAPS_TEXTURE 
#define DDS_SURFACE_FLAGS_MIPMAP  0x00400008 //DDSCAPS_COMPLEX | DDSCAPS_MIPMAP 
#define DDS_SURFACE_FLAGS_CUBEMAP 0x00000008 //DDSCAPS_COMPLEX 

#define DDS_CUBEMAP_POSITIVEX 0x00000600 //DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_POSITIVEX 
#define DDS_CUBEMAP_NEGATIVEX 0x00000a00 //DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_NEGATIVEX 
#define DDS_CUBEMAP_POSITIVEY 0x00001200 //DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_POSITIVEY 
#define DDS_CUBEMAP_NEGATIVEY 0x00002200 //DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_NEGATIVEY 
#define DDS_CUBEMAP_POSITIVEZ 0x00004200 //DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_POSITIVEZ 
#define DDS_CUBEMAP_NEGATIVEZ 0x00008200 //DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_NEGATIVEZ 

#define DDS_CUBEMAP_ALLFACES ( DDS_CUBEMAP_POSITIVEX | DDS_CUBEMAP_NEGATIVEX | DDS_CUBEMAP_POSITIVEY | DDS_CUBEMAP_NEGATIVEY | DDS_CUBEMAP_POSITIVEZ | DDS_CUBEMAP_NEGATIVEZ)

#define DDS_CUBEMAP 0x00000200 //DDSCAPS2_CUBEMAP 

#define DDS_FLAGS_VOLUME 0x00200000 //DDSCAPS2_VOLUME 

#define IS_SAME_BIT_MASK(r, g, b, a) (this->RBitMask == r && this->GBitMask == g && this->BBitMask == b && this->ABitMask == a)
#define DDS_RGB 0x00000040
#define DDS_LUMINANCE 0x00020000
#define DDS_ALPHA 0x00000002

enum DDS_RESOURCE_MISC_FLAG
{
    DDS_RESOURCE_MISC_TEXTURECUBE = 0x4L
};

enum DDS_MISC_FLAGS2
{
    DDS_MISC_FLAGS2_ALPHA_MODE_MASK = 0x7L
};


//Implementation------------------------------------------------------

//DDSReader::PixelFormat
#if FINJIN_TARGET_GPU_SYSTEM == FINJIN_TARGET_GPU_SYSTEM_D3D12
DXGI_FORMAT DDSReader::PixelFormat::GetDXGIFormat() const
{
    if (this->flags & DDS_RGB)
    {
        //Note that sRGB formats are written using the "DX10" extended header

        switch (this->RGBBitCount)
        {
            case 32:
            {
                if (IS_SAME_BIT_MASK(0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000))
                    return DXGI_FORMAT_R8G8B8A8_UNORM;
                else if (IS_SAME_BIT_MASK(0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000))
                    return DXGI_FORMAT_B8G8R8A8_UNORM;
                else if (IS_SAME_BIT_MASK(0x00ff0000, 0x0000ff00, 0x000000ff, 0x00000000))
                    return DXGI_FORMAT_B8G8R8X8_UNORM;

                //No DXGI format maps to IS_SAME_BIT_MASK(0x000000ff,0x0000ff00,0x00ff0000,0x00000000) aka D3DFMT_X8B8G8R8

                //Note that many common DDS reader/writers (including D3DX) swap the
                //the RED/BLUE masks for 10:10:10:2 formats. We assume
                //below that the 'backwards' header mask is being used since it is most
                //likely written by D3DX. The more robust solution is to use the 'DX10'
                //header extension and specify the DXGI_FORMAT_R10G10B10A2_UNORM format directly

                //For 'correct' writers, this should be 0x000003ff,0x000ffc00,0x3ff00000 for RGB data
                else if (IS_SAME_BIT_MASK(0x3ff00000, 0x000ffc00, 0x000003ff, 0xc0000000))
                    return DXGI_FORMAT_R10G10B10A2_UNORM;

                //No DXGI format maps to IS_SAME_BIT_MASK(0x000003ff,0x000ffc00,0x3ff00000,0xc0000000) aka D3DFMT_A2R10G10B10

                else if (IS_SAME_BIT_MASK(0x0000ffff, 0xffff0000, 0x00000000, 0x00000000))
                    return DXGI_FORMAT_R16G16_UNORM;
                else if (IS_SAME_BIT_MASK(0xffffffff, 0x00000000, 0x00000000, 0x00000000))
                {
                    //Only 32-bit color channel format in D3D9 was R32F
                    return DXGI_FORMAT_R32_FLOAT; //D3DX writes this out as a FourCC of 114
                }
                break;
            }
            case 24:
            {
                //No 24bpp DXGI formats aka D3DFMT_R8G8B8
                break;
            }
            case 16:
            {
                if (IS_SAME_BIT_MASK(0x7c00, 0x03e0, 0x001f, 0x8000))
                    return DXGI_FORMAT_B5G5R5A1_UNORM;
                else if (IS_SAME_BIT_MASK(0xf800, 0x07e0, 0x001f, 0x0000))
                    return DXGI_FORMAT_B5G6R5_UNORM;

                //No DXGI format maps to IS_SAME_BIT_MASK(0x7c00,0x03e0,0x001f,0x0000) aka D3DFMT_X1R5G5B5
                else if (IS_SAME_BIT_MASK(0x0f00, 0x00f0, 0x000f, 0xf000))
                    return DXGI_FORMAT_B4G4R4A4_UNORM;

                //No DXGI format maps to IS_SAME_BIT_MASK(0x0f00,0x00f0,0x000f,0x0000) aka D3DFMT_X4R4G4B4

                //No 3:3:2, 3:3:2:8, or paletted DXGI formats aka D3DFMT_A8R3G3B2, D3DFMT_R3G3B2, D3DFMT_P8, D3DFMT_A8P8, etc.
                break;
            }
        }
    }
    else if (this->flags & DDS_LUMINANCE)
    {
        if (this->RGBBitCount == 8)
        {
            if (IS_SAME_BIT_MASK(0x000000ff, 0x00000000, 0x00000000, 0x00000000))
                return DXGI_FORMAT_R8_UNORM; //D3DX10/11 writes this out as DX10 extension
            
            //No DXGI format maps to IS_SAME_BIT_MASK(0x0f,0x00,0x00,0xf0) aka D3DFMT_A4L4
        }
        else if (this->RGBBitCount == 16)
        {
            if (IS_SAME_BIT_MASK(0x0000ffff, 0x00000000, 0x00000000, 0x00000000))
                return DXGI_FORMAT_R16_UNORM; //D3DX10/11 writes this out as DX10 extension
            if (IS_SAME_BIT_MASK(0x000000ff, 0x00000000, 0x00000000, 0x0000ff00))
                return DXGI_FORMAT_R8G8_UNORM; //D3DX10/11 writes this out as DX10 extension
        }
    }
    else if (this->flags & DDS_ALPHA)
    {
        if (this->RGBBitCount == 8)
            return DXGI_FORMAT_A8_UNORM;
    }
    else if (this->flags & FINJIN_DDS_FOURCC)
    {
        if (this->fourCC == FINJIN_FOURCC('D', 'X', 'T', '1'))
            return DXGI_FORMAT_BC1_UNORM;
        else if (this->fourCC == FINJIN_FOURCC('D', 'X', 'T', '3'))
            return DXGI_FORMAT_BC2_UNORM;
        else if (this->fourCC == FINJIN_FOURCC('D', 'X', 'T', '5'))
            return DXGI_FORMAT_BC3_UNORM;
        
        //While pre-multiplied alpha isn't directly supported by the DXGI formats, they are basically the same as these BC formats
        else if (this->fourCC == FINJIN_FOURCC('D', 'X', 'T', '2'))
            return DXGI_FORMAT_BC2_UNORM;
        else if (this->fourCC == FINJIN_FOURCC('D', 'X', 'T', '4'))
            return DXGI_FORMAT_BC3_UNORM;
        
        else if (this->fourCC == FINJIN_FOURCC('A', 'T', 'I', '1'))
            return DXGI_FORMAT_BC4_UNORM;
        else if (this->fourCC == FINJIN_FOURCC('B', 'C', '4', 'U'))
            return DXGI_FORMAT_BC4_UNORM;
        else if (this->fourCC == FINJIN_FOURCC('B', 'C', '4', 'S'))
            return DXGI_FORMAT_BC4_SNORM;
        
        else if (this->fourCC == FINJIN_FOURCC('A', 'T', 'I', '2'))
            return DXGI_FORMAT_BC5_UNORM;
        else if (this->fourCC == FINJIN_FOURCC('B', 'C', '5', 'U'))
            return DXGI_FORMAT_BC5_UNORM;
        else if (this->fourCC == FINJIN_FOURCC('B', 'C', '5', 'S'))
            return DXGI_FORMAT_BC5_SNORM;
        
        //BC6H and BC7 are written using the "DX10" extended header
        else if (this->fourCC == FINJIN_FOURCC('R', 'G', 'B', 'G'))
            return DXGI_FORMAT_R8G8_B8G8_UNORM;
        else if (this->fourCC == FINJIN_FOURCC('G', 'R', 'G', 'B'))
            return DXGI_FORMAT_G8R8_G8B8_UNORM;
        
        else if (this->fourCC == FINJIN_FOURCC('Y', 'U', 'Y', '2'))
            return DXGI_FORMAT_YUY2;
        
        //Check for D3DFORMAT enums being set here
        switch (this->fourCC)
        {
            case 36: return DXGI_FORMAT_R16G16B16A16_UNORM;
            case 110: return DXGI_FORMAT_R16G16B16A16_SNORM;
            case 111: return DXGI_FORMAT_R16_FLOAT;
            case 112: return DXGI_FORMAT_R16G16_FLOAT;
            case 113: return DXGI_FORMAT_R16G16B16A16_FLOAT;
            case 114: return DXGI_FORMAT_R32_FLOAT;
            case 115: return DXGI_FORMAT_R32G32_FLOAT;
            case 116: return DXGI_FORMAT_R32G32B32A32_FLOAT;
        }
    }

    return DXGI_FORMAT_UNKNOWN;
}
#endif

//DDSReader::Header
DDSReader::AlphaMode DDSReader::Header::GetAlphaMode() const
{
    if (this->ddspf.flags & FINJIN_DDS_FOURCC)
    {
        if (this->ddspf.fourCC == FINJIN_FOURCC('D', 'X', '1', '0'))
        {
            auto d3d10HeaderExtension = reinterpret_cast<const DDSReader::DX10HeaderExtension*>((const uint8_t*)this + sizeof(DDSReader::Header));
            
            auto mode = static_cast<DDSReader::AlphaMode>(d3d10HeaderExtension->miscFlags2 & DDS_MISC_FLAGS2_ALPHA_MODE_MASK);
            switch (mode)
            {
                case DDSReader::AlphaMode::STRAIGHT_ALPHA:
                case DDSReader::AlphaMode::PREMULTIPLIED_ALPHA:
                case DDSReader::AlphaMode::OPAQUE_ALPHA:
                case DDSReader::AlphaMode::CUSTOM_ALPHA: return mode;
                default: break;
            }
        }
        else if (this->ddspf.fourCC == FINJIN_FOURCC('D', 'X', 'T', '2') || this->ddspf.fourCC == FINJIN_FOURCC('D', 'X', 'T', '4'))
            return DDSReader::AlphaMode::PREMULTIPLIED_ALPHA;
    }

    return DDSReader::AlphaMode::UNKNOWN_ALPHA;
}

bool DDSReader::Header::IsVolume() const
{
    return (this->flags & DDS_HEADER_FLAGS_VOLUME) != 0;
}

bool DDSReader::Header::IsCube() const
{
    return (this->caps2 & DDS_CUBEMAP) != 0;
}

bool DDSReader::Header::IsCubeAllFaces() const
{
    return (this->caps2 & DDS_CUBEMAP_ALLFACES) == DDS_CUBEMAP_ALLFACES;
}

bool DDSReader::Header::HasWidth() const
{
    return (this->flags & DDS_WIDTH) != 0;
}

bool DDSReader::Header::HasHeight() const
{
    return (this->flags & DDS_HEIGHT) != 0;
}

//DDSReader::DX10HeaderExtension
bool DDSReader::DX10HeaderExtension::IsCube() const
{
    return (this->miscFlag & DDS_RESOURCE_MISC_TEXTURECUBE) != 0;
}

//DDSReader
DDSReader::DDSReader()
{
    this->isHeaderDX10 = false;
}

DDSReader::ReadHeaderResult DDSReader::ReadHeader(const void* vbytes, size_t byteCount)
{
    auto bytes = static_cast<const uint8_t*>(vbytes);

    if (bytes == 0 || byteCount == 0)
        return ReadHeaderResult::FAILED_TO_READ_MAGIC_VALUE;
    
    auto magicNumber = *reinterpret_cast<const uint32_t*>(bytes);
    if (magicNumber != FINJIN_DDS_MAGIC)
        return ReadHeaderResult::INVALID_MAGIC_VALUE;

    auto headerBytes = &bytes[sizeof(uint32_t)];
    auto header = reinterpret_cast<const Header*>(headerBytes);

    if (header->size != sizeof(Header))
        return ReadHeaderResult::INVALID_HEADER_SIZE;

    if (header->ddspf.size != sizeof(PixelFormat))
        return ReadHeaderResult::INVALID_PIXEL_FORMAT_SIZE;

    FINJIN_COPY_MEMORY(&this->header, header, sizeof(Header));

    this->isHeaderDX10 = false;
    if ((header->ddspf.flags & FINJIN_DDS_FOURCC) != 0 && header->ddspf.fourCC == FINJIN_FOURCC('D', 'X', '1', '0'))
    {
        if (byteCount < (sizeof(Header) + sizeof(uint32_t) + sizeof(DX10HeaderExtension)))
            return ReadHeaderResult::NO_DXT10_HEADER;

        this->isHeaderDX10 = true;
        FINJIN_COPY_MEMORY(&this->headerDX10, &headerBytes[sizeof(Header)], sizeof(DX10HeaderExtension));
    }

    return DDSReader::ReadHeaderResult::SUCCESS;
}

Utf8String DDSReader::GetReadHeaderResultString(ReadHeaderResult result) const
{
    switch (result)
    {
        case ReadHeaderResult::FAILED_TO_READ_MAGIC_VALUE: return "No DDS bytes were specified.";
        case ReadHeaderResult::INVALID_MAGIC_VALUE: return "Invalid DDS magic number.";
        case ReadHeaderResult::INVALID_HEADER_SIZE: return "Invalid header size.";
        case ReadHeaderResult::INVALID_PIXEL_FORMAT_SIZE: return "Invalid pixel format size.";
        case ReadHeaderResult::NO_DXT10_HEADER: return "Header indicated a DXT10 header format but no DXT10 header could be found.";
        default: return Utf8String::Empty();
    }
}

void DDSReader::ReadHeader(const void* bytes, size_t byteCount, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto result = ReadHeader(bytes, byteCount);
    if (result != ReadHeaderResult::SUCCESS)
        FINJIN_SET_ERROR(error, GetReadHeaderResultString(result));
}

const DDSReader::Header& DDSReader::GetHeader() const
{
    return this->header;
}

const DDSReader::DX10HeaderExtension* DDSReader::GetHeaderDX10Extension() const
{
    if (this->isHeaderDX10)
        return &this->headerDX10;
    else
        return nullptr;
}
