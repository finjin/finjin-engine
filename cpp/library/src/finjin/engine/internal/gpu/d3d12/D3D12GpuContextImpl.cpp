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


//Includes----------------------------------------------------------------------
#include "FinjinPrecompiled.hpp"

#if FINJIN_TARGET_GPU_SYSTEM == FINJIN_TARGET_GPU_SYSTEM_D3D12

#include "D3D12GpuContextImpl.hpp"
#include "finjin/common/Convert.hpp"
#include "finjin/common/DebugLog.hpp"
#include "finjin/common/PNGReader.hpp"
#include "finjin/common/WrappedFileReader.hpp"
#include "finjin/engine/DDSReader.hpp"
#include "finjin/engine/OSWindow.hpp"
#include "finjin/engine/StandardAssetDocumentPropertyValues.hpp"
#include "D3D12System.hpp"
#include "D3D12SystemImpl.hpp"
#include "D3D12Utilities.hpp"

using namespace Finjin::Engine;


//Implementation----------------------------------------------------------------

//D3D12GpuContextImpl::InternalSettings
D3D12GpuContextImpl::InternalSettings::InternalSettings()
{
    this->contextImpl = nullptr;
}

void D3D12GpuContextImpl::InternalSettings::ParseSettings(const ByteBufferReader& configFileBuffer, const Path& assetFilePath, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    ConfigDocumentReader reader;
    if (reader.Start(configFileBuffer) != nullptr)
    {
        DefaultBufferPackingRules<GpuInputFormatStruct> packingRules(1, sizeof(float), 1);

        GpuInputFormatStruct::Create
            (
            this->inputFormats,
            this->contextImpl->GetAllocator(),
            reader,
            packingRules,
            GpuInputFormatStruct::Defines::NONE,
            error
            );
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to create input formats.");
            return;
        }

        for (auto line = reader.Current(); line != nullptr; line = reader.Next())
        {
            switch (line->GetType())
            {
                case ConfigDocumentLine::Type::KEY_AND_VALUE:
                {
                    if (line->GetDepth() == 0)
                    {
                        Utf8StringView key, value;
                        line->GetKeyAndValue(key, value);

                        if (key == "max-materials")
                            this->contextImpl->settings.maxMaterials = Convert::ToInteger(value, this->contextImpl->settings.maxMaterials);
                        else if (key == "max-2d-textures")
                            this->contextImpl->settings.max2DTextures = Convert::ToInteger(value, this->contextImpl->settings.max2DTextures);
                        else if (key == "max-cube-textures")
                            this->contextImpl->settings.maxCubeTextures = Convert::ToInteger(value, this->contextImpl->settings.maxCubeTextures);
                        else if (key == "max-3d-textures")
                            this->contextImpl->settings.max3DTextures = Convert::ToInteger(value, this->contextImpl->settings.max3DTextures);
                        else if (key == "max-shaders")
                            this->contextImpl->settings.maxShaders = Convert::ToInteger(value, this->contextImpl->settings.maxShaders);
                        else if (key == "max-meshes")
                            this->contextImpl->settings.maxMeshes = Convert::ToInteger(value, this->contextImpl->settings.maxMeshes);
                        else if (key == "max-lights")
                            this->contextImpl->settings.maxLights = Convert::ToInteger(value, this->contextImpl->settings.maxLights);
                    }
                    break;
                }
                default: break;
            }
        }
    }
}

void D3D12GpuContextImpl::InternalSettings::Validate(Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    if (this->inputFormats.empty())
    {
        FINJIN_SET_ERROR(error, "No input formats were specified.");
        return;
    }

    if (this->contextImpl->settings.maxShaders == 0)
    {
        FINJIN_SET_ERROR(error, "The max shader count is zero.");
        return;
    }

    if (this->contextImpl->settings.maxMeshes == 0)
    {
        FINJIN_SET_ERROR(error, "The max mesh count is zero.");
        return;
    }

    if (this->contextImpl->settings.maxMaterials == 0)
    {
        FINJIN_SET_ERROR(error, "The max material count is zero.");
        return;
    }

    //maxTextures and maxLights are not checked since it is conceivable that they are not needed
}

GpuInputFormatStruct* D3D12GpuContextImpl::InternalSettings::GetInputFormat(const Utf8String& typeName, const DynamicVector<FinjinVertexElementFormat>& elements)
{
    if (!typeName.empty())
    {
        //Get format with matching name
        for (auto& inputFormat : this->inputFormats)
        {
            if (inputFormat.GetTypeName() == typeName)
                return &inputFormat;
        }
    }
    else
    {
        //Get format with matching elements
        auto count = elements.size();
        for (auto& inputFormat : this->inputFormats)
        {
            if (count == inputFormat.GetElementCount())
            {
                size_t i = 0;
                for (i = 0; i < count; i++)
                {
                    auto inputFormatElement = inputFormat.GetElement(i);
                    if (elements[i].id != inputFormatElement->elementID || elements[i].type != inputFormatElement->type)
                        break;
                }
                if (i == count)
                    return &inputFormat;
            }
        }
    }

    return nullptr;
}

//D3D12GpuContextImpl
D3D12GpuContextImpl::D3D12GpuContextImpl(Allocator* allocator) : 
    AllocatedClass(allocator), 
    settings(allocator), 
    clearColor(0, 0, 0, 0),
    materialMapTypeToGpuElements
        (
        StandardAssetDocumentPropertyValues::TypeName::MATERIAL_MAP_DIFFUSE, MaterialMapTypeToGpuElements(D3D12Material::MapIndex::DIFFUSE, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_DIFFUSE_TEXTURE_INDEX, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_DIFFUSE_AMOUNT, ShaderFeatureFlag::MAP_DIFFUSE),
        StandardAssetDocumentPropertyValues::TypeName::MATERIAL_MAP_SPECULAR, MaterialMapTypeToGpuElements(D3D12Material::MapIndex::SPECULAR, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_SPECULAR_TEXTURE_INDEX, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_SPECULAR_AMOUNT, ShaderFeatureFlag::MAP_SPECULAR),
        StandardAssetDocumentPropertyValues::TypeName::MATERIAL_MAP_REFLECTION, MaterialMapTypeToGpuElements(D3D12Material::MapIndex::REFLECTION, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_REFLECTION_TEXTURE_INDEX, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_REFLECTION_AMOUNT, ShaderFeatureFlag::MAP_REFLECTION),
        StandardAssetDocumentPropertyValues::TypeName::MATERIAL_MAP_REFRACTION, MaterialMapTypeToGpuElements(D3D12Material::MapIndex::REFRACTION, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_REFRACTION_TEXTURE_INDEX, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_REFRACTION_AMOUNT, ShaderFeatureFlag::MAP_REFRACTION),
        StandardAssetDocumentPropertyValues::TypeName::MATERIAL_MAP_BUMP, MaterialMapTypeToGpuElements(D3D12Material::MapIndex::BUMP, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_BUMP_TEXTURE_INDEX, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_BUMP_AMOUNT, ShaderFeatureFlag::MAP_BUMP),
        StandardAssetDocumentPropertyValues::TypeName::MATERIAL_MAP_HEIGHT, MaterialMapTypeToGpuElements(D3D12Material::MapIndex::HEIGHT, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_HEIGHT_TEXTURE_INDEX, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_HEIGHT_AMOUNT, ShaderFeatureFlag::MAP_HEIGHT),
        StandardAssetDocumentPropertyValues::TypeName::MATERIAL_MAP_SELF_ILLUMINATION, MaterialMapTypeToGpuElements(D3D12Material::MapIndex::SELF_ILLUMINATION, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_SELF_ILLUMINATION_TEXTURE_INDEX, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_SELF_ILLUMINATION_AMOUNT, ShaderFeatureFlag::MAP_SELF_ILLUMINATION),
        StandardAssetDocumentPropertyValues::TypeName::MATERIAL_MAP_OPACITY, MaterialMapTypeToGpuElements(D3D12Material::MapIndex::OPACITY, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_OPACITY_TEXTURE_INDEX, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_OPACITY_AMOUNT, ShaderFeatureFlag::MAP_OPACITY),
        StandardAssetDocumentPropertyValues::TypeName::MATERIAL_MAP_ENVIRONMENT, MaterialMapTypeToGpuElements(D3D12Material::MapIndex::ENVIRONMENT, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_ENVIRONMENT_TEXTURE_INDEX, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_ENVIRONMENT_AMOUNT, ShaderFeatureFlag::MAP_ENVIRONMENT),
        StandardAssetDocumentPropertyValues::TypeName::MATERIAL_MAP_SHININESS, MaterialMapTypeToGpuElements(D3D12Material::MapIndex::SHININESS, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_SHININESS_TEXTURE_INDEX, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_SHININESS_AMOUNT, ShaderFeatureFlag::MAP_SHININESS)
        )
{
    this->d3dSystem = nullptr;

    this->sequenceIndex = 0;

    this->nextFrameBufferIndex = 0;

    this->fenceEventHandle = nullptr;
    this->fenceValue = 0;

    FINJIN_ZERO_ITEM(this->viewport);
    FINJIN_ZERO_ITEM(this->scissorRect);
}

