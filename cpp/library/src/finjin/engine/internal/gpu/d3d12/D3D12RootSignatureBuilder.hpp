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
#include "finjin/common/StaticVector.hpp"
#include "D3D12Includes.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    struct D3D12DescriptorRange : public D3D12_DESCRIPTOR_RANGE
    {
        D3D12DescriptorRange();

        D3D12DescriptorRange
            (
            D3D12_DESCRIPTOR_RANGE_TYPE rangeType,
            UINT numDescriptors,
            UINT baseShaderRegister,
            UINT registerSpace,
            UINT offsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND
            );
    };

    struct D3D12ShaderRootParameterBuilder
    {
        D3D12ShaderRootParameterBuilder();
        D3D12ShaderRootParameterBuilder(D3D12_ROOT_PARAMETER_TYPE parameterType, UINT shaderRegister, UINT registerSpace, D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL);
        D3D12ShaderRootParameterBuilder(UINT shaderRegister, UINT registerSpace, UINT num32BitValues, D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL);

        D3D12_ROOT_PARAMETER_TYPE parameterType;
        UINT shaderRegister;
        UINT registerSpace;
        UINT num32BitValues;
        D3D12_SHADER_VISIBILITY visibility;

        StaticVector<D3D12DescriptorRange, 8> descriptorTable;
    };

    class D3D12RootParametersBuilder
    {
    public:
        D3D12RootParametersBuilder();

        void Reset();

        void Set(const D3D12ShaderRootParameterBuilder* params, size_t count, Error& error);

        void Add(const D3D12ShaderRootParameterBuilder& param, Error& error);

        const D3D12_ROOT_PARAMETER* Get() const;

        UINT GetCount() const;

    private:
        StaticVector<CD3DX12_ROOT_PARAMETER, 64> params;
    };

    class D3D12StaticSamplersBuilder
    {
    public:
        D3D12StaticSamplersBuilder();

        void Reset();

        ValueOrError<bool> Add(const D3D12_STATIC_SAMPLER_DESC& sampler); //Directly adds the sampler description
        ValueOrError<bool> AddSequential(const D3D12_STATIC_SAMPLER_DESC& sampler); //Modifies the sampler description's ShaderRegister member to be 1 more than current last ShaderRegister
        ValueOrError<bool> AddSequential(D3D12_FILTER filter, D3D12_TEXTURE_ADDRESS_MODE addressMode);
        void CreateDefaults();

        const D3D12_STATIC_SAMPLER_DESC* Get() const;

        UINT GetCount() const;

    private:
        StaticVector<D3D12_STATIC_SAMPLER_DESC, 16> samplers;
    };

    class D3D12RootSignatureBuilder
    {
    public:
        D3D12RootSignatureBuilder();

        void Reset();

        D3D12RootParametersBuilder& GetRootParameters();
        D3D12StaticSamplersBuilder& GetStaticSamplers();

        D3D12_ROOT_SIGNATURE_FLAGS GetFlags() const;
        void SetFlags(D3D12_ROOT_SIGNATURE_FLAGS value);

        void Create(Microsoft::WRL::ComPtr<ID3D12RootSignature>& rootSig, ID3D12Device* device, Error& error);

    private:
        D3D12RootParametersBuilder rootParameters;
        D3D12StaticSamplersBuilder staticSamplers;
        D3D12_ROOT_SIGNATURE_FLAGS flags;
    };

} }
