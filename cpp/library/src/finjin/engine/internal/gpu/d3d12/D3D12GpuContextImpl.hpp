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


//Includes---------------------------------------------------------------
#include "finjin/common/AllocatedClass.hpp"
#include "finjin/common/ByteBuffer.hpp"
#include "finjin/common/Error.hpp"
#include "finjin/common/FiberSpinLock.hpp"
#include "finjin/common/OperationStatus.hpp"
#include "finjin/common/SimpleSpinLockMutex.hpp"
#include "finjin/common/StaticUnorderedMap.hpp"
#include "finjin/common/Vector.hpp"
#include "finjin/common/WindowsUtilities.hpp"
#include "finjin/engine/Camera.hpp"
#include "finjin/engine/GenericGpuNumericStructs.hpp"
#include "finjin/engine/OSWindowEventListener.hpp"
#include "finjin/engine/WindowSize.hpp"
#include "D3D12GpuContextSettings.hpp"
#include "D3D12DescriptorHeap.hpp"
#include "D3D12DisplayMode.hpp"
#include "D3D12FrameBuffer.hpp"
#include "D3D12GpuDescription.hpp"
#include "D3D12FrameStage.hpp"
#include "D3D12Resources.hpp"
#include "D3D12StaticMeshRenderer.hpp"
#include "D3D12Texture.hpp"
#include "D3D12Utilities.hpp"


//Classes----------------------------------------------------------------
namespace Finjin { namespace Engine {
    
    using namespace Finjin::Common;
    
    class D3D12System;

    class D3D12GpuContextImpl : public AllocatedClass, public OSWindowEventListener
    {
    public:
        D3D12GpuContextImpl(Allocator* allocator);

        void CreateDevice(Error& error);
        void CreateDeviceSupportObjects(Error& error);
        void CreateRenderers(Error& error);

        void Create(const D3D12GpuContextSettings& settings, Error& error);
        void Destroy();

        void GetBestFullScreenDisplayMode(D3D12DisplayMode& bestDisplayMode, size_t width, size_t height, D3D12RefreshRate refresh, HMONITOR theMonitor, Error& error);

        bool ToggleFullScreenExclusive(Error& error);
        bool StartResizeTargets(bool minimized, Error& error);
        void FinishResizeTargets(Error& error);

        uint64_t FlushGpu(Error& error);
        uint64_t FlushGpu(); //SETS_ERROR_STATE

        uint64_t EmitFenceValue(Error& error);
        uint64_t EmitFenceValue(); //SETS_ERROR_STATE

        void WaitForFenceValue(uint64_t v, Error& error);
        void WaitForFenceValue(uint64_t v); //SETS_ERROR_STATE
        size_t BusyWaitForFenceValue(uint64_t v);

        WindowBounds GetRenderingPixelBounds();

        void WindowResized(OSWindow* osWindow) override;

        bool ResolveMeshRef(FinjinMesh& mesh, const AssetReference& assetRef);
        bool ResolveMaterialRef(FinjinMaterial& material, const AssetReference& assetRef);
        bool ResolveMaterialMaps(FinjinMaterial& material);

        D3D12Texture* CreateTextureFromMainThread(FinjinTexture* texture, Error& error);
        void UploadTexture(ID3D12GraphicsCommandList* commandList, FinjinTexture* texture);
        
        void* CreateLightFromMainThread(FinjinSceneObjectLight* light, Error& error);
        
        void* CreateMaterial(ID3D12GraphicsCommandList* commandList, FinjinMaterial* material, Error& error);
        
        void* CreateMeshFromMainThread(FinjinMesh* mesh, Error& error);
        void UploadMesh(ID3D12GraphicsCommandList* commandList, FinjinMesh* mesh, Error& error);
                
        void CreateShader(D3D12ShaderType shaderType, const Utf8String& name, const uint8_t* bytes, size_t byteCount, bool makeLocalCopy, Error& error);
        void CreateShader(D3D12ShaderType shaderType, const AssetReference& assetRef, Error& error);

        void CreateCoreAssets(Error& error);

        D3D12RenderTarget* GetRenderTarget(D3D12FrameStage& frameStage, const Utf8String& name);

        void StartGraphicsCommandList(D3D12FrameStage& frameStage, Error& error);
        void StartRenderTarget(D3D12FrameStage& frameStage, D3D12RenderTarget* renderTarget, StaticVector<D3D12RenderTarget*, EngineConstants::MAX_RENDER_TARGET_DEPENDENCIES>& dependentRenderTargets, Error& error);
        void FinishRenderTarget(D3D12FrameStage& frameStage, Error& error);
        void FinishGraphicsCommandList(D3D12FrameStage& frameStage, Error& error);

        void Execute(D3D12FrameStage& frameStage, GpuEvents& events, GpuCommands& commands, Error& error);

        void UpdateResourceGpuResidencyStatus();

