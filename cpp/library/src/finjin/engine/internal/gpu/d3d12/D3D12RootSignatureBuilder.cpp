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

#include "D3D12RootSignatureBuilder.hpp"
#include "finjin/common/Convert.hpp"
#include "D3D12Utilities.hpp"

using namespace Finjin::Engine;


//Implementation----------------------------------------------------------------

//D3D12DescriptorRange
D3D12DescriptorRange::D3D12DescriptorRange()
{
    FINJIN_ZERO_ITEM(*this);
}

D3D12DescriptorRange::D3D12DescriptorRange
    (
    D3D12_DESCRIPTOR_RANGE_TYPE rangeType,
    UINT numDescriptors,
    UINT baseShaderRegister,
    UINT registerSpace,
    UINT offsetInDescriptorsFromTableStart
    )
{
    this->RangeType = rangeType;
    this->NumDescriptors = numDescriptors;
    this->BaseShaderRegister = baseShaderRegister;
    this->RegisterSpace = registerSpace;
    this->OffsetInDescriptorsFromTableStart = offsetInDescriptorsFromTableStart;
}

//D3D12ShaderRootParameterBuilder
D3D12ShaderRootParameterBuilder::D3D12ShaderRootParameterBuilder()
{
    this->parameterType = (D3D12_ROOT_PARAMETER_TYPE)-1;
    this->shaderRegister = 0;
    this->registerSpace = 0;
    this->num32BitValues = 0;
    this->visibility = D3D12_SHADER_VISIBILITY_ALL;
}

D3D12ShaderRootParameterBuilder::D3D12ShaderRootParameterBuilder(D3D12_ROOT_PARAMETER_TYPE parameterType, UINT shaderRegister, UINT registerSpace, D3D12_SHADER_VISIBILITY visibility)
{
    this->parameterType = parameterType;
    this->shaderRegister = shaderRegister;
    this->registerSpace = registerSpace;
    this->num32BitValues = 0;
    this->visibility = visibility;
}

D3D12ShaderRootParameterBuilder::D3D12ShaderRootParameterBuilder(UINT shaderRegister, UINT registerSpace, UINT num32BitValues, D3D12_SHADER_VISIBILITY visibility)
{
    this->parameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
    this->shaderRegister = shaderRegister;
    this->registerSpace = registerSpace;
    this->num32BitValues = num32BitValues;
    this->visibility = visibility;
}

//D3D12RootParametersBuilder
D3D12RootParametersBuilder::D3D12RootParametersBuilder()
{
    Reset();
}

void D3D12RootParametersBuilder::Reset()
{
    this->params.clear();
}

void D3D12RootParametersBuilder::Set(const D3D12ShaderRootParameterBuilder* params, size_t count, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    if (count > this->params.max_size())
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("The specified number of parameters '%1%' exceeds the maximum '%2%'.", count, this->params.max_size()));
        return;
    }

    this->params.clear();
    for (size_t i = 0; i < count; i++)
    {
        Add(params[i], error);
        if (error)
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to add root parameter %1%.", i));
            return;
        }
    }
}

void D3D12RootParametersBuilder::Add(const D3D12ShaderRootParameterBuilder& param, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    if (this->params.full())
    {
        FINJIN_SET_ERROR(error, "The maximum number of root parameters have been added.");
        return;
    }

    CD3DX12_ROOT_PARAMETER newParam;
    FINJIN_ZERO_ITEM(newParam);

    switch (param.parameterType)
    {
        case D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE:
        {
            newParam.InitAsDescriptorTable(static_cast<UINT>(param.descriptorTable.size()), param.descriptorTable.data(), param.visibility);
            break;
        }
        case D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS:
        {
            newParam.InitAsConstants(param.num32BitValues, param.shaderRegister, param.registerSpace, param.visibility);
            break;
        }
        case D3D12_ROOT_PARAMETER_TYPE_CBV:
        {
            newParam.InitAsConstantBufferView(param.shaderRegister, param.registerSpace, param.visibility);
            break;
        }
        case D3D12_ROOT_PARAMETER_TYPE_SRV:
        {
            newParam.InitAsShaderResourceView(param.shaderRegister, param.registerSpace, param.visibility);
            break;
        }
        case D3D12_ROOT_PARAMETER_TYPE_UAV:
        {
            newParam.InitAsUnorderedAccessView(param.shaderRegister, param.registerSpace, param.visibility);
            break;
        }
    }

    this->params.push_back(newParam);
}

const D3D12_ROOT_PARAMETER* D3D12RootParametersBuilder::Get() const
{
    return this->params.data();
}

