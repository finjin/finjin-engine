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

#include "VulkanStaticMeshRenderer.hpp"
#include "VulkanGpuContextImpl.hpp"
#include "finjin/common/Convert.hpp"
#include "finjin/common/Math.hpp"
#include "finjin/engine/ShaderFeatures.hpp"
#include "VulkanDescriptorSetLayout.hpp"
#include "VulkanPipelineLayout.hpp"

using namespace Finjin::Engine;


//Macros------------------------------------------------------------------------
#define ALLOW_LIGHT_FALLBACK_IN_FAILED_SIGNATURE_LOOKUP 1
#define MAX_HEAP_DESCRIPTOR_COMBINATIONS 10
#define VERTEX_BUFFER_BIND_ID 0
#define MAX_DESCRIPTOR_SET_LAYOUT_BINDINGS 64


//Implementation----------------------------------------------------------------

//VulkanStaticMeshRenderer::Settings
VulkanStaticMeshRenderer::Settings::Settings()
{
    this->contextImpl = nullptr;
    this->maxRenderables = 100;
}

VulkanStaticMeshRenderer::Settings& VulkanStaticMeshRenderer::Settings::operator = (Settings&& other)
{
    this->contextImpl = other.contextImpl;

    this->maxRenderables = other.maxRenderables;

    this->pipelineShaderFileReference = std::move(other.pipelineShaderFileReference);

    this->uniformBufferFormats = std::move(other.uniformBufferFormats);
    this->descriptorSetLayouts = std::move(other.descriptorSetLayouts);
    this->pipelineLayouts = std::move(other.pipelineLayouts);
    this->pipelines = std::move(other.pipelines);

    return *this;
}

void VulkanStaticMeshRenderer::Settings::ParseSettings(const ByteBufferReader& configFileBuffer, const Path& assetFilePath, Error& error)
{
    ConfigDocumentReader reader;
    if (reader.Start(configFileBuffer) != nullptr)
    {
        if (assetFilePath.EndsWith("buffer-formats.cfg"))
        {
            Std140ConstantBufferPackingRules<GpuRenderingConstantBufferStruct> packingRules(this->contextImpl->physicalDeviceDescription.properties.limits.minUniformBufferOffsetAlignment);

            GpuRenderingConstantBufferStruct::Create
                (
                this->uniformBufferFormats,
                this->contextImpl->GetAllocator(),
                reader,
                packingRules,
                GpuRenderingConstantBufferStruct::Defines::NONE,
                error
                );
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to create uniform buffer structs.");
                return;
            }
        }
        else if (assetFilePath.EndsWith("descriptor-set-layouts.cfg"))
        {
            VulkanDescriptorSetLayoutDescription::Create(this->descriptorSetLayouts, this->contextImpl->GetAllocator(), reader, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to create Vulkan descriptor set layout settings.");
                return;
            }
        }
        else if (assetFilePath.EndsWith("pipeline-layouts.cfg"))
        {
            VulkanPipelineLayoutDescription::Create(this->pipelineLayouts, this->contextImpl->GetAllocator(), reader, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to create Vulkan pipeline layout settings.");
                return;
            }
        }
        else if (assetFilePath.EndsWith("render-states.cfg"))
        {
            SimpleUri tempUri(this->contextImpl->GetAllocator());

            VulkanPipelineDescription::Create(this->pipelineShaderFileReference, this->pipelines, this->contextImpl->GetAllocator(), reader, tempUri, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to create Vulkan pipeline settings.");
                return;
            }
        }
        else
        {
            for (auto line = reader.Current(); line != nullptr; line = reader.Next())
            {
                switch (line->GetType())
                {
                    case ConfigDocumentLine::Type::KEY_AND_VALUE:
                    {
                        if (line->GetDepth() == 0)
                        {
                            Utf8StringView key, value;
                            line->GetKeyAndValue(key, value);

                            if (key == "max-renderables")
                                this->maxRenderables = Convert::ToInteger(value, this->maxRenderables);
                        }
                        break;
                    }
                    default: break;
                }
            }
        }
    }
}

//VulkanStaticMeshRenderer
VulkanStaticMeshRenderer::VulkanStaticMeshRenderer() :
    knownPipelineLayoutDescriptions(nullptr),
    knownUniformBufferDescriptions(nullptr)
{
    this->pipelineCache = VK_NULL_HANDLE;

    //Vulkan clip space has inverted Y and half Z
    this->clipInvertMatrix <<
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, -1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.5f, 0.0f,
        0.0f, 0.0f, 0.5f, 1.0f
        ;
}

