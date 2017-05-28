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

#include "D3D12Texture.hpp"
#include "D3D12Utilities.hpp"

using namespace Finjin::Engine;


//Local functions---------------------------------------------------------------
static uint64_t GetSubresourceFootprintByteCount(size_t subresourceCount)
{
    //From UpdateSubresources() in d3dx12.h
    return (sizeof(D3D12_PLACED_SUBRESOURCE_FOOTPRINT) + sizeof(UINT) + sizeof(uint64_t)) * subresourceCount;
}

static void GetSurfaceInfo
    (
    size_t width,
    size_t height,
    DXGI_FORMAT format,
    size_t* outsurfaceByteCount = nullptr,
    size_t* outsurfaceRowByteCount = nullptr,
    size_t* outNumRows = nullptr
    )
{
    size_t surfaceByteCount = 0;
    size_t surfaceRowByteCount = 0;
    size_t numRows = 0;

    bool bc = false;
    bool packed = false;
    bool planar = false;
    size_t bytesPerElement = 0;
    switch (format)
    {
        case DXGI_FORMAT_BC1_TYPELESS:
        case DXGI_FORMAT_BC1_UNORM:
        case DXGI_FORMAT_BC1_UNORM_SRGB:
        case DXGI_FORMAT_BC4_TYPELESS:
        case DXGI_FORMAT_BC4_UNORM:
        case DXGI_FORMAT_BC4_SNORM: bc = true; bytesPerElement = 8; break;

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
        case DXGI_FORMAT_BC7_UNORM_SRGB: bc = true; bytesPerElement = 16; break;

        case DXGI_FORMAT_R8G8_B8G8_UNORM:
        case DXGI_FORMAT_G8R8_G8B8_UNORM:
        case DXGI_FORMAT_YUY2: packed = true; bytesPerElement = 4; break;

        case DXGI_FORMAT_Y210:
        case DXGI_FORMAT_Y216: packed = true; bytesPerElement = 8; break;

        case DXGI_FORMAT_NV12:
        case DXGI_FORMAT_420_OPAQUE: planar = true; bytesPerElement = 2; break;

        case DXGI_FORMAT_P010:
        case DXGI_FORMAT_P016: planar = true; bytesPerElement = 4; break;
    }

    if (bc)
    {
        size_t numBlocksWide = 0;
        if (width > 0)
            numBlocksWide = std::max<size_t>(1, (width + 3) / 4);
        size_t numBlocksHigh = 0;
        if (height > 0)
            numBlocksHigh = std::max<size_t>(1, (height + 3) / 4);
        surfaceRowByteCount = numBlocksWide * bytesPerElement;
        numRows = numBlocksHigh;
        surfaceByteCount = surfaceRowByteCount * numBlocksHigh;
    }
    else if (packed)
    {
        surfaceRowByteCount = ((width + 1) >> 1) * bytesPerElement;
        numRows = height;
        surfaceByteCount = surfaceRowByteCount * height;
    }
    else if (format == DXGI_FORMAT_NV11)
    {
        surfaceRowByteCount = ((width + 3) >> 2) * 4;
        numRows = height * 2; //Direct3D makes this simplifying assumption, although it is larger than the 4:1:1 data
        surfaceByteCount = surfaceRowByteCount * numRows;
    }
    else if (planar)
    {
        surfaceRowByteCount = ((width + 1) >> 1) * bytesPerElement;
        surfaceByteCount = (surfaceRowByteCount * height) + ((surfaceRowByteCount * height + 1) >> 1);
        numRows = height + ((height + 1) >> 1);
    }
    else
    {
        auto bitsPerPixel = D3D12Utilities::GetBitsPerPixel(format);
        surfaceRowByteCount = (width * bitsPerPixel + 7) / 8; //round up to nearest byte
        numRows = height;
        surfaceByteCount = surfaceRowByteCount * height;
    }

    if (outsurfaceByteCount != nullptr)
        *outsurfaceByteCount = surfaceByteCount;
    if (outsurfaceRowByteCount != nullptr)
        *outsurfaceRowByteCount = surfaceRowByteCount;
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
    size_t& outDepth,
    size_t& skipMipCount,
    D3D12_SUBRESOURCE_DATA* preallocatedSubresourceData, //At least mipCount*arraySize
    void* preallocatedFootprintSubresourceData, //At least GetSubresourceFootprintByteCount(mipCount*arraySize)
    Error& error
    )
{
    FINJIN_ERROR_METHOD_START(error);

    if (bytes == nullptr || preallocatedSubresourceData == nullptr)
    {
        FINJIN_SET_ERROR(error, "Input data not specified.");
        return;
    }

    outDepth = 0;
    skipMipCount = 0;

    size_t surfaceByteCount = 0;
    size_t surfaceRowByteCount = 0;
    auto bytesStart = bytes;
    auto bytesEnd = bytes + byteCount;

    size_t index = 0;
    for (size_t arrayItemIndex = 0; arrayItemIndex < arraySize; arrayItemIndex++)
    {
        size_t currentWidth = width;
        size_t currentHeight = height;
        size_t currentDepth = depth;
        for (size_t mipMapIndex = 0; mipMapIndex < mipCount; mipMapIndex++)
        {
            if (currentWidth == 0)
                currentWidth = 1;
            if (currentHeight == 0)
                currentHeight = 1;
            if (currentDepth == 0)
                currentDepth = 1;

            GetSurfaceInfo(currentWidth, currentHeight, format, &surfaceByteCount, &surfaceRowByteCount, nullptr);

            if (mipCount <= 1 || maxDimension == 0 || (currentWidth <= maxDimension && currentHeight <= maxDimension && currentDepth <= maxDimension))
            {
                if (outDepth == 0)
                    outDepth = currentDepth;

                assert(index < mipCount * arraySize);
                preallocatedSubresourceData[index].pData = bytesStart;
                preallocatedSubresourceData[index].RowPitch = static_cast<UINT>(surfaceRowByteCount);
                preallocatedSubresourceData[index].SlicePitch = static_cast<UINT>(surfaceByteCount);
                ++index;
            }
            else if (arrayItemIndex == 0)
            {
                //Count number of skipped mipmaps (first item only)
                ++skipMipCount;
            }

            bytesStart += surfaceByteCount * currentDepth;

            if (bytesStart > bytesEnd)
            {
                FINJIN_SET_ERROR(error, "Unexpectedly reached end of source image data.");
                return;
            }

            currentWidth /= 2;
            currentHeight /= 2;
            currentDepth /= 2;
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
    const DDSReader::DX10HeaderExtension* d3d10HeaderExtension,
    const uint8_t* bytes,
    size_t byteCount,
    size_t maxDimension,
    bool forceSRGB,
    DynamicVector<D3D12_SUBRESOURCE_DATA>& preallocatedSubresourceData,
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

    if (d3d10HeaderExtension != nullptr)
    {
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
                if (D3D12Utilities::GetBitsPerPixel(static_cast<DXGI_FORMAT>(d3d10HeaderExtension->dxgiFormat)) == 0)
                {
                    FINJIN_SET_ERROR(error, "Unsupported texture format specified in DDS file.");
                    return;
                }
            }
        }

        format = static_cast<DXGI_FORMAT>(d3d10HeaderExtension->dxgiFormat);

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

        assert(D3D12Utilities::GetBitsPerPixel(format) != 0);
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

    size_t tdepth = 0;
    size_t skipMipCount = 0;

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
        tdepth,
        skipMipCount,
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
        width,
        height,
        tdepth,
        mipCount - skipMipCount,
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
    DynamicVector<D3D12_SUBRESOURCE_DATA>& preallocatedSubresourceData,
    ByteBuffer& preallocatedFootprintSubresourceData,
    Microsoft::WRL::ComPtr<ID3D12Resource>& texture,
    Microsoft::WRL::ComPtr<ID3D12Resource>& textureUploadHeap,
    Error& error
    )
{
    FINJIN_ERROR_METHOD_START(error);

    uint32_t resourceDimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
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
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Unsupported RGBA bytes per component '%1%'.", reader.GetBytesPerChannel()));
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
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Unsupported gray bytes per component '%1%'.", reader.GetBytesPerChannel()));
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
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Unsupported gray/alpha bytes per component '%1%'.", reader.GetBytesPerChannel()));
            return;
        }
    }
    else
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Invalid PNG pixel format '%1%'.", pngPixelFormat));
        return;
    }

    auto isCubeMap = false;
    UINT depth = 1;
    UINT arraySize = 1;
    UINT mipCount = 1;

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

    size_t tdepth = 0;
    size_t skipMipCount = 0;

    FillSubresourceData
        (
        reader.GetWidth(),
        reader.GetHeight(),
        depth,
        mipCount,
        arraySize,
        format,
        maxDimension,
        bytes,
        byteCount,
        tdepth,
        skipMipCount,
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
        reader.GetWidth(),
        reader.GetHeight(),
        tdepth,
        mipCount - skipMipCount,
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


//Implementation----------------------------------------------------------------
D3D12Texture::D3D12Texture(Allocator* allocator) : name(allocator)
{
    this->textureIndex = 0;
    this->textureDimension = TextureDimension::DIMENSION_2D;
    this->isResidentCountdown = 0;
    this->waitingToBeResidentNext = nullptr;
}

void D3D12Texture::CreateFromDDS
    (
    const DDSReader& ddsReader,
    ID3D12Device* device,
    const void* vbytes,
    size_t byteCount,
    DynamicVector<D3D12_SUBRESOURCE_DATA>& preallocatedSubresourceData,
    ByteBuffer& preallocatedFootprintSubresourceData,
    size_t maxDimension,
    Error& error
    )
{
    FINJIN_ERROR_METHOD_START(error);

    auto bytes = static_cast<const uint8_t*>(vbytes);
    if (bytes == nullptr || byteCount == 0)
    {
        FINJIN_SET_ERROR(error, "No DDS bytes were specified.");
        return;
    }

    CreateDDSTextureSubresources
        (
        device,
        &ddsReader.GetHeader(),
        ddsReader.GetHeaderDX10Extension(),
        bytes,
        byteCount,
        maxDimension,
        false,
        preallocatedSubresourceData,
        preallocatedFootprintSubresourceData,
        this->resource,
        this->uploadHeapResource,
        error
        );
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to create DDS texture subresources.");
        return;
    }
}