void D3D12GpuContextImpl::CreateDevice(Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto systemImpl = this->d3dSystem->GetImpl();

    if (this->settings.useSoftwareGpu)
    {
        if (systemImpl->softwareGpuDescriptions.empty())
        {
            FINJIN_SET_ERROR(error, "No software adapters are available.");
            return;
        }

        Microsoft::WRL::ComPtr<IDXGIAdapter> warpAdapter;
        if (FINJIN_CHECK_HRESULT_FAILED(systemImpl->dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter))))
        {
            FINJIN_SET_ERROR(error, "Failed to enumerate software adapter.");
            return;
        }

        if (FINJIN_CHECK_HRESULT_FAILED(D3D12CreateDevice(warpAdapter.Get(), systemImpl->settings.d3dMinimumFeatureLevel, IID_PPV_ARGS(&this->device))))
        {
            FINJIN_SET_ERROR(error, "Failed to create software adapter D3D device.");
            return;
        }

        this->desc = systemImpl->softwareGpuDescriptions[0];
    }
    else
    {
        Microsoft::WRL::ComPtr<IDXGIAdapter1> hardwareAdapter;
        systemImpl->GetHardwareD3DAdapter(this->settings.gpuID, &hardwareAdapter, this->desc, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to get hardware D3D adapter.");
            return;
        }

        if (hardwareAdapter == nullptr && !this->settings.gpuID.IsZero())
        {
            //No error, but adapter could not be found. Drop back to default
            FINJIN_ZERO_ITEM(this->settings.gpuID);

            systemImpl->GetHardwareD3DAdapter(this->settings.gpuID, &hardwareAdapter, this->desc, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to get hardware D3D adapter.");
                return;
            }
        }

        if (FINJIN_CHECK_HRESULT_FAILED(D3D12CreateDevice(hardwareAdapter.Get(), systemImpl->settings.d3dMinimumFeatureLevel, IID_PPV_ARGS(&this->device))))
        {
            FINJIN_SET_ERROR(error, "Failed to create hardware adapter D3D device.");
            return;
        }
    }

    //Determine some settings-------------------------------------------
    UpdateCachedFrameBufferSize();

    this->settings.frameBufferCount.actual = this->settings.frameBufferCount.requested;
    this->frameBuffers.resize(this->settings.frameBufferCount.actual);
    for (size_t frameBufferIndex = 0; frameBufferIndex < this->frameBuffers.size(); frameBufferIndex++)
    {
        auto& frameBuffer = this->frameBuffers[frameBufferIndex];
        frameBuffer.renderTarget.renderTargetOutputResources.push_back(nullptr); //Swap chain render targets only have a single output
        frameBuffer.SetIndex(frameBufferIndex);
    }

    this->settings.jobProcessingPipelineSize = GpuContextCommonSettings::CalculateJobPipelineSize(this->settings.frameBufferCount.actual);
    this->frameStages.resize(this->settings.jobProcessingPipelineSize);
    for (size_t frameStageIndex = 0; frameStageIndex < this->frameStages.size(); frameStageIndex++)
        this->frameStages[frameStageIndex].index = frameStageIndex;

    if (this->settings.colorFormat.actual == DXGI_FORMAT_UNKNOWN)
    {
        if (this->settings.colorFormat.requested == DXGI_FORMAT_UNKNOWN)
            this->settings.colorFormat.actual = DXGI_FORMAT_R8G8B8A8_UNORM;
        else
            this->settings.colorFormat.actual = this->settings.colorFormat.requested;
    }

    //DXGI_FORMAT_D32_FLOAT_S8X24_UINT, DXGI_FORMAT_D32_FLOAT, DXGI_FORMAT_D24_UNORM_S8_UINT, DXGI_FORMAT_D16_UNORM
    if (this->settings.depthStencilFormat.actual == DXGI_FORMAT_UNKNOWN)
    {
        if (this->settings.depthStencilFormat.requested == DXGI_FORMAT_UNKNOWN)
        {
            if (this->settings.stencilRequired)
                this->settings.depthStencilFormat.actual = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
            else
                this->settings.depthStencilFormat.actual = DXGI_FORMAT_D32_FLOAT;
        }
        else
            this->settings.depthStencilFormat.actual = this->settings.depthStencilFormat.requested;
    }

    //Multisample quality levels
    D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS qualityLevels;
    qualityLevels.Format = this->settings.colorFormat.actual;
    qualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
    for (qualityLevels.SampleCount = 1;
        qualityLevels.SampleCount <= D3D12_MAX_MULTISAMPLE_SAMPLE_COUNT &&
        !this->desc.features.multisampleQualityLevels.full(); qualityLevels.SampleCount++)
    {
        if (SUCCEEDED(this->device->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &qualityLevels, sizeof(qualityLevels))))
        {
            if (qualityLevels.NumQualityLevels > 0)
                this->desc.features.multisampleQualityLevels.push_back(qualityLevels);
        }
    }

    //Update 'actual' settings by looking for an exact match
    for (auto& level : this->desc.features.multisampleQualityLevels)
    {
        if (level.SampleCount == settings.multisampleCount.requested && (level.NumQualityLevels - 1) == settings.multisampleQuality.requested)
        {
            settings.multisampleCount.actual = settings.multisampleCount.requested;
            settings.multisampleQuality.actual = settings.multisampleQuality.requested;
            break;
        }
    }
}

void D3D12GpuContextImpl::CreateDeviceSupportObjects(Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    //Set more selector settings
    Utf8String shaderModelString;
    if (D3D12Utilities::ShaderModelToString(shaderModelString, this->desc.features.maxShaderModel).HasError())
    {
        FINJIN_SET_ERROR(error, "Failed to convert shader model to string.");
        return;
    }
    this->settings.initialAssetFileSelector.Set(AssetPathComponent::GPU_SHADER_MODEL, shaderModelString);

    Utf8String featureLevelString;
    if (D3D12Utilities::FeatureLevelToString(featureLevelString, this->desc.features.standardFeatures.featureLevel).HasError())
    {
        FINJIN_SET_ERROR(error, "Failed to convert feature level to string.");
        return;
    }
    this->settings.initialAssetFileSelector.Set(AssetPathComponent::GPU_FEATURE_LEVEL, featureLevelString);

    if (this->desc.features.standardFeatures.bc6bc7Compression && this->desc.features.standardFeatures.bc4bc5Compression)
        this->settings.initialAssetFileSelector.Set(AssetPathComponent::GPU_PREFERRED_TEXTURE_FORMAT, "bc");

    this->settings.initialAssetFileSelector.Set(AssetPathComponent::GPU_MODEL, this->desc.desc.Description);

    //Set up asset readers
    this->settingsAssetClassFileReader.Create(*this->settings.assetFileReader, this->settings.initialAssetFileSelector, AssetClass::SETTINGS, GetAllocator(), error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to create settings asset file reader.");
        return;
    }

    this->shaderAssetClassFileReader.Create(*this->settings.assetFileReader, this->settings.initialAssetFileSelector, AssetClass::SHADER, GetAllocator(), error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to create shader asset file reader.");
        return;
    }

    //Read internal settings
    this->internalSettings.contextImpl = this;
    this->settingsAssetClassFileReader.ReadAndParseSettingsFiles(this->internalSettings, this->readBuffer, this->settings.contextSettingsFileNames, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to read internal settings.");
        return;
    }
    this->internalSettings.Validate(error);
    if (error)
    {
        FINJIN_SET_ERROR_NO_MESSAGE(error);
        return;
    }

    D3D12InputFormat::CreateUnorderedMap(this->assetResources.inputFormatsByNameHash, GetAllocator(), this->internalSettings.inputFormats, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to create input formats.");
        return;
    }

    auto maxTextures = this->settings.max2DTextures + this->settings.maxCubeTextures + this->settings.max3DTextures;
    if (!this->assetResources.texturesByNameHash.Create(maxTextures, maxTextures, GetAllocator(), GetAllocator()))
    {
        FINJIN_SET_ERROR(error, "Failed to create textures lookup.");
        return;
    }

    for (auto& shadersByNameHash : this->assetResources.shadersByShaderTypeAndNameHash)
    {
        if (!shadersByNameHash.Create(this->settings.maxShaders, this->settings.maxShaders, GetAllocator(), GetAllocator()))
        {
            FINJIN_SET_ERROR(error, "Failed to create shaders lookup.");
            return;
        }
    }

    if (!this->assetResources.materialsByNameHash.Create(this->settings.maxMaterials, this->settings.maxMaterials, GetAllocator(), GetAllocator()))
    {
        FINJIN_SET_ERROR(error, "Failed to create materials lookup.");
        return;
    }

    if (!this->assetResources.meshesByNameHash.Create(this->settings.maxMeshes, this->settings.maxMeshes, GetAllocator(), GetAllocator()))
    {
        FINJIN_SET_ERROR(error, "Failed to allocate mesh lookup.");
        return;
    }

    if (!this->lights.Create(this->settings.maxLights, GetAllocator(), GetAllocator()))
    {
        FINJIN_SET_ERROR(error, "Failed to allocate light lookup.");
        return;
    }
    for (size_t lightIndex = 0; lightIndex < this->lights.items.size(); lightIndex++)
        this->lights.items[lightIndex].gpuBufferIndex = lightIndex;

    CreateGraphicsCommandQueue(error);
    if (error)
    {
        FINJIN_SET_ERROR_NO_MESSAGE(error);
        return;
    }

    CreateSwapChain(error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to create swap chain.");
        return;
    }

    //RTV descriptor heap for swap chain views
    {
        D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
        rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        rtvHeapDesc.NumDescriptors = static_cast<UINT>(this->frameBuffers.size());
        rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        this->swapChainRtvDescHeap.Create(this->device.Get(), rtvHeapDesc, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to create RTV descriptor heap.");
            return;
        }
    }

    //DSV descriptor heap for back buffer depth buffer view
    {
        D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
        dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
        dsvHeapDesc.NumDescriptors = 1;
        dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        this->swapChainDsvDescHeap.Create(this->device.Get(), dsvHeapDesc, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to create depth stencil descriptor heap.");
            return;
        }
    }

    //SRV heap for textures
    {
        size_t textureOffset = 0;

        this->assetResources.textureOffsetsByDimension[TextureDimension::DIMENSION_2D].SetOffsetAndCount(textureOffset, this->settings.max2DTextures);
        textureOffset += this->settings.max2DTextures;

        this->assetResources.textureOffsetsByDimension[TextureDimension::DIMENSION_CUBE].SetOffsetAndCount(textureOffset, this->settings.maxCubeTextures);
        textureOffset += this->settings.maxCubeTextures;

        this->assetResources.textureOffsetsByDimension[TextureDimension::DIMENSION_3D].SetOffsetAndCount(textureOffset, this->settings.max3DTextures);
        textureOffset += this->settings.max3DTextures;

        D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
        srvHeapDesc.NumDescriptors = static_cast<UINT>(textureOffset);
        srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        this->textureResources.srvTextureDescHeap.Create(this->device.Get(), srvHeapDesc, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to create texture SRV heap.");
            return;
        }
    }

    //Per-frame objects
    for (auto& frameBuffer : this->frameBuffers)
    {
        if (FINJIN_CHECK_HRESULT_FAILED(this->device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&frameBuffer.commandAllocator))))
        {
            FINJIN_SET_ERROR(error, "Failed to create D3D12 job pipeline frameStage command allocator.");
            return;
        }
        FINJIN_D3D12_SET_RESOURCE_NAME(frameBuffer.commandAllocator, Utf8StringFormatter::Format("D3D12 frame buffer %1% graphics command allocator", frameBuffer.index));

        if (!frameBuffer.graphicsCommandLists.Create(this->settings.maxGpuCommandListsPerStage, GetAllocator(), nullptr))
        {
            FINJIN_SET_ERROR(error, "Failed to create command lists.");
            return;
        }
        for (size_t i = 0; i < frameBuffer.graphicsCommandLists.size(); i++)
        {
            auto& commandList = frameBuffer.graphicsCommandLists[i];
            if (FINJIN_CHECK_HRESULT_FAILED(this->device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, frameBuffer.commandAllocator.Get(), nullptr, IID_PPV_ARGS(&commandList))))
            {
                FINJIN_SET_ERROR(error, "Failed to create D3D12 job pipeline frameStage command list.");
                return;
            }
            FINJIN_D3D12_SET_RESOURCE_NAME(frameBuffer.commandAllocator, Utf8StringFormatter::Format("D3D12 frame buffer %1% graphics command list", frameBuffer.index));

            commandList->Close();
        }
        frameBuffer.ResetCommandLists();

        if (!frameBuffer.commandListsToExecute.CreateEmpty(this->settings.maxGpuCommandListsPerStage, GetAllocator()))
        {
            FINJIN_SET_ERROR(error, "Failed to create graphics command lists lookup.");
            return;
        }
    }

    //Synchronization objects
    {
        if (FINJIN_CHECK_HRESULT_FAILED(this->device->CreateFence(this->fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&this->fence))))
        {
            FINJIN_SET_ERROR(error, "Failed to create fence.");
            return;
        }
        this->fenceValue++;

        //Create an event handle to use for flushing the GPU
        this->fenceEventHandle = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        if (this->fenceEventHandle == nullptr)
        {
            FINJIN_SET_ERROR(error, "Failed to create fence event handle.");
            return;
        }
    }

    //Allocate some working buffers
    //TODO: Going forward it would make sense to declare in the application settings the maximum allowed sizes, rather than use the absolute maximums for 'arrayCount' and 3D texture dimension/depth
    auto mipCount = RoundToInt(log2(this->desc.features.standardFeatures.maxTextureDimension)) + 1; //Less or equal to D3D12_REQ_MIP_LEVELS
    auto arrayCount = 1; //std::max(D3D12_REQ_TEXTURE1D_ARRAY_AXIS_DIMENSION, D3D12_REQ_TEXTURE2D_ARRAY_AXIS_DIMENSION);
    UINT predictedSubresourceCount = mipCount * arrayCount;
    if (this->settings.max3DTextures > 0)
        predictedSubresourceCount *= D3D12_REQ_TEXTURECUBE_DIMENSION; //Depth
    if (!this->preallocatedSubresourceData.Create(predictedSubresourceCount, GetAllocator()))
    {
        FINJIN_SET_ERROR(error, "Failed to create subresource data table.");
        return;
    }

    if (!this->preallocatedFootprintSubresourceData.Create((sizeof(D3D12_PLACED_SUBRESOURCE_FOOTPRINT) + sizeof(UINT) + sizeof(uint64_t)) * predictedSubresourceCount, GetAllocator()))
    {
        FINJIN_SET_ERROR(error, "Failed to create footprint subresource data table.");
        return;
    }

    CreateScreenSizeDependentResources(error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to create GPU core assets.");
        return;
    }
}