void VulkanStaticMeshRenderer::Create(Settings&& settings, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    this->settings = std::move(settings);

    if (this->settings.uniformBufferFormats.empty())
    {
        FINJIN_SET_ERROR(error, "No uniform buffer formats were specified.");
        return;
    }

    //Look up various uniform buffer descriptions
    {
        EnumArray<VulkanStaticMeshRendererKnownUniformBuffer, VulkanStaticMeshRendererKnownUniformBuffer::COUNT, const char*> uniformBufferDescriptionTypeNames;
        uniformBufferDescriptionTypeNames[VulkanStaticMeshRendererKnownUniformBuffer::PASS] = "finjin.static-mesh-renderer.buffer.pass";
        uniformBufferDescriptionTypeNames[VulkanStaticMeshRendererKnownUniformBuffer::OBJECT] = "finjin.static-mesh-renderer.buffer.object";
        uniformBufferDescriptionTypeNames[VulkanStaticMeshRendererKnownUniformBuffer::MATERIAL] = "finjin.static-mesh-renderer.buffer.material";
        uniformBufferDescriptionTypeNames[VulkanStaticMeshRendererKnownUniformBuffer::LIGHT] = "finjin.static-mesh-renderer.buffer.light";
        for (size_t descriptionIndex = 0; descriptionIndex < (size_t)VulkanStaticMeshRendererKnownUniformBuffer::COUNT; descriptionIndex++)
        {
            this->knownUniformBufferDescriptions[descriptionIndex] = GpuRenderingConstantBufferStruct::GetByTypeName(this->settings.uniformBufferFormats, uniformBufferDescriptionTypeNames[descriptionIndex]);
            if (this->knownUniformBufferDescriptions[descriptionIndex] == nullptr)
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to find '%1%' uniform buffer description.", uniformBufferDescriptionTypeNames[descriptionIndex]));
                return;
            }
        }
    }

    //Look up various pipeline layout descriptions
    {
        EnumArray<VulkanStaticMeshRendererKnownPipelineLayoutDescription, VulkanStaticMeshRendererKnownPipelineLayoutDescription::COUNT, const char*> pipelineLayoutDescriptionTypeNames;
        pipelineLayoutDescriptionTypeNames[VulkanStaticMeshRendererKnownPipelineLayoutDescription::PRIMARY] = "finjin.static-mesh-renderer.pipeline-layout.primary";
        for (size_t descriptionIndex = 0; descriptionIndex < (size_t)VulkanStaticMeshRendererKnownPipelineLayoutDescription::COUNT; descriptionIndex++)
        {
            this->knownPipelineLayoutDescriptions[descriptionIndex] = VulkanPipelineLayoutDescription::GetByTypeName(this->settings.pipelineLayouts, pipelineLayoutDescriptionTypeNames[descriptionIndex]);
            if (this->knownPipelineLayoutDescriptions[descriptionIndex] == nullptr)
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to find '%1%' pipeline layout description.", pipelineLayoutDescriptionTypeNames[descriptionIndex]));
                return;
            }
        }
    }

    //Set up base graphics pipeline builder
    this->baseGraphicsPipelineBuilder.SetDefault();

    //Create shaders
    if (this->settings.pipelineShaderFileReference.IsValid())
    {
        auto readResult = this->settings.contextImpl->shaderAssetClassFileReader.ReadAsset(this->settings.contextImpl->readBuffer, this->settings.pipelineShaderFileReference);
        if (readResult == FileOperationResult::SUCCESS)
        {
            if (!this->shaderFileBytes.Create(this->settings.contextImpl->readBuffer.size(), this->settings.contextImpl->GetAllocator()))
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to allocate shader buffer for '%1%'.", this->settings.pipelineShaderFileReference.ToUriString()));
                return;
            }

            FINJIN_COPY_MEMORY(this->shaderFileBytes.data(), this->settings.contextImpl->readBuffer.data(), this->settings.contextImpl->readBuffer.size());
        }

        for (auto& pipelineDescription : this->settings.pipelines)
        {
            for (size_t shaderType = 0; shaderType < (size_t)VulkanShaderType::COUNT; shaderType++)
            {
                auto& assetOffset = pipelineDescription.shaderOffsets[shaderType];
                if (assetOffset.IsValid())
                {
                    this->settings.contextImpl->CreateShader
                        (
                        static_cast<VulkanShaderType>(shaderType),
                        pipelineDescription.typeName,
                        this->shaderFileBytes.data() + assetOffset.offset,
                        assetOffset.size,
                        false,
                        error
                        );
                    if (error)
                    {
                        FINJIN_SET_ERROR_NO_MESSAGE(error);
                        return;
                    }
                }
            }
        }
    }
    else
    {
        for (auto& pipelineDescription : this->settings.pipelines)
        {
            for (size_t shaderType = 0; shaderType < (size_t)VulkanShaderType::COUNT; shaderType++)
            {
                auto& assetRef = pipelineDescription.shaderRefs[shaderType];
                if (assetRef.IsValid())
                {
                    this->settings.contextImpl->CreateShader(static_cast<VulkanShaderType>(shaderType), assetRef, error);
                    if (error)
                    {
                        FINJIN_SET_ERROR_NO_MESSAGE(error);
                        return;
                    }
                }
            }
        }
    }

    //Create descriptor set layouts
    if (!this->descriptorSetLayouts.CreateEmpty(this->settings.descriptorSetLayouts.size(), this->settings.contextImpl->GetAllocator()))
    {
        FINJIN_SET_ERROR(error, "Failed to allocate descriptor set layout collection.");
        return;
    }
    for (size_t descriptorSetLayoutIndex = 0; descriptorSetLayoutIndex < this->settings.descriptorSetLayouts.size(); descriptorSetLayoutIndex++)
    {
        this->descriptorSetLayouts.push_back();

        auto& descriptorSetLayoutDescription = this->settings.descriptorSetLayouts[descriptorSetLayoutIndex];

        //Special case setting of descriptor count
        {
            if (descriptorSetLayoutDescription.typeName == "finjin.static-mesh-renderer.descriptor-set-layout.textures")
            {
                auto texturesIndex = descriptorSetLayoutDescription.FindBindingIndexByID("textures");
                if (texturesIndex != (size_t)-1)
                    descriptorSetLayoutDescription.vkbindings[texturesIndex].descriptorCount = this->settings.contextImpl->settings.max2DTextures;
            }
        }

        //Create
        descriptorSetLayoutDescription.CreateDescriptorSetLayout
            (
            this->descriptorSetLayouts.back(),
            this->settings.contextImpl->vk,
            this->settings.contextImpl->settings.deviceAllocationCallbacks,
            error
            );
        if (error)
        {
            this->descriptorSetLayouts.pop_back();

            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create descriptor set layout '%1%'.", this->settings.descriptorSetLayouts[descriptorSetLayoutIndex].typeName));
            return;
        }
    }

    //Create pipeline layouts
    if (!this->pipelineLayouts.CreateEmpty(this->settings.pipelineLayouts.size(), this->settings.contextImpl->GetAllocator()))
    {
        FINJIN_SET_ERROR(error, "Failed to allocate pipeline layout collection.");
        return;
    }
    for (size_t pipelineLayoutIndex = 0; pipelineLayoutIndex < this->settings.pipelineLayouts.size(); pipelineLayoutIndex++)
    {
        this->pipelineLayouts.push_back();

        auto& pipelineLayoutDescription = this->settings.pipelineLayouts[pipelineLayoutIndex];
        pipelineLayoutDescription.CreatePipelineLayout
            (
            this->pipelineLayouts.back(),
            this->settings.contextImpl->vk,
            this->settings.contextImpl->settings.deviceAllocationCallbacks,
            this->descriptorSetLayouts,
            error
            );
        if (error)
        {
            this->pipelineLayouts.pop_back();

            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create pipeline layout '%1%'.", this->settings.pipelineLayouts[pipelineLayoutIndex].typeName));
            return;
        }
    }

    //Look up various binding indexes
    {
        auto primaryPipelineLayoutDescriptionIndex = this->knownPipelineLayoutDescriptions[VulkanStaticMeshRendererKnownPipelineLayoutDescription::PRIMARY] - this->settings.pipelineLayouts.data();
        auto& primaryPipelineLayout = this->pipelineLayouts[primaryPipelineLayoutDescriptionIndex];

        EnumArray<VulkanStaticMeshRendererPrimaryPipelineLayoutBindings::Uniforms, VulkanStaticMeshRendererPrimaryPipelineLayoutBindings::Uniforms::COUNT, const char*> knownPrimaryDescriptorSetBindingIDs;
        knownPrimaryDescriptorSetBindingIDs[VulkanStaticMeshRendererPrimaryPipelineLayoutBindings::Uniforms::PASS] = "pass-buffer";
        knownPrimaryDescriptorSetBindingIDs[VulkanStaticMeshRendererPrimaryPipelineLayoutBindings::Uniforms::OBJECT] = "object-buffer";
        knownPrimaryDescriptorSetBindingIDs[VulkanStaticMeshRendererPrimaryPipelineLayoutBindings::Uniforms::MATERIAL] = "material-buffer";
        knownPrimaryDescriptorSetBindingIDs[VulkanStaticMeshRendererPrimaryPipelineLayoutBindings::Uniforms::LIGHT] = "light-buffer";
        for (size_t bindingIndex = 0; bindingIndex < (size_t)VulkanStaticMeshRendererPrimaryPipelineLayoutBindings::Uniforms::COUNT; bindingIndex++)
        {
            auto foundAt = primaryPipelineLayout.FindBindingByID(knownPrimaryDescriptorSetBindingIDs[bindingIndex]);
            if (!foundAt.IsValid())
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to find '%1%' binding.", knownPrimaryDescriptorSetBindingIDs[bindingIndex]));
                return;
            }

            this->knownPrimaryDescriptorSetBindings.uniforms[bindingIndex] = foundAt.vkbinding;
        }
    }

    //Create uniform buffer descriptor pool
    {
        StaticVector<VkDescriptorPoolSize, MAX_DESCRIPTOR_SET_LAYOUT_BINDINGS> descriptorPoolSizes;

        for (size_t i = 0; i < (size_t)VulkanStaticMeshRendererKnownPipelineLayoutDescription::COUNT; i++)
        {
            auto pipelineLayoutDescription = this->knownPipelineLayoutDescriptions[i];
            auto pipelineLayoutDescriptionIndex = pipelineLayoutDescription - this->settings.pipelineLayouts.data();
            auto& pipelineLayout = this->pipelineLayouts[pipelineLayoutDescriptionIndex];

            pipelineLayout.GetDescriptorSetPoolSizes(descriptorPoolSizes, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to get descriptor pool sizes for pipeline set layout '%1%'", pipelineLayoutDescription->typeName));
                return;
            }

            this->knownPipelineLayoutDescriptorPools[i].pipelineLayout = &pipelineLayout;

            VulkanDescriptorPoolCreateInfo descriptorPoolCreateInfo(descriptorPoolSizes.data(), descriptorPoolSizes.size());
            descriptorPoolCreateInfo.maxSets = static_cast<uint32_t>(this->settings.contextImpl->frameBuffers.size());
            auto result = this->settings.contextImpl->vk.CreateDescriptorPool
                (
                this->settings.contextImpl->vk.device,
                &descriptorPoolCreateInfo,
                this->settings.contextImpl->settings.deviceAllocationCallbacks,
                &this->knownPipelineLayoutDescriptorPools[i].vkpool
                );
            if (result != VK_SUCCESS)
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create descriptor pool for pipeline set layout '%1%'. Vulkan error: %2%", pipelineLayoutDescription->typeName, VulkanResult::ToString(result)));
                return;
            }

            descriptorPoolSizes.clear();
        }
    }

    //Create pipeline cache
    VulkanPipelineCacheCreateInfo pipelineCacheCreateInfo; //No special setup needed
    auto result = this->settings.contextImpl->vk.CreatePipelineCache
        (
        this->settings.contextImpl->vk.device,
        &pipelineCacheCreateInfo,
        this->settings.contextImpl->settings.deviceAllocationCallbacks,
        &this->pipelineCache
        );
    if (result != VK_SUCCESS)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create pipeline cache. Vulkan error: %1%", VulkanResult::ToString(result)));
        return;
    }

    //Create pipelines
    {
        //Create pipeline collection
        if (!this->pipelines.CreateEmpty(this->settings.pipelines.size(), this->settings.contextImpl->GetAllocator()))
        {
            FINJIN_SET_ERROR(error, "Failed to allocate pipeline objects collection.");
            return;
        }

        //Create lookup collection
        if (!this->graphicsPipelineLayoutAndPipelines.Create(this->settings.pipelines.size(), this->settings.pipelines.size(), this->settings.contextImpl->GetAllocator()))
        {
            FINJIN_SET_ERROR(error, "Failed to create descriptor set/pipeline lookup.");
            return;
        }

        //For each pipeline description in settings
        for (size_t pipelineIndex = 0; pipelineIndex < this->settings.pipelines.size(); pipelineIndex++)
        {
            auto& pipelineDescription = this->settings.pipelines[pipelineIndex];

            //Validate--------------------------------
            if (pipelineDescription.inputFormatType.empty())
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Pipeline '%1%' does not have an input type.", pipelineDescription.typeName));
                return;
            }

            if (pipelineDescription.pipelineLayoutType.empty())
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Pipeline '%1%' does not have a pipeline layout type.", pipelineDescription.typeName));
                return;
            }

            //Set up pipeline creator---------------------------------------------------------------
            VulkanGraphicsPipelineCreateInfoBuilder graphicsPipelineCreateInfoCreator = this->baseGraphicsPipelineBuilder; //Copy

            //Vertex input format------------------------------
            {
                //Look up input/vertex format
                auto vulkanInputFormat = this->settings.contextImpl->assetResources.GetInputFormatByTypeName(pipelineDescription.inputFormatType);
                if (vulkanInputFormat == nullptr)
                {
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to find input format '%1%' for pipeline '%2%'.", pipelineDescription.inputFormatType, pipelineDescription.typeName));
                    return;
                }
                vulkanInputFormat->SetBinding(VERTEX_BUFFER_BIND_ID);
                pipelineDescription.graphicsState.shaderFeatures.inputFormatHash = vulkanInputFormat->inputFormatHash;

                auto& vertexInputStateCreateInfo = graphicsPipelineCreateInfoCreator.UseVertexInputState();
                vertexInputStateCreateInfo.vertexBindingDescriptionCount = 1;
                vertexInputStateCreateInfo.pVertexBindingDescriptions = &vulkanInputFormat->vertexBinding;
                vertexInputStateCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(vulkanInputFormat->attributes.size());
                vertexInputStateCreateInfo.pVertexAttributeDescriptions = vulkanInputFormat->attributes.data();
            }

            //Ensure the pipeline hasn't been created yet
            size_t renderStateHash = 0;
            {
                renderStateHash = pipelineDescription.graphicsState.GetHash();
                if (this->graphicsPipelineLayoutAndPipelines.contains(renderStateHash))
                {
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Render state for pipeline object '%1%' already exists in the lookup.", pipelineDescription.typeName));
                    return;
                }
            }

            //Pipeline layout-----------------------------
            VulkanPipelineLayout* pipelineLayout = nullptr;
            {
                auto pipelineLayoutDescription = VulkanPipelineLayoutDescription::GetByTypeName(this->settings.pipelineLayouts, pipelineDescription.pipelineLayoutType);
                if (pipelineLayoutDescription == nullptr)
                {
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to find pipeline layout '%1%'.", pipelineDescription.pipelineLayoutType));
                    return;
                }
                auto pipelineLayoutDescriptionIndex = pipelineLayoutDescription - this->settings.pipelineLayouts.data();
                pipelineLayout = &this->pipelineLayouts[pipelineLayoutDescriptionIndex];
                graphicsPipelineCreateInfoCreator.graphicsPipelineCreateInfo.layout = pipelineLayout->vkpipelineLayout;
            }

            //Pass/subpass-----------------------------------
            graphicsPipelineCreateInfoCreator.graphicsPipelineCreateInfo.renderPass = this->settings.contextImpl->renderPass;
            graphicsPipelineCreateInfoCreator.graphicsPipelineCreateInfo.subpass = VK_NULL_HANDLE;

            //Shaders-----------------------------------
            for (size_t shaderType = 0; shaderType < (size_t)VulkanShaderType::COUNT; shaderType++)
            {
                VulkanShader* shader = nullptr;

                auto& shaderOffset = pipelineDescription.shaderOffsets[shaderType];
                auto& shaderRef = pipelineDescription.shaderRefs[shaderType];

                if (shaderOffset.IsValid())
                {
                    shader = this->settings.contextImpl->assetResources.GetShaderByName(static_cast<VulkanShaderType>(shaderType), pipelineDescription.typeName);
                    if (shader == nullptr)
                    {
                        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to find shader '%1%'.", pipelineDescription.typeName));
                        return;
                    }
                }
                else if (shaderRef.IsValid())
                {
                    shader = this->settings.contextImpl->assetResources.GetShaderByName(static_cast<VulkanShaderType>(shaderType), shaderRef.objectName);
                    if (shader == nullptr)
                    {
                        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to find shader '%1%'.", shaderRef.ToUriString()));
                        return;
                    }
                }

                if (shader != nullptr)
                {
                    auto& shaderStageToCreate = graphicsPipelineCreateInfoCreator.AddShaderStage();
                    shaderStageToCreate.pName = "main";
                    shaderStageToCreate.module = shader->shaderModule;

                    switch (static_cast<VulkanShaderType>(shaderType))
                    {
                        case VulkanShaderType::VERTEX: shaderStageToCreate.stage = VK_SHADER_STAGE_VERTEX_BIT; break;
                        case VulkanShaderType::TESSELLATION_CONTROL: shaderStageToCreate.stage = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT; break;
                        case VulkanShaderType::TESSELLATION_EVALUATION: shaderStageToCreate.stage = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT; break;
                        case VulkanShaderType::GEOMETRY: shaderStageToCreate.stage = VK_SHADER_STAGE_GEOMETRY_BIT; break;
                        case VulkanShaderType::FRAGMENT: shaderStageToCreate.stage = VK_SHADER_STAGE_FRAGMENT_BIT; break;
                        case VulkanShaderType::COMPUTE: shaderStageToCreate.stage = VK_SHADER_STAGE_COMPUTE_BIT; break;
                        default:
                        {
                            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Shader type '%1%' improperly specified for a graphics pipeline.", VulkanShaderTypeUtilities::ToString(static_cast<VulkanShaderType>(shaderType))));
                            return;
                        }
                    }
                }
            }

            //Rasterization state-------------------------
            if (AnySet(pipelineDescription.graphicsState.shaderFeatures.flags & ShaderFeatureFlag::RENDERING_FILL_WIREFRAME))
            {
                auto& rasterizationState = graphicsPipelineCreateInfoCreator.UseRasterizationState();
                rasterizationState.polygonMode = VK_POLYGON_MODE_LINE;
            }

            //Primitive topology-------------------------
            if (pipelineDescription.graphicsState.primitiveTopology.IsSet())
            {
                auto& inputAssemblyState = graphicsPipelineCreateInfoCreator.UseInputAssemblyState();
                inputAssemblyState.topology = pipelineDescription.graphicsState.primitiveTopology;
            }

            //Dynamic state-------------------------
            if (!pipelineDescription.graphicsState.dynamicState.empty())
            {
                auto& dynamicState = graphicsPipelineCreateInfoCreator.UseDynamicState();
                dynamicState.dynamicStateCount = static_cast<uint32_t>(pipelineDescription.graphicsState.dynamicState.size());
                dynamicState.pDynamicStates = pipelineDescription.graphicsState.dynamicState.data();
            }

            //Create pipeline--------------------------------
            this->pipelines.push_back();
            result = this->settings.contextImpl->vk.CreateGraphicsPipelines
                (
                this->settings.contextImpl->vk.device,
                this->pipelineCache,
                1,
                &graphicsPipelineCreateInfoCreator.graphicsPipelineCreateInfo,
                this->settings.contextImpl->settings.deviceAllocationCallbacks,
                &this->pipelines.back()
                );
            if (result != VK_SUCCESS)
            {
                this->pipelines.pop_back();

                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create pipeline '%1%'. Vulkan result: %2%", pipelineDescription.typeName, VulkanResult::ToString(result)));
                return;
            }

            //Add to lookup-------------------------------------
            auto graphicsPipelineLayoutAndPipelineResult = this->graphicsPipelineLayoutAndPipelines.GetOrAdd(renderStateHash);
            if (graphicsPipelineLayoutAndPipelineResult.HasError() || graphicsPipelineLayoutAndPipelineResult.value == nullptr)
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to add pipeline object '%1%' to lookup.", pipelineDescription.typeName));
                return;
            }
            graphicsPipelineLayoutAndPipelineResult.value->pipelineLayout = pipelineLayout;
            graphicsPipelineLayoutAndPipelineResult.value->pipeline = this->pipelines.back();
            graphicsPipelineLayoutAndPipelineResult.value->pipelineLayoutBindingIndexes = this->knownPrimaryDescriptorSetBindings;
        }
    }

    //Build frame resources
    for (auto& frameBuffer : this->settings.contextImpl->frameBuffers)
    {
        auto& staticMeshRendererFrameState = frameBuffer.staticMeshRendererFrameState;

        //Uniform buffers
        {
            EnumArray<VulkanStaticMeshRendererKnownUniformBuffer, VulkanStaticMeshRendererKnownUniformBuffer::COUNT, size_t> instanceCounts;
            instanceCounts[VulkanStaticMeshRendererKnownUniformBuffer::PASS] = 1;
            instanceCounts[VulkanStaticMeshRendererKnownUniformBuffer::OBJECT] = this->settings.maxRenderables;
            instanceCounts[VulkanStaticMeshRendererKnownUniformBuffer::MATERIAL] = this->settings.contextImpl->settings.maxMaterials;
            instanceCounts[VulkanStaticMeshRendererKnownUniformBuffer::LIGHT] = this->settings.contextImpl->settings.maxLights;

            for (size_t i = 0; i < (size_t)VulkanStaticMeshRendererKnownUniformBuffer::COUNT; i++)
            {
                staticMeshRendererFrameState.uniformBuffers[i].Create(this->settings.contextImpl->vk, this->settings.contextImpl->settings.deviceAllocationCallbacks, *this->knownUniformBufferDescriptions[i], instanceCounts[i], this->settings.contextImpl->physicalDeviceDescription, error);
                if (error)
                {
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create known uniform buffer '%1%' for frame '%2%'.", i, frameBuffer.index));
                    return;
                }
            }
        }

        //Descriptor set
        auto& pipelineLayoutDescriptorPool = this->knownPipelineLayoutDescriptorPools[VulkanStaticMeshRendererKnownPipelineLayoutDescription::PRIMARY];
        auto pipelineLayoutDescriptorSetLayouts = pipelineLayoutDescriptorPool.pipelineLayout->GetVkDescriptorSetLayouts();

        VulkanDescriptorSetAllocateInfo descriptorSetAllocateInfo(pipelineLayoutDescriptorPool.vkpool);
        descriptorSetAllocateInfo.descriptorSetCount = 1;
        descriptorSetAllocateInfo.pSetLayouts = pipelineLayoutDescriptorSetLayouts.data();
        result = this->settings.contextImpl->vk.AllocateDescriptorSets(this->settings.contextImpl->vk.device, &descriptorSetAllocateInfo, &staticMeshRendererFrameState.descriptorSet);
        if (result != VK_SUCCESS)
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to allocate primary descriptor set for frame '%1%'. Vulkan result: %2%", frameBuffer.index, VulkanResult::ToString(result)));
            return;
        }

        //Update values
        {
            VulkanDescriptorBufferInfo writeBufferInfo(staticMeshRendererFrameState.uniformBuffers[VulkanStaticMeshRendererKnownUniformBuffer::PASS].GetResource().buffer);

            VulkanWriteDescriptorSet writeDescriptorSet;
            writeDescriptorSet.dstSet = staticMeshRendererFrameState.descriptorSet;
            writeDescriptorSet.dstBinding = this->knownPrimaryDescriptorSetBindings.uniforms[VulkanStaticMeshRendererPrimaryPipelineLayoutBindings::Uniforms::PASS]->binding;
            writeDescriptorSet.descriptorCount = 1;
            writeDescriptorSet.descriptorType = this->knownPrimaryDescriptorSetBindings.uniforms[VulkanStaticMeshRendererPrimaryPipelineLayoutBindings::Uniforms::PASS]->descriptorType;
            writeDescriptorSet.pBufferInfo = &writeBufferInfo;
            this->settings.contextImpl->vk.UpdateDescriptorSets(this->settings.contextImpl->vk.device, 1, &writeDescriptorSet, 0, nullptr);
        }
        {
            VulkanDescriptorBufferInfo writeBufferInfo(staticMeshRendererFrameState.uniformBuffers[VulkanStaticMeshRendererKnownUniformBuffer::OBJECT].GetResource().buffer);

            VulkanWriteDescriptorSet writeDescriptorSet;
            writeDescriptorSet.dstSet = staticMeshRendererFrameState.descriptorSet;
            writeDescriptorSet.dstBinding = this->knownPrimaryDescriptorSetBindings.uniforms[VulkanStaticMeshRendererPrimaryPipelineLayoutBindings::Uniforms::OBJECT]->binding;
            writeDescriptorSet.descriptorCount = 1;
            writeDescriptorSet.descriptorType = this->knownPrimaryDescriptorSetBindings.uniforms[VulkanStaticMeshRendererPrimaryPipelineLayoutBindings::Uniforms::OBJECT]->descriptorType;
            writeDescriptorSet.pBufferInfo = &writeBufferInfo;
            this->settings.contextImpl->vk.UpdateDescriptorSets(this->settings.contextImpl->vk.device, 1, &writeDescriptorSet, 0, nullptr);
        }
        {
            VulkanDescriptorBufferInfo writeBufferInfo(staticMeshRendererFrameState.uniformBuffers[VulkanStaticMeshRendererKnownUniformBuffer::MATERIAL].GetResource().buffer);

            VulkanWriteDescriptorSet writeDescriptorSet;
            writeDescriptorSet.dstSet = staticMeshRendererFrameState.descriptorSet;
            writeDescriptorSet.dstBinding = this->knownPrimaryDescriptorSetBindings.uniforms[VulkanStaticMeshRendererPrimaryPipelineLayoutBindings::Uniforms::MATERIAL]->binding;
            writeDescriptorSet.descriptorCount = 1;
            writeDescriptorSet.descriptorType = this->knownPrimaryDescriptorSetBindings.uniforms[VulkanStaticMeshRendererPrimaryPipelineLayoutBindings::Uniforms::MATERIAL]->descriptorType;
            writeDescriptorSet.pBufferInfo = &writeBufferInfo;
            this->settings.contextImpl->vk.UpdateDescriptorSets(this->settings.contextImpl->vk.device, 1, &writeDescriptorSet, 0, nullptr);
        }
        {
            VulkanDescriptorBufferInfo writeBufferInfo(staticMeshRendererFrameState.uniformBuffers[VulkanStaticMeshRendererKnownUniformBuffer::LIGHT].GetResource().buffer);

            VulkanWriteDescriptorSet writeDescriptorSet;
            writeDescriptorSet.dstSet = staticMeshRendererFrameState.descriptorSet;
            writeDescriptorSet.dstBinding = this->knownPrimaryDescriptorSetBindings.uniforms[VulkanStaticMeshRendererPrimaryPipelineLayoutBindings::Uniforms::LIGHT]->binding;
            writeDescriptorSet.descriptorCount = 1;
            writeDescriptorSet.descriptorType = this->knownPrimaryDescriptorSetBindings.uniforms[VulkanStaticMeshRendererPrimaryPipelineLayoutBindings::Uniforms::LIGHT]->descriptorType;
            writeDescriptorSet.pBufferInfo = &writeBufferInfo;
            this->settings.contextImpl->vk.UpdateDescriptorSets(this->settings.contextImpl->vk.device, 1, &writeDescriptorSet, 0, nullptr);
        }

        if (!staticMeshRendererFrameState.opaqueMaterials.Create(this->settings.contextImpl->settings.maxMaterials, this->settings.contextImpl->settings.maxMaterials, this->settings.contextImpl->GetAllocator()))
        {
            FINJIN_SET_ERROR(error, "Failed to create 'opaque' materials collection.");
            return;
        }

        if (!staticMeshRendererFrameState.lights.Create(this->settings.contextImpl->settings.maxLights, this->settings.contextImpl->settings.maxLights, this->settings.contextImpl->GetAllocator()))
        {
            FINJIN_SET_ERROR(error, "Failed to create lights collection.");
            return;
        }

        for (auto& renderTargetState : staticMeshRendererFrameState.renderTargetRenderStates)
        {
            if (!renderTargetState.opaqueEntities.CreateEmpty(this->settings.maxRenderables, this->settings.contextImpl->GetAllocator()))
            {
                FINJIN_SET_ERROR(error, "Failed to create 'opaque' renderables collection.");
                return;
            }

            if (!renderTargetState.pipelineLayoutSlots.Create(this->settings.pipelineLayouts.size(), this->settings.pipelineLayouts.size(), this->settings.contextImpl->GetAllocator()))
            {
                FINJIN_SET_ERROR(error, "Failed to create 'opaque' pipeline layout lookup.");
                return;
            }
            for (auto& pipelineLayoutSlot : renderTargetState.pipelineLayoutSlots.GetValueEntries())
            {
                if (!pipelineLayoutSlot.value.second.pipelineSlots.Create(this->settings.pipelines.size(), this->settings.pipelines.size(), this->settings.contextImpl->GetAllocator()))
                {
                    FINJIN_SET_ERROR(error, "Failed to create 'opaque' pipeline lookup.");
                    return;
                }
            }

            if (!renderTargetState.pipelineSlots.Create(this->settings.pipelines.size(), this->settings.pipelines.size(), this->settings.contextImpl->GetAllocator()))
            {
                FINJIN_SET_ERROR(error, "Failed to create 'opaque' pipeline lookup.");
                return;
            }

            for (auto& pipelineSlot : renderTargetState.pipelineSlots.GetValueEntries())
            {
                if (!pipelineSlot.value.second.heapRefs.Create(MAX_HEAP_DESCRIPTOR_COMBINATIONS, this->settings.contextImpl->GetAllocator()))
                {
                    FINJIN_SET_ERROR(error, "Failed to create heap refs for render pipeline.");
                    return;
                }
                for (auto& heapRef : pipelineSlot.value.second.heapRefs)
                {
                    if (!heapRef.opaqueEntities.CreateEmpty(this->settings.maxRenderables, this->settings.contextImpl->GetAllocator()))
                    {
                        FINJIN_SET_ERROR(error, "Failed to create 'opaque' renderables collection for pipeline.");
                        return;
                    }
                }
                pipelineSlot.value.second.heapRefs.clear();
            }
        }
    }
}

