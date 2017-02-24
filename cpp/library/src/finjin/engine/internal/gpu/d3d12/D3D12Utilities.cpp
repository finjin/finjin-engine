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

#if FINJIN_TARGET_GPU_SYSTEM == FINJIN_TARGET_GPU_SYSTEM_D3D12

#include "D3D12Utilities.hpp"
#include "finjin/common/Convert.hpp"

using namespace Finjin::Engine;


//Local functions--------------------------------------------------------------
static uint64_t GetSubresourceFootprintByteCount(size_t subresourceCount)
{
    //From UpdateSubresources() in d3dx12.h
    return (sizeof(D3D12_PLACED_SUBRESOURCE_FOOTPRINT) + sizeof(UINT) + sizeof(uint64_t)) * subresourceCount;
}

static UINT GetTextureBitsPerPixel(DXGI_FORMAT format)
{
    switch (format)
    {
        case DXGI_FORMAT_R32G32B32A32_TYPELESS:
        case DXGI_FORMAT_R32G32B32A32_FLOAT:
        case DXGI_FORMAT_R32G32B32A32_UINT:
        case DXGI_FORMAT_R32G32B32A32_SINT: return 128;

        case DXGI_FORMAT_R32G32B32_TYPELESS:
        case DXGI_FORMAT_R32G32B32_FLOAT:
        case DXGI_FORMAT_R32G32B32_UINT:
        case DXGI_FORMAT_R32G32B32_SINT: return 96;

        case DXGI_FORMAT_R16G16B16A16_TYPELESS:
        case DXGI_FORMAT_R16G16B16A16_FLOAT:
        case DXGI_FORMAT_R16G16B16A16_UNORM:
        case DXGI_FORMAT_R16G16B16A16_UINT:
        case DXGI_FORMAT_R16G16B16A16_SNORM:
        case DXGI_FORMAT_R16G16B16A16_SINT:
        case DXGI_FORMAT_R32G32_TYPELESS:
        case DXGI_FORMAT_R32G32_FLOAT:
        case DXGI_FORMAT_R32G32_UINT:
        case DXGI_FORMAT_R32G32_SINT:
        case DXGI_FORMAT_R32G8X24_TYPELESS:
        case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
        case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
        case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
        case DXGI_FORMAT_Y416:
        case DXGI_FORMAT_Y210:
        case DXGI_FORMAT_Y216: return 64;

        case DXGI_FORMAT_R10G10B10A2_TYPELESS:
        case DXGI_FORMAT_R10G10B10A2_UNORM:
        case DXGI_FORMAT_R10G10B10A2_UINT:
        case DXGI_FORMAT_R11G11B10_FLOAT:
        case DXGI_FORMAT_R8G8B8A8_TYPELESS:
        case DXGI_FORMAT_R8G8B8A8_UNORM:
        case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
        case DXGI_FORMAT_R8G8B8A8_UINT:
        case DXGI_FORMAT_R8G8B8A8_SNORM:
        case DXGI_FORMAT_R8G8B8A8_SINT:
        case DXGI_FORMAT_R16G16_TYPELESS:
        case DXGI_FORMAT_R16G16_FLOAT:
        case DXGI_FORMAT_R16G16_UNORM:
        case DXGI_FORMAT_R16G16_UINT:
        case DXGI_FORMAT_R16G16_SNORM:
        case DXGI_FORMAT_R16G16_SINT:
        case DXGI_FORMAT_R32_TYPELESS:
        case DXGI_FORMAT_D32_FLOAT:
        case DXGI_FORMAT_R32_FLOAT:
        case DXGI_FORMAT_R32_UINT:
        case DXGI_FORMAT_R32_SINT:
        case DXGI_FORMAT_R24G8_TYPELESS:
        case DXGI_FORMAT_D24_UNORM_S8_UINT:
        case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
        case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
        case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
        case DXGI_FORMAT_R8G8_B8G8_UNORM:
        case DXGI_FORMAT_G8R8_G8B8_UNORM:
        case DXGI_FORMAT_B8G8R8A8_UNORM:
        case DXGI_FORMAT_B8G8R8X8_UNORM:
        case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
        case DXGI_FORMAT_B8G8R8A8_TYPELESS:
        case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
        case DXGI_FORMAT_B8G8R8X8_TYPELESS:
        case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
        case DXGI_FORMAT_AYUV:
        case DXGI_FORMAT_Y410:
        case DXGI_FORMAT_YUY2: return 32;

        case DXGI_FORMAT_P010:
        case DXGI_FORMAT_P016: return 24;

        case DXGI_FORMAT_R8G8_TYPELESS:
        case DXGI_FORMAT_R8G8_UNORM:
        case DXGI_FORMAT_R8G8_UINT:
        case DXGI_FORMAT_R8G8_SNORM:
        case DXGI_FORMAT_R8G8_SINT:
        case DXGI_FORMAT_R16_TYPELESS:
        case DXGI_FORMAT_R16_FLOAT:
        case DXGI_FORMAT_D16_UNORM:
        case DXGI_FORMAT_R16_UNORM:
        case DXGI_FORMAT_R16_UINT:
        case DXGI_FORMAT_R16_SNORM:
        case DXGI_FORMAT_R16_SINT:
        case DXGI_FORMAT_B5G6R5_UNORM:
        case DXGI_FORMAT_B5G5R5A1_UNORM:
        case DXGI_FORMAT_A8P8:
        case DXGI_FORMAT_B4G4R4A4_UNORM: return 16;

        case DXGI_FORMAT_NV12:
        case DXGI_FORMAT_420_OPAQUE:
        case DXGI_FORMAT_NV11: return 12;

        case DXGI_FORMAT_R8_TYPELESS:
        case DXGI_FORMAT_R8_UNORM:
        case DXGI_FORMAT_R8_UINT:
        case DXGI_FORMAT_R8_SNORM:
        case DXGI_FORMAT_R8_SINT:
        case DXGI_FORMAT_A8_UNORM:
        case DXGI_FORMAT_AI44:
        case DXGI_FORMAT_IA44:
        case DXGI_FORMAT_P8: return 8;

        case DXGI_FORMAT_R1_UNORM: return 1;

        case DXGI_FORMAT_BC1_TYPELESS:
        case DXGI_FORMAT_BC1_UNORM:
        case DXGI_FORMAT_BC1_UNORM_SRGB:
        case DXGI_FORMAT_BC4_TYPELESS:
        case DXGI_FORMAT_BC4_UNORM:
        case DXGI_FORMAT_BC4_SNORM: return 4;

        case DXGI_FORMAT_BC2_TYPELESS:
        case DXGI_FORMAT_BC2_UNORM:
        case DXGI_FORMAT_BC2_UNORM_SRGB:
        case DXGI_FORMAT_BC3_TYPELESS:
        case DXGI_FORMAT_BC3_UNORM:
        case DXGI_FORMAT_BC3_UNORM_SRGB:
        case DXGI_FORMAT_BC5_TYPELESS:
        case DXGI_FORMAT_BC5_UNORM:
        case DXGI_FORMAT_BC5_SNORM:
        case DXGI_FORMAT_BC6H_TYPELESS:
        case DXGI_FORMAT_BC6H_UF16:
        case DXGI_FORMAT_BC6H_SF16:
        case DXGI_FORMAT_BC7_TYPELESS:
        case DXGI_FORMAT_BC7_UNORM:
        case DXGI_FORMAT_BC7_UNORM_SRGB: return 8;

        default: return 0;
    }
}

