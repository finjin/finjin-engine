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
#include "VulkanFrameBuffer.hpp"
#include "VulkanFrameStage.hpp"
#include "VulkanGpuContextSettings.hpp"
#include "VulkanGpuDescription.hpp"
#include "VulkanIncludes.hpp"
#include "VulkanNamePointers.hpp"
#include "VulkanPresentModes.hpp"
#include "VulkanPrimaryQueues.hpp"
#include "VulkanQueueFamilyIndexes.hpp"
#include "VulkanResources.hpp"
#include "VulkanShaderType.hpp"
#include "VulkanSimpleWork.hpp"
#include "VulkanStaticMeshRenderer.hpp"
#include "VulkanSurfaceFormats.hpp"
#include "VulkanSwapChainFrameBuffer.hpp"
#include "VulkanTexture.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    class VulkanSystem;

    class VulkanGpuContextImpl : public AllocatedClass, public GpuContextCommonImpl, public OSWindowEventListener
    {
    public:
        VulkanGpuContextImpl(Allocator* allocator);

        void Create(const VulkanGpuContextSettings& settings, Error& error);
        void Destroy();

        void CreateDevice(Error& error);
        void CreateDeviceSupportObjects(Error& error);
        void CreateRenderers(Error& error);

        VulkanFrameStage& StartFrameStage(size_t index, SimpleTimeDelta elapsedTime, SimpleTimeCounter totalElapsedTime);
        void PresentFrameStage(VulkanFrameStage& frameStage, RenderStatus renderStatus, size_t presentSyncIntervalOverride, Error& error);

        void Execute(VulkanFrameStage& frameStage, GpuEvents& events, GpuCommands& commands, Error& error);

        void FlushGpu();

        void CreateScreenSizeDependentResources(Error& error);
        void DestroyScreenSizeDependentResources(bool resizing);

        void WindowResized(OSWindow* osWindow) override;

        void UpdateResourceGpuResidencyStatus();

        VulkanTexture* CreateTextureFromMainThread(FinjinTexture* texture, Error& error);
        void* CreateLightFromMainThread(FinjinSceneObjectLight* light, Error& error);
        void* CreateMeshFromMainThread(FinjinMesh* mesh, Error& error);

        void CreateShader(VulkanShaderType shaderType, const Utf8String& name, const uint8_t* bytes, size_t byteCount, bool makeLocalCopy, Error& error);
        void CreateShader(VulkanShaderType shaderType, const AssetReference& assetRef, Error& error);

        bool ResolveMeshRef(FinjinMesh& mesh, const AssetReference& assetRef);
        bool ResolveMaterialRef(FinjinMaterial& material, const AssetReference& assetRef);
        bool ResolveMaterialMaps(FinjinMaterial& material);

        ScreenCaptureResult GetScreenCapture(ScreenCapture& screenCapture, VulkanFrameStage& frameStage);

    private:
        void* CreateMaterial(VkCommandBuffer commandBuffer, FinjinMaterial* material, Error& error);

        void UploadTexture(VkCommandBuffer commandBuffer, FinjinTexture* texture);
        void UploadMesh(VkCommandBuffer commandBuffer, FinjinMesh* mesh, Error& error);

        VulkanRenderTarget* GetRenderTarget(VulkanFrameStage& frameStage, const Utf8String& name);
        void StartGraphicsCommands(VulkanFrameStage& frameStage, Error& error);
        void FinishGraphicsCommands(VulkanFrameStage& frameStage, Error& error);
        void StartRenderTarget(VulkanFrameStage& frameStage, VulkanRenderTarget* renderTarget, StaticVector<VulkanRenderTarget*, EngineConstants::MAX_RENDER_TARGET_DEPENDENCIES>& dependentRenderTargets, Error& error);
        void FinishRenderTarget(VulkanFrameStage& frameStage, Error& error);

        VulkanFrameBuffer& GetFrameBuffer(size_t index);

        void CreateSwapChain(Error& error);

        void UpdatedCachedWindowSize();

        void CreateSwapChainFrameBufferObjects(Error& error);
        void CreateNonSwapChainFrameBufferObjects(Error& error);

    public:
        VulkanGpuContextSettings settings;

        struct InternalSettings
        {
            InternalSettings();

            void ParseSettings(const ByteBufferReader& configFileBuffer, const Path& assetFilePath, Error& error);

            void Validate(Error& error);

            GpuInputFormatStruct* GetInputFormat(const Utf8String& typeName, const DynamicVector<FinjinVertexElementFormat>& elements);

            VulkanGpuContextImpl* contextImpl;

            DynamicVector<GpuInputFormatStruct> inputFormats;

            VulkanNamePointers extensions;
        };
        InternalSettings internalSettings;

        VulkanSystem* vulkanSystem;
        VulkanDeviceFunctions vk;

        VulkanGpuDescription physicalDeviceDescription;
        VkPhysicalDevice physicalDevice;

        VkViewport windowViewport;
        VkRect2D windowScissorRect;

        VkViewport renderViewport;
        VkRect2D renderScissorRect;

        FINJIN_LITERAL_STRING_STATIC_UNORDERED_MAP(GpuMaterialMapIndexToConstantBufferElements<VulkanMaterial::MapIndex>, VulkanMaterial::MapIndex::COUNT) materialMapTypeToElements;

        VkSurfaceKHR surface;
        VkSurfaceCapabilitiesKHR surfaceCapabilities;

        VulkanPresentModes presentModes;

        VulkanQueueFamilyIndexes queueFamilyIndexes;
        VulkanPrimaryQueues primaryQueues;

        VulkanSurfaceFormats surfaceFormats;

        VkSwapchainKHR swapChain;
        StaticVector<VulkanSwapChainFrameBuffer, VulkanSwapChainFrameBuffer::MAX_SWAP_CHAIN_IMAGES> swapChainFrameBuffers;

        VkRenderPass sceneRenderPass;
        VkRenderPass toFullScreenQuadRenderPass;

        VkSemaphore presentCompleteSemaphore;

        size_t frameBufferScreenCaptureColorFormatBytesPerPixel;
        ScreenCapturePixelFormat frameBufferScreenCapturePixelFormat;

        VulkanSimpleWork tempGraphicsWork;

        VulkanCommonResources commonResources;
        VulkanAssetResources assetResources;

        VulkanStaticMeshRenderer staticMeshRenderer;

        VulkanRenderTarget depthStencilRenderTarget;

        StaticVector<VulkanFrameBuffer, EngineConstants::MAX_FRAME_BUFFERS> frameBuffers;
        StaticVector<VulkanFrameStage, EngineConstants::MAX_FRAME_STAGES> frameStages;
    };

} }
