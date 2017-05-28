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
#include "finjin/engine/GpuContextCommonImpl.hpp"
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

    class MetalGpuContextImpl : public AllocatedClass, public GpuContextCommonImpl, public OSWindowEventListener
    {
    public:
        MetalGpuContextImpl(Allocator* allocator);

        void Create(const MetalGpuContextSettings& settings, Error& error);
        void Destroy();

        void CreateDevice(Error& error);
        void CreateDeviceSupportObjects(Error& error);
        void CreateRenderers(Error& error);

        MetalFrameStage& StartFrameStage(size_t index, SimpleTimeDelta elapsedTime, SimpleTimeCounter totalElapsedTime);
        void PresentFrameStage(MetalFrameStage& frameStage, RenderStatus renderStatus, size_t presentSyncIntervalOverride, Error& error);

        void Execute(MetalFrameStage& frameStage, GpuEvents& events, GpuCommands& commands, Error& error);

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

        ScreenCaptureResult GetScreenCapture(ScreenCapture& screenCapture, MetalFrameStage& frameStage);

    private:
        MetalRenderTarget* GetRenderTarget(MetalFrameStage& frameStage, const Utf8String& name);
        void StartGraphicsCommands(MetalFrameStage& frameStage, Error& error);
        void FinishGraphicsCommands(MetalFrameStage& frameStage, Error& error);
        void StartRenderTarget(MetalFrameStage& frameStage, MetalRenderTarget* renderTarget, StaticVector<MetalRenderTarget*, EngineConstants::MAX_RENDER_TARGET_DEPENDENCIES>& dependentRenderTargets, Error& error);
        void FinishRenderTarget(MetalFrameStage& frameStage, Error& error);

        MetalFrameBuffer& GetFrameBuffer(size_t index);

        void UpdatedCachedWindowSize();

    public:
        MetalGpuContextSettings settings;

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

        FINJIN_LITERAL_STRING_STATIC_UNORDERED_MAP(GpuMaterialMapIndexToConstantBufferElements<MetalMaterial::MapIndex>, MetalMaterial::MapIndex::COUNT) materialMapTypeToElements;

        MTLViewport windowViewport;
        MTLScissorRect windowScissorRect;

        MTLViewport renderViewport;
        MTLScissorRect renderScissorRect;

        id<MTLDevice> device;

        size_t frameBufferScreenCaptureColorFormatBytesPerPixel;
        ScreenCapturePixelFormat frameBufferScreenCapturePixelFormat;

        MetalCommonResources commonResources;
        MetalAssetResources assetResources;

        MetalStaticMeshRenderer staticMeshRenderer;

        MetalRenderTarget depthStencilRenderTarget;

        StaticVector<MetalFrameBuffer, EngineConstants::MAX_FRAME_BUFFERS> frameBuffers;
        StaticVector<MetalFrameStage, EngineConstants::MAX_FRAME_STAGES> frameStages;
    };

} }
