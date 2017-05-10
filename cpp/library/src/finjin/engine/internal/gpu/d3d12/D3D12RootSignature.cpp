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

#include "D3D12RootSignature.hpp"
#include "finjin/common/Convert.hpp"
#include "finjin/common/StaticVector.hpp"
#include "D3D12Utilities.hpp"

using namespace Finjin::Engine;


//Local types-------------------------------------------------------------------
struct D3D12DescriptorRange : public D3D12_DESCRIPTOR_RANGE
{
    D3D12DescriptorRange() { FINJIN_ZERO_ITEM(*this); }

    D3D12DescriptorRange
        (
        D3D12_DESCRIPTOR_RANGE_TYPE rangeType,
        UINT numDescriptors,
        UINT baseShaderRegister,
        UINT registerSpace,
        UINT offsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND
        )
    {
        this->RangeType = rangeType;
        this->NumDescriptors = numDescriptors;
        this->BaseShaderRegister = baseShaderRegister;
        this->RegisterSpace = registerSpace;
        this->OffsetInDescriptorsFromTableStart = offsetInDescriptorsFromTableStart;
    }
};

struct D3D12ShaderRootParameter
{
    D3D12ShaderRootParameter()
    {
        this->parameterType = (D3D12_ROOT_PARAMETER_TYPE)-1;
        this->shaderRegister = 0;
        this->registerSpace = 0;
        this->num32BitValues = 0;
        this->visibility = D3D12_SHADER_VISIBILITY_ALL;
    }

    D3D12ShaderRootParameter(D3D12_ROOT_PARAMETER_TYPE parameterType, UINT shaderRegister, UINT registerSpace, D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL)
    {
        this->parameterType = parameterType;
        this->shaderRegister = shaderRegister;
        this->registerSpace = registerSpace;
        this->num32BitValues = 0;
        this->visibility = visibility;
    }

    D3D12ShaderRootParameter(UINT shaderRegister, UINT registerSpace, UINT num32BitValues, D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL)
    {
        this->parameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
        this->shaderRegister = shaderRegister;
        this->registerSpace = registerSpace;
        this->num32BitValues = num32BitValues;
        this->visibility = visibility;
    }

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
    D3D12RootParametersBuilder() { }

    void Reset() { this->params.clear(); }

    void Set(const D3D12ShaderRootParameter* params, size_t count, Error& error)
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

    void Add(const D3D12ShaderRootParameter& param, Error& error)
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

    const D3D12_ROOT_PARAMETER* Get() const { return this->params.data(); }

    UINT GetCount() const { return static_cast<UINT>(this->params.size()); }

private:
    StaticVector<CD3DX12_ROOT_PARAMETER, 64> params;
};

class D3D12StaticSamplersBuilder
{
public:
    D3D12StaticSamplersBuilder() { }

    void Reset() { return this->samplers.clear(); }

    void CreateDefaults()
    {
        const CD3DX12_STATIC_SAMPLER_DESC pointWrap
            (
            0,
            D3D12_FILTER_MIN_MAG_MIP_POINT,
            D3D12_TEXTURE_ADDRESS_MODE_WRAP,
            D3D12_TEXTURE_ADDRESS_MODE_WRAP,
            D3D12_TEXTURE_ADDRESS_MODE_WRAP
            );

        const CD3DX12_STATIC_SAMPLER_DESC pointClamp
            (
            1,
            D3D12_FILTER_MIN_MAG_MIP_POINT,
            D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
            D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
            D3D12_TEXTURE_ADDRESS_MODE_CLAMP
            );

        const CD3DX12_STATIC_SAMPLER_DESC linearWrap
            (
            2,
            D3D12_FILTER_MIN_MAG_MIP_LINEAR,
            D3D12_TEXTURE_ADDRESS_MODE_WRAP,
            D3D12_TEXTURE_ADDRESS_MODE_WRAP,
            D3D12_TEXTURE_ADDRESS_MODE_WRAP
            );

        const CD3DX12_STATIC_SAMPLER_DESC linearClamp
            (
            3,
            D3D12_FILTER_MIN_MAG_MIP_LINEAR,
            D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
            D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
            D3D12_TEXTURE_ADDRESS_MODE_CLAMP
            );

        const CD3DX12_STATIC_SAMPLER_DESC anisotropicWrap
            (
            4,
            D3D12_FILTER_ANISOTROPIC,
            D3D12_TEXTURE_ADDRESS_MODE_WRAP,
            D3D12_TEXTURE_ADDRESS_MODE_WRAP,
            D3D12_TEXTURE_ADDRESS_MODE_WRAP,
            0.0f, //mipLODBias
            8 //maxAnisotropy
            );

        const CD3DX12_STATIC_SAMPLER_DESC anisotropicClamp
            (
            5,
            D3D12_FILTER_ANISOTROPIC,
            D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
            D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
            D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
            0.0f, //mipLODBias
            8 //maxAnisotropy
            );

        CD3DX12_STATIC_SAMPLER_DESC staticSamplers[] =
            {
            pointWrap,
            pointClamp,
            linearWrap,
            linearClamp,
            anisotropicWrap,
            anisotropicClamp
            };

        this->samplers.assign(staticSamplers, FINJIN_COUNT_OF(staticSamplers));
    }

