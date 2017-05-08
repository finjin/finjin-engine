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
#include "MetalPipelineState.hpp"
#include "MetalResources.hpp"
#include "MetalStaticMeshRendererFrameState.hpp"
#include "MetalUtilities.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    class MetalGpuContextImpl;

    class MetalStaticMeshRenderer
    {
    public:
        struct Settings
        {
            Settings();

            Settings& operator = (Settings&& other);

            void ParseSettings(const ByteBufferReader& configFileBuffer, const Path& assetFilePath, Error& error);

            MetalGpuContextImpl* contextImpl;

            size_t maxRenderables;

            AssetReference pipelineShaderFileReference;

            DynamicVector<GpuRenderingConstantBufferStruct> uniformBufferFormats;
            DynamicVector<MetalPipelineStateDescriptor> graphicsPipelineStates;
        };

        MetalStaticMeshRenderer();

        void Create(Settings&& settings, Error& error);
        void Destroy();

        void UpdatePassAndMaterialConstants(MetalStaticMeshRendererFrameState& frameState, SimpleTimeDelta elapsedTime, SimpleTimeCounter totalElapsedTime);

        void RenderEntity(MetalStaticMeshRendererFrameState& frameState, SceneNodeState& sceneNodeState, FinjinSceneObjectEntity* entity, const RenderShaderFeatureFlags& entityShaderFeatureFlags, const GpuCommandLights& sortedLights, const MathVector4& ambientLight);
        void RenderQueued(MetalStaticMeshRendererFrameState& frameState, id<MTLCommandBuffer> commandBuffer, id<MTLRenderCommandEncoder> renderCommandEncoder);

    public:
        Settings settings;

        MTLRenderPipelineDescriptor* graphicsPipelineStateDesc;
        DynamicVector<id<MTLRenderPipelineState> > graphicsPipelineStates;

        ByteBuffer shaderFileBytes;
        dispatch_data_t shaderLibraryDispatchData;
        id<MTLLibrary> shaderLibrary;

        struct PipelineStateByHash
        {
            id<MTLRenderPipelineState> pipelineState;
        };
        DynamicUnorderedMap<size_t, PipelineStateByHash, MapPairConstructNone<size_t, PipelineStateByHash>, PassthroughHash> graphicsPipelineStatesByHash;

        EnumArray<MetalStaticMeshRendererKnownUniformBuffer, MetalStaticMeshRendererKnownUniformBuffer::COUNT, const GpuRenderingConstantBufferStruct*> knownUniformBufferDescriptions;
    };

} }
