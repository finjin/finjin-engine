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

#if FINJIN_TARGET_GPU_SYSTEM == FINJIN_TARGET_GPU_SYSTEM_VULKAN

#include "VulkanPipeline.hpp"
#include "VulkanUtilities.hpp"

using namespace Finjin::Engine;


//Implementation----------------------------------------------------------------
VulkanPipelineDescription::VulkanPipelineDescription(Allocator* allocator) :
    typeName(allocator),
    pipelineLayoutType(allocator),
    inputFormatType(allocator)
{
    for (auto& shaderRef : this->shaderRefs)
        shaderRef.Create(allocator);
}

void VulkanPipelineDescription::Create
    (
    AssetReference& shaderFileRef,
    DynamicVector<VulkanPipelineDescription>& pipelines,
    Allocator* allocator,
    const ByteBuffer& readBuffer,
    SimpleUri& tempUri,
    Error& error
    )
{
    FINJIN_ERROR_METHOD_START(error);

    ConfigDocumentReader reader;
    reader.Start(readBuffer);

    Create(shaderFileRef, pipelines, allocator, reader, tempUri, error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

void VulkanPipelineDescription::Create
    (
    AssetReference& shaderFileRef,
    DynamicVector<VulkanPipelineDescription>& pipelines,
    Allocator* allocator,
    ConfigDocumentReader& reader,
    SimpleUri& tempUri,
    Error& error
    )
{
    FINJIN_ERROR_METHOD_START(error);

    static Utf8String configSectionName("pipelines");

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

        size_t pipelineCount = 0;

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
                        auto pipelineCount = Convert::ToInteger(value.ToString(), (size_t)0);
                        if (pipelineCount > 0)
                        {
                            if (!pipelines.Create(pipelineCount, allocator, allocator))
                            {
                                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to allocate '%1%' pipelines.", pipelineCount));
                                return;
                            }

                            pipelineCount = 0;
                        }
                    }

                    break;
                }
                case ConfigDocumentLine::Type::SECTION:
                {
                    auto& pipeline = pipelines[pipelineCount++];

                    CreateFromScope(pipeline, allocator, reader, tempUri, pipelines.data(), pipelineCount - 1, error);
                    if (error)
                    {
                        FINJIN_SET_ERROR(error, "Failed to read buffer pipeline.");
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

void VulkanPipelineDescription::CreateFromScope
    (
    VulkanPipelineDescription& pipeline,
    Allocator* allocator,
    ConfigDocumentReader& reader,
    SimpleUri& tempUri,
    VulkanPipelineDescription* otherPipelines,
    size_t otherPipelineCount,
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
                    if (pipeline.typeName.assign(value.begin(), value.size()).HasError())
                    {
                        FINJIN_SET_ERROR(error, "Failed to assign pipeline type.");
                        return;
                    }
                }
                else if (key == "pipeline-layout")
                {
                    if (pipeline.pipelineLayoutType.assign(value.begin(), value.size()).HasError())
                    {
                        FINJIN_SET_ERROR(error, "Failed to assign descriptor set layout type.");
                        return;
                    }
                }
                else if (key == "input-format")
                {
                    if (pipeline.inputFormatType.assign(value.begin(), value.size()).HasError())
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
                        auto splitResult = pipeline.shaderOffsets[VulkanShaderType::VERTEX].Parse(value);
                        if (splitResult.HasError())
                        {
                            FINJIN_SET_ERROR(error, "Failed to split 'vertex-shader-offset' values.");
                            return;
                        }
                    }
                    else if (component == "ref")
                    {
                        pipeline.shaderRefs[VulkanShaderType::VERTEX].ForUriString(value, tempUri, error);
                        if (error)
                        {
                            FINJIN_SET_ERROR(error, "Failed to assign vertex shader reference.");
                            return;
                        }
                    }
                }
                if (key.StartsWith("fragment-shader-"))
                {
                    auto component = key.substr(16);
                    if (component == "offset")
                    {
                        auto splitResult = pipeline.shaderOffsets[VulkanShaderType::FRAGMENT].Parse(value);
                        if (splitResult.HasError())
                        {
                            FINJIN_SET_ERROR(error, "Failed to split 'fragment-shader-offset' values.");
                            return;
                        }
                    }
                    else if (component == "ref")
                    {
                        pipeline.shaderRefs[VulkanShaderType::FRAGMENT].ForUriString(value, tempUri, error);
                        if (error)
                        {
                            FINJIN_SET_ERROR(error, "Failed to assign fragment shader reference.");
                            return;
                        }
                    }
                }
                else if (key == "primitive-topology")
                {
                    if (value == "tr" || value == "triangle")
                        pipeline.graphicsState.primitiveTopology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
                    else if (value == "li" || value == "line")
                        pipeline.graphicsState.primitiveTopology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
                    else if (value == "po" || value == "point")
                        pipeline.graphicsState.primitiveTopology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
                    else if (value == "pa" || value == "patch")
                        pipeline.graphicsState.primitiveTopology = VK_PRIMITIVE_TOPOLOGY_PATCH_LIST;
                    else
                    {
                        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Invalid primitive topology '%1%'.", value));
                        return;
                    }
                }
                else if (key == "dynamic-state")
                {
                    auto splitResult = Split(value, ' ', [&pipeline](Utf8StringView& value)
                    {
                        if (value == "viewport" || value == "vp")
                            pipeline.graphicsState.dynamicState.push_back(VK_DYNAMIC_STATE_VIEWPORT);
                        else if (value == "scissor" || value == "sc")
                            pipeline.graphicsState.dynamicState.push_back(VK_DYNAMIC_STATE_SCISSOR);
                        else if (value == "line-width" || value == "lw")
                            pipeline.graphicsState.dynamicState.push_back(VK_DYNAMIC_STATE_LINE_WIDTH);
                        else if (value == "depth-bias" || value == "db")
                            pipeline.graphicsState.dynamicState.push_back(VK_DYNAMIC_STATE_DEPTH_BIAS);
                        else if (value == "blend-constants" || value == "bc")
                            pipeline.graphicsState.dynamicState.push_back(VK_DYNAMIC_STATE_BLEND_CONSTANTS);
                        else if (value == "depth-bounds" || value == "db")
                            pipeline.graphicsState.dynamicState.push_back(VK_DYNAMIC_STATE_DEPTH_BOUNDS);
                        else if (value == "stencil-compare-mask" || value == "scm")
                            pipeline.graphicsState.dynamicState.push_back(VK_DYNAMIC_STATE_STENCIL_COMPARE_MASK);
                        else if (value == "stencil-write-mask" || value == "swm")
                            pipeline.graphicsState.dynamicState.push_back(VK_DYNAMIC_STATE_STENCIL_WRITE_MASK);
                        else if (value == "stencil-reference" || value == "sr")
                            pipeline.graphicsState.dynamicState.push_back(VK_DYNAMIC_STATE_STENCIL_REFERENCE);
                        else
                            return ValueOrError<bool>::CreateError();

                        return ValueOrError<bool>(true);
                    });
                    if (splitResult.HasError())
                    {
                        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Invalid dynamic state values '%1%'.", value));
                        return;
                    }
                }
                else if (key == "pipeline-layout")
                {
                    if (pipeline.pipelineLayoutType.assign(value.begin(), value.size()).HasError())
                    {
                        FINJIN_SET_ERROR(error, "Failed to assign pipeline layout type.");
                        return;
                    }
                }
                else if (key == "feature-flags")
                {
                    pipeline.graphicsState.shaderFeatures.ParseAndAddFlags(value, error);
                    if (error)
                    {
                        FINJIN_SET_ERROR(error, "Failed to parse feature flags.");
                        return;
                    }
                }
                else if (key == "lights")
                {
                    pipeline.graphicsState.shaderFeatures.ParseAndAddLightTypes(value, error);
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