static void GetSurfaceInfo
    (
    size_t width,
    size_t height,
    DXGI_FORMAT format,
    size_t* outNumBytes = nullptr,
    size_t* outRowBytes = nullptr,
    size_t* outNumRows = nullptr
    )
{
    size_t numBytes = 0;
    size_t rowBytes = 0;
    size_t numRows = 0;

    bool bc = false;
    bool packed = false;
    bool planar = false;
    size_t bpe = 0;
    switch (format)
    {
        case DXGI_FORMAT_BC1_TYPELESS:
        case DXGI_FORMAT_BC1_UNORM:
        case DXGI_FORMAT_BC1_UNORM_SRGB:
        case DXGI_FORMAT_BC4_TYPELESS:
        case DXGI_FORMAT_BC4_UNORM:
        case DXGI_FORMAT_BC4_SNORM: bc = true; bpe = 8; break;

        case DXGI_FORMAT_BC2_TYPELESS:
        case DXGI_FORMAT_BC2_UNORM:
        case DXGI_FORMAT_BC2_UNORM_SRGB:
        case DXGI_FORMAT_BC3_TYPELESS:
        case DXGI_FORMAT_BC3_UNORM:
        case DXGI_FORMAT_BC3_UNORM_SRGB:
        case DXGI_FORMAT_BC5_TYPELESS:
        case DXGI_FORMAT_BC5_UNORM:
        case DXGI_FORMAT_BC5_SNORM:
        case DXGI_FORMAT_BC6H_TYPELESS:
        case DXGI_FORMAT_BC6H_UF16:
        case DXGI_FORMAT_BC6H_SF16:
        case DXGI_FORMAT_BC7_TYPELESS:
        case DXGI_FORMAT_BC7_UNORM:
        case DXGI_FORMAT_BC7_UNORM_SRGB: bc = true; bpe = 16; break;

        case DXGI_FORMAT_R8G8_B8G8_UNORM:
        case DXGI_FORMAT_G8R8_G8B8_UNORM:
        case DXGI_FORMAT_YUY2: packed = true; bpe = 4; break;

        case DXGI_FORMAT_Y210:
        case DXGI_FORMAT_Y216: packed = true; bpe = 8; break;

        case DXGI_FORMAT_NV12:
        case DXGI_FORMAT_420_OPAQUE: planar = true; bpe = 2; break;

        case DXGI_FORMAT_P010:
        case DXGI_FORMAT_P016: planar = true; bpe = 4; break;
    }

    if (bc)
    {
        size_t numBlocksWide = 0;
        if (width > 0)
            numBlocksWide = std::max<size_t>(1, (width + 3) / 4);
        size_t numBlocksHigh = 0;
        if (height > 0)
            numBlocksHigh = std::max<size_t>(1, (height + 3) / 4);
        rowBytes = numBlocksWide * bpe;
        numRows = numBlocksHigh;
        numBytes = rowBytes * numBlocksHigh;
    }
    else if (packed)
    {
        rowBytes = ((width + 1) >> 1) * bpe;
        numRows = height;
        numBytes = rowBytes * height;
    }
    else if (format == DXGI_FORMAT_NV11)
    {
        rowBytes = ((width + 3) >> 2) * 4;
        numRows = height * 2; //Direct3D makes this simplifying assumption, although it is larger than the 4:1:1 data
        numBytes = rowBytes * numRows;
    }
    else if (planar)
    {
        rowBytes = ((width + 1) >> 1) * bpe;
        numBytes = (rowBytes * height) + ((rowBytes * height + 1) >> 1);
        numRows = height + ((height + 1) >> 1);
    }
    else
    {
        auto bpp = GetTextureBitsPerPixel(format);
        rowBytes = (width * bpp + 7) / 8; //round up to nearest byte
        numRows = height;
        numBytes = rowBytes * height;
    }

    if (outNumBytes != nullptr)
        *outNumBytes = numBytes;
    if (outRowBytes != nullptr)
        *outRowBytes = rowBytes;
    if (outNumRows != nullptr)
        *outNumRows = numRows;
}