void D3D12GpuContextImpl::CreateRenderers(Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    //Static mesh renderer
    {
        //Settings
        D3D12StaticMeshRenderer::Settings staticMeshRendererSettings;
        staticMeshRendererSettings.contextImpl = this;
        for (auto fileName : { "gpu-context-static-mesh-renderer-root-signatures.cfg", "gpu-context-static-mesh-renderer-render-states.cfg" })
        {
            this->settings.staticMeshRendererSettingsFileNames.AddLocalFile(fileName, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to add '%1%' GPU context static mesh renderer settings file.", fileName));
                return;
            }
        }
        this->settingsAssetClassFileReader.ReadAndParseSettingsFiles(staticMeshRendererSettings, this->readBuffer, this->settings.staticMeshRendererSettingsFileNames, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to read static mesh renderer settings.");
            return;
        }

        //Renderer
        this->staticMeshRenderer.Create(std::move(staticMeshRendererSettings), error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to create static mesh renderer.");
            return;
        }
    }
}

void D3D12GpuContextImpl::Create(const D3D12GpuContextSettings& settings, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    assert(settings.osWindow != nullptr);
    assert(settings.assetFileReader != nullptr);

    //Copy settings
    this->settings = settings;

    if (!this->readBuffer.Create(this->settings.tempReadBufferSize, GetAllocator()))
    {
        FINJIN_SET_ERROR(error, "Failed to create read buffer.");
        return;
    }

    //Create device
    CreateDevice(error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to find a suitable D3D12 device.");
        return;
    }

    //Create other objects
    CreateDeviceSupportObjects(error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to create support objects.");
        return;
    }

    //Renderers
    CreateRenderers(error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to create renderers.");
        return;
    }

    this->settings.osWindow->AddWindowEventListener(this);
}

void D3D12GpuContextImpl::Destroy()
{
    //Wait for the device to go idle
    if (this->device != nullptr && this->fenceEventHandle != nullptr)
    {
        auto value = EmitFenceValue();
        WaitForFenceValue(value);
    }

    this->settings.osWindow->RemoveWindowEventListener(this);

#if !FINJIN_TARGET_PLATFORM_IS_WINDOWS_UWP
    //Full screen state should always be false before shutting down
    if (this->swapChain != nullptr)
    {
        auto result = this->swapChain->SetFullscreenState(FALSE, nullptr);
        if (FINJIN_CHECK_HRESULT_FAILED(result))
            FINJIN_DEBUG_LOG_ERROR("D3D12GpuContextImpl::Destroy(): swapChain->SetFullscreenState() returned a failure code.");
    }
#endif

    if (this->fenceEventHandle != nullptr)
    {
        CloseHandle(this->fenceEventHandle);
        this->fenceEventHandle = nullptr;
    }

    this->staticMeshRenderer.Destroy();

    this->assetResources.Destroy();

    //The remaining resources are released by their smart pointers after this D3D12GpuContextImpl is destroyed
}

void D3D12GpuContextImpl::GetBestFullScreenDisplayMode(D3D12DisplayMode& bestDisplayMode, size_t width, size_t height, D3D12RefreshRate refresh, HMONITOR theMonitor, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto systemImpl = this->d3dSystem->GetImpl();

    auto adapterOutputs = systemImpl->GetAdapterOutputs(this->settings.gpuID);
    assert(adapterOutputs != nullptr); //Based on how CreateDevice() works this should not happen!
    if (adapterOutputs == nullptr)
    {
        FINJIN_SET_ERROR(error, "Failed to get adapter outputs for selected adapter.");
        return;
    }

    OSWindowRect fullScreenRect;
    D3D12GpuOutput fullScreenAdapterOutput;
    for (const auto& output : *adapterOutputs)
    {
        if (theMonitor == nullptr || output.desc.AttachedToDesktop && output.desc.Monitor == theMonitor)
        {
            auto width = static_cast<OSWindowDimension>(output.desc.DesktopCoordinates.right - output.desc.DesktopCoordinates.left);
            auto height = static_cast<OSWindowDimension>(output.desc.DesktopCoordinates.bottom - output.desc.DesktopCoordinates.top);
            fullScreenRect = OSWindowRect(static_cast<OSWindowCoordinate>(output.desc.DesktopCoordinates.left), static_cast<OSWindowCoordinate>(output.desc.DesktopCoordinates.top), width, height);
            fullScreenAdapterOutput = output;
            break;
        }
    }

    D3D12DisplayMode desiredDisplayMode(width, height, this->settings.colorFormat.actual, refresh);
    D3D12DisplayMode defaultDisplayMode(fullScreenRect.width, fullScreenRect.height, this->settings.colorFormat.actual, D3D12RefreshRate(60, 1)); //This seems like a reasonable refresh
    if (!fullScreenRect.IsEmpty())
    {
        if (fullScreenAdapterOutput.FindBestDisplayMode(bestDisplayMode, desiredDisplayMode))
        {
            //Found original or fallback
        }
        else
        {
            //Failed to find a match
            if (fullScreenAdapterOutput.FindBestDisplayMode(bestDisplayMode, defaultDisplayMode))
            {
                //Found default or fallback
            }
            else
            {
                //Failed
                FINJIN_SET_ERROR(error, "Failed to find best resolution and/or refresh rate.");
                return;
            }
        }


        //std::cout << "Best mode: " << bestDisplayMode.width << "x" << bestDisplayMode.height << "@" << bestDisplayMode.refresh.numerator << "/" << bestDisplayMode.refresh.denominator << std::endl;
    }
}

bool D3D12GpuContextImpl::ToggleFullScreenExclusive(Error& error)
{
    FINJIN_DEBUG_LOG_INFO("D3D12GpuContextImpl::ToggleFullScreenExclusive");

    FINJIN_ERROR_METHOD_START(error);

#if !FINJIN_TARGET_PLATFORM_IS_WINDOWS_UWP
    BOOL fullscreenState;
    if (FINJIN_CHECK_HRESULT_FAILED(this->swapChain->GetFullscreenState(&fullscreenState, nullptr)))
    {
        FINJIN_SET_ERROR(error, "Failed to set get full screen state in swap chain.");
        return true;
    }

    auto newFullscreenState = !fullscreenState;
    if (FINJIN_CHECK_HRESULT_FAILED(this->swapChain->SetFullscreenState(newFullscreenState, nullptr)))
    {
        FINJIN_SET_ERROR(error, "Failed to set full screen state in swap chain.");
        return true;
    }

    return true;
#else
    return false;
#endif
}

bool D3D12GpuContextImpl::StartResizeTargets(bool minimized, Error& error)
{
    FINJIN_DEBUG_LOG_INFO("D3D12GpuContextImpl::StartResizeTargets");

    FINJIN_ERROR_METHOD_START(error);

    auto callerNeedsToFinishResize = false;

    //Release the resources holding references to the swap chain
    DestroyScreenSizeDependentResources(true);

    if (this->settings.osWindow->GetWindowSize().IsFullScreenExclusive())
    {
        FINJIN_DEBUG_LOG_INFO("  FullScreenExclusive");

        callerNeedsToFinishResize = true;

        //Set up mode description
        DXGI_MODE_DESC modeDesc = {};
        modeDesc.Width = static_cast<UINT>(this->renderingPixelBounds.GetClientWidth());
        modeDesc.Height = static_cast<UINT>(this->renderingPixelBounds.GetClientHeight());
        //modeDesc.Format = this->settings.colorFormat; //NOTE: These couple of lines do not work! Will cause the driver to fall back to some weird mode
        //modeDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
        //modeDesc.Scaling = DXGI_MODE_SCALING_CENTERED;

        D3D12DisplayMode bestDisplayMode;
        GetBestFullScreenDisplayMode(bestDisplayMode, modeDesc.Width, modeDesc.Height, D3D12RefreshRate::GetDefault(), (HMONITOR)this->settings.osWindow->GetMonitorHandle(), error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to get best full screen display mode.");
            return false;
        }

        modeDesc.RefreshRate.Numerator = static_cast<UINT>(bestDisplayMode.refresh.numerator);
        modeDesc.RefreshRate.Denominator = static_cast<UINT>(bestDisplayMode.refresh.denominator);

        //Resize target
        auto resizeTargetResult = this->swapChain->ResizeTarget(&modeDesc); //Triggers a window size message to be processed later
        if (FINJIN_CHECK_HRESULT_FAILED(resizeTargetResult) && resizeTargetResult != DXGI_ERROR_NOT_CURRENTLY_AVAILABLE) //DXGI_ERROR_NOT_CURRENTLY_AVAILABLE occurs on Windows Store apps
        {
            FINJIN_SET_ERROR(error, "Failed to resize swap chain target.");
            return false;
        }
    }
    else
    {
        FINJIN_DEBUG_LOG_INFO("  Finishing resize targets");

        FinishResizeTargets(error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to finish resizing swap chain target.");
            return false;
        }
    }

    return callerNeedsToFinishResize;
}

