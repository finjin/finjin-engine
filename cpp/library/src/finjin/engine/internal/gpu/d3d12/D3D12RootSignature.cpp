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
#include "D3D12RootSignatureBuilder.hpp"
#include "D3D12Utilities.hpp"

using namespace Finjin::Engine;


//Implementation----------------------------------------------------------------

//D3D12RootSignatureDescriptor
D3D12RootSignatureDescriptor::D3D12RootSignatureDescriptor(Allocator* allocator) : typeName(allocator)
{
}

void D3D12RootSignatureDescriptor::CreateRootSignature(Microsoft::WRL::ComPtr<ID3D12RootSignature>& result, ID3D12Device* device, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    D3D12RootSignatureBuilder rootSignatureBuilder;

    for (auto& rootSignatureElement : this->elements)
    {
        if (rootSignatureElement.type == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE)
        {
            D3D12ShaderRootParameterBuilder table;
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
            D3D12ShaderRootParameterBuilder parameter(rootSignatureElement.type, static_cast<UINT>(rootSignatureElement.shaderRegister), static_cast<UINT>(rootSignatureElement.registerSpace), rootSignatureElement.visibility);
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

    auto elementCount = reader.GetSectionCountBeneathCurrent(Utf8String::GetEmpty());
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

                        descriptorTableElementCount = reader.GetSectionCountBeneathCurrent(Utf8String::GetEmpty());
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
