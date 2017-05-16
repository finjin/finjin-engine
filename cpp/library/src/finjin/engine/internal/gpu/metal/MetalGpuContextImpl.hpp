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
#include "finjin/common/AllocatedClass.hpp"
#include "finjin/common/ByteBuffer.hpp"
#include "finjin/common/Error.hpp"
#include "finjin/common/FiberSpinLock.hpp"
#include "finjin/common/OperationStatus.hpp"
#include "finjin/common/StaticVector.hpp"
#include "finjin/engine/Camera.hpp"
#include "finjin/engine/GpuContextCommonSettings.hpp"
#include "finjin/engine/OSWindowEventListener.hpp"
#include "finjin/engine/WindowSize.hpp"
#include "MetalFrameBuffer.hpp"
#include "MetalFrameStage.hpp"
#include "MetalGpuContextSettings.hpp"
#include "MetalGpuDescription.hpp"
#include "MetalResources.hpp"
#include "MetalShaderType.hpp"
#include "MetalStaticMeshRenderer.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    class MetalSystem;

    class MetalGpuContextImpl : public AllocatedClass, public OSWindowEventListener
    {
    public:
        MetalGpuContextImpl(Allocator* allocator);

        void Create(const MetalGpuContextSettings& settings, Error& error);
        void Destroy();

        void CreateDevice(Error& error);
        void CreateDeviceSupportObjects(Error& error);
        void CreateRenderers(Error& error);

        MetalFrameStage& StartFrameStage(size_t index, SimpleTimeDelta elapsedTime, SimpleTimeCounter totalElapsedTime);
        void FinishBackFrameBufferRender(MetalFrameStage& frameStage, bool continueRendering, bool modifyingRenderTarget, size_t presentSyncIntervalOverride, Error& error);
        MetalRenderTarget* GetRenderTarget(MetalFrameStage& frameStage, const Utf8String& name);
        void StartGraphicsCommandList(MetalFrameStage& frameStage, Error& error);
        void FinishGraphicsCommandList(MetalFrameStage& frameStage, Error& error);

        void Execute(MetalFrameStage& frameStage, GpuEvents& events, GpuCommands& commands, Error& error);

        MetalFrameBuffer& GetFrameBuffer(size_t index);

        void FlushGpu();

        void CreateScreenSizeDependentResources(Error& error);
        void DestroyScreenSizeDependentResources(bool resizing);

        void WindowResized(OSWindow* osWindow) override;

        MetalTexture* CreateTextureFromMainThread(FinjinTexture* texture, Error& error);
        void* CreateLightFromMainThread(FinjinSceneObjectLight* light, Error& error);
        void* CreateMaterialFromMainThread(FinjinMaterial* material, Error& error);
        void* CreateMeshFromMainThread(FinjinMesh* mesh, Error& error);

        void CreateShader(MetalShaderType shaderType, const Utf8String& name, id<MTLLibrary> shaderLibrary, Error& error);

        bool ResolveMeshRef(FinjinMesh& mesh, const AssetReference& assetRef);
        bool ResolveMaterialRef(FinjinMaterial& material, const AssetReference& assetRef);
        bool ResolveMaterialMaps(FinjinMaterial& material);

    private:
        void UpdateCachedFrameBufferSize();

        void StartRenderTarget(MetalFrameStage& frameStage, MetalRenderTarget* renderTarget, StaticVector<MetalRenderTarget*, EngineConstants::MAX_RENDER_TARGET_DEPENDENCIES>& dependentRenderTargets, Error& error);
        void FinishRenderTarget(MetalFrameStage& frameStage, Error& error);

    public:
        MetalGpuContextSettings settings;

        AssetClassFileReader settingsAssetClassFileReader;
        AssetClassFileReader shaderAssetClassFileReader;

        struct InternalSettings
        {
            InternalSettings();

            void ParseSettings(const ByteBufferReader& configFileBuffer, const Path& assetFilePath, Error& error);

            void Validate(Error& error);

            GpuInputFormatStruct* GetInputFormat(const Utf8String& typeName, const DynamicVector<FinjinVertexElementFormat>& elements);

            MetalGpuContextImpl* contextImpl;

            DynamicVector<GpuInputFormatStruct> inputFormats;
        };
        InternalSettings internalSettings;

        MetalSystem* metalSystem;

        MetalGpuDescription deviceDescription;
        id<MTLDevice> device;

        WindowBounds renderingPixelBounds;

        ByteBuffer readBuffer;

        struct MaterialMapTypeToGpuElements
        {
            MaterialMapTypeToGpuElements() {}
            MaterialMapTypeToGpuElements(MetalMaterial::MapIndex gpuMaterialMapIndex, GpuStructuredAndConstantBufferStructMetadata::ElementID gpuBufferTextureIndexElementID, ShaderFeatureFlag gpuMaterialMapFlag)
            {
                this->gpuMaterialMapIndex = gpuMaterialMapIndex;
                this->gpuBufferTextureIndexElementID = gpuBufferTextureIndexElementID;
                this->gpuBufferAmountElementID = GpuStructuredAndConstantBufferStructMetadata::ElementID::NONE;
                this->gpuMaterialMapFlag = gpuMaterialMapFlag;
            }
            MaterialMapTypeToGpuElements(MetalMaterial::MapIndex gpuMaterialMapIndex, GpuStructuredAndConstantBufferStructMetadata::ElementID gpuBufferTextureIndexElementID, GpuStructuredAndConstantBufferStructMetadata::ElementID gpuBufferAmountElementID, ShaderFeatureFlag gpuMaterialMapFlag)
            {
                this->gpuMaterialMapIndex = gpuMaterialMapIndex;
                this->gpuBufferTextureIndexElementID = gpuBufferTextureIndexElementID;
                this->gpuBufferAmountElementID = gpuBufferAmountElementID;
                this->gpuMaterialMapFlag = gpuMaterialMapFlag;
            }

            MetalMaterial::MapIndex gpuMaterialMapIndex; //Index into MetalMaterial maps
            GpuStructuredAndConstantBufferStructMetadata::ElementID gpuBufferTextureIndexElementID; //ElementID (index) into shader constant or structured buffer for 'texture index'
            GpuStructuredAndConstantBufferStructMetadata::ElementID gpuBufferAmountElementID; //ElementID (index) into shader constant or structured buffer for 'amount'
            ShaderFeatureFlag gpuMaterialMapFlag; //Single flag identifying map's shader usage
        };

        FINJIN_LITERAL_STRING_STATIC_UNORDERED_MAP(MaterialMapTypeToGpuElements, MetalMaterial::MapIndex::COUNT) materialMapTypeToGpuElements;

        std::atomic<size_t> currentFrameBufferIndex;

        size_t sequenceIndex;

        StaticVector<MetalFrameBuffer, EngineConstants::MAX_FRAME_BUFFERS> frameBuffers;
        StaticVector<MetalFrameStage, EngineConstants::MAX_FRAME_STAGES> frameStages;

        MetalTextureResources textureResources;

        MTLViewport viewport;
        MTLScissorRect scissorRect;

        Camera camera;
        MathVector4 clearColor;

        MetalRenderTarget depthStencilRenderTarget;

        MetalAssetResources assetResources;

        UsableDynamicVector<MetalLight> lights;

        MetalStaticMeshRenderer staticMeshRenderer;

        FiberSpinLock createLock;
    };

} }