void VulkanStaticMeshRenderer::Destroy()
{
    if (this->settings.contextImpl == nullptr)
        return;

    for (auto& pool : this->knownPipelineLayoutDescriptorPools)
        this->settings.contextImpl->vk.DestroyDescriptorPool(this->settings.contextImpl->vk.device, pool.vkpool, this->settings.contextImpl->settings.deviceAllocationCallbacks);

    for (auto& pipeline : this->pipelines)
        this->settings.contextImpl->vk.DestroyPipeline(this->settings.contextImpl->vk.device, pipeline, this->settings.contextImpl->settings.deviceAllocationCallbacks);

    this->settings.contextImpl->vk.DestroyPipelineCache(this->settings.contextImpl->vk.device, this->pipelineCache, this->settings.contextImpl->settings.deviceAllocationCallbacks);

    for (auto& pipelineLayout : this->pipelineLayouts)
        this->settings.contextImpl->vk.DestroyPipelineLayout(this->settings.contextImpl->vk.device, pipelineLayout.vkpipelineLayout, this->settings.contextImpl->settings.deviceAllocationCallbacks);

    for (auto& descriptorSetLayout : this->descriptorSetLayouts)
        this->settings.contextImpl->vk.DestroyDescriptorSetLayout(this->settings.contextImpl->vk.device, descriptorSetLayout.vkdescriptorSetLayout, this->settings.contextImpl->settings.deviceAllocationCallbacks);

    for (auto& frameBuffer : this->settings.contextImpl->frameBuffers)
    {
        auto& staticMeshRendererFrameState = frameBuffer.staticMeshRendererFrameState;

        for (size_t i = 0; i < (size_t)VulkanStaticMeshRendererKnownUniformBuffer::COUNT; i++)
        {
            staticMeshRendererFrameState.uniformBuffers[i].Destroy(this->settings.contextImpl->vk, this->settings.contextImpl->settings.deviceAllocationCallbacks);
        }

        /*for (auto& renderTarget : staticMeshRendererFrameState.renderTargetRenderStates)
        {
        }*/
    }
}