void D3D12GpuContextImpl::FinishResizeTargets(Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto resizeWidth = static_cast<UINT>(this->renderingPixelBounds.GetClientWidth());
    auto resizeHeight = static_cast<UINT>(this->renderingPixelBounds.GetClientHeight());

    FINJIN_DEBUG_LOG_INFO("D3D12GpuContextImpl::FinishResizeTargets: %1% x %2%", resizeWidth, resizeHeight);

    //Resize swap chain buffers
    DXGI_SWAP_CHAIN_DESC swapDesc = {};
    this->swapChain->GetDesc(&swapDesc);
    if (FINJIN_CHECK_HRESULT_FAILED(this->swapChain->ResizeBuffers(static_cast<UINT>(this->frameBuffers.size()), resizeWidth, resizeHeight, swapDesc.BufferDesc.Format, swapDesc.Flags)))
    {
        FINJIN_SET_ERROR(error, "Failed to resize swap chain buffers.");
        return;
    }

    //Resize resources
    CreateScreenSizeDependentResources(error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to create screen size dependent resources.");
        return;
    }

    this->nextFrameBufferIndex = this->swapChain->GetCurrentBackBufferIndex();
}

uint64_t D3D12GpuContextImpl::FlushGpu()
{
    auto v = EmitFenceValue();
    WaitForFenceValue(v);
    return v;
}

uint64_t D3D12GpuContextImpl::EmitFenceValue()
{
    auto emittedValue = this->fenceValue++;
    if (FINJIN_CHECK_HRESULT_FAILED(this->graphicsCommandQueue->Signal(this->fence.Get(), emittedValue)))
    {
    }

    return emittedValue;
}

void D3D12GpuContextImpl::WaitForFenceValue(uint64_t v)
{
    const uint64_t lastCompletedFence = this->fence->GetCompletedValue();
    if (lastCompletedFence < v)
    {
        if (FINJIN_CHECK_HRESULT_FAILED(this->fence->SetEventOnCompletion(v, this->fenceEventHandle)))
        {
            return;
        }

        WaitForSingleObjectEx(this->fenceEventHandle, INFINITE, FALSE);
    }
}

size_t D3D12GpuContextImpl::BusyWaitForFenceValue(uint64_t v)
{
    size_t count = 0;
    while (this->fence->GetCompletedValue() < v)
        count++;
    return count;
}

WindowBounds D3D12GpuContextImpl::GetRenderingPixelBounds()
{
    return this->renderingPixelBounds;
}

void D3D12GpuContextImpl::WindowResized(OSWindow* osWindow)
{
    UpdateCachedFrameBufferSize();

    //FINJIN_DEBUG_LOG_INFO("New resized size: %1% x %2%", this->renderingPixelBounds.GetClientWidth(), this->renderingPixelBounds.GetClientHeight());
}

void D3D12GpuContextImpl::CreateGraphicsCommandQueue(Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    D3D12_COMMAND_QUEUE_DESC graphicsQueueDescription = {};
    graphicsQueueDescription.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    graphicsQueueDescription.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    if (FINJIN_CHECK_HRESULT_FAILED(this->device->CreateCommandQueue(&graphicsQueueDescription, IID_PPV_ARGS(&this->graphicsCommandQueue))))
    {
        FINJIN_SET_ERROR(error, "Failed to create graphics command queue.");
        return;
    }
}

void D3D12GpuContextImpl::CreateSwapChain(Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto systemImpl = this->d3dSystem->GetImpl();

    UINT bestWidth = 0;
    UINT bestHeight = 0;
    UINT bestRefreshRateNumerator = 0;
    UINT bestRefreshRateDenominator = 0;

    if (!this->settings.useSoftwareGpu && this->settings.osWindow->GetWindowSize().IsFullScreenExclusive())
    {
        D3D12DisplayMode bestDisplayMode;
        GetBestFullScreenDisplayMode
            (
            bestDisplayMode,
            static_cast<size_t>(this->renderingPixelBounds.GetClientWidth()),
            static_cast<size_t>(this->renderingPixelBounds.GetClientHeight()),
            D3D12RefreshRate::GetDefault(),
            (HMONITOR)this->settings.osWindow->GetMonitorHandle(),
            error
            );
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to get best full screen display mode.");
            return;
        }

        bestWidth = static_cast<UINT>(bestDisplayMode.width);
        bestHeight = static_cast<UINT>(bestDisplayMode.height);
        bestRefreshRateNumerator = static_cast<UINT>(bestDisplayMode.refresh.numerator);
        bestRefreshRateDenominator = static_cast<UINT>(bestDisplayMode.refresh.denominator);
    }
    else
    {
        bestWidth = static_cast<UINT>(this->renderingPixelBounds.GetClientWidth());
        bestHeight = static_cast<UINT>(this->renderingPixelBounds.GetClientHeight());
    }

#if FINJIN_TARGET_PLATFORM_IS_WINDOWS_UWP
    DXGI_SWAP_CHAIN_DESC1 swapChainDescription = {};
    swapChainDescription.Width = bestWidth; //Match the size of the window.
    swapChainDescription.Height = bestHeight;
    swapChainDescription.Format = this->settings.colorFormat.actual;
    swapChainDescription.SampleDesc.Count = this->settings.multisampleCount.actual;
    swapChainDescription.SampleDesc.Quality = this->settings.multisampleQuality.actual;
    swapChainDescription.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDescription.BufferCount = static_cast<UINT>(this->frameBuffers.size());
    swapChainDescription.Scaling = DXGI_SCALING_STRETCH;//DisplayMetrics::SupportHighResolutions ? DXGI_SCALING_NONE : DXGI_SCALING_STRETCH;
    swapChainDescription.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDescription.AlphaMode = DXGI_ALPHA_MODE_IGNORE;

    Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain;
    if (FINJIN_CHECK_HRESULT_FAILED(systemImpl->dxgiFactory->CreateSwapChainForCoreWindow(this->graphicsCommandQueue.Get(), static_cast<IUnknown*>(this->settings.osWindow->GetWindowHandle()), &swapChainDescription, nullptr, &swapChain)))
#else
    DXGI_SWAP_CHAIN_DESC swapChainDescription = {};
    swapChainDescription.BufferCount = static_cast<UINT>(this->frameBuffers.size());
    swapChainDescription.BufferDesc.Width = bestWidth;
    swapChainDescription.BufferDesc.Height = bestHeight;
    swapChainDescription.BufferDesc.RefreshRate.Numerator = bestRefreshRateNumerator;
    swapChainDescription.BufferDesc.RefreshRate.Denominator = bestRefreshRateDenominator;
    swapChainDescription.BufferDesc.Format = this->settings.colorFormat.actual;
    swapChainDescription.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDescription.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDescription.OutputWindow = static_cast<HWND>(this->settings.osWindow->GetWindowHandle());
    swapChainDescription.SampleDesc.Count = this->settings.multisampleCount.actual;
    swapChainDescription.SampleDesc.Quality = this->settings.multisampleQuality.actual;
    swapChainDescription.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    swapChainDescription.Windowed = TRUE;

    Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;
    if (FINJIN_CHECK_HRESULT_FAILED(systemImpl->dxgiFactory->CreateSwapChain(this->graphicsCommandQueue.Get(), &swapChainDescription, &swapChain)))
#endif
    {
        FINJIN_SET_ERROR(error, "Failed to create swap chain.");
        return;
    }
    if (FINJIN_CHECK_HRESULT_FAILED(swapChain.As(&this->swapChain)))
    {
        FINJIN_SET_ERROR(error, "Failed to get swap chain pointer.");
        return;
    }

#if !FINJIN_TARGET_PLATFORM_IS_WINDOWS_UWP
    if (FINJIN_CHECK_HRESULT_FAILED(systemImpl->dxgiFactory->MakeWindowAssociation(static_cast<HWND>(this->settings.osWindow->GetWindowHandle()), DXGI_MWA_NO_ALT_ENTER | DXGI_MWA_NO_WINDOW_CHANGES)))
    {
        FINJIN_SET_ERROR(error, "Failed to set alt+enter window behavior.");
        return;
    }
#endif
}

bool D3D12GpuContextImpl::ResolveMeshRef(FinjinMesh& mesh, const AssetReference& assetRef)
{
    if (mesh.gpuMesh == nullptr)
        return false;

    auto d3d12Mesh = static_cast<D3D12Mesh*>(mesh.gpuMesh);
    return d3d12Mesh->IsResidentOnGpu();
}

bool D3D12GpuContextImpl::ResolveMaterialRef(FinjinMaterial& material, const AssetReference& assetRef)
{
    return material.gpuMaterial != nullptr;
}

bool D3D12GpuContextImpl::ResolveMaterialMaps(FinjinMaterial& material)
{
    auto d3d12Material = static_cast<D3D12Material*>(material.gpuMaterial);

    if (!d3d12Material->isFullyResolved)
    {
        auto isFullyResolved = true; //Assume resolution will succeed
        for (auto& map : d3d12Material->finjinMaterial->maps)
        {
            if (map.gpuMaterialMapIndex != (size_t)-1)
            {
                auto& d3d12Map = d3d12Material->maps[map.gpuMaterialMapIndex];
                if (d3d12Map.texture == nullptr)
                {
                    d3d12Map.texture = this->assetResources.GetTextureByName(map.textureHandle.assetRef.objectName);
                    if (d3d12Map.texture == nullptr)
                        isFullyResolved = false; //Failed to find the texture
                }
            }
        }
        d3d12Material->isFullyResolved = isFullyResolved;
    }

    return d3d12Material->isFullyResolved;
}

