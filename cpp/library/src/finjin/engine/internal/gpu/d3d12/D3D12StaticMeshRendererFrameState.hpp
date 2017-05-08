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
#include "D3D12GpuBuffer.hpp"
#include "D3D12RenderableEntity.hpp"
#include "D3D12Texture.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    enum class D3D12StaticMeshRendererKnownConstantBuffer
    {
        PASS,
        OBJECT,
        MATERIAL,
        LIGHT,

        COUNT
    };

    enum class D3D12StaticMeshRendererKnownRootSignatureBindings
    {
        OBJECT,
        PASS,
        MATERIAL,
        LIGHT,
        TEXTURE
    };

    struct D3D12StaticMeshRendererRenderTargetRenderState
    {
        DynamicVector<D3D12RenderableEntity> opaqueEntities;

        struct PipelineStateSlot
        {
            struct HeapRef
            {
                HeapRef()
                {
                }

                void ClearState()
                {
                    this->descriptorHeaps.clear();
                    this->opaqueEntities.clear();
                }

                StaticVector<ID3D12DescriptorHeap*, 4> descriptorHeaps;

                DynamicVector<D3D12RenderableEntity*> opaqueEntities;
            };

            HeapRef* Record(ID3D12DescriptorHeap** heaps, size_t argCount)
            {
                for (auto& heapRef : this->heapRefs)
                {
                    if (heapRef.descriptorHeaps.Equals(heaps, argCount))
                        return &heapRef;
                }

                if (this->heapRefs.full())
                    return nullptr;

                this->heapRefs.push_back();
                auto heapRef = &this->heapRefs.back();
                heapRef->descriptorHeaps.assign(heaps, argCount);

                return heapRef;
            }

            void ClearState()
            {
                this->pipelineState = nullptr;

                for (auto& heapRef : this->heapRefs)
                    heapRef.ClearState();

                this->heapRefs.clear();
            }

            ID3D12PipelineState* pipelineState;
            DynamicVector<HeapRef> heapRefs;
        };

        struct RootSignatureSlot
        {
            void ClearState()
            {
                this->rootSignature = nullptr;

                this->pipelineStateSlots.clear();
            }

            ID3D12RootSignature* rootSignature;
            DynamicUnorderedMap<size_t, PipelineStateSlot*, MapPairConstructNone<size_t, PipelineStateSlot*>, PassthroughHash> pipelineStateSlots;
        };

        RootSignatureSlot* RecordRootSignature(ID3D12RootSignature* rootSignature)
        {
            std::hash<ID3D12RootSignature*> rootSignatureHash;
            auto hash = rootSignatureHash(rootSignature);

            auto foundAt = this->rootSignatureSlots.find(hash);
            if (foundAt != this->rootSignatureSlots.end())
                return &foundAt->second;

            if (this->rootSignatureSlots.full())
                return nullptr;

            auto state = this->rootSignatureSlots.GetOrAdd(hash);
            if (state.HasError() || state.value == nullptr)
            {
                assert(0 && "Unexpectedly failed to add root signature.");
                return nullptr;
            }

            state.value->rootSignature = rootSignature;

            return state.value;
        }

        PipelineStateSlot* RecordPipelineState(RootSignatureSlot* rootSignatureSlot, ID3D12PipelineState* pipelineState)
        {
            std::hash<ID3D12PipelineState*> pipelineStateHash;
            auto hash = pipelineStateHash(pipelineState);

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

            rootSignatureSlot->pipelineStateSlots.insert(hash, state.value, false);

            return state.value;
        }

        void ClearState()
        {
            for (auto& state : this->rootSignatureSlots)
                state.second.ClearState();
            this->rootSignatureSlots.clear();

            for (auto& state : this->pipelineStateSlots)
                state.second.ClearState();
            this->pipelineStateSlots.clear();
        }

        DynamicUnorderedMap<size_t, RootSignatureSlot, MapPairConstructNone<size_t, RootSignatureSlot>, PassthroughHash> rootSignatureSlots;
        DynamicUnorderedMap<size_t, PipelineStateSlot, MapPairConstructNone<size_t, PipelineStateSlot>, PassthroughHash> pipelineStateSlots;
    };

    struct D3D12StaticMeshRendererFrameState
    {
        //An instance of this struct goes into D3D12FrameBuffer
        size_t index;

        EnumArray<D3D12StaticMeshRendererKnownConstantBuffer, D3D12StaticMeshRendererKnownConstantBuffer::COUNT, D3D12GpuRenderingConstantBuffer> constantBuffers;

        DynamicUnorderedSet<FinjinMaterial*> opaqueMaterials;
        DynamicUnorderedMap<FinjinSceneObjectLight*, LightState*> lights;

        std::array<D3D12StaticMeshRendererRenderTargetRenderState, EngineConstants::MAX_RENDER_TARGET_DEPENDENCIES> renderTargetRenderStates;
    };

} }
