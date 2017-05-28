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
#include "D3D12DescriptorHeap.hpp"
#include "D3D12DisplayMode.hpp"
#include "D3D12FrameBuffer.hpp"
#include "D3D12FrameStage.hpp"
#include "D3D12GpuContextSettings.hpp"
#include "D3D12GpuDescription.hpp"
#include "D3D12Resources.hpp"
#include "D3D12ShaderType.hpp"
#include "D3D12StaticMeshRenderer.hpp"
#include "D3D12Texture.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    class D3D12System;

    class D3D12GpuContextImpl : public AllocatedClass, public GpuContextCommonImpl, public OSWindowEventListener
    {
    public:
        D3D12GpuContextImpl(Allocator* allocator);

        void Create(const D3D12GpuContextSettings& settings, Error& error);
        void Destroy();

        void CreateDevice(Error& error);
        void CreateDeviceSupportObjects(Error& error);
        void CreateRenderers(Error& error);

        D3D12FrameStage& StartFrameStage(size_t index, SimpleTimeDelta elapsedTime, SimpleTimeCounter totalElapsedTime);
        void PresentFrameStage(D3D12FrameStage& frameStage, RenderStatus renderStatus, size_t presentSyncIntervalOverride, Error& error);

        void Execute(D3D12FrameStage& frameStage, GpuEvents& events, GpuCommands& commands, Error& error);

        uint64_t FlushGpu();

        void CreateScreenSizeDependentResources(Error& error);
        void DestroyScreenSizeDependentResources(bool resizing);

        void WindowResized(OSWindow* osWindow) override;

        void UpdateResourceGpuResidencyStatus();

        D3D12Texture* CreateTextureFromMainThread(FinjinTexture* texture, Error& error);
        void* CreateLightFromMainThread(FinjinSceneObjectLight* light, Error& error);
        void* CreateMeshFromMainThread(FinjinMesh* mesh, Error& error);

        void CreateShader(D3D12ShaderType shaderType, const Utf8String& name, const uint8_t* bytes, size_t byteCount, bool makeLocalCopy, Error& error);
        void CreateShader(D3D12ShaderType shaderType, const AssetReference& assetRef, Error& error);

        bool ResolveMeshRef(FinjinMesh& mesh, const AssetReference& assetRef);
        bool ResolveMaterialRef(FinjinMaterial& material, const AssetReference& assetRef);
        bool ResolveMaterialMaps(FinjinMaterial& material);

        ScreenCaptureResult GetScreenCapture(ScreenCapture& screenCapture, D3D12FrameStage& frameStage);

        bool ToggleFullScreenExclusive(Error& error);
        bool StartResizeTargets(bool minimized, Error& error);
        void FinishResizeTargets(Error& error);

    private:
        void* CreateMaterial(ID3D12GraphicsCommandList* commandList, FinjinMaterial* material, Error& error);

        void UploadTexture(ID3D12GraphicsCommandList* commandList, FinjinTexture* texture);
        void UploadMesh(ID3D12GraphicsCommandList* commandList, FinjinMesh* mesh, Error& error);

        D3D12RenderTarget* GetRenderTarget(D3D12FrameStage& frameStage, const Utf8String& name);
        void StartGraphicsCommands(D3D12FrameStage& frameStage, Error& error);
        void FinishGraphicsCommands(D3D12FrameStage& frameStage, Error& error);
        void StartRenderTarget(D3D12FrameStage& frameStage, D3D12RenderTarget* renderTarget, StaticVector<D3D12RenderTarget*, EngineConstants::MAX_RENDER_TARGET_DEPENDENCIES>& dependentRenderTargets, Error& error);
        void FinishRenderTarget(D3D12FrameStage& frameStage, Error& error);

        uint64_t NewFenceValue();
        uint64_t EmitFenceValue();
        void SleepWaitForFenceValue(uint64_t fenceValue);

        void GetBestFullScreenDisplayMode(D3D12DisplayMode& bestDisplayMode, size_t width, size_t height, D3D12RefreshRate refresh, HMONITOR theMonitor, Error& error);

        void CreateGraphicsCommandQueue(Error& error);
        void CreateSwapChain(Error& error);

        void UpdatedCachedWindowSize();

    public:
        D3D12GpuContextSettings settings;

        struct InternalSettings
        {
            InternalSettings();

            void ParseSettings(const ByteBufferReader& configFileBuffer, const Path& assetFilePath, Error& error);

            void Validate(Error& error);

            GpuInputFormatStruct* GetInputFormat(const Utf8String& typeName, const DynamicVector<FinjinVertexElementFormat>& elements);

            D3D12GpuContextImpl* contextImpl;

            DynamicVector<GpuInputFormatStruct> inputFormats;
        };
        InternalSettings internalSettings;

        D3D12System* d3dSystem;

        D3D12GpuDescription deviceDescription;
        StaticVector<D3D12_FEATURE_DATA_FORMAT_SUPPORT, (size_t)DXGI_FORMAT_V408 + 1> formatSupportByFormat; //Format support indexed by the format itself

        FINJIN_LITERAL_STRING_STATIC_UNORDERED_MAP(GpuMaterialMapIndexToConstantBufferElements<D3D12Material::MapIndex>, D3D12Material::MapIndex::COUNT) materialMapTypeToElements;

        DynamicVector<D3D12_SUBRESOURCE_DATA> preallocatedSubresourceData;
        ByteBuffer preallocatedFootprintSubresourceData;

        D3D12_VIEWPORT windowViewport;
        D3D12_RECT windowScissorRect;

        D3D12_VIEWPORT renderViewport;
        D3D12_RECT renderScissorRect;

        Microsoft::WRL::ComPtr<ID3D12Device> device;

        Microsoft::WRL::ComPtr<ID3D12CommandQueue> graphicsCommandQueue;
        Microsoft::WRL::ComPtr<IDXGISwapChain3> swapChain;
        D3D12DescriptorHeap rtvDescHeap;
        D3D12DescriptorHeap dsvDescHeap;

        HANDLE fenceEventHandle;
        Microsoft::WRL::ComPtr<ID3D12Fence> fence;
        std::atomic<uint64_t> fenceValue;

        size_t frameBufferScreenCaptureColorFormatBytesPerPixel;
        ScreenCapturePixelFormat frameBufferScreenCapturePixelFormat;

        D3D12CommonResources commonResources;
        D3D12AssetResources assetResources;

        D3D12StaticMeshRenderer staticMeshRenderer;

        D3D12RenderTarget depthStencilRenderTarget;

        StaticVector<D3D12FrameBuffer, EngineConstants::MAX_FRAME_BUFFERS> frameBuffers;
        StaticVector<D3D12FrameStage, EngineConstants::MAX_FRAME_STAGES> frameStages;
    };

} }