static DXGI_FORMAT MakeSRGB(DXGI_FORMAT format)
{
    switch (format)
    {
        case DXGI_FORMAT_R8G8B8A8_UNORM: return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
        case DXGI_FORMAT_BC1_UNORM: return DXGI_FORMAT_BC1_UNORM_SRGB;
        case DXGI_FORMAT_BC2_UNORM: return DXGI_FORMAT_BC2_UNORM_SRGB;
        case DXGI_FORMAT_BC3_UNORM: return DXGI_FORMAT_BC3_UNORM_SRGB;
        case DXGI_FORMAT_B8G8R8A8_UNORM: return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
        case DXGI_FORMAT_B8G8R8X8_UNORM: return DXGI_FORMAT_B8G8R8X8_UNORM_SRGB;
        case DXGI_FORMAT_BC7_UNORM: return DXGI_FORMAT_BC7_UNORM_SRGB;
        default: return format;
    }
}

static void FillSubresourceData
    (
    size_t width,
    size_t height,
    size_t depth,
    size_t mipCount,
    size_t arraySize,
    DXGI_FORMAT format,
    size_t maxDimension,
    const uint8_t* bytes,
    size_t byteCount,
    size_t& twidth,
    size_t& theight,
    size_t& tdepth,
    size_t& skipMip,
    D3D12_SUBRESOURCE_DATA* preallocatedSubresourceData, //At least mipCount*arraySize
    void* preallocatedFootprintSubresourceData, //At least GetSubresourceFootprintByteCount(mipCount*arraySize)
    Error& error
    )
{
    FINJIN_ERROR_METHOD_START(error);

    if (!bytes || !preallocatedSubresourceData)
    {
        FINJIN_SET_ERROR(error, "Input data not specified.");
        return;
    }
    
    skipMip = 0;
    twidth = 0;
    theight = 0;
    tdepth = 0;

    size_t NumBytes = 0;
    size_t RowBytes = 0;
    const uint8_t* pSrcBits = bytes;
    const uint8_t* pEndBits = bytes + byteCount;

    size_t index = 0;
    for (size_t j = 0; j < arraySize; j++)
    {
        size_t w = width;
        size_t h = height;
        size_t d = depth;
        for (size_t i = 0; i < mipCount; i++)
        {
            GetSurfaceInfo(w, h, format, &NumBytes, &RowBytes, nullptr);

            if (mipCount <= 1 || maxDimension == 0 || (w <= maxDimension && h <= maxDimension && d <= maxDimension))
            {
                if (twidth == 0)
                {
                    twidth = w;
                    theight = h;
                    tdepth = d;
                }

                assert(index < mipCount * arraySize);
                preallocatedSubresourceData[index].pData = (const void*)pSrcBits;
                preallocatedSubresourceData[index].RowPitch = static_cast<UINT>(RowBytes);
                preallocatedSubresourceData[index].SlicePitch = static_cast<UINT>(NumBytes);
                ++index;
            }
            else if (j == 0)
            {
                //Count number of skipped mipmaps (first item only)
                ++skipMip;
            }

            if (pSrcBits + (NumBytes*d) > pEndBits)
            {
                FINJIN_SET_ERROR(error, "Unexpectedly reached end of source image data.");
                return;
            }

            pSrcBits += NumBytes * d;

            w = w >> 1;
            h = h >> 1;
            d = d >> 1;
            if (w == 0)
                w = 1;
            if (h == 0)
                h = 1;
            if (d == 0)
                d = 1;
        }
    }

    if (index == 0)
    {
        FINJIN_SET_ERROR(error, "No image data was filled in.");
        return;
    }
}