UINT D3D12RootParametersBuilder::GetCount() const
{
    return static_cast<UINT>(this->params.size());
}

//D3D12StaticSamplersBuilder
D3D12StaticSamplersBuilder::D3D12StaticSamplersBuilder()
{
    Reset();
}

void D3D12StaticSamplersBuilder::Reset()
{
    return this->samplers.clear();
}

ValueOrError<bool> D3D12StaticSamplersBuilder::Add(const D3D12_STATIC_SAMPLER_DESC& sampler)
{
    return this->samplers.push_back(sampler);
}

ValueOrError<bool> D3D12StaticSamplersBuilder::AddSequential(const D3D12_STATIC_SAMPLER_DESC& sampler)
{
    auto copy = sampler;
    copy.ShaderRegister = this->samplers.empty() ? 0 : this->samplers.back().ShaderRegister + 1;
    return this->samplers.push_back(copy);
}

ValueOrError<bool> D3D12StaticSamplersBuilder::AddSequential(D3D12_FILTER filter, D3D12_TEXTURE_ADDRESS_MODE addressMode)
{
    return AddSequential(CD3DX12_STATIC_SAMPLER_DESC(0, filter, addressMode, addressMode, addressMode));
}

void D3D12StaticSamplersBuilder::CreateDefaults()
{
    this->samplers.clear();
    Add(CD3DX12_STATIC_SAMPLER_DESC(0, D3D12_FILTER_MIN_MAG_MIP_POINT, D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP)); //pointWrap
    Add(CD3DX12_STATIC_SAMPLER_DESC(1, D3D12_FILTER_MIN_MAG_MIP_POINT, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP)); //pointClamp
    Add(CD3DX12_STATIC_SAMPLER_DESC(2, D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP)); //linearWrap
    Add(CD3DX12_STATIC_SAMPLER_DESC(3, D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP)); //linearClamp
    Add(CD3DX12_STATIC_SAMPLER_DESC(4, D3D12_FILTER_ANISOTROPIC, D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP, 0.0f, 8)); //anisotropicWrap
    Add(CD3DX12_STATIC_SAMPLER_DESC(5, D3D12_FILTER_ANISOTROPIC, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, 0.0f, 8)); //anisotropicClamp
}

const D3D12_STATIC_SAMPLER_DESC* D3D12StaticSamplersBuilder::Get() const
{
    return this->samplers.data();
}

UINT D3D12StaticSamplersBuilder::GetCount() const
{
    return static_cast<UINT>(this->samplers.size());
}

//D3D12RootSignatureBuilder
D3D12RootSignatureBuilder::D3D12RootSignatureBuilder()
{
    Reset();
}

void D3D12RootSignatureBuilder::Reset()
{
    this->rootParameters.Reset();
    this->staticSamplers.Reset();
    this->flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
}

D3D12RootParametersBuilder& D3D12RootSignatureBuilder::GetRootParameters()
{
    return this->rootParameters;
}

D3D12StaticSamplersBuilder& D3D12RootSignatureBuilder::GetStaticSamplers()
{
    return this->staticSamplers;
}

D3D12_ROOT_SIGNATURE_FLAGS D3D12RootSignatureBuilder::GetFlags() const
{
    return this->flags;
}

void D3D12RootSignatureBuilder::SetFlags(D3D12_ROOT_SIGNATURE_FLAGS value)
{
    this->flags = value;
}

void D3D12RootSignatureBuilder::Create(Microsoft::WRL::ComPtr<ID3D12RootSignature>& rootSig, ID3D12Device* device, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc
        (
        this->rootParameters.GetCount(),
        this->rootParameters.Get(),
        this->staticSamplers.GetCount(),
        this->staticSamplers.Get(),
        this->flags
        );

    Microsoft::WRL::ComPtr<ID3DBlob> serializedRootSig;
    Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;
    auto result = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1, serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());
    if (FINJIN_CHECK_HRESULT_FAILED(result))
    {
        FINJIN_D3D12_SET_ERROR_BLOB(error, "Failed to serialize root signature", errorBlob);
        return;
    }

    result = device->CreateRootSignature
        (
        0,
        serializedRootSig->GetBufferPointer(),
        serializedRootSig->GetBufferSize(),
        IID_PPV_ARGS(rootSig.GetAddressOf())
        );
    if (FINJIN_CHECK_HRESULT_FAILED(result))
    {
        FINJIN_SET_ERROR(error, "Failed to create root signature.");
        return;
    }
}

#endif