    const CD3DX12_STATIC_SAMPLER_DESC* Get() const { return this->samplers.data(); }

    UINT GetCount() const { return static_cast<UINT>(this->samplers.size()); }

private:
    StaticVector<CD3DX12_STATIC_SAMPLER_DESC, 16> samplers;
};

class D3D12RootSignature
{
public:
    D3D12RootSignature() { Reset(); }

    void Reset()
    {
        this->rootParameters.Reset();
        this->staticSamplers.Reset();
        this->flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
    }

    D3D12RootParametersBuilder& GetRootParameters() { return this->rootParameters; }
    D3D12StaticSamplersBuilder& GetStaticSamplers() { return this->staticSamplers; }

    D3D12_ROOT_SIGNATURE_FLAGS GetFlags() const { return this->flags; }
    void SetFlags(D3D12_ROOT_SIGNATURE_FLAGS value) { this->flags = value; }

    void Create(Microsoft::WRL::ComPtr<ID3D12RootSignature>& rootSig, ID3D12Device* device, Error& error)
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

private:
    D3D12RootParametersBuilder rootParameters;
    D3D12StaticSamplersBuilder staticSamplers;
    D3D12_ROOT_SIGNATURE_FLAGS flags;
};


//Implementation----------------------------------------------------------------

//D3D12RootSignatureDescriptor
D3D12RootSignatureDescriptor::D3D12RootSignatureDescriptor(Allocator* allocator) : typeName(allocator)
{
}

void D3D12RootSignatureDescriptor::CreateRootSignature(Microsoft::WRL::ComPtr<ID3D12RootSignature>& result, ID3D12Device* device, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    D3D12RootSignature rootSignatureBuilder;

    for (auto& rootSignatureElement : this->elements)
    {
        if (rootSignatureElement.type == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE)
        {
            D3D12ShaderRootParameter table;
            table.parameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
            table.visibility = rootSignatureElement.visibility;

            for (size_t descriptorTableElementIndex = 0; descriptorTableElementIndex < rootSignatureElement.descriptorTable.size(); descriptorTableElementIndex++)
            {
                auto& descriptorTableElement = rootSignatureElement.descriptorTable[descriptorTableElementIndex];

                D3D12DescriptorRange range(descriptorTableElement.type, static_cast<UINT>(descriptorTableElement.descriptorCount), static_cast<UINT>(descriptorTableElement.shaderRegister), static_cast<UINT>(descriptorTableElement.registerSpace));
                if (table.descriptorTable.push_back(range).HasErrorOrValue(false))
                {
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to add descriptor range for element '%1%'. Too many entries.", rootSignatureElement.id));
                    return;
                }
            }

            rootSignatureBuilder.GetRootParameters().Add(table, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to add descriptor table entry element '%1%'.", rootSignatureElement.id));
                return;
            }
        }
        else
        {
            D3D12ShaderRootParameter parameter(rootSignatureElement.type, static_cast<UINT>(rootSignatureElement.shaderRegister), static_cast<UINT>(rootSignatureElement.registerSpace), rootSignatureElement.visibility);
            rootSignatureBuilder.GetRootParameters().Add(parameter, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to add element '%1%'.", rootSignatureElement.id));
                return;
            }
        }
    }

    rootSignatureBuilder.GetStaticSamplers().CreateDefaults();

    rootSignatureBuilder.Create(result, device, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create root signature '%1%'.", this->typeName));
        return;
    }
}