D3D12Texture* D3D12GpuContextImpl::CreateTextureFromMainThread(FinjinTexture* texture, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    std::unique_lock<FiberSpinLock> thisLock(this->createLock);

    this->assetResources.ValidateTextureForCreation(texture->name, error);
    if (error)
    {
        FINJIN_SET_ERROR_NO_MESSAGE(error);
        return nullptr;
    }

    ByteBufferReader reader(texture->fileBytes);

    auto isDDS = false;
    auto isPNG = false;
    auto foundWrappedFormat = false;

    WrappedFileReader wrappedFileReader;
    auto wrappedReadHeaderResult = wrappedFileReader.ReadHeader(reader);
    if (wrappedReadHeaderResult == WrappedFileReader::ReadHeaderResult::SUCCESS)
    {
        if (wrappedFileReader.GetHeader().fileExtension == "dds")
            isDDS = foundWrappedFormat = true;
        else if (wrappedFileReader.GetHeader().fileExtension == "png")
            isPNG = foundWrappedFormat = true;
        else
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Invalid texture format '%1%'.", wrappedFileReader.GetHeader().fileExtension));
            return nullptr;
        }
    }
    else if (wrappedReadHeaderResult == WrappedFileReader::ReadHeaderResult::INVALID_SIGNATURE_VALUE)
    {
        //Not a wrapped file
    }
    else
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Unexpected failure while reading file: %1%", wrappedFileReader.GetReadHeaderResultString(wrappedReadHeaderResult)));
        return nullptr;
    }


    D3D12Texture* d3d12Texture = nullptr;

    if (d3d12Texture == nullptr && (isDDS || !foundWrappedFormat))
    {
        DDSReader ddsReader;
        auto ddsReadHeaderResult = ddsReader.ReadHeader(reader);
        if (ddsReadHeaderResult == DDSReader::ReadHeaderResult::SUCCESS)
        {
            auto addedTexture = this->assetResources.texturesByNameHash.GetOrAdd(texture->name.GetHash());
            if (addedTexture.HasErrorOrValue(nullptr))
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to add texture '%1%' to collection.", texture->name));
                return nullptr;
            }

            d3d12Texture = addedTexture.value;

            if (!d3d12Texture->textureData.Create(reader.data_left(), reader.size_left(), GetAllocator()))
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to allocate memory for texture '%1%'.", texture->name));
                return nullptr;
            }

            d3d12Texture->textureDimension = ddsReader.GetDimension();

            d3d12Texture->CreateFromDDS
                (
                ddsReader,
                this->device.Get(),
                d3d12Texture->textureData.data(), //reader.data_left(),
                d3d12Texture->textureData.size(), //reader.size_left(),
                this->preallocatedSubresourceData,
                this->preallocatedFootprintSubresourceData,
                0,
                error
                );
            if (error)
            {
                d3d12Texture->HandleCreationFailure();
                this->assetResources.texturesByNameHash.remove(texture->name.GetHash());

                FINJIN_SET_ERROR(error, "Failed to create texture for DDS texture file.");
                return nullptr;
            }
        }
        else if (ddsReadHeaderResult == DDSReader::ReadHeaderResult::INVALID_SIGNATURE)
        {
            //Do nothing
        }
        else
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read header for DDS texture: %1%", ddsReader.GetReadHeaderResultString(ddsReadHeaderResult)));
            return nullptr;
        }
    }

    if (d3d12Texture == nullptr && (isPNG || !foundWrappedFormat))
    {
        PNGReader pngReader;
        pngReader.SetConvert16bitTo8bit(true);
        pngReader.SetAddAlpha(true);

        auto pngReadResult = pngReader.ReadImage(reader, this->readBuffer);
        if (pngReadResult == PNGReader::ReadResult::SUCCESS)
        {
            auto addedTexture = this->assetResources.texturesByNameHash.GetOrAdd(texture->name.GetHash());
            if (addedTexture.HasErrorOrValue(nullptr))
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to add texture '%1%' to collection.", texture->name));
                return nullptr;
            }

            d3d12Texture = addedTexture.value;

            if (!d3d12Texture->textureData.Create(this->readBuffer.data(), this->readBuffer.size(), GetAllocator()))
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to allocate memory for texture '%1%'.", texture->name));
                return nullptr;
            }

            d3d12Texture->textureDimension = TextureDimension::DIMENSION_2D;

            d3d12Texture->CreateFromPNG
                (
                pngReader,
                this->device.Get(),
                d3d12Texture->textureData.data(), //this->readBuffer.data()
                d3d12Texture->textureData.size(), //this->readBuffer.size()
                this->preallocatedSubresourceData,
                this->preallocatedFootprintSubresourceData,
                0,
                error
                );
            if (error)
            {
                d3d12Texture->HandleCreationFailure();
                this->assetResources.texturesByNameHash.remove(texture->name.GetHash());

                FINJIN_SET_ERROR(error, "Failed to create texture for PNG texture file.");
                return nullptr;
            }
        }
        else if (pngReadResult == PNGReader::ReadResult::INVALID_SIGNATURE)
        {
            //Do nothing
        }
        else
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read PNG texture: %1%", pngReader.GetReadResultString(pngReadResult)));
            return nullptr;
        }
    }

    if (d3d12Texture == nullptr)
    {
        FINJIN_SET_ERROR(error, "The texture file contained an invalid signature.");
        return nullptr;
    }

    if (!d3d12Texture->subresourceData.Create(this->preallocatedSubresourceData.size(), GetAllocator()))
    {
        d3d12Texture->HandleCreationFailure();
        this->assetResources.texturesByNameHash.remove(texture->name.GetHash());

        FINJIN_SET_ERROR(error, "Failed to create subresource data.");
        return nullptr;
    }
    FINJIN_COPY_MEMORY(d3d12Texture->subresourceData.data(), this->preallocatedSubresourceData.data(), this->preallocatedSubresourceData.size() * sizeof(D3D12_SUBRESOURCE_DATA));
    assert(!d3d12Texture->subresourceData.empty());

    if (!d3d12Texture->footprintSubresourceData.Create(this->preallocatedFootprintSubresourceData.data(), this->preallocatedFootprintSubresourceData.size(), GetAllocator()))
    {
        d3d12Texture->HandleCreationFailure();
        this->assetResources.texturesByNameHash.remove(texture->name.GetHash());

        FINJIN_SET_ERROR(error, "Failed to create footprint subresource data.");
        return nullptr;
    }
    assert(!d3d12Texture->footprintSubresourceData.empty());

    auto d3d12TextureIndex = this->assetResources.textureOffsetsByDimension[d3d12Texture->textureDimension].GetCurrentOffset();
    d3d12Texture->textureIndex = d3d12TextureIndex;
    if (d3d12Texture->name.assign(texture->name).HasError())
    {
        d3d12Texture->HandleCreationFailure();
        this->assetResources.texturesByNameHash.remove(texture->name.GetHash());

        FINJIN_SET_ERROR(error, "Failed to assign texture name.");
        return nullptr;
    }

    //Add descriptor to heap
    CD3DX12_CPU_DESCRIPTOR_HANDLE srvHandle(this->textureResources.srvTextureDescHeap.GetCpuHeapStart(), d3d12Texture->textureIndex, this->textureResources.srvTextureDescHeap.descriptorSize);

    auto textureResourceDesc = d3d12Texture->resource->GetDesc();

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = textureResourceDesc.Format;
    switch (textureResourceDesc.Dimension)
    {
        case D3D12_RESOURCE_DIMENSION_TEXTURE1D:
        {
            if (textureResourceDesc.DepthOrArraySize == 1)
                srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1D;
            else
                srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1DARRAY;
            break;
        }
        case D3D12_RESOURCE_DIMENSION_TEXTURE2D:
        {
            if (textureResourceDesc.DepthOrArraySize == 1)
                srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
            else
                srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;

            srvDesc.Texture2D.MostDetailedMip = 0;
            srvDesc.Texture2D.MipLevels = textureResourceDesc.MipLevels;
            srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

            break;
        }
        case D3D12_RESOURCE_DIMENSION_TEXTURE3D:
        {
            srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE3D;
            break;
        }
    }

    this->device->CreateShaderResourceView(d3d12Texture->resource.Get(), &srvDesc, srvHandle);

    d3d12Texture->isResidentCountdown = (size_t)-1; //Infinite countdown. Not set properly until UploadTexture() is called
    d3d12Texture->waitingToBeResidentNext = this->assetResources.waitingToBeResidentTexturesHead;
    this->assetResources.waitingToBeResidentTexturesHead = d3d12Texture;

    this->assetResources.textureOffsetsByDimension[d3d12Texture->textureDimension].count++;

    texture->gpuTexture = d3d12Texture;

    return d3d12Texture;
}

void D3D12GpuContextImpl::UploadTexture(ID3D12GraphicsCommandList* commandList, FinjinTexture* texture)
{
    auto d3d12Texture = static_cast<D3D12Texture*>(texture->gpuTexture);

    commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(d3d12Texture->resource.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST));

    {
        auto textureDesc = d3d12Texture->resource->GetDesc();

        const UINT num2DSubresources = textureDesc.DepthOrArraySize * textureDesc.MipLevels;

        auto footprints = reinterpret_cast<D3D12_PLACED_SUBRESOURCE_FOOTPRINT*>(d3d12Texture->footprintSubresourceData.data());
        auto pRowSizesInBytes = reinterpret_cast<uint64_t*>(footprints + num2DSubresources);
        auto pNumRows = reinterpret_cast<UINT*>(pRowSizesInBytes + num2DSubresources);

        uint64_t requiredSize = 0;
        device->GetCopyableFootprints(&textureDesc, 0, num2DSubresources, 0, footprints, pNumRows, pRowSizesInBytes, &requiredSize);

        UpdateSubresources
            (
            commandList,
            d3d12Texture->resource.Get(),
            d3d12Texture->uploadHeapResource.Get(),
            0,
            num2DSubresources,
            requiredSize,
            footprints,
            pNumRows,
            pRowSizesInBytes,
            d3d12Texture->subresourceData.data()
            );
    }

    commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(d3d12Texture->resource.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));

    d3d12Texture->isResidentCountdown = this->settings.frameBufferCount.actual + 1;
}

void* D3D12GpuContextImpl::CreateLightFromMainThread(FinjinSceneObjectLight* light, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    std::unique_lock<FiberSpinLock> thisLock(this->createLock);

    auto d3d12Light = this->lights.Use();
    if (d3d12Light == nullptr)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to add light '%1%' to collection.", light->name));
        return nullptr;
    }

    d3d12Light->finjinLight = light;

    light->gpuLight = d3d12Light;

    return d3d12Light;
}

void* D3D12GpuContextImpl::CreateMaterial(ID3D12GraphicsCommandList* commandList, FinjinMaterial* material, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    this->assetResources.ValidateMaterialForCreation(material->name, error);
    if (error)
    {
        FINJIN_SET_ERROR_NO_MESSAGE(error);
        return nullptr;
    }

    auto d3d12MaterialIndex = this->assetResources.materialsByNameHash.size();

    auto addedMaterial = this->assetResources.materialsByNameHash.GetOrAdd(material->name.GetHash());
    if (addedMaterial.HasErrorOrValue(nullptr))
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to add material '%1%' to collection.", material->name));
        return nullptr;
    }

    auto& d3d12Material = *addedMaterial.value;
    d3d12Material.finjinMaterial = material;
    d3d12Material.gpuBufferIndex = d3d12MaterialIndex;

    material->gpuMaterial = &d3d12Material;

    for (auto& map : material->maps)
    {
        auto foundAt = this->materialMapTypeToGpuElements.find(map.typeName);
        assert(foundAt != this->materialMapTypeToGpuElements.end());
        if (foundAt != this->materialMapTypeToGpuElements.end())
        {
            map.gpuMaterialMapIndex = static_cast<size_t>(foundAt->second.gpuMaterialMapIndex);
            map.gpuBufferTextureIndexElementID = static_cast<size_t>(foundAt->second.gpuBufferTextureIndexElementID);
            map.gpuBufferAmountElementID = static_cast<size_t>(foundAt->second.gpuBufferAmountElementID);
            map.gpuMaterialMapFlag = foundAt->second.gpuMaterialMapFlag;

            d3d12Material.maps[map.gpuMaterialMapIndex].finjinMap = &map;
            material->gpuMapFlags |= map.gpuMaterialMapFlag;
        }
    }
    ResolveMaterialMaps(*material);

    return &d3d12Material;
}