static void CreateResources
    (
    ID3D12Device* device,
    uint32_t resourceDimension,
    size_t width,
    size_t height,
    size_t depth,
    size_t mipCount,
    size_t arraySize,
    DXGI_FORMAT format,
    bool forceSRGB,
    bool isCubeMap,
    Microsoft::WRL::ComPtr<ID3D12Resource>& texture,
    Microsoft::WRL::ComPtr<ID3D12Resource>& textureUploadHeap,
    Error& error
    )
{
    FINJIN_ERROR_METHOD_START(error);

    assert(device != nullptr);

    if (forceSRGB)
        format = MakeSRGB(format);

    switch (resourceDimension)
    {
        case D3D12_RESOURCE_DIMENSION_TEXTURE2D:
        {
            D3D12_RESOURCE_DESC texDesc = {};
            texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
            texDesc.Alignment = 0;
            texDesc.Width = width;
            texDesc.Height = (uint32_t)height;
            texDesc.DepthOrArraySize = (depth > 1) ? (uint16_t)depth : (uint16_t)arraySize;
            texDesc.MipLevels = (uint16_t)mipCount;
            texDesc.Format = format;
            texDesc.SampleDesc.Count = 1;
            texDesc.SampleDesc.Quality = 0;
            texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
            texDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

            auto result = device->CreateCommittedResource
                (
                &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
                D3D12_HEAP_FLAG_NONE,
                &texDesc,
                D3D12_RESOURCE_STATE_COMMON,
                nullptr,
                IID_PPV_ARGS(&texture)
                );
            if (FINJIN_CHECK_HRESULT_FAILED(result))
            {
                texture = nullptr;
                
                FINJIN_SET_ERROR(error, "Failed to create committed resource for texture.");
                return;
            }

            const UINT num2DSubresources = texDesc.DepthOrArraySize * texDesc.MipLevels;
            const uint64_t uploadBufferSize = GetRequiredIntermediateSize(texture.Get(), 0, num2DSubresources);

            result = device->CreateCommittedResource
                (
                &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
                D3D12_HEAP_FLAG_NONE,
                &CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize),
                D3D12_RESOURCE_STATE_GENERIC_READ,
                nullptr,
                IID_PPV_ARGS(&textureUploadHeap)
                );
            if (FINJIN_CHECK_HRESULT_FAILED(result))
            {
                texture = nullptr;
                    
                FINJIN_SET_ERROR(error, "Failed to create upload resource for texture.");
                return;
            }
            
            break;
        } 
    }
}