void VulkanStaticMeshRenderer::UpdatePassAndMaterialConstants(VulkanStaticMeshRendererFrameState& frameState, SimpleTimeDelta elapsedTime, SimpleTimeCounter totalElapsedTime)
{
    //Pass
    {
        auto windowBounds = this->settings.contextImpl->renderingPixelBounds;

        auto clientWidth = RoundToFloat(windowBounds.GetClientWidth());
        auto clientHeight = RoundToFloat(windowBounds.GetClientHeight());

        this->settings.contextImpl->camera.SetLens(this->settings.contextImpl->camera.GetFovY(), clientWidth / clientHeight, this->settings.contextImpl->camera.GetNearZ(), this->settings.contextImpl->camera.GetFarZ());
        this->settings.contextImpl->camera.Update();

        auto viewMatrix = this->settings.contextImpl->camera.GetViewMatrix();
        MathMatrix4 projectionMatrix = this->clipInvertMatrix * this->settings.contextImpl->camera.GetProjectionMatrix();

        MathMatrix4 viewProjectionMatrix = projectionMatrix * viewMatrix;
        MathMatrix4 viewInverseMatrix = viewMatrix.inverse();
        MathMatrix4 viewInverseTransposeMatrix = viewInverseMatrix.transpose();
        MathMatrix4 projectionInverseMatrix = projectionMatrix.inverse();
        MathMatrix4 viewProjectionInverseMatrix = viewInverseMatrix * projectionInverseMatrix; //viewProjectionMatrix.inverse();

        MathMatrix4Values vulkanMatrix;

        auto& framePassUniformBuffer = frameState.uniformBuffers[VulkanStaticMeshRendererKnownUniformBuffer::PASS];
        framePassUniformBuffer.StartWrites(this->settings.contextImpl->vk);
        {
            size_t offset;

            //offset = framePassUniformBuffer.Set(vulkanWorldEnvironmentMatrix, 0, GpuStructuredAndConstantBufferStructMetadata::ElementID::ENVIRONMENT_MATRIX);

            GetColumnOrderMatrixData(vulkanMatrix, viewMatrix);
            offset = framePassUniformBuffer.Set(vulkanMatrix, 0, GpuStructuredAndConstantBufferStructMetadata::ElementID::VIEW_MATRIX);

            GetColumnOrderMatrixData(vulkanMatrix, viewInverseMatrix);
            offset = framePassUniformBuffer.Set(vulkanMatrix, 0, GpuStructuredAndConstantBufferStructMetadata::ElementID::INVERSE_VIEW_MATRIX);

            GetColumnOrderMatrixData(vulkanMatrix, viewInverseTransposeMatrix);
            offset = framePassUniformBuffer.Set(vulkanMatrix, 0, GpuStructuredAndConstantBufferStructMetadata::ElementID::INVERSE_TRANSPOSE_VIEW_MATRIX);

            GetColumnOrderMatrixData(vulkanMatrix, projectionMatrix);
            offset = framePassUniformBuffer.Set(vulkanMatrix, 0, GpuStructuredAndConstantBufferStructMetadata::ElementID::PROJECTION_MATRIX);

            GetColumnOrderMatrixData(vulkanMatrix, projectionInverseMatrix);
            offset = framePassUniformBuffer.Set(vulkanMatrix, 0, GpuStructuredAndConstantBufferStructMetadata::ElementID::INVERSE_PROJECTION_MATRIX);

            GetColumnOrderMatrixData(vulkanMatrix, viewProjectionMatrix);
            offset = framePassUniformBuffer.Set(vulkanMatrix, 0, GpuStructuredAndConstantBufferStructMetadata::ElementID::VIEW_PROJECTION_MATRIX);

            GetColumnOrderMatrixData(vulkanMatrix, viewProjectionInverseMatrix);
            offset = framePassUniformBuffer.Set(vulkanMatrix, 0, GpuStructuredAndConstantBufferStructMetadata::ElementID::INVERSE_VIEW_PROJECTION_MATRIX);

            auto vulkanEyePosition = this->settings.contextImpl->camera.GetPosition();
            offset = framePassUniformBuffer.Set(vulkanEyePosition, 0, GpuStructuredAndConstantBufferStructMetadata::ElementID::EYE_POSITION);

            auto vulkanRenderTargetSize = MathVector2(clientWidth, clientHeight);
            offset = framePassUniformBuffer.Set(vulkanRenderTargetSize, 0, GpuStructuredAndConstantBufferStructMetadata::ElementID::RENDER_TARGET_SIZE);

            auto vulkanRenderTargetInverseSize = MathVector2(1.0f / clientWidth, 1.0f / clientHeight);
            offset = framePassUniformBuffer.Set(vulkanRenderTargetInverseSize, 0, GpuStructuredAndConstantBufferStructMetadata::ElementID::INVERSE_RENDER_TARGET_SIZE);

            float vulkanNearZ = this->settings.contextImpl->camera.GetNearZ();
            offset = framePassUniformBuffer.Set(vulkanNearZ, 0, GpuStructuredAndConstantBufferStructMetadata::ElementID::Z_NEAR);

            float vulkanFarZ = this->settings.contextImpl->camera.GetFarZ();
            offset = framePassUniformBuffer.Set(vulkanFarZ, 0, GpuStructuredAndConstantBufferStructMetadata::ElementID::Z_FAR);

            auto vulkanTotalTime = SimpleTimeCounterToFloat(totalElapsedTime);
            offset = framePassUniformBuffer.Set(vulkanTotalTime, 0, GpuStructuredAndConstantBufferStructMetadata::ElementID::TOTAL_ELAPSED_TIME_SECONDS);

            auto vulkanDeltaTime = SimpleTimeDeltaToFloat(elapsedTime);
            offset = framePassUniformBuffer.Set(vulkanDeltaTime, 0, GpuStructuredAndConstantBufferStructMetadata::ElementID::FRAME_ELAPSED_TIME_SECONDS);
        }
        framePassUniformBuffer.FinishWrites(this->settings.contextImpl->vk);
    }

    //Materials
    {
        //auto textureOffsetsByDimension = this->settings.contextImpl->assetResources.textureOffsetsByDimension; //Make a copy

        MathMatrix4 textureTransform;
        MathMatrix4Values vulkanMatrix;

        auto& frameMaterialUniformBuffer = frameState.uniformBuffers[VulkanStaticMeshRendererKnownUniformBuffer::MATERIAL];
        frameMaterialUniformBuffer.StartWrites(this->settings.contextImpl->vk);
        for (auto material : frameState.opaqueMaterials)
        {
            auto vulkanMaterial = static_cast<VulkanMaterial*>(material->gpuMaterial);

            auto offset = frameMaterialUniformBuffer.Set(material->diffuseColor, vulkanMaterial->gpuBufferIndex, GpuStructuredAndConstantBufferStructMetadata::ElementID::MATERIAL_DIFFUSE_COLOR);

            offset = frameMaterialUniformBuffer.Set(material->ambientColor, vulkanMaterial->gpuBufferIndex, GpuStructuredAndConstantBufferStructMetadata::ElementID::MATERIAL_AMBIENT_COLOR);

            offset = frameMaterialUniformBuffer.Set(material->specularColor, vulkanMaterial->gpuBufferIndex, GpuStructuredAndConstantBufferStructMetadata::ElementID::MATERIAL_SPECULAR_COLOR);

            offset = frameMaterialUniformBuffer.Set(material->selfIlluminationColor, vulkanMaterial->gpuBufferIndex, GpuStructuredAndConstantBufferStructMetadata::ElementID::MATERIAL_SELF_ILLUMINATION_COLOR);

            offset = frameMaterialUniformBuffer.Set(material->shininess, vulkanMaterial->gpuBufferIndex, GpuStructuredAndConstantBufferStructMetadata::ElementID::MATERIAL_SHININESS);

            offset = frameMaterialUniformBuffer.Set(material->opacity, vulkanMaterial->gpuBufferIndex, GpuStructuredAndConstantBufferStructMetadata::ElementID::MATERIAL_OPACITY);

            if (vulkanMaterial->maps[VulkanMaterial::MapIndex::DIFFUSE].finjinMap != nullptr)
            {
                auto map = static_cast<FinjinMaterial::Map*>(vulkanMaterial->maps[VulkanMaterial::MapIndex::DIFFUSE].finjinMap);
                textureTransform = (MathTranslation(map->textureOffset) * MathAngleAxis(map->textureRotation.ToRadiansValue(), MathVector3(1, 0, 0)) * MathScaling(map->textureScale)).matrix();
            }
            else
                textureTransform.setIdentity();
            GetColumnOrderMatrixData(vulkanMatrix, textureTransform);
            offset = frameMaterialUniformBuffer.Set(vulkanMatrix, vulkanMaterial->gpuBufferIndex, GpuStructuredAndConstantBufferStructMetadata::ElementID::MATERIAL_TEXTURE_COORDINATE_MATRIX);

            for (auto& vulkanMap : vulkanMaterial->maps)
            {
                if (vulkanMap.texture != nullptr)
                {
                    auto vulkanTextureIndex = static_cast<uint32_t>(vulkanMap.texture->textureIndex);// - textureOffsetsByDimension[vulkanMap.texture->type].baseOffset); //This commented code is for setting the index back to its zero base with respect to the appropriate sampler table
                    offset = frameMaterialUniformBuffer.Set(vulkanTextureIndex, vulkanMaterial->gpuBufferIndex, (GpuStructuredAndConstantBufferStructMetadata::ElementID)vulkanMap.finjinMap->gpuBufferTextureIndexElementID);

                    offset = frameMaterialUniformBuffer.Set(vulkanMap.finjinMap->amount, vulkanMaterial->gpuBufferIndex, (GpuStructuredAndConstantBufferStructMetadata::ElementID)vulkanMap.finjinMap->gpuBufferAmountElementID);
                }
            }
        }
        frameMaterialUniformBuffer.FinishWrites(this->settings.contextImpl->vk);
        frameState.opaqueMaterials.clear();
    }

    //Lights
    {
        SceneNodeState sceneNodeState;

        MathVector4 position4;

        MathVector4 direction4Temp;
        MathVector3 direction3;

        auto& frameLightUniformBuffer = frameState.uniformBuffers[VulkanStaticMeshRendererKnownUniformBuffer::LIGHT];
        frameLightUniformBuffer.StartWrites(this->settings.contextImpl->vk);
        for (auto& lightItem : frameState.lights)
        {
            auto light = lightItem.first;
            auto lightState = lightItem.second;

            auto vulkanLight = static_cast<VulkanLight*>(light->gpuLight);

            auto offset = frameLightUniformBuffer.Set(lightState->color, vulkanLight->gpuBufferIndex, GpuStructuredAndConstantBufferStructMetadata::ElementID::LIGHT_COLOR);

            lightState->GetWorldPosition(position4);
            offset = frameLightUniformBuffer.Set(position4, vulkanLight->gpuBufferIndex, GpuStructuredAndConstantBufferStructMetadata::ElementID::LIGHT_POSITION);

            lightState->GetWorldDirection(direction3, direction4Temp);
            offset = frameLightUniformBuffer.Set(direction3, vulkanLight->gpuBufferIndex, GpuStructuredAndConstantBufferStructMetadata::ElementID::LIGHT_DIRECTION);

            offset = frameLightUniformBuffer.Set(lightState->range, vulkanLight->gpuBufferIndex, GpuStructuredAndConstantBufferStructMetadata::ElementID::LIGHT_RANGE);

            offset = frameLightUniformBuffer.Set(lightState->power, vulkanLight->gpuBufferIndex, GpuStructuredAndConstantBufferStructMetadata::ElementID::LIGHT_POWER);

            float coneRange[2] = { lightState->coneRange[0].ToRadiansValue(), lightState->coneRange[1].ToRadiansValue() };
            offset = frameLightUniformBuffer.Set(coneRange, vulkanLight->gpuBufferIndex, GpuStructuredAndConstantBufferStructMetadata::ElementID::LIGHT_CONE_RANGE);

            float sinConeRange[2] = { sinf(coneRange[0] * 0.5f), sinf(coneRange[1] * 0.5f) };
            offset = frameLightUniformBuffer.Set(sinConeRange, vulkanLight->gpuBufferIndex, GpuStructuredAndConstantBufferStructMetadata::ElementID::LIGHT_SIN_CONE_RANGE);
        }
        frameLightUniformBuffer.FinishWrites(this->settings.contextImpl->vk);
        frameState.lights.clear();
    }
}