void* D3D12GpuContextImpl::CreateMeshFromMainThread(FinjinMesh* mesh, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    std::unique_lock<FiberSpinLock> thisLock(this->createLock);

    this->assetResources.ValidateMeshForCreation(mesh->name, error);
    if (error)
    {
        FINJIN_SET_ERROR_NO_MESSAGE(error);
        return nullptr;
    }

    auto addedMesh = this->assetResources.meshesByNameHash.GetOrAdd(mesh->name.GetHash());
    if (addedMesh.HasErrorOrValue(nullptr))
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to add mesh '%1%' to collection.", mesh->name));
        return nullptr;
    }

    auto& d3d12Mesh = *addedMesh.value;

    d3d12Mesh.sharedIndexBuffer.CreateIndexBuffer(mesh->indexBuffer, GetAllocator(), this->device.Get(), error);
    if (error)
    {
        d3d12Mesh.HandleCreationFailure();
        this->assetResources.meshesByNameHash.remove(mesh->name.GetHash());

        FINJIN_SET_ERROR(error, "Failed to create main mesh index buffer.");
        return nullptr;
    }

    if (!mesh->vertexBuffers.empty())
    {
        if (!d3d12Mesh.sharedVertexBuffers.Create(mesh->vertexBuffers.size(), GetAllocator()))
        {
            d3d12Mesh.HandleCreationFailure();
            this->assetResources.meshesByNameHash.remove(mesh->name.GetHash());

            FINJIN_SET_ERROR(error, "Failed to create shared vertex buffers collection.");
            return nullptr;
        }

        for (size_t vertexBufferIndex = 0; vertexBufferIndex < mesh->vertexBuffers.size(); vertexBufferIndex++)
        {
            auto inputFormat = this->internalSettings.GetInputFormat(mesh->vertexBuffers[vertexBufferIndex].formatName, mesh->vertexBuffers[vertexBufferIndex].formatElements);
            d3d12Mesh.sharedVertexBuffers[vertexBufferIndex].CreateVertexBuffer(mesh->vertexBuffers[vertexBufferIndex], inputFormat, GetAllocator(), this->device.Get(), error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to create shared vertex buffer.");
                return nullptr;
            }
        }
    }

    if (!d3d12Mesh.submeshes.Create(mesh->submeshes.size(), GetAllocator(), &d3d12Mesh))
    {
        d3d12Mesh.HandleCreationFailure();
        this->assetResources.meshesByNameHash.remove(mesh->name.GetHash());

        FINJIN_SET_ERROR(error, "Failed to create submeshes collection.");
        return nullptr;
    }

    for (size_t submeshIndex = 0; submeshIndex < mesh->submeshes.size(); submeshIndex++)
    {
        auto& d3d12Submesh = d3d12Mesh.submeshes[submeshIndex];
        auto& submesh = mesh->submeshes[submeshIndex];

        d3d12Submesh.vertexBufferIndex = static_cast<UINT>(submesh.vertexBufferRange.bufferIndex);
        d3d12Submesh.startIndexLocation = static_cast<UINT>(submesh.indexBufferRange.startIndex);
        d3d12Submesh.indexCount = static_cast<UINT>(submesh.indexBufferRange.count);
        d3d12Submesh.baseVertexLocation = static_cast<UINT>(submesh.vertexBufferRange.startIndex);
        d3d12Submesh.vertexCount = static_cast<UINT>(submesh.vertexBufferRange.count);

        auto inputFormat = this->internalSettings.GetInputFormat(submesh.vertexBuffer.formatName, submesh.vertexBuffer.formatElements);
        d3d12Submesh.vertexBuffer.CreateVertexBuffer(submesh.vertexBuffer, inputFormat, GetAllocator(), this->device.Get(), error);
        if (error)
        {
            d3d12Mesh.HandleCreationFailure();
            this->assetResources.meshesByNameHash.remove(mesh->name.GetHash());

            FINJIN_SET_ERROR(error, "Failed to create submesh vertex buffer.");
            return nullptr;
        }

        d3d12Submesh.indexBuffer.CreateIndexBuffer(submesh.indexBuffer, GetAllocator(), this->device.Get(), error);
        if (error)
        {
            d3d12Mesh.HandleCreationFailure();
            this->assetResources.meshesByNameHash.remove(mesh->name.GetHash());

            FINJIN_SET_ERROR(error, "Failed to create submesh index buffer.");
            return nullptr;
        }
    }

    d3d12Mesh.isResidentCountdown = (size_t)-1; //Infinite countdown. Not set properly until UploadMesh() is called
    d3d12Mesh.waitingToBeResidentNext = this->assetResources.waitingToBeResidentMeshesHead;
    this->assetResources.waitingToBeResidentMeshesHead = &d3d12Mesh;

    mesh->gpuMesh = &d3d12Mesh;
    d3d12Mesh.finjinMesh = mesh;

    return &d3d12Mesh;
}

void D3D12GpuContextImpl::UploadMesh(ID3D12GraphicsCommandList* commandList, FinjinMesh* mesh, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto d3d12Mesh = static_cast<D3D12Mesh*>(mesh->gpuMesh);

    d3d12Mesh->sharedIndexBuffer.Upload(commandList);

    for (size_t vertexBufferIndex = 0; vertexBufferIndex < mesh->vertexBuffers.size(); vertexBufferIndex++)
        d3d12Mesh->sharedVertexBuffers[vertexBufferIndex].Upload(commandList);

    for (size_t submeshIndex = 0; submeshIndex < mesh->submeshes.size(); submeshIndex++)
    {
        auto& d3d12Submesh = d3d12Mesh->submeshes[submeshIndex];

        d3d12Submesh.vertexBuffer.Upload(commandList);
        d3d12Submesh.indexBuffer.Upload(commandList);
    }

    d3d12Mesh->isResidentCountdown = this->settings.frameBufferCount.actual + 1;
}

void D3D12GpuContextImpl::CreateShader(D3D12ShaderType shaderType, const Utf8String& name, const uint8_t* bytes, size_t byteCount, bool makeLocalCopy, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    std::unique_lock<FiberSpinLock> thisLock(this->createLock);

    auto isNewResource = this->assetResources.ValidateShaderForCreation(shaderType, name, error);
    if (error)
    {
        FINJIN_SET_ERROR_NO_MESSAGE(error);
        return;
    }
    if (!isNewResource)
        return;

    auto& shadersByNameHash = this->assetResources.shadersByShaderTypeAndNameHash[shaderType];
    auto addedShader = shadersByNameHash.GetOrAdd(name.GetHash());
    if (addedShader.HasErrorOrValue(nullptr))
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to add shader '%1%' to collection.", name));
        return;
    }

    auto& d3d12Shader = *addedShader.value;

    d3d12Shader.Create(GetAllocator(), name, bytes, byteCount, makeLocalCopy, error);
    if (error)
    {
        d3d12Shader.HandleCreationFailed();
        shadersByNameHash.remove(name.GetHash());

        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create shader '%1%'.", name));
        return;
    }
}

void D3D12GpuContextImpl::CreateShader(D3D12ShaderType shaderType, const AssetReference& assetRef, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    std::unique_lock<FiberSpinLock> thisLock(this->createLock);

    auto& name = assetRef.objectName;

    auto isNewResource = this->assetResources.ValidateShaderForCreation(shaderType, name, error);
    if (error)
    {
        FINJIN_SET_ERROR_NO_MESSAGE(error);
        return;
    }
    if (!isNewResource)
        return;

    auto& shadersByNameHash = this->assetResources.shadersByShaderTypeAndNameHash[shaderType];
    auto addedShader = shadersByNameHash.GetOrAdd(name.GetHash());
    if (addedShader.HasErrorOrValue(nullptr))
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to add shader '%1%' to collection.", name));
        return;
    }

    auto& d3d12Shader = *addedShader.value;

    auto readResult = this->shaderAssetClassFileReader.ReadAsset(this->readBuffer, assetRef);
    if (readResult == FileOperationResult::SUCCESS)
    {
        d3d12Shader.Create(GetAllocator(), name, this->readBuffer.data(), this->readBuffer.size(), true, error);
        if (error)
        {
            d3d12Shader.HandleCreationFailed();
            shadersByNameHash.remove(name.GetHash());

            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create shader '%1%'.", name));
            return;
        }
    }
    else
    {
        d3d12Shader.HandleCreationFailed();
        shadersByNameHash.remove(name.GetHash());

        if (readResult == FileOperationResult::NOT_FOUND)
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read shader '%1%'. File could not be found.", assetRef.ToUriString()));
        else
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read shader '%1%'. There was an error reading the file.", assetRef.ToUriString()));
    }
}

D3D12RenderTarget* D3D12GpuContextImpl::GetRenderTarget(D3D12FrameStage& frameStage, const Utf8String& name)
{
    auto& frameBuffer = this->frameBuffers[frameStage.frameBufferIndex];

    if (name.empty())
        return &frameBuffer.renderTarget;

    return nullptr;
}

void D3D12GpuContextImpl::StartGraphicsCommandList(D3D12FrameStage& frameStage, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto& frameBuffer = this->frameBuffers[frameStage.frameBufferIndex];

    //Clear command allocator if it's the first command list
    auto commandAllocator = frameBuffer.commandAllocator.Get();
    if (frameBuffer.commandListsToExecute.empty())
    {
        if (FINJIN_CHECK_HRESULT_FAILED(commandAllocator->Reset()))
        {
            FINJIN_SET_ERROR(error, "Failed to reset command allocator.");
            return;
        }
    }

    //Clear command list
    auto commandList = frameBuffer.NewGraphicsCommandList();
    if (commandList == nullptr)
    {
        FINJIN_SET_ERROR(error, "Failed to get an available graphics command list.");
        return;
    }

    if (FINJIN_CHECK_HRESULT_FAILED(commandList->Reset(commandAllocator, nullptr)))
    {
        FINJIN_SET_ERROR(error, "Failed to reset command list for population.");
        return;
    }
}

