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

#include "D3D12PipelineState.hpp"
#include "D3D12Utilities.hpp"

using namespace Finjin::Engine;


//Implementation----------------------------------------------------------------
D3D12PipelineStateDescriptor::D3D12PipelineStateDescriptor(Allocator* allocator) :
    typeName(allocator),
    rootSignatureType(allocator),
    inputFormatType(allocator)
{
    for (auto& shaderRef : this->shaderRefs)
        shaderRef.Create(allocator);
}

void D3D12PipelineStateDescriptor::Create
    (
    AssetReference& shaderFileRef,
    DynamicVector<D3D12PipelineStateDescriptor>& pipelineStates,
    Allocator* allocator,
    const ByteBuffer& readBuffer,
    SimpleUri& tempUri,
    Error& error
    )
{
    FINJIN_ERROR_METHOD_START(error);

    ConfigDocumentReader reader;
    reader.Start(readBuffer);

    Create(shaderFileRef, pipelineStates, allocator, reader, tempUri, error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

void D3D12PipelineStateDescriptor::Create
    (
    AssetReference& shaderFileRef,
    DynamicVector<D3D12PipelineStateDescriptor>& pipelineStates,
    Allocator* allocator,
    ConfigDocumentReader& reader,
    SimpleUri& tempUri,
    Error& error
    )
{
    FINJIN_ERROR_METHOD_START(error);

    static Utf8String configSectionName("pipeline-states");

    if (reader.Current() == nullptr)
    {
        FINJIN_SET_ERROR(error, "The specified reader ended unexpectedly.");
        return;
    }

    auto startLine = *reader.Current();

    auto searchDone = false;
    Utf8StringView sectionName;
    for (auto line = reader.Current(); line != nullptr && !searchDone; line = reader.Next())
    {
        switch (line->GetType())
        {
            case ConfigDocumentLine::Type::SECTION:
            {
                line->GetSectionName(sectionName);

                if (sectionName != configSectionName)
                    searchDone = true;

                break;
            }
            case ConfigDocumentLine::Type::SCOPE_START:
            {
                searchDone = true;
                break;
            }
            default: break;
        }
    }

    if (sectionName == configSectionName)
    {
        sectionName.clear();

        size_t pipelineStateCount = 0;

        for (auto line = reader.Current(); line != nullptr; line = reader.Next())
        {
            switch (line->GetType())
            {
                case ConfigDocumentLine::Type::KEY_AND_VALUE:
                {
                    Utf8StringView key, value;
                    line->GetKeyAndValue(key, value);

                    if (key == "shader-file-ref")
                    {
                        shaderFileRef.ForUriString(value, tempUri, error);
                        if (error)
                        {
                            FINJIN_SET_ERROR(error, "Failed to parse shader file reference string.");
                            return;
                        }
                    }
                    else if (key == "count")
                    {
                        auto pipelineStateCount = Convert::ToInteger(value.ToString(), (size_t)0);
                        if (pipelineStateCount > 0)
                        {
                            if (!pipelineStates.Create(pipelineStateCount, allocator, allocator))
                            {
                                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to allocate '%1%' pipeline states.", pipelineStateCount));
                                return;
                            }

                            pipelineStateCount = 0;
                        }
                    }

                    break;
                }
                case ConfigDocumentLine::Type::SECTION:
                {
                    auto& pipelineState = pipelineStates[pipelineStateCount++];

                    CreateFromScope(pipelineState, allocator, reader, tempUri, pipelineStates.data(), pipelineStateCount - 1, error);
                    if (error)
                    {
                        FINJIN_SET_ERROR(error, "Failed to read buffer pipeline state.");
                        return;
                    }

                    break;
                }
                default: break;
            }
        }
    }

    reader.Restart(startLine);
}

void D3D12PipelineStateDescriptor::CreateFromScope
    (
    D3D12PipelineStateDescriptor& pipelineState,
    Allocator* allocator,
    ConfigDocumentReader& reader,
    SimpleUri& tempUri,
    D3D12PipelineStateDescriptor* otherPipelineStates,
    size_t otherPipelineStateCount,
    Error& error
    )
{
    FINJIN_ERROR_METHOD_START(error);

    int depth = 0;
    auto descriptionDone = false;

    for (auto line = reader.Next(); line != nullptr && !descriptionDone; )
    {
        switch (line->GetType())
        {
            case ConfigDocumentLine::Type::KEY_AND_VALUE:
            {
                Utf8StringView key, value;
                line->GetKeyAndValue(key, value);

                if (key == "type")
                {
                    if (pipelineState.typeName.assign(value.begin(), value.size()).HasError())
                    {
                        FINJIN_SET_ERROR(error, "Failed to assign pipeline state type.");
                        return;
                    }
                }
                else if (key == "root-signature")
                {
                    if (pipelineState.rootSignatureType.assign(value.begin(), value.size()).HasError())
                    {
                        FINJIN_SET_ERROR(error, "Failed to assign root signature type.");
                        return;
                    }
                }
                else if (key == "input-format")
                {
                    if (pipelineState.inputFormatType.assign(value.begin(), value.size()).HasError())
                    {
                        FINJIN_SET_ERROR(error, "Failed to assign input format type.");
                        return;
                    }
                }
                else if (key.StartsWith("vertex-shader-"))
                {
                    auto component = key.substr(14);
                    if (component == "offset")
                    {
                        auto splitResult = pipelineState.shaderOffsets[D3D12ShaderType::VERTEX].Parse(value);
                        if (splitResult.HasError())
                        {
                            FINJIN_SET_ERROR(error, "Failed to split 'vertex-shader-offset' values.");
                            return;
                        }
                    }
                    else if (component == "ref")
                    {
                        pipelineState.shaderRefs[D3D12ShaderType::VERTEX].ForUriString(value, tempUri, error);
                        if (error)
                        {
                            FINJIN_SET_ERROR(error, "Failed to assign vertex shader reference.");
                            return;
                        }
                    }
                }
                if (key.StartsWith("pixel-shader-"))
                {
                    auto component = key.substr(13);
                    if (component == "offset")
                    {
                        auto splitResult = pipelineState.shaderOffsets[D3D12ShaderType::PIXEL].Parse(value);
                        if (splitResult.HasError())
                        {
                            FINJIN_SET_ERROR(error, "Failed to split 'pixel-shader-offset' values.");
                            return;
                        }
                    }
                    else if (component == "ref")
                    {
                        pipelineState.shaderRefs[D3D12ShaderType::PIXEL].ForUriString(value, tempUri, error);
                        if (error)
                        {
                            FINJIN_SET_ERROR(error, "Failed to assign pixel shader reference.");
                            return;
                        }
                    }
                }
                else if (key == "primitive-topology")
                {
                    if (value == "tr" || value == "triangle")
                        pipelineState.graphicsState.primitiveTopology = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
                    else if (value == "li" || value == "line")
                        pipelineState.graphicsState.primitiveTopology = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
                    else if (value == "po" || value == "point")
                        pipelineState.graphicsState.primitiveTopology = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
                    else if (value == "pa" || value == "patch")
                        pipelineState.graphicsState.primitiveTopology = D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;
                    else
                    {
                        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Invalid primitive topology '%1%'.", value));
                        return;
                    }
                }
                else if (key == "feature-flags")
                {
                    pipelineState.graphicsState.shaderFeatures.ParseAndAddFlags(value, error);
                    if (error)
                    {
                        FINJIN_SET_ERROR(error, "Failed to parse feature flags.");
                        return;
                    }
                }
                else if (key == "lights")
                {
                    pipelineState.graphicsState.shaderFeatures.ParseAndAddLightTypes(value, error);
                    if (error)
                    {
                        FINJIN_SET_ERROR(error, "Failed to parse light types.");
                        return;
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

#endif
