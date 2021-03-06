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
#include "finjin/common/DynamicUnorderedMap.hpp"
#include "finjin/common/DynamicUnorderedSet.hpp"
#include "finjin/common/DynamicVector.hpp"
#include "finjin/common/Error.hpp"
#include "finjin/common/Math.hpp"
#include "finjin/engine/FinjinSceneAssets.hpp"
#include "finjin/engine/GpuContextCommonSettings.hpp"
#include "D3D12Includes.hpp"
#include "D3D12DescriptorHeap.hpp"
#include "D3D12PipelineState.hpp"
#include "D3D12Resources.hpp"
#include "D3D12RootSignature.hpp"
#include "D3D12StaticMeshRendererFrameState.hpp"
#include "D3D12Utilities.hpp"


//Types-------------------------------------------------------------------------
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

            AssetReference pipelineShaderFileReference;

            DynamicVector<GpuRenderingConstantBufferStruct> constantBufferFormats;
            DynamicVector<D3D12RootSignatureDescriptor> rootSignatures;
            DynamicVector<D3D12PipelineStateDescriptor> graphicsPipelineStates;
        };

        D3D12StaticMeshRenderer();

        void Create(Settings&& settings, Error& error);
        void Destroy();

        void UpdatePassAndMaterialConstants(D3D12StaticMeshRendererFrameState& frameState, SimpleTimeDelta elapsedTime, SimpleTimeCounter totalElapsedTime);

        void RenderEntity(D3D12StaticMeshRendererFrameState& frameState, SceneNodeState& sceneNodeState, FinjinSceneObjectEntity* entity, const RenderShaderFeatureFlags& entityShaderFeatureFlags, const GpuCommandLights& sortedLights, const MathVector4& ambientLight);
        void RenderQueued(D3D12StaticMeshRendererFrameState& frameState, ID3D12GraphicsCommandList* commandList);

    public:
        Settings settings;

        DynamicVector<Microsoft::WRL::ComPtr<ID3D12RootSignature> > rootSignatures;

        D3D12_GRAPHICS_PIPELINE_STATE_DESC baseGraphicsPipelineStateDesc;
        DynamicVector<Microsoft::WRL::ComPtr<ID3D12PipelineState> > graphicsPipelineStates;

        ByteBuffer shaderFileBytes;

        struct RootSignatureAndPipelineState
        {
            ID3D12RootSignature* rootSignature;
            ID3D12PipelineState* pipelineState;
        };
        DynamicUnorderedMap<size_t, RootSignatureAndPipelineState, MapPairConstructNone<size_t, RootSignatureAndPipelineState>, PassthroughHash> graphicsRootSignatureAndPipelineStates;

        EnumArray<D3D12StaticMeshRendererKnownConstantBuffer, D3D12StaticMeshRendererKnownConstantBuffer::COUNT, const GpuRenderingConstantBufferStruct*> knownConstantBufferDescriptions;
    };

} }
