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


//Includes---------------------------------------------------------------------
#include "finjin/common/AllocatedVector.hpp"
#include "finjin/common/AllocatedUnorderedMap.hpp"
#include "finjin/common/AllocatedUnorderedSet.hpp"
#include "finjin/common/Error.hpp"
#include "finjin/common/Math.hpp"
#include "finjin/engine/GpuContextCommonSettings.hpp"
#include "finjin/engine/FinjinSceneAssets.hpp"
#include "D3D12Includes.hpp"
#include "D3D12DescriptorHeap.hpp"
#include "D3D12Resources.hpp"
#include "D3D12RootSignature.hpp"
#include "D3D12PipelineState.hpp"
#include "D3D12StaticMeshRendererFrameState.hpp"
#include "D3D12Utilities.hpp"


//Classes----------------------------------------------------------------------
namespace Finjin { namespace Engine {
    
    using namespace Finjin::Common;
    
    class D3D12GpuContextImpl;

    class D3D12StaticMeshRenderer
    {
    public:
        struct Settings
        {
            Settings();
            
            Settings& operator = (Settings&& other);

            void ParseSettings(const ByteBufferReader& configFileBuffer, const Path& assetFilePath, Error& error);

            D3D12GpuContextImpl* contextImpl;
            
            size_t maxRenderables;
            
            AssetReference pipelineStatesShaderFileReference;

            AllocatedVector<GpuRenderingConstantBufferStruct> constantBufferFormats;
            AllocatedVector<GpuRenderingStructuredBufferStruct> structuredBufferFormats;
            AllocatedVector<D3D12RootSignatureDescriptor> rootSignatures;
            AllocatedVector<D3D12PipelineStateDescriptor> pipelineStates;
        };

        D3D12StaticMeshRenderer();

        void Create(Settings&& settings, Error& error);
        void Destroy();

        void UpdatePassAndMaterialConstants(D3D12StaticMeshRendererFrameState& frameState, SimpleTimeDelta elapsedTime, SimpleTimeCounter totalElapsedTime);

        void RenderEntity(D3D12StaticMeshRendererFrameState& frameState, SceneNodeState& sceneNodeState, FinjinSceneObjectEntity* entity, const RenderShaderFeatureFlags& entityShaderFeatureFlags, const GpuCommandLights& sortedLights, const MathVector4& ambientLight);
        void RenderQueued(D3D12StaticMeshRendererFrameState& frameState, ID3D12GraphicsCommandList* commandList);
        
    public:
        Settings settings;

        AllocatedVector<Microsoft::WRL::ComPtr<ID3D12RootSignature> > rootSignatures;

        D3D12_GRAPHICS_PIPELINE_STATE_DESC basePipelineStateDesc;
        AllocatedVector<Microsoft::WRL::ComPtr<ID3D12PipelineState> > pipelineStates;

        ByteBuffer pipelineStatesShaderFileBytes;

        struct RootSignatureAndPipelineState
        {
            ID3D12RootSignature* rootSignature;
            ID3D12PipelineState* pipelineState;

            D3D12StaticMeshRendererRootSignatureIndexes rootSignatureIndexes;
        };
        AllocatedUnorderedMap<size_t, RootSignatureAndPipelineState, MapPairConstructNone<size_t, RootSignatureAndPipelineState>, PassthroughHash> graphicsRootSignatureAndPipelineStates;

        const GpuRenderingConstantBufferStruct* passConstantsDescription;
        const GpuRenderingConstantBufferStruct* objectConstantBufferDescription;
        const GpuRenderingStructuredBufferStruct* objectStructuredBufferDescription;
        const GpuRenderingConstantBufferStruct* materialConstantBufferDescription;
        const GpuRenderingStructuredBufferStruct* materialStructuredBufferDescription;
        const GpuRenderingConstantBufferStruct* lightConstantBufferDescription;        
        const GpuRenderingStructuredBufferStruct* lightStructuredBufferDescription;
    };
    
} }