void D3D12RootSignatureDescriptor::Create
    (
    DynamicVector<D3D12RootSignatureDescriptor>& rootSignatures,
    Allocator* allocator,
    const ByteBuffer& readBuffer,
    Error& error
    )
{
    FINJIN_ERROR_METHOD_START(error);

    ConfigDocumentReader reader;
    reader.Start(readBuffer);

    Create(rootSignatures, allocator, reader, error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

void D3D12RootSignatureDescriptor::Create
    (
    DynamicVector<D3D12RootSignatureDescriptor>& rootSignatures,
    Allocator* allocator,
    ConfigDocumentReader& reader,
    Error& error
    )
{
    FINJIN_ERROR_METHOD_START(error);

    static Utf8String configSectionName("root-signature");

    if (reader.Current() == nullptr)
    {
        FINJIN_SET_ERROR(error, "The specified reader ended unexpectedly.");
        return;
    }

    auto startLine = *reader.Current();

    auto rootSignatureCount = reader.GetSectionCount(configSectionName);
    if (rootSignatureCount > 0)
    {
        if (!rootSignatures.Create(rootSignatureCount, allocator, allocator))
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to allocate '%1%' root signatures.", rootSignatureCount));
            return;
        }

        rootSignatureCount = 0;

        for (auto line = reader.Current(); line != nullptr; line = reader.Next())
        {
            switch (line->GetType())
            {
                case ConfigDocumentLine::Type::SECTION:
                {
                    Utf8StringView sectionName;
                    line->GetSectionName(sectionName);

                    if (sectionName == configSectionName)
                    {
                        auto& rootSignature = rootSignatures[rootSignatureCount++];

                        CreateFromScope(rootSignature, allocator, reader, error);
                        if (error)
                        {
                            FINJIN_SET_ERROR(error, "Failed to read buffer root signature.");
                            return;
                        }
                    }

                    break;
                }
                default: break;
            }
        }
    }

    reader.Restart(startLine);
}

void D3D12RootSignatureDescriptor::CreateFromScope
    (
    D3D12RootSignatureDescriptor& rootSignature,
    Allocator* allocator,
    ConfigDocumentReader& reader,
    Error& error
    )
{
    FINJIN_ERROR_METHOD_START(error);

    auto elementCount = reader.GetSectionCountBeneathCurrent(Utf8String::Empty());
    if (elementCount > 0)
    {
        if (!rootSignature.elements.Create(elementCount, allocator, allocator))
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Unable to allocate '%1%' elements.", elementCount));
            return;
        }

        elementCount = 0;
        size_t descriptorTableElementCount = 0;

        int depth = 0;
        auto descriptionDone = false;

        for (auto line = reader.Next(); line != nullptr && !descriptionDone; )
        {
            switch (line->GetType())
            {
                case ConfigDocumentLine::Type::SECTION:
                {
                    Utf8StringView sectionName;
                    line->GetSectionName(sectionName);

                    if (sectionName == "constant-buffer-view")
                    {
                        descriptorTableElementCount = 0;

                        elementCount++;
                        auto& element = rootSignature.elements[elementCount - 1];

                        element.type = D3D12_ROOT_PARAMETER_TYPE_CBV;
                    }
                    else if (sectionName == "shader-resource-view")
                    {
                        descriptorTableElementCount = 0;

                        elementCount++;
                        auto& element = rootSignature.elements[elementCount - 1];

                        element.type = D3D12_ROOT_PARAMETER_TYPE_SRV;
                    }
                    else if (sectionName == "unordered-access-view")
                    {
                        descriptorTableElementCount = 0;

                        elementCount++;
                        auto& element = rootSignature.elements[elementCount - 1];

                        element.type = D3D12_ROOT_PARAMETER_TYPE_UAV;
                    }
                    else if (sectionName == "descriptor-table")
                    {
                        elementCount++;
                        auto& element = rootSignature.elements[elementCount - 1];

                        element.type = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;

                        descriptorTableElementCount = reader.GetSectionCountBeneathCurrent(Utf8String::Empty());
                        if (!element.descriptorTable.Create(descriptorTableElementCount, allocator, allocator))
                        {
                            FINJIN_SET_ERROR(error, "Failed to create descriptor table elements.");
                            return;
                        }

                        descriptorTableElementCount = 0;
                    }
                    else if (sectionName == "constant-buffer-view-range")
                    {
                        auto& element = rootSignature.elements[elementCount - 1];

                        descriptorTableElementCount++;
                        auto& descriptorTableElement = element.descriptorTable[descriptorTableElementCount - 1];

                        descriptorTableElement.type = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
                    }
                    else if (sectionName == "shader-resource-view-range")
                    {
                        auto& element = rootSignature.elements[elementCount - 1];

                        descriptorTableElementCount++;
                        auto& descriptorTableElement = element.descriptorTable[descriptorTableElementCount - 1];

                        descriptorTableElement.type = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
                    }
                    else if (sectionName == "unordered-access-view-range")
                    {
                        auto& element = rootSignature.elements[elementCount - 1];

                        descriptorTableElementCount++;
                        auto& descriptorTableElement = element.descriptorTable[descriptorTableElementCount - 1];

                        descriptorTableElement.type = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
                    }
                    else
                        reader.SkipScope();

                    break;
                }
                case ConfigDocumentLine::Type::KEY_AND_VALUE:
                {
                    Utf8StringView key, value;
                    line->GetKeyAndValue(key, value);

                    auto& element = rootSignature.elements[elementCount - 1];

                    if (depth == 1)
                    {
                        if (key == "type")
                        {
                            if (rootSignature.typeName.assign(value.begin(), value.size()).HasError())
                            {
                                FINJIN_SET_ERROR(error, "Failed to assign root signature type.");
                                return;
                            }
                        }
                    }
                    else if (depth == 2)
                    {
                        if (key == "id")
                            element.id = value;
                        else if (key == "register")
                            element.shaderRegister = Convert::ToInteger(value, element.shaderRegister);
                        else if (key == "space")
                            element.registerSpace = Convert::ToInteger(value, element.registerSpace);
                        else if (key == "visibility")
                        {
                            if (value == "all")
                                element.visibility = D3D12_SHADER_VISIBILITY_ALL;
                            else if (value == "vertex")
                                element.visibility = D3D12_SHADER_VISIBILITY_VERTEX;
                            else if (value == "hull")
                                element.visibility = D3D12_SHADER_VISIBILITY_HULL;
                            else if (value == "domain")
                                element.visibility = D3D12_SHADER_VISIBILITY_DOMAIN;
                            else if (value == "geometry")
                                element.visibility = D3D12_SHADER_VISIBILITY_GEOMETRY;
                            else if (value == "pixel")
                                element.visibility = D3D12_SHADER_VISIBILITY_PIXEL;
                        }
                    }
                    else if (depth == 3)
                    {
                        auto& descriptorTableElement = element.descriptorTable[descriptorTableElementCount - 1];

                        if (key == "id")
                            descriptorTableElement.id = value;
                        else if (key == "register")
                            descriptorTableElement.shaderRegister = Convert::ToInteger(value, descriptorTableElement.shaderRegister);
                        else if (key == "space")
                            descriptorTableElement.registerSpace = Convert::ToInteger(value, descriptorTableElement.registerSpace);
                        else if (key == "descriptor-count")
                        {
                            if (value == "-1")
                                descriptorTableElement.descriptorCount = (size_t)-1;
                            else
                                descriptorTableElement.descriptorCount = Convert::ToInteger(value, descriptorTableElement.descriptorCount);
                        }
                    }

                    break;
                }
                case ConfigDocumentLine::Type::SCOPE_START:
                {
                    depth++;
                    break;
                }
                case ConfigDocumentLine::Type::SCOPE_END:
                {
                    depth--;
                    if (depth == 0)
                        descriptionDone = true;
                    break;
                }
                default: break;
            }

            if (!descriptionDone)
                line = reader.Next();
        }
    }
}

#endif