        void CreateScreenSizeDependentResources(Error& error);
        void DestroyScreenSizeDependentResources();

    private:
        void CreateGraphicsCommandQueue(Error& error);
        void CreateSwapChain(Error& error);

    public:
        OperationStatus initializationStatus;

        D3D12GpuContextSettings settings;
        AssetClassFileReader settingsAssetClassFileReader;
        AssetClassFileReader shaderAssetClassFileReader;

        struct InternalSettings
        {
            InternalSettings();

            void ParseSettings(const ByteBufferReader& configFileBuffer, const Path& assetFilePath, Error& error);

            void Validate(Error& error);

            GpuInputFormatStruct* GetInputFormat(const Utf8String& typeName, const AllocatedVector<FinjinVertexElementFormat>& elements);

            D3D12GpuContextImpl* contextImpl;

            AllocatedVector<GpuInputFormatStruct> inputFormats;
        };
        InternalSettings internalSettings;
        
        D3D12System* d3dSystem;

        D3D12GpuDescription desc;
        WindowBounds renderingPixelBounds;
        
        ByteBuffer readBuffer;

        struct MaterialMapTypeToGpuElements
        {
            MaterialMapTypeToGpuElements() {}
            MaterialMapTypeToGpuElements(D3D12Material::MapIndex gpuMaterialMapIndex, GpuStructuredAndConstantBufferStructMetadata::ElementID gpuBufferTextureIndexElementID, ShaderFeatureFlag gpuMaterialMapFlag)
            {
                this->gpuMaterialMapIndex = gpuMaterialMapIndex;
                this->gpuBufferTextureIndexElementID = gpuBufferTextureIndexElementID;
                this->gpuBufferAmountElementID = GpuStructuredAndConstantBufferStructMetadata::ElementID::NONE;
                this->gpuMaterialMapFlag = gpuMaterialMapFlag;
            }
            MaterialMapTypeToGpuElements(D3D12Material::MapIndex gpuMaterialMapIndex, GpuStructuredAndConstantBufferStructMetadata::ElementID gpuBufferTextureIndexElementID, GpuStructuredAndConstantBufferStructMetadata::ElementID gpuBufferAmountElementID, ShaderFeatureFlag gpuMaterialMapFlag)
            {
                this->gpuMaterialMapIndex = gpuMaterialMapIndex;
                this->gpuBufferTextureIndexElementID = gpuBufferTextureIndexElementID;
                this->gpuBufferAmountElementID = gpuBufferAmountElementID;
                this->gpuMaterialMapFlag = gpuMaterialMapFlag;
            }

            D3D12Material::MapIndex gpuMaterialMapIndex; //Index into D3D12Material maps
            GpuStructuredAndConstantBufferStructMetadata::ElementID gpuBufferTextureIndexElementID; //ElementID (index) into shader constant or structured buffer for 'texture index'
            GpuStructuredAndConstantBufferStructMetadata::ElementID gpuBufferAmountElementID; //ElementID (index) into shader constant or structured buffer for 'amount'
            ShaderFeatureFlag gpuMaterialMapFlag; //Single flag identifying map's shader usage
        };

        StaticUnorderedMap<Utf8String, MaterialMapTypeToGpuElements, (size_t)D3D12Material::MapIndex::COUNT, FINJIN_OVERSIZE_FULL_STATIC_MAP_BUCKET_COUNT(D3D12Material::MapIndex::COUNT)> materialMapTypeToGpuElements;
        
        AllocatedVector<D3D12_SUBRESOURCE_DATA> preallocatedSubresourceData;
        ByteBuffer preallocatedFootprintSubresourceData;

        Microsoft::WRL::ComPtr<ID3D12Device> device;
        Microsoft::WRL::ComPtr<ID3D12CommandQueue> graphicsCommandQueue;
        Microsoft::WRL::ComPtr<IDXGISwapChain3> swapChain;
        D3D12DescriptorHeap swapChainRtvDescHeap;        
        D3D12DescriptorHeap swapChainDsvDescHeap;

        size_t sequenceIndex;

        size_t nextFrameBufferIndex;
        StaticVector<D3D12FrameBuffer, EngineConstants::MAX_FRAME_BUFFERS> frameBuffers;
        StaticVector<D3D12FrameStage, EngineConstants::MAX_FRAME_STAGES> frameStages;

        D3D12DescriptorHeap srvTextureDescHeap;

        HANDLE fenceEventHandle;
        Microsoft::WRL::ComPtr<ID3D12Fence> fence;
        std::atomic<uint64_t> fenceValue;
        
        D3D12_VIEWPORT viewport;
        D3D12_RECT scissorRect;

        Camera camera;
        MathVector4 clearColor;
        
        D3D12Resources resources;

        UsableAllocatedVector<D3D12Light> lights;

        D3D12StaticMeshRenderer staticMeshRenderer;

        FiberSpinLock createLock;
    };
    
} }