void D3D12GpuContextImpl::FinishGraphicsCommandList(D3D12FrameStage& frameStage, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto& frameBuffer = this->frameBuffers[frameStage.frameBufferIndex];

    //Close command list
    auto commandList = frameBuffer.GetCurrentGraphicsCommandList();
    if (FINJIN_CHECK_HRESULT_FAILED(commandList->Close()))
    {
        FINJIN_SET_ERROR(error, "Failed to close populated command list.");
        return;
    }
}

D3D12FrameStage& D3D12GpuContextImpl::StartFrameStage(size_t index, SimpleTimeDelta elapsedTime, SimpleTimeCounter totalElapsedTime)
{
    auto& frameStage = this->frameStages[index];

    //Hold onto current frame buffer index
    auto frameBufferIndex = this->nextFrameBufferIndex;

    //Calculate next frame buffer index
    this->nextFrameBufferIndex = (this->nextFrameBufferIndex + 1) % this->frameBuffers.size();

    //Make sure the buffer has been presented and frame's state can be reused
    auto& frameBuffer = this->frameBuffers[frameBufferIndex];
    BusyWaitForFenceValue(frameBuffer.framePresentCompletedFenceValue);
    frameStage.frameBufferIndex = frameBufferIndex;

    frameStage.elapsedTime = elapsedTime;

    frameStage.totalElapsedTime = totalElapsedTime;

    frameStage.sequenceIndex = this->sequenceIndex++;

    return frameStage;
}

void D3D12GpuContextImpl::FinishBackFrameBufferRender(D3D12FrameStage& frameStage, bool continueRendering, bool modifyingRenderTarget, size_t presentSyncIntervalOverride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto& frameBuffer = this->frameBuffers[frameStage.frameBufferIndex];

    auto successRequired = continueRendering && !modifyingRenderTarget;

    frameBuffer.ExecuteCommandLists(this->graphicsCommandQueue.Get());

    auto presentSyncInterval = static_cast<UINT>(presentSyncIntervalOverride != (size_t)-1 ? presentSyncIntervalOverride : this->settings.presentSyncInterval);
    auto result = this->swapChain->Present(presentSyncInterval, 0);
    if (successRequired)
    {
        if (FINJIN_CHECK_HRESULT_FAILED(result))
        {
            FINJIN_SET_ERROR(error, "Failed to present back buffer.");
            return;
        }
    }

    frameBuffer.ResetCommandLists();

    frameBuffer.framePresentCompletedFenceValue = EmitFenceValue();
}

void D3D12GpuContextImpl::Execute(D3D12FrameStage& frameStage, GpuEvents& events, GpuCommands& commands, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto& frameBuffer = this->frameBuffers[frameStage.frameBufferIndex];

    D3D12RenderTarget* lastRenderTarget = nullptr;

    for (auto& baseCommand : commands)
    {
        switch (baseCommand.type)
        {
            case GpuCommand::Type::NOTIFY:
            {
                if (baseCommand.eventInfo.HasEventID())
                {
                    if (!events.push_back())
                    {
                        FINJIN_SET_ERROR(error, "Failed to record 'notify' GPU event.");
                        return;
                    }

                    auto& event = events.back();
                    event.type = GpuEvent::Type::NOTIFY;
                    event.eventInfo = baseCommand.eventInfo;
                }

                break;
            }
            case GpuCommand::Type::START_GRAPHICS_COMMAND_LIST:
            {
                StartGraphicsCommandList(frameStage, error);
                if (error)
                {
                    FINJIN_SET_ERROR(error, "Failed to start command list.");
                    return;
                }
                break;
            }
            case GpuCommand::Type::FINISH_GRAPHICS_COMMAND_LIST:
            {
                FinishGraphicsCommandList(frameStage, error);
                if (error)
                {
                    FINJIN_SET_ERROR(error, "Failed to finish command list.");
                    return;
                }
                break;
            }
            case GpuCommand::Type::EXECUTE_GRAPHICS_COMMAND_LISTS:
            {
                assert(0 && "Not yet supported or needed");
                //frameBuffer.ExecuteCommandLists(this->graphicsCommandQueue.Get());
                break;
            }
            case GpuCommand::Type::START_RENDER_TARGET:
            {
                auto& command = static_cast<StartRenderTargetGpuCommand&>(baseCommand);

                auto renderTarget = GetRenderTarget(frameStage, command.renderTargetName);
                StaticVector<D3D12RenderTarget*, EngineConstants::MAX_RENDER_TARGET_DEPENDENCIES> dependentRenderTargets;
                for (auto& renderTargetName : command.dependentRenderTargetNames)
                {
                    dependentRenderTargets.push_back(GetRenderTarget(frameStage, renderTargetName));
                    if (dependentRenderTargets.back() == nullptr)
                    {
                        FINJIN_SET_ERROR(error, "Failed to look up render target.");
                        return;
                    }
                }
                StartRenderTarget(frameStage, renderTarget, dependentRenderTargets, error);
                if (error)
                {
                    FINJIN_SET_ERROR(error, "Failed to start render target.");
                    return;
                }

                lastRenderTarget = renderTarget;

                break;
            }
            case GpuCommand::Type::FINISH_RENDER_TARGET:
            {
                auto& command = static_cast<FinishRenderTargetGpuCommand&>(baseCommand);

                if (lastRenderTarget == nullptr)
                {
                    FINJIN_SET_ERROR(error, "Failed to finish render target. There was no render target to finish.");
                    return;
                }

                if (lastRenderTarget == GetRenderTarget(frameStage, Utf8String::Empty()))
                {
                    //Main render target finished
                    auto commandList = frameBuffer.GetCurrentGraphicsCommandList();
                    assert(commandList != nullptr);

                    auto& frameBuffer = this->frameBuffers[frameStage.frameBufferIndex];
                    this->staticMeshRenderer.UpdatePassAndMaterialConstants(frameBuffer.staticMeshRendererFrameState, frameStage.elapsedTime, frameStage.totalElapsedTime);
                    this->staticMeshRenderer.RenderQueued(frameBuffer.staticMeshRendererFrameState, commandList);
                }

                FinishRenderTarget(frameStage, error);
                if (error)
                {
                    FINJIN_SET_ERROR(error, "Failed to finish command list");
                    return;
                }

                lastRenderTarget = nullptr;

                break;
            }
            case GpuCommand::Type::CREATE_MESH:
            {
                auto& command = static_cast<CreateMeshGpuCommand&>(baseCommand);

                auto meshAsset = command.asset;
                auto commandList = frameBuffer.GetCurrentGraphicsCommandList();
                assert(commandList != nullptr);
                UploadMesh(commandList, meshAsset, error);
                if (error)
                {
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to upload mesh '%1%'.", meshAsset->name));
                    return;
                }

                if (command.eventInfo.HasEventID())
                {
                    if (!events.push_back())
                    {
                        FINJIN_SET_ERROR(error, "Failed to record 'upload mesh' GPU event.");
                        return;
                    }

                    auto& event = events.back();
                    event.type = GpuEvent::Type::CREATE_MESH_FINISHED;
                    event.meshResource = meshAsset->gpuMesh;
                    event.eventInfo = command.eventInfo;
                }

                break;
            }
            case GpuCommand::Type::CREATE_TEXTURE:
            {
                auto& command = static_cast<CreateTextureGpuCommand&>(baseCommand);

                auto textureAsset = command.asset;
                auto commandList = frameBuffer.GetCurrentGraphicsCommandList();
                assert(commandList != nullptr);
                UploadTexture(commandList, textureAsset);
                if (error)
                {
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to upload texture '%1%'.", textureAsset->name));
                    return;
                }

                if (command.eventInfo.HasEventID())
                {
                    if (!events.push_back())
                    {
                        FINJIN_SET_ERROR(error, "Failed to record 'upload texture' GPU event.");
                        return;
                    }

                    auto& event = events.back();
                    event.type = GpuEvent::Type::CREATE_TEXTURE_FINISHED;
                    event.textureResource = textureAsset->gpuTexture;
                    event.eventInfo = command.eventInfo;
                }

                break;
            }
            case GpuCommand::Type::CREATE_MATERIAL:
            {
                auto& command = static_cast<CreateMaterialGpuCommand&>(baseCommand);

                auto materialAsset = command.asset;
                auto commandList = frameBuffer.GetCurrentGraphicsCommandList();
                assert(commandList != nullptr);
                auto materialResource = CreateMaterial(commandList, materialAsset, error);
                if (error)
                {
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to upload material '%1%'.", materialAsset->name));
                    return;
                }

                if (command.eventInfo.HasEventID())
                {
                    if (!events.push_back())
                    {
                        FINJIN_SET_ERROR(error, "Failed to record 'upload material' GPU event.");
                        return;
                    }

                    auto& event = events.back();
                    event.type = GpuEvent::Type::CREATE_MATERIAL_FINISHED;
                    event.materialResource = materialResource;
                    event.eventInfo = command.eventInfo;
                }

                break;
            }
            case GpuCommand::Type::SET_CLEAR_COLOR:
            {
                auto& command = static_cast<SetClearColorGpuCommand&>(baseCommand);

                this->clearColor = command.clearColor;

                if (command.eventInfo.HasEventID())
                {
                    if (!events.push_back())
                    {
                        FINJIN_SET_ERROR(error, "Failed to record 'set clear color' GPU event.");
                        return;
                    }

                    auto& event = events.back();
                    event.type = GpuEvent::Type::SET_CLEAR_COLOR_FINISHED;
                    event.eventInfo = command.eventInfo;
                }

                break;
            }
            case GpuCommand::Type::SET_CAMERA:
            {
                auto& command = static_cast<SetCameraGpuCommand&>(baseCommand);

                this->camera = command.camera;

                if (command.eventInfo.HasEventID())
                {
                    if (!events.push_back())
                    {
                        FINJIN_SET_ERROR(error, "Failed to record 'set camera' GPU event.");
                        return;
                    }

                    auto& event = events.back();
                    event.type = GpuEvent::Type::SET_CAMERA_FINISHED;
                    event.eventInfo = command.eventInfo;
                }

                break;
            }
            case GpuCommand::Type::RENDER_ENTITY:
            {
                auto& command = static_cast<RenderEntityGpuCommand&>(baseCommand);

                auto& frameBuffer = this->frameBuffers[frameStage.frameBufferIndex];

                this->staticMeshRenderer.RenderEntity(frameBuffer.staticMeshRendererFrameState, command.sceneNodeState, command.entity, command.shaderFeatureFlags, command.sortedLights, command.ambientLight);

                if (command.eventInfo.HasEventID())
                {
                    if (!events.push_back())
                    {
                        FINJIN_SET_ERROR(error, "Failed to record 'render entity' GPU event.");
                        return;
                    }

                    auto& event = events.back();
                    event.type = GpuEvent::Type::RENDER_ENTITY_FINISHED;
                    event.eventInfo = command.eventInfo;
                }

                break;
            }
        }
    }
}

