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
#include "finjin/common/EnumArray.hpp"
#include "finjin/common/Error.hpp"
#include "finjin/common/Hash.hpp"
#include "finjin/engine/FinjinSceneAssets.hpp"
#include "VulkanGpuBuffer.hpp"
#include "VulkanPipelineLayout.hpp"
#include "VulkanRenderableEntity.hpp"
#include "VulkanTexture.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    enum class VulkanStaticMeshRendererKnownUniformBuffer
    {
        PASS,
        OBJECT,
        MATERIAL,
        LIGHT,

        COUNT
    };

    //Corresponds to elements in gpu-context-static-mesh-renderer-pipeline-layouts.cfg----------------------
    enum class VulkanStaticMeshRendererKnownPipelineLayoutDescription
    {
        PRIMARY,

        COUNT
    };

    //Corresponds to elements in gpu-context-static-mesh-renderer-descriptor-set-layouts.cfg---------------------

    //Corresponds to each configured descriptor set layout
    enum class VulkanStaticMeshRendererKnownDescriptorSetLayout
    {
        UNIFORMS,
        TEXTURES,

        COUNT
    };

    struct VulkanStaticMeshRendererPrimaryPipelineLayoutBindings
    {
        //Corresponds to elements in configured descriptor-set-layout type 'finjin.static-mesh-renderer.descriptor-set-layout.uniforms'
        enum class Uniforms
        {
            PASS,
            OBJECT,
            MATERIAL,
            LIGHT,

            COUNT
        };

        VulkanStaticMeshRendererPrimaryPipelineLayoutBindings() : uniforms(nullptr)
        {
        }

        EnumArray<Uniforms, Uniforms::COUNT, VkDescriptorSetLayoutBinding*> uniforms;
    };

    //Per frame state-------------------------------
    struct VulkanStaticMeshRendererRenderTargetRenderState
    {
        struct PipelineSlot
        {
            PipelineSlot()
            {
                this->pipeline = VK_NULL_HANDLE;
            }

            struct HeapRef
            {
                HeapRef()
                {
                }

                void ClearState()
                {
                    this->descriptorSets.clear();
                    this->opaqueEntities.clear();
                }

                StaticVector<VkDescriptorSet, 4> descriptorSets;

                DynamicVector<VulkanRenderableEntity*> opaqueEntities;
            };

            HeapRef* Record(VkDescriptorSet* descriptorSets, size_t argCount)
            {
                for (auto& heapRef : this->heapRefs)
                {
                    if (heapRef.descriptorSets.Equals(descriptorSets, argCount))
                        return &heapRef;
                }

                if (this->heapRefs.full())
                    return nullptr;

                this->heapRefs.push_back();
                auto heapRef = &this->heapRefs.back();
                heapRef->descriptorSets.assign(descriptorSets, argCount);

                return heapRef;
            }

            void ClearState()
            {
                this->pipeline = VK_NULL_HANDLE;

                for (auto& heapRef : this->heapRefs)
                    heapRef.ClearState();
                this->heapRefs.clear();
            }

            VkPipeline pipeline;
            DynamicVector<HeapRef> heapRefs;
        };

        struct PipelineLayoutSlot
        {
            void ClearState()
            {
                this->pipelineLayout = nullptr;

                this->pipelineSlots.clear();
            }

            VulkanPipelineLayout* pipelineLayout;
            DynamicUnorderedMap<size_t, PipelineSlot*, MapPairConstructNone<size_t, PipelineSlot*>, PassthroughHash> pipelineSlots;
        };

        PipelineLayoutSlot* RecordPipelineLayout(VulkanPipelineLayout* pipelineLayout)
        {
            std::hash<VulkanPipelineLayout*> pipelineLayoutHash;
            auto hash = pipelineLayoutHash(pipelineLayout);

            auto foundAt = this->pipelineLayoutSlots.find(hash);
            if (foundAt != this->pipelineLayoutSlots.end())
                return &foundAt->second;

            if (this->pipelineLayoutSlots.full())
                return nullptr;

            auto state = this->pipelineLayoutSlots.GetOrAdd(hash);
            if (state.HasError() || state.value == nullptr)
            {
                assert(0 && "Unexpectedly failed to add pipeline layout.");
                return nullptr;
            }

            state.value->pipelineLayout = pipelineLayout;

            return state.value;
        }

        PipelineSlot* RecordPipeline(PipelineLayoutSlot* pipelineLayoutSlot, VkPipeline pipeline)
        {
            std::hash<VkPipeline> pipelineHash;
            auto hash = pipelineHash(pipeline);

            auto foundAt = this->pipelineSlots.find(hash);
            if (foundAt != this->pipelineSlots.end())
                return &foundAt->second;

            if (this->pipelineSlots.full())
                return nullptr;

            auto state = this->pipelineSlots.GetOrAdd(hash);
            if (state.HasError() || state.value == nullptr)
            {
                assert(0 && "Unexpectedly failed to add pipeline.");
                return nullptr;
            }

            state.value->pipeline = pipeline;

            pipelineLayoutSlot->pipelineSlots.insert(hash, state.value, false);

            return state.value;
        }

        void ClearState()
        {
            for (auto& state : this->pipelineLayoutSlots)
                state.second.ClearState();
            this->pipelineLayoutSlots.clear();

            for (auto& state : this->pipelineSlots)
                state.second.ClearState();
            this->pipelineSlots.clear();
        }

        DynamicVector<VulkanRenderableEntity> opaqueEntities;
        DynamicUnorderedMap<size_t, PipelineLayoutSlot, MapPairConstructNone<size_t, PipelineLayoutSlot>, PassthroughHash> pipelineLayoutSlots;
        DynamicUnorderedMap<size_t, PipelineSlot, MapPairConstructNone<size_t, PipelineSlot>, PassthroughHash> pipelineSlots;
    };

    struct VulkanStaticMeshRendererFrameState
    {
        //An instance of this struct goes into VulkanFrameBuffer

        VulkanStaticMeshRendererFrameState()
        {
            this->index = 0;
            this->descriptorSet = VK_NULL_HANDLE;
        }

        size_t index;

        EnumArray<VulkanStaticMeshRendererKnownUniformBuffer, VulkanStaticMeshRendererKnownUniformBuffer::COUNT, VulkanGpuRenderingUniformBuffer> uniformBuffers;
        VkDescriptorSet descriptorSet;

        DynamicUnorderedSet<FinjinMaterial*> opaqueMaterials;
        DynamicUnorderedMap<FinjinSceneObjectLight*, LightState*> lights;

        std::array<VulkanStaticMeshRendererRenderTargetRenderState, EngineConstants::MAX_RENDER_TARGET_DEPENDENCIES> renderTargetRenderStates;
    };

} }