void D3D12Texture::CreateFromPNG
    (
    const PNGReader& pngReader,
    ID3D12Device* device,
    const void* bytes,
    size_t byteCount,
    DynamicVector<D3D12_SUBRESOURCE_DATA>& preallocatedSubresourceData,
    ByteBuffer& preallocatedFootprintSubresourceData,
    size_t maxDimension,
    Error& error
    )
{
    FINJIN_ERROR_METHOD_START(error);

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
        this->resource,
        this->uploadHeapResource,
        error
        );
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to create PNG texture subresources.");
        return;
    }
}

void D3D12Texture::Destroy()
{
    this->resource = nullptr;
    this->uploadHeapResource = nullptr;

    this->subresourceData.Destroy();
    this->footprintSubresourceData.Destroy();

    this->textureData.Destroy();

    this->name.Destroy();
}

void D3D12Texture::HandleCreationFailure()
{
    Destroy();
}

void D3D12Texture::ReleaseUploaders()
{
    this->uploadHeapResource = nullptr;
    this->textureData.Destroy();
}

bool D3D12Texture::IsResidentOnGpu() const
{
    return this->isResidentCountdown == 0;
}

void D3D12Texture::UpdateResidentOnGpuStatus()
{
    if (this->isResidentCountdown > 0)
    {
        this->isResidentCountdown--;

        if (IsResidentOnGpu())
        {
            //Became resident
            ReleaseUploaders();
        }
    }
}

#endif