void D3D12GpuContextImpl::UpdateResourceGpuResidencyStatus()
{
    std::unique_lock<FiberSpinLock> thisLock(this->createLock);

    {
        D3D12Mesh* previous = nullptr;
        for (auto resource = this->assetResources.waitingToBeResidentMeshesHead; resource != nullptr; resource = resource->waitingToBeResidentNext)
        {
            resource->UpdateResidentOnGpuStatus();
            if (resource->IsResidentOnGpu())
            {
                //Remove from 'waiting to be resident' list
                if (previous == nullptr)
                    this->assetResources.waitingToBeResidentMeshesHead = resource->waitingToBeResidentNext;
                else
                    previous->waitingToBeResidentNext = resource->waitingToBeResidentNext;
            }
            previous = resource;
        }
    }

    {
        D3D12Texture* previous = nullptr;
        for (auto resource = this->assetResources.waitingToBeResidentTexturesHead; resource != nullptr; resource = resource->waitingToBeResidentNext)
        {
            resource->UpdateResidentOnGpuStatus();
            if (resource->IsResidentOnGpu())
            {
                //Remove from 'waiting to be resident' list
                if (previous == nullptr)
                    this->assetResources.waitingToBeResidentTexturesHead = resource->waitingToBeResidentNext;
                else
                    previous->waitingToBeResidentNext = resource->waitingToBeResidentNext;
            }
            previous = resource;
        }
    }
}

void D3D12GpuContextImpl::StartRenderTarget(D3D12FrameStage& frameStage, D3D12RenderTarget* renderTarget, StaticVector<D3D12RenderTarget*, EngineConstants::MAX_RENDER_TARGET_DEPENDENCIES>& dependentRenderTargets, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto& frameBuffer = this->frameBuffers[frameStage.frameBufferIndex];

    //Hold onto render targets for FinishRenderTarget()----------------------
    if (renderTarget == nullptr)
        renderTarget = &frameBuffer.renderTarget;
    frameStage.renderTarget = renderTarget;
    frameStage.dependentRenderTargets = dependentRenderTargets;

    //Get command list---------------------
    auto commandList = frameBuffer.GetCurrentGraphicsCommandList();

    //Set viewport(s)----------------------
    D3D12_VIEWPORT* viewports = nullptr;
    size_t viewportCount = 0;
    if (!renderTarget->viewports.empty())
    {
        viewports = renderTarget->viewports.data();
        viewportCount = renderTarget->viewports.size();
    }
    else
    {
        viewports = &this->viewport;
        viewportCount = 1;
    }
    commandList->RSSetViewports(static_cast<UINT>(viewportCount), viewports);

    //Set scissor rectangle(s)----------------------
    D3D12_RECT* scissorRects = nullptr;
    size_t scissorRectCount = 0;
    if (!renderTarget->scissorRects.empty())
    {
        scissorRects = renderTarget->scissorRects.data();
        scissorRectCount = renderTarget->scissorRects.size();
    }
    else
    {
        scissorRects = &this->scissorRect;
        scissorRectCount = 1;
    }
    commandList->RSSetScissorRects(static_cast<UINT>(scissorRectCount), scissorRects);

    //Indicate that the back buffer will be used as a render target
    StaticVector<CD3DX12_RESOURCE_BARRIER, EngineConstants::MAX_RENDER_TARGET_BARRIERS> barriers;
    {
        D3D12_RESOURCE_STATES stateBefore = (renderTarget == &frameBuffer.renderTarget) ? D3D12_RESOURCE_STATE_PRESENT : D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
        for (size_t i = 0; i < renderTarget->renderTargetOutputResources.size(); i++)
        {
            if (barriers.push_back(CD3DX12_RESOURCE_BARRIER::Transition(renderTarget->renderTargetOutputResources[i].Get(), stateBefore, D3D12_RESOURCE_STATE_RENDER_TARGET)).HasErrorOrValue(false))
            {
            }
        }
    }
    for (auto dependentRenderTarget : frameStage.dependentRenderTargets)
    {
        for (size_t i = 0; i < dependentRenderTarget->renderTargetOutputResources.size(); i++)
        {
            if (barriers.push_back(CD3DX12_RESOURCE_BARRIER::Transition(dependentRenderTarget->renderTargetOutputResources[i].Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET)).HasErrorOrValue(false))
            {
            }
        }
    }
    commandList->ResourceBarrier(static_cast<UINT>(barriers.size()), barriers.data());

    //Set render targets----------------------
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(this->swapChainRtvDescHeap.GetCpuHeapStart(), static_cast<INT>(renderTarget->renderTargetResourceHeapIndex), static_cast<UINT>(this->swapChainRtvDescHeap.descriptorSize));
    CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(this->swapChainDsvDescHeap.GetCpuHeapStart(), static_cast<INT>(renderTarget->depthStencilResourceHeapIndex), static_cast<UINT>(this->swapChainDsvDescHeap.descriptorSize));
    commandList->OMSetRenderTargets(static_cast<UINT>(renderTarget->renderTargetOutputResources.size()), &rtvHandle, FALSE, renderTarget->HasDepthStencil() ? &dsvHandle : nullptr);

    //Clear render target and depth stencil----------------------
    commandList->ClearRenderTargetView(rtvHandle, renderTarget->clearColor.IsSet() ? renderTarget->clearColor.value.data() : this->clearColor.data(), static_cast<UINT>(scissorRectCount), scissorRects);
    if (renderTarget->HasDepthStencil())
        commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, static_cast<UINT>(scissorRectCount), scissorRects);
}

void D3D12GpuContextImpl::FinishRenderTarget(D3D12FrameStage& frameStage, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto& frameBuffer = this->frameBuffers[frameStage.frameBufferIndex];

    auto renderTarget = frameStage.renderTarget;

    //Indicate that the back buffer will now be used to present----------------------
    StaticVector<CD3DX12_RESOURCE_BARRIER, EngineConstants::MAX_RENDER_TARGET_BARRIERS> barriers;
    for (auto dependentRenderTarget : frameStage.dependentRenderTargets)
    {
        for (size_t i = 0; i < dependentRenderTarget->renderTargetOutputResources.size(); i++)
        {
            if (barriers.push_back(CD3DX12_RESOURCE_BARRIER::Transition(dependentRenderTarget->renderTargetOutputResources[i].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE)).HasErrorOrValue(false))
            {
            }
        }
    }
    {
        D3D12_RESOURCE_STATES stateAfter = (renderTarget == &frameBuffer.renderTarget) ? D3D12_RESOURCE_STATE_PRESENT : D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
        for (size_t i = 0; i < renderTarget->renderTargetOutputResources.size(); i++)
        {
            if (barriers.push_back(CD3DX12_RESOURCE_BARRIER::Transition(renderTarget->renderTargetOutputResources[i].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, stateAfter)).HasErrorOrValue(false))
            {
            }
        }
    }
    auto commandList = frameBuffer.GetCurrentGraphicsCommandList();
    commandList->ResourceBarrier(static_cast<UINT>(barriers.size()), barriers.data());
}

void D3D12GpuContextImpl::CreateScreenSizeDependentResources(Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    this->viewport.TopLeftX = 0;
    this->viewport.TopLeftY = 0;
    this->viewport.Width = RoundToFloat(this->renderingPixelBounds.GetClientWidth());
    this->viewport.Height = RoundToFloat(this->renderingPixelBounds.GetClientHeight());
    this->viewport.MinDepth = 0;
    this->viewport.MaxDepth = this->settings.maxDepthValue;

    this->scissorRect.left = 0;
    this->scissorRect.top = 0;
    this->scissorRect.right = static_cast<LONG>(this->renderingPixelBounds.GetClientWidth());
    this->scissorRect.bottom = static_cast<LONG>(this->renderingPixelBounds.GetClientHeight());

    //Frame resources
    {
        CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(this->swapChainRtvDescHeap.GetCpuHeapStart());

        //Render target and view
        for (auto& frameBuffer : this->frameBuffers)
        {
            auto& resource = frameBuffer.renderTarget.renderTargetOutputResources[0];

            //Get render target buffer
            assert(!frameBuffer.renderTarget.renderTargetOutputResources.empty());
            if (FINJIN_CHECK_HRESULT_FAILED(this->swapChain->GetBuffer(static_cast<UINT>(frameBuffer.index), IID_PPV_ARGS(&resource))))
            {
                FINJIN_SET_ERROR(error, "Failed to get swap chain buffer.");
                return;
            }
            FINJIN_D3D12_SET_RESOURCE_NAME(resource, Utf8StringFormatter::Format("Frame buffer %1%", frameBuffer.index));

            //Create view on the buffer
            this->device->CreateRenderTargetView(resource.Get(), nullptr, rtvHandle);
            frameBuffer.renderTarget.renderTargetResourceHeapIndex = frameBuffer.index;

            rtvHandle.Offset(1, this->swapChainRtvDescHeap.descriptorSize);
        }
    }

    //Depth stencil and view
    {
        this->frameBuffers[0].renderTarget.CreateDepthStencil
            (
            this->device.Get(),
            static_cast<UINT>(this->renderingPixelBounds.GetClientWidth()),
            static_cast<UINT>(this->renderingPixelBounds.GetClientHeight()),
            this->settings.depthStencilFormat.actual,
            this->settings.maxDepthValue,
            this->settings.multisampleCount.actual,
            this->settings.multisampleQuality.actual,
            error
            );

        //Create view on the resource
        D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilDesc = {};
        depthStencilDesc.Format = this->settings.depthStencilFormat.actual;
        depthStencilDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
        depthStencilDesc.Flags = D3D12_DSV_FLAG_NONE;

        CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(this->swapChainDsvDescHeap.GetCpuHeapStart(), 0, this->swapChainDsvDescHeap.descriptorSize);
        this->device->CreateDepthStencilView(this->frameBuffers[0].renderTarget.depthStencilResource.Get(), &depthStencilDesc, dsvHandle);
        this->frameBuffers[0].renderTarget.depthStencilResourceHeapIndex = 0;

        for (size_t i = 1; i < this->frameBuffers.size(); i++)
        {
            this->frameBuffers[i].renderTarget.depthStencilResource = this->frameBuffers[0].renderTarget.depthStencilResource;
            this->frameBuffers[i].renderTarget.depthStencilResourceHeapIndex = this->frameBuffers[0].renderTarget.depthStencilResourceHeapIndex;
        }
    }
}

void D3D12GpuContextImpl::DestroyScreenSizeDependentResources(bool resizing)
{
    for (auto& frameBuffer : this->frameBuffers)
        frameBuffer.renderTarget.DestroyScreenSizeDependentResources();
}

void D3D12GpuContextImpl::UpdateCachedFrameBufferSize()
{
    this->renderingPixelBounds = this->settings.osWindow->GetWindowSize().GetSafeCurrentBounds();
    this->renderingPixelBounds.Scale(this->settings.osWindow->GetDisplayDensity());
}

#endif