void VulkanStaticMeshRenderer::RenderEntity(VulkanStaticMeshRendererFrameState& frameState, SceneNodeState& sceneNodeState, FinjinSceneObjectEntity* entity, const RenderShaderFeatureFlags& entityShaderFeatureFlags, const GpuCommandLights& sortedLights, const MathVector4& ambientLight)
{
    auto& renderTargetState = frameState.renderTargetRenderStates[0];

    assert(!renderTargetState.opaqueEntities.full() && "The entities list is full. Try reconfiguring the system with a higher maximum.");

    auto mesh = entity->meshHandle.asset;
    if (mesh == nullptr)
        return;

    if (!this->settings.contextImpl->ResolveMeshRef(*mesh, entity->meshHandle.assetRef))
        return;

    auto vulkanMesh = static_cast<VulkanMesh*>(mesh->gpuMesh);
    if (vulkanMesh->submeshes.empty())
        return;

    MathMatrix4 entityWorldMatrix = sceneNodeState.worldMatrix * entity->transform;
    MathMatrix4 entityInverseWorldMatrix = entityWorldMatrix.inverse();
    MathMatrix4 entityInverseTransposeWorldMatrix = entityInverseWorldMatrix.transpose();

    StaticVector<LightType, EngineConstants::MAX_LIGHTS_PER_OBJECT> sortedLightTypes;
    for (auto& light : sortedLights)
    {
        sortedLightTypes.push_back(light.light->lightType);
        frameState.lights.insert(light.light, light.state, false);
    }

    for (size_t subentityIndex = 0; subentityIndex < entity->subentities.size() && !renderTargetState.opaqueEntities.full(); subentityIndex++)
    {
        auto& vulkanSubmesh = vulkanMesh->submeshes[subentityIndex];
        auto& submesh = mesh->submeshes[subentityIndex];
        auto& subentity = entity->subentities[subentityIndex];
        auto material = subentity.materialHandle.asset;
        if (material != nullptr &&
            this->settings.contextImpl->ResolveMaterialRef(*material, subentity.materialHandle.assetRef) &&
            this->settings.contextImpl->ResolveMaterialMaps(*material))
        {
            auto vulkanMaterial = static_cast<VulkanMaterial*>(material->gpuMaterial);
            auto inputFormat = vulkanSubmesh.GetInputFormat();
            if (inputFormat != nullptr)
            {
                VulkanPipelineDescription::GraphicsState graphicsState;
                graphicsState.primitiveTopology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
                graphicsState.shaderFeatures.inputFormatHash = inputFormat->GetElementHash();
                graphicsState.shaderFeatures.flags = entityShaderFeatureFlags.ForMaterialMapsAndMesh(material->gpuMapFlags, mesh->gpuMeshFlags);
                graphicsState.shaderFeatures.lights = sortedLightTypes;
                auto renderStateHash = graphicsState.GetHash();

                auto graphicsPipelineLayoutAndPipelineFoundAt = this->graphicsPipelineLayoutAndPipelines.find(renderStateHash);

            #if ALLOW_LIGHT_FALLBACK_IN_FAILED_SIGNATURE_LOOKUP
                //If the lookup failed, keep removing lights in an attempt to find a match
                while (graphicsPipelineLayoutAndPipelineFoundAt == this->graphicsPipelineLayoutAndPipelines.end() &&
                    !graphicsState.shaderFeatures.lights.empty())
                {
                    graphicsState.shaderFeatures.lights.pop_back();
                    renderStateHash = graphicsState.GetHash();
                    graphicsPipelineLayoutAndPipelineFoundAt = this->graphicsPipelineLayoutAndPipelines.find(renderStateHash);
                }
            #endif

                //Continue if there's a matching pipeline layout/pipeline state
                if (graphicsPipelineLayoutAndPipelineFoundAt != this->graphicsPipelineLayoutAndPipelines.end())
                {
                    auto& foundPipelineLayoutAndPipeline = graphicsPipelineLayoutAndPipelineFoundAt->second;

                    //Get/add pipeline layout
                    auto foundPipelineLayoutSlot = renderTargetState.RecordPipelineLayout(foundPipelineLayoutAndPipeline.pipelineLayout);
                    if (foundPipelineLayoutSlot != nullptr)
                    {
                        //Get/add pipeline
                        auto foundPipelineSlot = renderTargetState.RecordPipeline(foundPipelineLayoutSlot, foundPipelineLayoutAndPipeline.pipeline);
                        if (foundPipelineSlot != nullptr)
                        {
                            //Get/add descriptor sets
                            VkDescriptorSet descriptorSets[] = { this->settings.contextImpl->textureResources.textureDescriptorSet };
                            auto heapRef = foundPipelineSlot->Record(descriptorSets, FINJIN_COUNT_OF(descriptorSets));
                            if (heapRef != nullptr)
                            {
                                if (!renderTargetState.opaqueEntities.full())
                                {
                                    auto gpuBufferIndex = renderTargetState.opaqueEntities.size();
                                    renderTargetState.opaqueEntities.push_back();
                                    auto& instance = renderTargetState.opaqueEntities.back();

                                    instance.entity = entity;
                                    instance.subentityIndex = subentityIndex;
                                    instance.gpuBufferIndex = gpuBufferIndex;
                                    instance.worldMatrix = entityWorldMatrix;
                                    instance.worldInverseMatrix = entityInverseWorldMatrix;
                                    instance.worldInverseTransposeMatrix = entityInverseTransposeWorldMatrix;
                                    instance.ambientLightColor = ambientLight;

                                    for (size_t lightIndex = 0; lightIndex < graphicsState.shaderFeatures.lights.size(); lightIndex++)
                                    {
                                        auto& light = sortedLights[lightIndex];
                                        auto vulkanLight = static_cast<VulkanLight*>(light.light->gpuLight);
                                        instance.lightIndex[lightIndex] = vulkanLight->gpuBufferIndex;
                                    }

                                    heapRef->opaqueEntities.push_back(&instance);

                                    frameState.opaqueMaterials.insert(material);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void VulkanStaticMeshRenderer::RenderQueued(VulkanStaticMeshRendererFrameState& frameState, VkCommandBuffer commandBuffer)
{
    auto& renderTargetState = frameState.renderTargetRenderStates[0];

    {
        MathMatrix4Values vulkanMatrix;

        //Update object uniform buffers
        auto& frameObjectUniformBuffer = frameState.uniformBuffers[VulkanStaticMeshRendererKnownUniformBuffer::OBJECT];
        frameObjectUniformBuffer.StartWrites(this->settings.contextImpl->vk);
        for (auto& item : renderTargetState.opaqueEntities)
        {
            GetColumnOrderMatrixData(vulkanMatrix, item.worldMatrix);
            auto offset = frameObjectUniformBuffer.Set(vulkanMatrix, item.gpuBufferIndex, GpuStructuredAndConstantBufferStructMetadata::ElementID::MODEL_MATRIX);

            GetColumnOrderMatrixData(vulkanMatrix, item.worldInverseMatrix);
            offset = frameObjectUniformBuffer.Set(vulkanMatrix, item.gpuBufferIndex, GpuStructuredAndConstantBufferStructMetadata::ElementID::INVERSE_MODEL_MATRIX);

            GetColumnOrderMatrixData(vulkanMatrix, item.worldInverseTransposeMatrix);
            offset = frameObjectUniformBuffer.Set(vulkanMatrix, item.gpuBufferIndex, GpuStructuredAndConstantBufferStructMetadata::ElementID::INVERSE_TRANSPOSE_MODEL_MATRIX);

            offset = frameObjectUniformBuffer.Set(item.ambientLightColor, item.gpuBufferIndex, GpuStructuredAndConstantBufferStructMetadata::ElementID::AMBIENT_LIGHT_COLOR);

            auto material = item.entity->subentities[item.subentityIndex].materialHandle.asset;
            auto vulkanMaterial = static_cast<VulkanMaterial*>(material->gpuMaterial);

            auto materialIndex = static_cast<uint32_t>(vulkanMaterial->gpuBufferIndex);
            offset = frameObjectUniformBuffer.Set(materialIndex, item.gpuBufferIndex, GpuStructuredAndConstantBufferStructMetadata::ElementID::MATERIAL_INDEX);

            //frameObjectUniformBuffer.SetArray(item.lightIndex, item.gpuBufferIndex, GpuStructuredAndConstantBufferStructMetadata::ElementID::LIGHT_INDEX);
            offset = frameObjectUniformBuffer.Set(item.lightIndex, item.gpuBufferIndex, GpuStructuredAndConstantBufferStructMetadata::ElementID::LIGHT_INDEX);
        }
        frameObjectUniformBuffer.FinishWrites(this->settings.contextImpl->vk);
        renderTargetState.opaqueEntities.clear();
    }

    {
        for (auto& pipelineLayoutSlot : renderTargetState.pipelineLayoutSlots)
        {
            for (auto& pipelineSlot : pipelineLayoutSlot.second.pipelineSlots)
            {
                this->settings.contextImpl->vk.CmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineSlot.second->pipeline);

                for (auto& heapRef : pipelineSlot.second->heapRefs)
                {
                    this->settings.contextImpl->vk.CmdBindDescriptorSets
                        (
                        commandBuffer,
                        VK_PIPELINE_BIND_POINT_GRAPHICS,
                        pipelineLayoutSlot.second.pipelineLayout->vkpipelineLayout,
                        (uint32_t)VulkanStaticMeshRendererKnownDescriptorSetLayout::UNIFORMS,
                        1,
                        &frameState.descriptorSet,
                        0,
                        nullptr
                        );
                    this->settings.contextImpl->vk.CmdBindDescriptorSets
                        (
                        commandBuffer,
                        VK_PIPELINE_BIND_POINT_GRAPHICS,
                        pipelineLayoutSlot.second.pipelineLayout->vkpipelineLayout,
                        (uint32_t)VulkanStaticMeshRendererKnownDescriptorSetLayout::TEXTURES,
                        1,
                        &this->settings.contextImpl->textureResources.textureDescriptorSet,
                        0,
                        nullptr
                        );

                    for (auto item : heapRef.opaqueEntities)
                    {
                        auto& subentity = item->entity->subentities[item->subentityIndex];

                        auto mesh = item->entity->meshHandle.asset;
                        auto& submesh = mesh->submeshes[item->subentityIndex];

                        auto vulkanMesh = static_cast<VulkanMesh*>(mesh->gpuMesh);
                        auto& vulkanSubmesh = vulkanMesh->submeshes[item->subentityIndex];

                        {
                            uint32_t pushConstantOffsetInBytes = 0;

                            //TODO: Work out a better system where the metadata 'id' is used
                            EnumArray<StandardPushConstant, StandardPushConstant::COUNT, VulkanPushConstant> standardPushConstants;
                            standardPushConstants[StandardPushConstant::OBJECT_INDEX].ui = static_cast<uint32_t>(item->gpuBufferIndex);

                            auto pushConstantsInBytes = static_cast<uint32_t>(sizeof(VulkanPushConstant) * standardPushConstants.size());
                            this->settings.contextImpl->vk.CmdPushConstants
                                (
                                commandBuffer,
                                pipelineLayoutSlot.second.pipelineLayout->vkpipelineLayout,
                                VK_SHADER_STAGE_ALL,
                                pushConstantOffsetInBytes,
                                pushConstantsInBytes,
                                standardPushConstants.data()
                                );
                            pushConstantOffsetInBytes += pushConstantsInBytes;
                        }

                        //Vertex buffer
                        auto vertexBufferView = vulkanSubmesh.GetVertexBufferView();
                        this->settings.contextImpl->vk.CmdBindVertexBuffers(commandBuffer, VERTEX_BUFFER_BIND_ID, 1, &vertexBufferView.buffer, &vertexBufferView.bufferOffset);

                        //Index buffer + draw, or just draw
                        auto indexBufferView = vulkanSubmesh.GetIndexBufferView();
                        if (indexBufferView.buffer != VK_NULL_HANDLE)
                        {
                            this->settings.contextImpl->vk.CmdBindIndexBuffer(commandBuffer, indexBufferView.buffer, indexBufferView.bufferOffset, indexBufferView.indexType);
                            this->settings.contextImpl->vk.CmdDrawIndexed(commandBuffer, vulkanSubmesh.indexCount, 1, vulkanSubmesh.startIndexLocation, vulkanSubmesh.baseVertexLocation, 0);
                        }
                        else
                        {
                            //this->settings.contextImpl->vk.CmdDrawIndirect()
                            this->settings.contextImpl->vk.CmdDraw(commandBuffer, vulkanSubmesh.vertexCount, 1, 0, 0);
                        }
                    }
                }
            }
        }
        renderTargetState.ClearState();
    }
}

#endif
