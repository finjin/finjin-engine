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
#include "finjin/common/Chrono.hpp"
#include "finjin/common/Error.hpp"
#include "finjin/common/FiberSpinLock.hpp"
#include "finjin/common/OperationStatus.hpp"
#include "finjin/common/SimpleSpinLockMutex.hpp"
#include "finjin/common/StaticVector.hpp"
#include "finjin/engine/Camera.hpp"
#include "finjin/engine/GpuContextCommonSettings.hpp"
#include "finjin/engine/OSWindowEventListener.hpp"
#include "finjin/engine/WindowSize.hpp"
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
#include "VulkanStaticMeshRenderer.hpp"
#include "VulkanSurfaceFormats.hpp"
#include "VulkanTexture.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    class VulkanSystem;

    class VulkanGpuContextImpl : public AllocatedClass, public OSWindowEventListener
    {
    public:
        VulkanGpuContextImpl(Allocator* allocator);

        void Create(const VulkanGpuContextSettings& settings, Error& error);
        void Destroy();

        void CreateDevice(Error& error);
        void CreateDeviceSupportObjects(Error& error);
        void CreateRenderers(Error& error);

        VulkanFrameStage& StartFrameStage(size_t index, SimpleTimeDelta elapsedTime, SimpleTimeCounter totalElapsedTime);
        void FinishBackFrameBufferRender(VulkanFrameStage& frameStage, bool continueRendering, bool modifyingRenderTarget, size_t presentSyncIntervalOverride, Error& error);
        VulkanRenderTarget* GetRenderTarget(VulkanFrameStage& frameStage, const Utf8String& name);
        void StartGraphicsCommandList(VulkanFrameStage& frameStage, Error& error);
        void FinishGraphicsCommandList(VulkanFrameStage& frameStage, Error& error);

        void Execute(VulkanFrameStage& frameStage, GpuEvents& events, GpuCommands& commands, Error& error);

        VulkanFrameBuffer& GetFrameBuffer(size_t index);

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

    private:
        VkPhysicalDevice GetPhysicalDevice();
        VkDevice GetDevice();
        VkAllocationCallbacks* GetAllocationCallbacks();
        VkQueue GetGraphicsQueue();

        void CreateSwapChain(VkCommandBuffer theGraphicsCommandBuffer, Error& error);
        void CreateDepthBuffer(VkCommandBuffer theGraphicsCommandBuffer, Error& error);

        void GetPrimaryQueues(Error& error);

        void DestroyImageView(VkImageView& view, VkImage& image, VkDeviceMemory& mem);
        void DestroyImageView(VkImageView& view, VkImage& image);
        void DestroyImageView(VkImageView& view);
        void DestroySwapChain(VkSwapchainKHR& swapChain);

        void StartRenderTarget(VulkanFrameStage& frameStage, VulkanRenderTarget* renderTarget, StaticVector<VulkanRenderTarget*, EngineConstants::MAX_RENDER_TARGET_DEPENDENCIES>& dependentRenderTargets, Error& error);
        void FinishRenderTarget(VulkanFrameStage& frameStage, Error& error);

        void UploadTexture(VkCommandBuffer commandBuffer, FinjinTexture* texture);

        void* CreateMaterial(VkCommandBuffer commandBuffer, FinjinMaterial* material, Error& error);

        void UploadMesh(VkCommandBuffer commandBuffer, FinjinMesh* mesh, Error& error);

        void UpdateCachedFrameBufferSize();

    public:
        VulkanGpuContextSettings settings;

        AssetClassFileReader settingsAssetClassFileReader;
        AssetClassFileReader shaderAssetClassFileReader;

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

        VkSurfaceKHR surface;
        VkSurfaceCapabilitiesKHR surfaceCapabilities;

        VulkanPresentModes presentModes;

        VulkanQueueFamilyIndexes queueFamilyIndexes;
        VulkanPrimaryQueues primaryQueues;

        VulkanSurfaceFormats surfaceFormats;

        VkSwapchainKHR swapChain;
        VkSemaphore presentPossibleSemaphore;

        VkRenderPass renderPass;

        struct TempWork
        {
            TempWork()
            {
                this->commandPool = VK_NULL_HANDLE;
                this->commandBuffer = VK_NULL_HANDLE;
                this->fence = VK_NULL_HANDLE;
            }

            VkCommandPool commandPool;
            VkCommandBuffer commandBuffer;
            VkFence fence;
        };
        TempWork tempGraphicsWork;

        WindowBounds renderingPixelBounds;

        ByteBuffer readBuffer;

        struct MaterialMapTypeToGpuElements
        {
            MaterialMapTypeToGpuElements() {}
            MaterialMapTypeToGpuElements(VulkanMaterial::MapIndex gpuMaterialMapIndex, GpuStructuredAndConstantBufferStructMetadata::ElementID gpuBufferTextureIndexElementID, ShaderFeatureFlag gpuMaterialMapFlag)
            {
                this->gpuMaterialMapIndex = gpuMaterialMapIndex;
                this->gpuBufferTextureIndexElementID = gpuBufferTextureIndexElementID;
                this->gpuBufferAmountElementID = GpuStructuredAndConstantBufferStructMetadata::ElementID::NONE;
                this->gpuMaterialMapFlag = gpuMaterialMapFlag;
            }
            MaterialMapTypeToGpuElements(VulkanMaterial::MapIndex gpuMaterialMapIndex, GpuStructuredAndConstantBufferStructMetadata::ElementID gpuBufferTextureIndexElementID, GpuStructuredAndConstantBufferStructMetadata::ElementID gpuBufferAmountElementID, ShaderFeatureFlag gpuMaterialMapFlag)
            {
                this->gpuMaterialMapIndex = gpuMaterialMapIndex;
                this->gpuBufferTextureIndexElementID = gpuBufferTextureIndexElementID;
                this->gpuBufferAmountElementID = gpuBufferAmountElementID;
                this->gpuMaterialMapFlag = gpuMaterialMapFlag;
            }

            VulkanMaterial::MapIndex gpuMaterialMapIndex; //Index into VulkanMaterial maps
            GpuStructuredAndConstantBufferStructMetadata::ElementID gpuBufferTextureIndexElementID; //ElementID (index) into shader constant or structured buffer for 'texture index'
            GpuStructuredAndConstantBufferStructMetadata::ElementID gpuBufferAmountElementID; //ElementID (index) into shader constant or structured buffer for 'amount'
            ShaderFeatureFlag gpuMaterialMapFlag; //Single flag identifying map's shader usage
        };

        StaticUnorderedMap<Utf8String, MaterialMapTypeToGpuElements, (size_t)VulkanMaterial::MapIndex::COUNT, FINJIN_OVERSIZE_FULL_STATIC_MAP_BUCKET_COUNT(VulkanMaterial::MapIndex::COUNT)> materialMapTypeToGpuElements;

        size_t sequenceIndex;

        size_t nextFrameBufferIndex;

        StaticVector<VulkanFrameBuffer, EngineConstants::MAX_FRAME_BUFFERS> frameBuffers;
        StaticVector<VulkanFrameStage, EngineConstants::MAX_FRAME_STAGES> frameStages;

        VulkanTextureResources textureResources;

        Camera camera;
        MathVector4 clearColor;

        VulkanRenderTarget depth;

        VulkanAssetResources assetResources;

        UsableDynamicVector<VulkanLight> lights;

        VulkanStaticMeshRenderer staticMeshRenderer;

        FiberSpinLock createLock;
    };

} }
