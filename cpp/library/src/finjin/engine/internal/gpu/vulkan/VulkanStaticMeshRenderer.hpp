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
#include "finjin/common/DynamicVector.hpp"
#include "finjin/engine/FinjinSceneAssets.hpp"
#include "finjin/engine/GpuContextCommonSettings.hpp"
#include "VulkanDescriptorPool.hpp"
#include "VulkanPipeline.hpp"
#include "VulkanPipelineLayout.hpp"
#include "VulkanDescriptorSetLayout.hpp"
#include "VulkanStaticMeshRendererFrameState.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    class VulkanGpuContextImpl;

    class VulkanStaticMeshRenderer
    {
    public:
        struct Settings
        {
            Settings();

            Settings& operator = (Settings&& other);

            void ParseSettings(const ByteBufferReader& configFileBuffer, const Path& assetFilePath, Error& error);

            VulkanGpuContextImpl* contextImpl;

            size_t maxRenderables;

            AssetReference pipelineShaderFileReference;

            DynamicVector<GpuRenderingConstantBufferStruct> uniformBufferFormats;
            DynamicVector<VulkanDescriptorSetLayoutDescription> descriptorSetLayouts;
            DynamicVector<VulkanPipelineLayoutDescription> pipelineLayouts;
            DynamicVector<VulkanPipelineDescription> pipelines;
        };

        VulkanStaticMeshRenderer();

        void Create(Settings&& settings, Error& error);
        void Destroy();

        void UpdatePassAndMaterialConstants(VulkanStaticMeshRendererFrameState& frameState, SimpleTimeDelta elapsedTime, SimpleTimeCounter totalElapsedTime);

        void RenderEntity(VulkanStaticMeshRendererFrameState& frameState, SceneNodeState& sceneNodeState, FinjinSceneObjectEntity* entity, const RenderShaderFeatureFlags& entityShaderFeatureFlags, const GpuCommandLights& sortedLights, const MathVector4& ambientLight);
        void RenderQueued(VulkanStaticMeshRendererFrameState& frameState, VkCommandBuffer commandBuffer);

    public:
        Settings settings;

        EnumArray<VulkanStaticMeshRendererKnownUniformBuffer, VulkanStaticMeshRendererKnownUniformBuffer::COUNT, const GpuRenderingConstantBufferStruct*> knownUniformBufferDescriptions;

        EnumArray<VulkanStaticMeshRendererKnownPipelineLayoutDescription, VulkanStaticMeshRendererKnownPipelineLayoutDescription::COUNT, const VulkanPipelineLayoutDescription*> knownPipelineLayoutDescriptions;

        VulkanStaticMeshRendererPrimaryPipelineLayoutBindings knownPrimaryDescriptorSetBindings;

        ByteBuffer shaderFileBytes;


        DynamicVector<VulkanDescriptorSetLayout> descriptorSetLayouts;

        VulkanGraphicsPipelineCreateInfoBuilder baseGraphicsPipelineBuilder;
        DynamicVector<VulkanPipelineLayout> pipelineLayouts;

        VkPipelineCache pipelineCache;
        DynamicVector<VkPipeline> pipelines;

        struct PipelineLayoutAndPipeline
        {
            PipelineLayoutAndPipeline()
            {
                this->pipelineLayout = nullptr;
                this->pipeline = VK_NULL_HANDLE;
            }

            VulkanPipelineLayout* pipelineLayout; //Pointer to entry in 'pipelineLayouts'
            VkPipeline pipeline; //One of the entries in 'pipelines'

            VulkanStaticMeshRendererPrimaryPipelineLayoutBindings pipelineLayoutBindingIndexes;
        };
        DynamicUnorderedMap<size_t, PipelineLayoutAndPipeline, MapPairConstructNone<size_t, PipelineLayoutAndPipeline>, PassthroughHash> graphicsPipelineLayoutAndPipelines;

        EnumArray<VulkanStaticMeshRendererKnownPipelineLayoutDescription, VulkanStaticMeshRendererKnownPipelineLayoutDescription::COUNT, VulkanDescriptorPool> knownPipelineLayoutDescriptorPools;

        MathMatrix4 clipInvertMatrix;

        enum class StandardPushConstant
        {
            OBJECT_INDEX,

            COUNT
        };
    };

} }