static void CreateDDSTextureSubresources
    (
    ID3D12Device* device,
    const DDSReader::Header* header,
    const uint8_t* bytes,
    size_t byteCount,
    size_t maxDimension,
    bool forceSRGB,
    AllocatedVector<D3D12_SUBRESOURCE_DATA>& preallocatedSubresourceData,
    ByteBuffer& preallocatedFootprintSubresourceData,
    Microsoft::WRL::ComPtr<ID3D12Resource>& texture,
    Microsoft::WRL::ComPtr<ID3D12Resource>& textureUploadHeap,    
    Error& error
    )
{
    FINJIN_ERROR_METHOD_START(error);

    UINT width = header->width;
    UINT height = header->height;
    UINT depth = header->depth;

    uint32_t resourceDimension = D3D12_RESOURCE_DIMENSION_UNKNOWN;
    UINT arraySize = 1;
    DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;
    bool isCubeMap = false;

    size_t mipCount = header->mipMapCount;
    if (mipCount == 0) 
        mipCount = 1;

    if ((header->ddspf.flags & FINJIN_DDS_FOURCC) != 0 && header->ddspf.fourCC == FINJIN_FOURCC('D', 'X', '1', '0'))
    {
        auto d3d10HeaderExtension = reinterpret_cast<const DDSReader::DDSReader::DX10HeaderExtension*>((const uint8_t*)header + sizeof(DDSReader::Header));

        arraySize = d3d10HeaderExtension->arraySize;
        if (arraySize == 0)
        {
            FINJIN_SET_ERROR(error, "Texture format has a zero array size.");
            return;
        }   

        switch (d3d10HeaderExtension->dxgiFormat)
        {
            case DXGI_FORMAT_AI44:
            {
                FINJIN_SET_ERROR(error, "Unsupported texture format 'DXGI_FORMAT_AI44' specified in DDS file.");
                return;
            }
            case DXGI_FORMAT_IA44:
            {
                FINJIN_SET_ERROR(error, "Unsupported texture format 'DXGI_FORMAT_IA44' specified in DDS file.");
                return;
            }
            case DXGI_FORMAT_P8:
            {
                FINJIN_SET_ERROR(error, "Unsupported texture format 'DXGI_FORMAT_P8' specified in DDS file.");
                return;
            }
            case DXGI_FORMAT_A8P8: 
            {
                FINJIN_SET_ERROR(error, "Unsupported texture format 'DXGI_FORMAT_A8P8' specified in DDS file.");
                return;
            }
            default:
            {
                if (GetTextureBitsPerPixel(d3d10HeaderExtension->dxgiFormat) == 0)
                {
                    FINJIN_SET_ERROR(error, "Unsupported texture format specified in DDS file.");
                    return;
                }
            }
        }

        format = d3d10HeaderExtension->dxgiFormat;

        switch (d3d10HeaderExtension->resourceDimension)
        {
            case D3D12_RESOURCE_DIMENSION_TEXTURE1D:
            {
                if (header->HasHeight() && height != 1)
                {
                    FINJIN_SET_ERROR(error, "1D texture has a non-1 height. This is not supported.");
                    return;
                }
                height = depth = 1;
                break;
            }
            case D3D12_RESOURCE_DIMENSION_TEXTURE2D:
            {
                if (d3d10HeaderExtension->IsCube())
                {
                    arraySize *= 6;
                    isCubeMap = true;
                }
                depth = 1;
                break;
            }
            case D3D12_RESOURCE_DIMENSION_TEXTURE3D:
            {
                if (!header->IsVolume())
                {
                    FINJIN_SET_ERROR(error, "DDS file specifies a 3D resource dimension, but DDS_HEADER_FLAGS_VOLUME flag is not set.");
                    return;
                }
                if (arraySize > 1)
                {
                    FINJIN_SET_ERROR(error, "DDS file specifies a 3D resource dimension with an array size greater than 1. This is not supported.");
                    return;
                }
                break;
            }
            default: 
            {
                FINJIN_SET_ERROR(error, "DDS file specifies an unsupported resource dimension.");
                return;
            }
        }

        switch (d3d10HeaderExtension->resourceDimension)
        {
            case D3D12_RESOURCE_DIMENSION_TEXTURE1D: resourceDimension = D3D12_RESOURCE_DIMENSION_TEXTURE1D; break;
            case D3D12_RESOURCE_DIMENSION_TEXTURE2D: resourceDimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D; break;
            case D3D12_RESOURCE_DIMENSION_TEXTURE3D: resourceDimension = D3D12_RESOURCE_DIMENSION_TEXTURE3D; break;
        }
    }
    else
    {
        format = header->ddspf.GetDXGIFormat();

        if (format == DXGI_FORMAT_UNKNOWN)
        {
            FINJIN_SET_ERROR(error, "DDS file specifies an unknown pixel format.");
            return;
        }

        if (header->IsVolume())
            resourceDimension = D3D12_RESOURCE_DIMENSION_TEXTURE3D;
        else
        {
            if (header->IsCube())
            {
                if (!header->IsCubeAllFaces())
                {
                    FINJIN_SET_ERROR(error, "DDS file specifies a cube map with an incomplete number of faces. This is not supported.");
                    return;
                }
                arraySize = 6;
                isCubeMap = true;
            }

            depth = 1;
            resourceDimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        }

        assert(GetTextureBitsPerPixel(format) != 0);
    }

    //Bound sizes (for security purposes we don't trust DDS file metadata larger than the D3D 11.x hardware requirements)
    if (mipCount > D3D12_REQ_MIP_LEVELS)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("DDS has a mip count of '%1%' which exceeds the maximum allowed '%2%'.", mipCount, D3D12_REQ_MIP_LEVELS));
        return;
    }
    
    switch (resourceDimension)
    {
        case D3D12_RESOURCE_DIMENSION_TEXTURE1D:
        {
            if (arraySize > D3D12_REQ_TEXTURE1D_ARRAY_AXIS_DIMENSION)
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("1D resource array axis dimension '%1%' exceeds the maximum allowed '%2%'.", arraySize, D3D12_REQ_TEXTURE1D_ARRAY_AXIS_DIMENSION));
                return;
            }

            if (width > D3D12_REQ_TEXTURE1D_U_DIMENSION)
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("1D resource dimension '%1%' exceeds the maximum allowed '%2%'.", width, D3D12_REQ_TEXTURE1D_U_DIMENSION));
                return;
            }
            break;
        }
        case D3D12_RESOURCE_DIMENSION_TEXTURE2D:
        {
            if (isCubeMap)
            {
                //This is the right bound because we set arraySize to (NumCubes*6) above
                if (arraySize > D3D12_REQ_TEXTURE2D_ARRAY_AXIS_DIMENSION)
                {
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("2D resource array axis dimension '%1%' exceeds the maximum allowed '%2%'.", arraySize, D3D12_REQ_TEXTURE2D_ARRAY_AXIS_DIMENSION));
                    return;
                }
                    
                if (width > D3D12_REQ_TEXTURECUBE_DIMENSION || height > D3D12_REQ_TEXTURECUBE_DIMENSION)
                {
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("2D texture cube resource dimension '%1%' exceeds the maximum allowed '%2%'.", std::max(width, height), D3D12_REQ_TEXTURECUBE_DIMENSION));
                    return;
                }
            }
            else
            {
                if (arraySize > D3D12_REQ_TEXTURE2D_ARRAY_AXIS_DIMENSION)
                {
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("2D resource array axis dimension '%1%' exceeds the maximum allowed '%2%'.", arraySize, D3D12_REQ_TEXTURE2D_ARRAY_AXIS_DIMENSION));
                    return;
                }

                if (width > D3D12_REQ_TEXTURE2D_U_OR_V_DIMENSION || height > D3D12_REQ_TEXTURE2D_U_OR_V_DIMENSION)
                {
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("2D resource dimension '%1%' exceeds the maximum allowed '%2%'.", std::max(width, height), D3D12_REQ_TEXTURE2D_U_OR_V_DIMENSION));
                    return;
                }
            }
            break;
        }
        case D3D12_RESOURCE_DIMENSION_TEXTURE3D:
        {
            if (arraySize > 1)
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("3D resource array axis dimension '%1%' exceeds the maximum allowed '%2%'.", arraySize, 1));
                return;
            }

            if (width > D3D12_REQ_TEXTURE3D_U_V_OR_W_DIMENSION || height > D3D12_REQ_TEXTURE3D_U_V_OR_W_DIMENSION || depth > D3D12_REQ_TEXTURE3D_U_V_OR_W_DIMENSION)
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("3D resource dimension '%1%' exceeds the maximum allowed '%2%'.", std::max(std::max(width, height), depth), D3D12_REQ_TEXTURE2D_U_OR_V_DIMENSION));
                return;
            }
            break;
        }
        default: 
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Invalid resource dimension '%1%'.", resourceDimension));
            return;
        }
    }

    auto subresourceCount = mipCount * arraySize;
    if (preallocatedSubresourceData.resize(subresourceCount) < subresourceCount)
    {        
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Not enough memory for subresource data. Need '%1%' entries but only '%2%' are available.", subresourceCount, preallocatedSubresourceData.max_size()));
        return;
    }
    
    auto subresourceFootprintByteCount = GetSubresourceFootprintByteCount(subresourceCount);
    if (preallocatedFootprintSubresourceData.resize(subresourceFootprintByteCount) < subresourceCount)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Not enough memory for subresource footprint data. Need '%1%' bytes but only '%2%' are available.", subresourceFootprintByteCount, preallocatedFootprintSubresourceData.max_size()));
        return;
    }
    
    size_t skipMip = 0;
    size_t twidth = 0;
    size_t theight = 0;
    size_t tdepth = 0;

    FillSubresourceData
        (
        width, 
        height, 
        depth, 
        mipCount, 
        arraySize, 
        format, 
        maxDimension, 
        bytes, 
        byteCount,
        twidth, 
        theight, 
        tdepth, 
        skipMip, 
        preallocatedSubresourceData.data(),
        preallocatedFootprintSubresourceData.data(),
        error
        );
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to fill texture subresource data.");
        return;
    }

    CreateResources
        (
        device, 
        resourceDimension, 
        twidth, 
        theight, 
        tdepth,
        mipCount - skipMip,
        arraySize,
        format,
        false, //forceSRGB
        isCubeMap,
        texture,
        textureUploadHeap,
        error
        );
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to create DDS texture subresources.");
        return;
    }
}

