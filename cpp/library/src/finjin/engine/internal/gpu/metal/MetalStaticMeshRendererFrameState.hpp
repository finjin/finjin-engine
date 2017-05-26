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
#include "MetalGpuBuffer.hpp"
#include "MetalRenderableEntity.hpp"
#include "MetalTexture.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    enum class MetalStaticMeshRendererKnownUniformBuffer
    {
        PASS,
        OBJECT,
        MATERIAL,
        LIGHT,

        COUNT
    };

    struct MetalStaticMeshRenderKnownBindings
    {
        struct VertexAndFragmentStageBuffer
        {
            enum
            {
                VERTEX,
                PASS,
                OBJECT,
                MATERIAL,
                LIGHT_BASE
            };
        };

        struct Sampler
        {
            enum
            {
                DEFAULT_SAMPLER
            };
        };

        struct Texture
        {
            //Corresponds to MetalMaterial::MapIndex
            /*enum
            {
                DIFFUSE
            };*/
        };
    };

    struct MetalStaticMeshRendererRenderTargetRenderState
    {
        MetalStaticMeshRendererRenderTargetRenderState()
        {
        }

        struct PipelineStateSlot
        {
            PipelineStateSlot()
            {
                this->pipelineState = nullptr;
            }

            void ClearState()
            {
                this->pipelineState = nullptr;

                this->opaqueEntities.clear();
            }

            id<MTLRenderPipelineState> pipelineState;
            DynamicVector<MetalRenderableEntity*> opaqueEntities;
        };

        PipelineStateSlot* RecordPipelineState(id<MTLRenderPipelineState> pipelineState)
        {
            std::hash<void*> pipelineStateHash;
            auto hash = pipelineStateHash((__bridge void*)pipelineState);

            auto foundAt = this->pipelineStateSlots.find(hash);
            if (foundAt != this->pipelineStateSlots.end())
                return &foundAt->second;

            if (this->pipelineStateSlots.full())
                return nullptr;

            auto state = this->pipelineStateSlots.GetOrAdd(hash);
            if (state.HasError() || state.value == nullptr)
            {
                assert(0 && "Unexpectedly failed to add pipeline state.");
                return nullptr;
            }

            state.value->pipelineState = pipelineState;

            return state.value;
        }

        void ClearState()
        {
            for (auto& state : this->pipelineStateSlots)
                state.second.ClearState();
            this->pipelineStateSlots.clear();
        }

        DynamicVector<MetalRenderableEntity> opaqueEntities;
        DynamicUnorderedMap<size_t, PipelineStateSlot, MapPairConstructNone<size_t, PipelineStateSlot>, PassthroughHash> pipelineStateSlots;
    };

    class MetalStaticMeshRendererFrameState
    {
    public:
        MetalStaticMeshRendererFrameState()
        {
            this->index = 0;
        }

        void Destroy()
        {
            for (auto& buffer : this->uniformBuffers)
                buffer.Destroy();

            this->opaqueMaterials.Destroy();
            this->lights.Destroy();
        }

    public:
        size_t index;

        EnumArray<MetalStaticMeshRendererKnownUniformBuffer, MetalStaticMeshRendererKnownUniformBuffer::COUNT, MetalGpuRenderingUniformBuffer> uniformBuffers;

        DynamicUnorderedSet<FinjinMaterial*> opaqueMaterials;
        DynamicUnorderedMap<FinjinSceneObjectLight*, LightState*> lights;

        std::array<MetalStaticMeshRendererRenderTargetRenderState, EngineConstants::MAX_RENDER_TARGET_DEPENDENCIES> renderTargetRenderStates;
    };

} }
