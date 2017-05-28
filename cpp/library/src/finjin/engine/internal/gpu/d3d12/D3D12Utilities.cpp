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


//Local functions---------------------------------------------------------------
static bool IsSupportedColorFormat(DXGI_FORMAT colorFormat, const D3D12_FEATURE_DATA_FORMAT_SUPPORT* formatSupportByFormat, size_t formatCount)
{
    assert(colorFormat < formatCount);

    if (colorFormat < formatCount)
    {
        auto& formatSupport = formatSupportByFormat[colorFormat];
        return formatSupport.Support1 != D3D12_FORMAT_SUPPORT1_NONE;
    }

    return false;
}

static bool IsSupportedDepthStencilFormat(DXGI_FORMAT depthStencilFormat, const D3D12_FEATURE_DATA_FORMAT_SUPPORT* formatSupportByFormat, size_t formatCount)
{
    assert(depthStencilFormat < formatCount);

    if (depthStencilFormat < formatCount)
    {
        auto& formatSupport = formatSupportByFormat[depthStencilFormat];
        return formatSupport.Support1 != D3D12_FORMAT_SUPPORT1_NONE;
    }

    return false;
}


//Implementation----------------------------------------------------------------
bool D3D12Utilities::GetBestColorFormat(RequestedValue<DXGI_FORMAT>& colorFormat, const D3D12_FEATURE_DATA_FORMAT_SUPPORT* formatSupportByFormat, size_t formatCount)
{
    colorFormat.actual = DXGI_FORMAT_UNKNOWN;

    if (colorFormat.requested == DXGI_FORMAT_UNKNOWN || !IsSupportedColorFormat(colorFormat.requested, formatSupportByFormat, formatCount))
        colorFormat.actual = DXGI_FORMAT_R8G8B8A8_UNORM;
    else
        colorFormat.actual = colorFormat.requested;

    return colorFormat.actual != DXGI_FORMAT_UNKNOWN;
}

bool D3D12Utilities::GetBestDepthStencilFormat(RequestedValue<DXGI_FORMAT>& depthStencilFormat, bool stencilRequired, const D3D12_FEATURE_DATA_FORMAT_SUPPORT* formatSupportByFormat, size_t formatCount)
{
    depthStencilFormat.actual = DXGI_FORMAT_UNKNOWN;

    if (depthStencilFormat.requested == DXGI_FORMAT_UNKNOWN || !IsSupportedColorFormat(depthStencilFormat.requested, formatSupportByFormat, formatCount))
    {
        if (stencilRequired)
            depthStencilFormat.actual = DXGI_FORMAT_D32_FLOAT_S8X24_UINT; //DXGI_FORMAT_D24_UNORM_S8_UINT
        else
            depthStencilFormat.actual = DXGI_FORMAT_D32_FLOAT; //DXGI_FORMAT_D16_UNORM
    }
    else
        depthStencilFormat.actual = depthStencilFormat.requested;

    return depthStencilFormat.actual != DXGI_FORMAT_UNKNOWN;
}

D3D_SHADER_MODEL D3D12Utilities::ParseShaderModel(const Utf8String& s, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    if (s == "5.1")
        return D3D_SHADER_MODEL_5_1;
    else if (s == "6" || s == "6.0")
        return D3D_SHADER_MODEL_6_0;

    return (D3D_SHADER_MODEL)0;
}

ValueOrError<void> D3D12Utilities::ShaderModelToString(Utf8String& result, D3D_SHADER_MODEL model)
{
    auto major = static_cast<uint8_t>(model) >> 4;
    auto minor = static_cast<uint8_t>(model) & 0xf;

    if (result.assign(Convert::ToString(major)).HasError())
        return ValueOrError<void>::CreateError();
    if (result.append(".").HasError())
        return ValueOrError<void>::CreateError();
    if (result.append(Convert::ToString(minor)).HasError())
        return ValueOrError<void>::CreateError();

    return ValueOrError<void>();
}

D3D_FEATURE_LEVEL D3D12Utilities::ParseFeatureLevel(const Utf8String& s, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    uint32_t featureLevel = 0;

    size_t dotFoundAt = s.find('.');
    if (dotFoundAt != Utf8String::npos)
    {
        Utf8StringView left, right;
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
    //Note: uploadBuffer has to be kept alive until after the data has been copied via execution in a command list

    FINJIN_ERROR_METHOD_START(error);

    Microsoft::WRL::ComPtr<ID3D12Resource> defaultBuffer;

    //Device memory resource
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

    //Host memory resource
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

    return defaultBuffer;
}

UINT D3D12Utilities::GetBitsPerPixel(DXGI_FORMAT format)
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

#endif