static void CreatePNGTextureSubresources
    (
    ID3D12Device* device,
    const PNGReader& reader,
    const uint8_t* bytes,
    size_t byteCount,
    size_t maxDimension,
    bool forceSRGB,
    AllocatedVector<D3D12_SUBRESOURCE_DATA>& preallocatedSubresourceData,
    ByteBuffer& preallocatedFootprintSubresourceData,
    Microsoft::WRL::ComPtr<ID3D12Resource>& texture,
    Microsoft::WRL::ComPtr<ID3D12Resource>& textureUploadHeap,    
    Error& error
    )
{
    FINJIN_ERROR_METHOD_START(error);

    UINT width = reader.GetWidth();
    UINT height = reader.GetHeight();
    UINT depth = 1;

    uint32_t resourceDimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    UINT arraySize = 1;
    DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;
    auto pngPixelFormat = reader.GetPixelFormat();
    if (pngPixelFormat == PNGReader::PixelFormat::RGBA)
    {
        if (reader.GetBytesPerChannel() == 1)
        {
            if (reader.IsSRGB())
                format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
            else
                format = DXGI_FORMAT_R8G8B8A8_UNORM;
        }
        else if (reader.GetBytesPerChannel() == 2)
            format = DXGI_FORMAT_R16G16B16A16_UNORM;
        else
        {
            FINJIN_SET_ERROR(error, Utf8StringFormatter::Format("Unsupported RGBA bytes per component '%1%'.", reader.GetBytesPerChannel()));
            return;
        }
    }
    else if (pngPixelFormat == PNGReader::PixelFormat::GRAY)
    {
        if (reader.GetBytesPerChannel() == 1)
            format = DXGI_FORMAT_R8_UNORM;
        else if (reader.GetBytesPerChannel() == 2)
            format = DXGI_FORMAT_R16_UNORM;
        else
        {
            FINJIN_SET_ERROR(error, Utf8StringFormatter::Format("Unsupported gray bytes per component '%1%'.", reader.GetBytesPerChannel()));
            return;
        }
    }
    else if (pngPixelFormat == PNGReader::PixelFormat::GA)
    {
        if (reader.GetBytesPerChannel() == 1)
            format = DXGI_FORMAT_R8G8_UNORM;
        else if (reader.GetBytesPerChannel() == 2)
            format = DXGI_FORMAT_R16G16_UNORM;
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

    bool isCubeMap = false;

    size_t mipCount = 1;

    auto subresourceCount = mipCount * arraySize;
    if (preallocatedSubresourceData.resize(subresourceCount) < subresourceCount)
    {        
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Not enough memory for subresource data. Need '%1%' entries but only '%2%' are available.", subresourceCount, preallocatedSubresourceData.max_size()));
        return;
    }
    
    auto subresourceFootprintByteCount = GetSubresourceFootprintByteCount(subresourceCount);
    if (preallocatedFootprintSubresourceData.resize(subresourceFootprintByteCount) < subresourceFootprintByteCount)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Not enough memory for subresource footprint data. Need '%1%' bytes but only '%2%' are available.", subresourceFootprintByteCount, preallocatedFootprintSubresourceData.max_size()));
        return;
    }
    
    size_t skipMip = 0;
    size_t twidth = 0;
    size_t theight = 0;
    size_t tdepth = 0;

    FillSubresourceData
        (
        width, 
        height, 
        depth, 
        mipCount, 
        arraySize, 
        format, 
        maxDimension, 
        bytes, 
        byteCount,
        twidth, 
        theight, 
        tdepth, 
        skipMip, 
        preallocatedSubresourceData.data(),
        preallocatedFootprintSubresourceData.data(),
        error
        );
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to fill texture subresource data.");
        return;
    }

    CreateResources
        (
        device, 
        resourceDimension, 
        twidth, 
        theight, 
        tdepth,
        mipCount - skipMip,
        arraySize,
        format,
        false, //forceSRGB
        isCubeMap,
        texture,
        textureUploadHeap,
        error
        );
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to create PNG texture subresources.");
        return;
    }
}


//Implementation---------------------------------------------------------------
D3D_FEATURE_LEVEL D3D12Utilities::ParseFeatureLevel(const Utf8String& s, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    uint32_t featureLevel = 0;

    size_t dotFoundAt = s.find('.');
    if (dotFoundAt != Utf8String::npos)
    {
        Utf8String left, right;
        s.substr(left, 0, dotFoundAt);
        s.substr(right, dotFoundAt + 1);

        uint32_t leftValue;
        Convert::ToInteger(leftValue, left, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to convert '%1%' to a Direct3D version.", s));
            return (D3D_FEATURE_LEVEL)0;
        }

        uint32_t rightValue;
        Convert::ToInteger(rightValue, right, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to convert '%1%' to a Direct3D version.", s));
            return (D3D_FEATURE_LEVEL)0;
        }

        featureLevel = (leftValue << 24) | (rightValue << 16);
    }
    else
    {
        uint32_t intValue;
        Convert::ToInteger(intValue, s, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to convert '%1%' to a Direct3D version.", s));
            return (D3D_FEATURE_LEVEL)0;
        }

        featureLevel = intValue << 24;
    }

    return static_cast<D3D_FEATURE_LEVEL>(featureLevel);
}

ValueOrError<void> D3D12Utilities::FeatureLevelToString(Utf8String& result, D3D_FEATURE_LEVEL level)
{
    auto major = static_cast<uint16_t>(level) >> 12;
    auto minor = (static_cast<uint16_t>(level) >> 8) & 0xf;

    if (result.assign(Convert::ToString(major)).HasError())
        return ValueOrError<void>::CreateError();
    if (result.append(".").HasError())
        return ValueOrError<void>::CreateError();
    if (result.append(Convert::ToString(minor)).HasError())
        return ValueOrError<void>::CreateError();

    return ValueOrError<void>();
}

UINT D3D12Utilities::GetConstantBufferAlignedSize(size_t paddedSize, size_t size, size_t count)
{
    auto byteSize = paddedSize * (count - 1) + size;
    return static_cast<UINT>(std::max(byteSize, (size_t)D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT));
}

Microsoft::WRL::ComPtr<ID3D12Resource> D3D12Utilities::CreateDefaultBufferAndUploader
    (
    ID3D12Device* device,
    const void* initData,
    uint64_t byteSize,
    Microsoft::WRL::ComPtr<ID3D12Resource>& uploadBuffer,
    Error& error
    )
{
    FINJIN_ERROR_METHOD_START(error);

    Microsoft::WRL::ComPtr<ID3D12Resource> defaultBuffer;

    //Create the actual default buffer resource.
    auto result = device->CreateCommittedResource
        (
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(byteSize),
        D3D12_RESOURCE_STATE_COMMON,
        nullptr,
        IID_PPV_ARGS(defaultBuffer.GetAddressOf())
        );
    if (FINJIN_CHECK_HRESULT_FAILED(result))
    {
        FINJIN_SET_ERROR(error, "Failed to create committed resource in default heap.");
        return nullptr;
    }

    //In order to copy CPU memory data into our default buffer, we need to create an intermediate upload heap. 
    result = device->CreateCommittedResource
        (
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(byteSize),
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(uploadBuffer.GetAddressOf())
        );
    if (FINJIN_CHECK_HRESULT_FAILED(result))
    {
        FINJIN_SET_ERROR(error, "Failed to create upload buffer.");
        return nullptr;
    }

    //Note: uploadBuffer has to be kept alive until after the data has been copied via execution in a command list
    return defaultBuffer;
}

void D3D12Utilities::CreateDDSTextureResourceFromMemory
    (
    DDSReader& ddsReader,
    ID3D12Device* device,
    const void* vbytes,
    size_t byteCount,
    AllocatedVector<D3D12_SUBRESOURCE_DATA>& preallocatedSubresourceData,
    ByteBuffer& preallocatedFootprintSubresourceData,
    Microsoft::WRL::ComPtr<ID3D12Resource>& texture,
    Microsoft::WRL::ComPtr<ID3D12Resource>& textureUploadHeap,
    size_t maxDimension,
    Error& error
    )
{
    FINJIN_ERROR_METHOD_START(error);

    assert(device != nullptr);

    auto bytes = static_cast<const uint8_t*>(vbytes);
    if (bytes == nullptr || byteCount == 0)
    {
        FINJIN_SET_ERROR(error, "No DDS bytes were specified.");
        return;
    }
    
    ptrdiff_t offset = sizeof(uint32_t) + sizeof(DDSReader::Header) + (ddsReader.GetHeaderDX10Extension() ? sizeof(DDSReader::DX10HeaderExtension) : 0);

    CreateDDSTextureSubresources
        (
        device,
        &ddsReader.GetHeader(),
        bytes + offset,
        byteCount - offset,
        maxDimension,
        false,
        preallocatedSubresourceData,
        preallocatedFootprintSubresourceData,
        texture,
        textureUploadHeap,
        error
        );
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to create DDS texture subresources.");
        return;
    }
}

void D3D12Utilities::CreatePNGTextureResourceFromMemory
    (
    PNGReader& pngReader,
    ID3D12Device* device,
    const void* bytes,
    size_t byteCount,
    AllocatedVector<D3D12_SUBRESOURCE_DATA>& preallocatedSubresourceData,
    ByteBuffer& preallocatedFootprintSubresourceData,
    Microsoft::WRL::ComPtr<ID3D12Resource>& texture,
    Microsoft::WRL::ComPtr<ID3D12Resource>& textureUploadHeap,
    size_t maxDimension,
    Error& error
    )
{
    FINJIN_ERROR_METHOD_START(error);

    assert(device != nullptr);

    if (bytes == nullptr || byteCount == 0)
    {
        FINJIN_SET_ERROR(error, "No PNG bytes were specified.");
        return;
    }
    
    CreatePNGTextureSubresources
        (
        device,
        pngReader,
        static_cast<const uint8_t*>(bytes),
        byteCount,
        maxDimension,
        false,
        preallocatedSubresourceData,
        preallocatedFootprintSubresourceData,
        texture,
        textureUploadHeap,
        error
        );
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to create PNG texture subresources.");
        return;
    }
}

#endif
