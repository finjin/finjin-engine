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
#include "D3D12RootSignatureBuilder.hpp"
#include "D3D12System.hpp"
#include "D3D12SystemImpl.hpp"
#include "D3D12Utilities.hpp"
#if FINJIN_TARGET_VR_SYSTEM != FINJIN_TARGET_VR_SYSTEM_NONE
    #include "finjin/engine/VRContext.hpp"
#endif

using namespace Finjin::Engine;


//Macros------------------------------------------------------------------------
#define MAX_OFFSCREEN_RENDER_TARGET_COUNT (EngineConstants::MAX_FRAME_BUFFERS * 2) //TODO: Make this a configurable value


//Local functions---------------------------------------------------------------
static size_t GetBufferCountForFrameDestination(GpuFrameDestination frameDestination)
{
    if (AnySet(frameDestination & GpuFrameDestination::VR_CONTEXT))
        return 2;
    else
        return 1;
}

static size_t CalculateOffscreenRenderTargetRTVIndex(size_t swapChainFrameBufferCount, size_t frameBufferIndex, size_t renderTargetIndex)
{
    return
        swapChainFrameBufferCount + //Go past swap chain frame buffer targets
        frameBufferIndex //Offset to the color target for the offscreen frame
        ;
}

static size_t CalculateOffscreenRenderTargetSRVIndex(size_t swapChainFrameBufferCount, size_t frameBufferIndex, size_t renderTargetIndex)
{
    return frameBufferIndex; //Offset to the color target for the offscreen frame
}


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
                size_t elementIndex = 0;
                for (elementIndex = 0; elementIndex < count; elementIndex++)
                {
                    auto inputFormatElement = inputFormat.GetElement(elementIndex);
                    if (elements[elementIndex].id != inputFormatElement->elementID || elements[elementIndex].type != inputFormatElement->type)
                        break;
                }
                if (elementIndex == count)
                    return &inputFormat;
            }
        }
    }

    return nullptr;
}

//D3D12GpuContextImpl
D3D12GpuContextImpl::D3D12GpuContextImpl(Allocator* allocator) :
    AllocatedClass(allocator),
    GpuContextCommonImpl(allocator),
    settings(allocator),
    materialMapTypeToElements
        (
        StandardAssetDocumentPropertyValues::TypeName::MATERIAL_MAP_DIFFUSE, GpuMaterialMapIndexToConstantBufferElements<D3D12Material::MapIndex>(D3D12Material::MapIndex::DIFFUSE, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_DIFFUSE_TEXTURE_INDEX, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_DIFFUSE_AMOUNT, ShaderFeatureFlag::MAP_DIFFUSE),
        StandardAssetDocumentPropertyValues::TypeName::MATERIAL_MAP_SPECULAR, GpuMaterialMapIndexToConstantBufferElements<D3D12Material::MapIndex>(D3D12Material::MapIndex::SPECULAR, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_SPECULAR_TEXTURE_INDEX, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_SPECULAR_AMOUNT, ShaderFeatureFlag::MAP_SPECULAR),
        StandardAssetDocumentPropertyValues::TypeName::MATERIAL_MAP_REFLECTION, GpuMaterialMapIndexToConstantBufferElements<D3D12Material::MapIndex>(D3D12Material::MapIndex::REFLECTION, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_REFLECTION_TEXTURE_INDEX, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_REFLECTION_AMOUNT, ShaderFeatureFlag::MAP_REFLECTION),
        StandardAssetDocumentPropertyValues::TypeName::MATERIAL_MAP_REFRACTION, GpuMaterialMapIndexToConstantBufferElements<D3D12Material::MapIndex>(D3D12Material::MapIndex::REFRACTION, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_REFRACTION_TEXTURE_INDEX, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_REFRACTION_AMOUNT, ShaderFeatureFlag::MAP_REFRACTION),
        StandardAssetDocumentPropertyValues::TypeName::MATERIAL_MAP_BUMP, GpuMaterialMapIndexToConstantBufferElements<D3D12Material::MapIndex>(D3D12Material::MapIndex::BUMP, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_BUMP_TEXTURE_INDEX, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_BUMP_AMOUNT, ShaderFeatureFlag::MAP_BUMP),
        StandardAssetDocumentPropertyValues::TypeName::MATERIAL_MAP_HEIGHT, GpuMaterialMapIndexToConstantBufferElements<D3D12Material::MapIndex>(D3D12Material::MapIndex::HEIGHT, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_HEIGHT_TEXTURE_INDEX, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_HEIGHT_AMOUNT, ShaderFeatureFlag::MAP_HEIGHT),
        StandardAssetDocumentPropertyValues::TypeName::MATERIAL_MAP_SELF_ILLUMINATION, GpuMaterialMapIndexToConstantBufferElements<D3D12Material::MapIndex>(D3D12Material::MapIndex::SELF_ILLUMINATION, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_SELF_ILLUMINATION_TEXTURE_INDEX, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_SELF_ILLUMINATION_AMOUNT, ShaderFeatureFlag::MAP_SELF_ILLUMINATION),
        StandardAssetDocumentPropertyValues::TypeName::MATERIAL_MAP_OPACITY, GpuMaterialMapIndexToConstantBufferElements<D3D12Material::MapIndex>(D3D12Material::MapIndex::OPACITY, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_OPACITY_TEXTURE_INDEX, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_OPACITY_AMOUNT, ShaderFeatureFlag::MAP_OPACITY),
        StandardAssetDocumentPropertyValues::TypeName::MATERIAL_MAP_ENVIRONMENT, GpuMaterialMapIndexToConstantBufferElements<D3D12Material::MapIndex>(D3D12Material::MapIndex::ENVIRONMENT, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_ENVIRONMENT_TEXTURE_INDEX, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_ENVIRONMENT_AMOUNT, ShaderFeatureFlag::MAP_ENVIRONMENT),
        StandardAssetDocumentPropertyValues::TypeName::MATERIAL_MAP_SHININESS, GpuMaterialMapIndexToConstantBufferElements<D3D12Material::MapIndex>(D3D12Material::MapIndex::SHININESS, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_SHININESS_TEXTURE_INDEX, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_SHININESS_AMOUNT, ShaderFeatureFlag::MAP_SHININESS)
        ),
    commonResources(allocator)
{
    this->d3dSystem = nullptr;

    FINJIN_ZERO_ITEM(this->renderViewport);
    FINJIN_ZERO_ITEM(this->renderScissorRect);

    this->fenceEventHandle = nullptr;
    this->fenceValue = 0;

    this->frameBufferScreenCaptureColorFormatBytesPerPixel = 0;
    this->frameBufferScreenCapturePixelFormat = ScreenCapturePixelFormat::NONE;
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
        SleepWaitForFenceValue(EmitFenceValue());

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

    for (auto& frameBuffer : this->frameBuffers)
        frameBuffer.Destroy();

    for (auto& frameStage : this->frameStages)
        frameStage.Destroy();

    this->depthStencilRenderTarget.Destroy();

    this->staticMeshRenderer.Destroy();

    this->assetResources.Destroy();
    this->commonResources.Destroy();

    if (this->fenceEventHandle != nullptr)
    {
        CloseHandle(this->fenceEventHandle);
        this->fenceEventHandle = nullptr;
    }

    //The remaining resources are released by their smart pointers after this D3D12GpuContextImpl is destroyed
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

        this->deviceDescription = systemImpl->softwareGpuDescriptions[0];
    }
    else
    {
        Microsoft::WRL::ComPtr<IDXGIAdapter1> hardwareAdapter;
        systemImpl->GetHardwareD3DAdapter(this->settings.gpuID, &hardwareAdapter, this->deviceDescription, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to get hardware D3D adapter.");
            return;
        }

        if (hardwareAdapter == nullptr && !this->settings.gpuID.IsZero())
        {
            //No error, but adapter could not be found. Drop back to default
            FINJIN_ZERO_ITEM(this->settings.gpuID);

            systemImpl->GetHardwareD3DAdapter(this->settings.gpuID, &hardwareAdapter, this->deviceDescription, error);
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

    //Color/depth formats-----------------
    for (size_t format = 0; format < this->formatSupportByFormat.max_size(); format++)
    {
        D3D12_FEATURE_DATA_FORMAT_SUPPORT formatSupport = {};
        formatSupport.Format = static_cast<DXGI_FORMAT>(format);
        this->device->CheckFeatureSupport(D3D12_FEATURE_FORMAT_SUPPORT, &formatSupport, sizeof(formatSupport));
        this->formatSupportByFormat.push_back(formatSupport); //Keep the value regardless of whether CheckFeatureSupport() succeeds or fails
    }

    if (!D3D12Utilities::GetBestColorFormat(this->settings.colorFormat, this->formatSupportByFormat.data(), this->formatSupportByFormat.size()))
    {
        FINJIN_SET_ERROR(error, "Failed to determine supported color format.");
        return;
    }
    this->frameBufferScreenCaptureColorFormatBytesPerPixel = D3D12Utilities::GetBitsPerPixel(this->settings.colorFormat.actual) / 8;
    switch (this->settings.colorFormat.actual)
    {
        case DXGI_FORMAT_R8G8B8A8_UNORM: this->frameBufferScreenCapturePixelFormat = ScreenCapturePixelFormat::RGBA8_UNORM; break;
        case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB: this->frameBufferScreenCapturePixelFormat = ScreenCapturePixelFormat::RGBA8_UNORM_SRGB; break;
        case DXGI_FORMAT_B8G8R8A8_UNORM: this->frameBufferScreenCapturePixelFormat = ScreenCapturePixelFormat::BGRA8_UNORM; break;
        case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB: this->frameBufferScreenCapturePixelFormat = ScreenCapturePixelFormat::BGRA8_UNORM_SRGB; break;
        case DXGI_FORMAT_R16G16B16A16_FLOAT: this->frameBufferScreenCapturePixelFormat = ScreenCapturePixelFormat::RGBA16_FLOAT; break;
        default: assert(0); this->frameBufferScreenCapturePixelFormat = ScreenCapturePixelFormat::NONE; break;
    }

    if (!D3D12Utilities::GetBestDepthStencilFormat(this->settings.depthStencilFormat, this->settings.stencilRequired, this->formatSupportByFormat.data(), this->formatSupportByFormat.size()))
    {
        FINJIN_SET_ERROR(error, "Failed to determine supported depth/stencil format.");
        return;
    }

    //Multisample quality levels------------------
    D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS qualityLevels;
    qualityLevels.Format = this->settings.colorFormat.actual;
    qualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
    for (qualityLevels.SampleCount = 1;
        qualityLevels.SampleCount <= D3D12_MAX_MULTISAMPLE_SAMPLE_COUNT &&
        !this->deviceDescription.features.multisampleQualityLevels.full(); qualityLevels.SampleCount++)
    {
        if (SUCCEEDED(this->device->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &qualityLevels, sizeof(qualityLevels))))
        {
            if (qualityLevels.NumQualityLevels > 0)
                this->deviceDescription.features.multisampleQualityLevels.push_back(qualityLevels);
        }
    }

    for (auto& level : this->deviceDescription.features.multisampleQualityLevels)
    {
        if (level.SampleCount == settings.multisampleCount.requested && (level.NumQualityLevels - 1) == settings.multisampleQuality.requested)
        {
            settings.multisampleCount.actual = settings.multisampleCount.requested;
            settings.multisampleQuality.actual = settings.multisampleQuality.requested;
            break;
        }
    }

    //Determine some settings-------------------------------------------
    UpdatedCachedWindowSize();
    auto maxRenderTargetSize = this->settings.renderTargetSize.EvaluateMax(nullptr, &this->windowPixelBounds);

    this->settings.screenCaptureFrequency.actual = this->settings.screenCaptureFrequency.requested;

    this->settings.frameBufferCount.actual = this->settings.frameBufferCount.requested;
    this->settings.jobProcessingPipelineSize = GpuContextCommonSettings::CalculateJobPipelineSize(this->settings.frameBufferCount.actual);

    //Create some things--------------------------------------------------
    //RTV descriptor heap for swap chain views
    {
        D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
        rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        rtvHeapDesc.NumDescriptors = static_cast<UINT>(this->frameBuffers.size()) + MAX_OFFSCREEN_RENDER_TARGET_COUNT;
        rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        this->rtvDescHeap.Create(this->device.Get(), rtvHeapDesc, error);
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
        dsvHeapDesc.NumDescriptors = 1 + MAX_OFFSCREEN_RENDER_TARGET_COUNT;
        dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        this->dsvDescHeap.Create(this->device.Get(), dsvHeapDesc, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to create depth stencil descriptor heap.");
            return;
        }
    }

    //SRV heap for textures
    {
        size_t textureOffset = this->settings.frameBufferCount.actual; //Reserve the first off screen buffers

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
        this->commonResources.srvTextureDescHeap.Create(this->device.Get(), srvHeapDesc, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to create texture SRV heap.");
            return;
        }
    }

    this->frameBuffers.resize(this->settings.frameBufferCount.actual);
    for (size_t frameBufferIndex = 0; frameBufferIndex < this->frameBuffers.size(); frameBufferIndex++)
    {
        auto& frameBuffer = this->frameBuffers[frameBufferIndex];

        frameBuffer.SetIndex(frameBufferIndex);

        frameBuffer.CreateCommandLists(this->device.Get(), this->settings.maxGpuCommandListsPerStage, GetAllocator(), error);
        if (error)
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create command list objects for frame buffer %1%.", frameBufferIndex));
            return;
        }

        if (this->settings.renderTargetSize.GetType() == GpuRenderTargetSizeType::EXPLICIT_SIZE)
        {
            //Create color buffer
            frameBuffer.renderTarget.CreateColor(this->device.Get(), maxRenderTargetSize[0], maxRenderTargetSize[1], this->settings.colorFormat.actual, this->settings.multisampleCount.actual, this->settings.multisampleQuality.actual, false, GetBufferCountForFrameDestination(this->settings.frameDestination), error);
            if (error)
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create color render target for frame buffer %1%.", frameBufferIndex));
                return;
            }

            auto& output = frameBuffer.renderTarget.colorOutputs[0];
            output.rtvDescHeapIndex = CalculateOffscreenRenderTargetRTVIndex(this->frameBuffers.size(), frameBuffer.index, 0);
            output.srvDescHeapIndex = CalculateOffscreenRenderTargetSRVIndex(this->frameBuffers.size(), frameBuffer.index, 0);
            this->device->CreateRenderTargetView(output.Get(), nullptr, this->rtvDescHeap.GetCpuHeapStart(output.rtvDescHeapIndex));
            this->device->CreateShaderResourceView(output.Get(), nullptr, this->commonResources.srvTextureDescHeap.GetCpuHeapStart(output.srvDescHeapIndex));

            //Create screen capture buffer
            if (this->settings.screenCaptureFrequency.actual != ScreenCaptureFrequency::NEVER)
            {
                auto byteCount = AlignSizeUp(maxRenderTargetSize[0], D3D12_TEXTURE_DATA_PITCH_ALIGNMENT) * maxRenderTargetSize[1] * this->frameBufferScreenCaptureColorFormatBytesPerPixel;
                frameBuffer.CreateScreenCaptureBuffer(this->device.Get(), byteCount, false, error);
                if (error)
                {
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create screen capture buffer for frame buffer %1%.", frameBufferIndex));
                    return;
                }
            }
        }
    }

    if (this->settings.renderTargetSize.GetType() == GpuRenderTargetSizeType::EXPLICIT_SIZE &&
        this->settings.depthStencilFormat.actual != DXGI_FORMAT_UNKNOWN)
    {
        this->depthStencilRenderTarget.CreateDepthStencil(this->device.Get(), maxRenderTargetSize[0], maxRenderTargetSize[1], this->settings.depthStencilFormat.actual, this->settings.maxDepthValue, this->settings.multisampleCount.actual, this->settings.multisampleQuality.actual, false, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to create depth/stencil buffer.");
            return;
        }

        this->depthStencilRenderTarget.depthStencilResource.dsvDescHeapIndex = 0;
        this->device->CreateDepthStencilView(this->depthStencilRenderTarget.depthStencilResource.Get(), nullptr, this->dsvDescHeap.GetCpuHeapStart(this->depthStencilRenderTarget.depthStencilResource.dsvDescHeapIndex));
    }

    this->frameStages.resize(this->settings.jobProcessingPipelineSize);
    for (size_t frameStageIndex = 0; frameStageIndex < this->frameStages.size(); frameStageIndex++)
    {
        auto& frameStage = this->frameStages[frameStageIndex];

        frameStage.SetIndex(frameStageIndex);
    }
}

void D3D12GpuContextImpl::CreateDeviceSupportObjects(Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    //Set more selector settings-----------

    //Shader language version used to compile shaders
    Utf8String shaderModelString;
    if (D3D12Utilities::ShaderModelToString(shaderModelString, this->deviceDescription.features.maxShaderModel).HasError())
    {
        FINJIN_SET_ERROR(error, "Failed to convert shader model to string.");
        return;
    }
    this->settings.initialAssetFileSelector.Set(AssetPathComponent::GPU_SHADER_MODEL, shaderModelString);

    //Feature level
    Utf8String featureLevelString;
    if (D3D12Utilities::FeatureLevelToString(featureLevelString, this->deviceDescription.features.standardFeatures.featureLevel).HasError())
    {
        FINJIN_SET_ERROR(error, "Failed to convert feature level to string.");
        return;
    }
    this->settings.initialAssetFileSelector.Set(AssetPathComponent::GPU_FEATURE_LEVEL, featureLevelString);

    //Preferred texture format
    if (this->deviceDescription.features.standardFeatures.bc6bc7Compression && this->deviceDescription.features.standardFeatures.bc4bc5Compression)
        this->settings.initialAssetFileSelector.Set(AssetPathComponent::GPU_PREFERRED_TEXTURE_FORMAT, "bc");

    //GPU model
    this->settings.initialAssetFileSelector.Set(AssetPathComponent::GPU_MODEL, this->deviceDescription.desc.Description);

    //Set up asset readers----------
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

    //Read internal settings----------
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

    //Create a number of other internal members-------------------
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

    if (!this->commonResources.lights.Create(this->settings.maxLights, GetAllocator(), GetAllocator()))
    {
        FINJIN_SET_ERROR(error, "Failed to allocate light lookup.");
        return;
    }
    for (size_t lightIndex = 0; lightIndex < this->commonResources.lights.items.size(); lightIndex++)
        this->commonResources.lights.items[lightIndex].gpuBufferIndex = lightIndex;

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
    auto mipCount = RoundToInt(log2(this->deviceDescription.features.standardFeatures.maxTextureDimension)) + 1; //Less or equal to D3D12_REQ_MIP_LEVELS
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

    //Create full screen shader resources
    {
        //Read shaders
        size_t shaderFileIndex = 0;
        for (auto shaderFileName : { "common-shaders-fullscreen-quad-vs.cso", "common-shaders-fullscreen-quad-ps.cso" })
        {
            this->workingAssetReference.ForUriString(shaderFileName, this->workingUri, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to parse common shaders asset reference.");
                return;
            }

            auto readResult = this->shaderAssetClassFileReader.ReadAsset(this->readBuffer, this->workingAssetReference);
            if (readResult != FileOperationResult::SUCCESS)
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read shader file '%1%'.", shaderFileName));
                return;
            }

            if (!this->commonResources.fullScreenShaderFileBytes[shaderFileIndex].Create(this->readBuffer, GetAllocator()))
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create shader for file '%1%'.", shaderFileName));
                return;
            }

            shaderFileIndex++;
        }

        //Create root signature
        D3D12RootSignatureBuilder rootSignatureBuilder;
        {
            D3D12ShaderRootParameterBuilder parameter(0, 0, 16, D3D12_SHADER_VISIBILITY_ALL);
            rootSignatureBuilder.GetRootParameters().Add(parameter, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to add model/view/projection matrix element.");
                return;
            }
        }
        {
            D3D12ShaderRootParameterBuilder parameter(1, 0, 4, D3D12_SHADER_VISIBILITY_ALL);
            rootSignatureBuilder.GetRootParameters().Add(parameter, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to add texture index.");
                return;
            }
        }
        {
            D3D12ShaderRootParameterBuilder parameter(2, 0, 4, D3D12_SHADER_VISIBILITY_ALL);
            rootSignatureBuilder.GetRootParameters().Add(parameter, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to add 'multiply' color.");
                return;
            }
        }
        {
            D3D12ShaderRootParameterBuilder parameter(3, 0, 4, D3D12_SHADER_VISIBILITY_ALL);
            rootSignatureBuilder.GetRootParameters().Add(parameter, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to add 'add' color.");
                return;
            }
        }
        {
            D3D12ShaderRootParameterBuilder parameter(D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE, 0, 0, D3D12_SHADER_VISIBILITY_PIXEL);
            parameter.descriptorTable.resize(1);
            parameter.descriptorTable[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
            parameter.descriptorTable[0].NumDescriptors = 1000;
            rootSignatureBuilder.GetRootParameters().Add(parameter, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to add SRV (texture) element.");
                return;
            }
        }
        rootSignatureBuilder.GetStaticSamplers().AddSequential(D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_TEXTURE_ADDRESS_MODE_WRAP);
        rootSignatureBuilder.Create(this->commonResources.fullScreenShaderRootSignature, this->device.Get(), error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to create full screen shader root signature.");
            return;
        }

        //Create pipeline state
        D3D12_GRAPHICS_PIPELINE_STATE_DESC fullScreenShaderGraphicsPipelineStateDesc;
        FINJIN_ZERO_ITEM(fullScreenShaderGraphicsPipelineStateDesc);
        fullScreenShaderGraphicsPipelineStateDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
        fullScreenShaderGraphicsPipelineStateDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
        fullScreenShaderGraphicsPipelineStateDesc.RasterizerState.FrontCounterClockwise = TRUE;
        fullScreenShaderGraphicsPipelineStateDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
        fullScreenShaderGraphicsPipelineStateDesc.SampleMask = UINT_MAX;
        fullScreenShaderGraphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        fullScreenShaderGraphicsPipelineStateDesc.NumRenderTargets = 1;
        fullScreenShaderGraphicsPipelineStateDesc.RTVFormats[0] = this->settings.colorFormat.actual;
        fullScreenShaderGraphicsPipelineStateDesc.SampleDesc.Count = this->settings.multisampleCount.actual;
        fullScreenShaderGraphicsPipelineStateDesc.SampleDesc.Quality = this->settings.multisampleQuality.actual;
        fullScreenShaderGraphicsPipelineStateDesc.pRootSignature = this->commonResources.fullScreenShaderRootSignature.Get();
        fullScreenShaderGraphicsPipelineStateDesc.VS = { this->commonResources.fullScreenShaderFileBytes[0].data(), static_cast<UINT>(this->commonResources.fullScreenShaderFileBytes[0].size()) };
        fullScreenShaderGraphicsPipelineStateDesc.PS = { this->commonResources.fullScreenShaderFileBytes[1].data(), static_cast<UINT>(this->commonResources.fullScreenShaderFileBytes[1].size()) };

        auto createGraphicsPipelineStateResult = this->device->CreateGraphicsPipelineState(&fullScreenShaderGraphicsPipelineStateDesc, IID_PPV_ARGS(&this->commonResources.fullScreenShaderGraphicsPipelineState));
        if (FINJIN_CHECK_HRESULT_FAILED(createGraphicsPipelineStateResult))
        {
            FINJIN_SET_ERROR(error, "Failed to create pipeline state object for full screen shader.");
            return;
        }
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

D3D12FrameStage& D3D12GpuContextImpl::StartFrameStage(size_t index, SimpleTimeDelta elapsedTime, SimpleTimeCounter totalElapsedTime)
{
    auto& frameStage = this->frameStages[index];

    auto frameBufferIndex = this->currentFrameBufferIndex++ % this->frameBuffers.size();

    //Wait for previous frame to complete
    auto& frameBuffer = this->frameBuffers[frameBufferIndex];
    frameBuffer.WaitForNotInUse();
    frameStage.frameBufferIndex = frameBufferIndex;

    //Update state
    frameStage.elapsedTime = elapsedTime;
    frameStage.totalElapsedTime = totalElapsedTime;
    frameStage.sequenceIndex = this->sequenceIndex++;

    return frameStage;
}

void D3D12GpuContextImpl::PresentFrameStage(D3D12FrameStage& frameStage, const RenderStatus& renderStatus, size_t presentSyncIntervalOverride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto& frameBuffer = this->frameBuffers[frameStage.frameBufferIndex];

    if (renderStatus.IsRenderingRequired())
    {
        //Get swap chain buffer
        Microsoft::WRL::ComPtr<ID3D12Resource> swapChainBuffer;
        if (FINJIN_CHECK_HRESULT_FAILED(this->swapChain->GetBuffer(static_cast<UINT>(frameBuffer.index), IID_PPV_ARGS(&swapChainBuffer))))
        {
            frameBuffer.ResetCommandLists();

            FINJIN_SET_ERROR(error, "Failed to get swap chain buffer.");
            return;
        }

        //Encode commands to draw the off screen render target onto a full screen quad on the back buffer----------------
        auto commandListHandle = frameBuffer.NewGraphicsCommandList();
        if (commandListHandle == nullptr)
        {
            frameBuffer.ResetCommandLists();

            FINJIN_SET_ERROR(error, "Failed to get command list.");
            return;
        }

        auto commandList = commandListHandle->commandList.Get();

        commandList->RSSetViewports(1, &this->windowViewport);
        commandList->RSSetScissorRects(1, &this->windowScissorRect);

        commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(swapChainBuffer.Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

        auto rtvHandle = this->rtvDescHeap.GetCpuHeapStart(frameBuffer.index);
        commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

        commandList->SetGraphicsRootSignature(this->commonResources.fullScreenShaderRootSignature.Get());
        commandList->SetPipelineState(this->commonResources.fullScreenShaderGraphicsPipelineState.Get());

        ID3D12DescriptorHeap* heaps[] = { this->commonResources.srvTextureDescHeap.heap.Get() };
        commandList->SetDescriptorHeaps(1, heaps);

        //Set model view matrix constant
        MathMatrix4Values d3d12Matrix;

        MathMatrix4 modelViewProjectionMatrix;
        modelViewProjectionMatrix.setIdentity();
        //auto translation = MathAffineTransform(MathTranslation(-.2f, .4f, 0.0f)); //Translation is in normalized device coordinates [-1, 1]
        //auto scale = MathAffineTransform(MathScaling(.5f, .5f, 1.0f));
        //modelViewProjectionMatrix = (translation * scale).matrix();
        GetRowOrderMatrixData(d3d12Matrix, modelViewProjectionMatrix);
        commandList->SetGraphicsRoot32BitConstants(0, 16, d3d12Matrix.a, 0);

        uint32_t textureIndex = frameBuffer.renderTarget.colorOutputs[0].srvDescHeapIndex;
        commandList->SetGraphicsRoot32BitConstants(1, 1, &textureIndex, 0);

        MathVector4 multiplyColor(1, 1, 1, 1);
        commandList->SetGraphicsRoot32BitConstants(2, 4, multiplyColor.data(), 0);

        MathVector4 addColor(0, 0, 0, 0);
        commandList->SetGraphicsRoot32BitConstants(3, 4, addColor.data(), 0);

        //Set texture
        commandList->SetGraphicsRootDescriptorTable(4, this->commonResources.srvTextureDescHeap.GetGpuHeapStart());

        commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
        commandList->DrawInstanced(4, 1, 0, 0);

        commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(swapChainBuffer.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

        if ((this->settings.screenCaptureFrequency.actual == ScreenCaptureFrequency::EVERY_FRAME ||
            (this->settings.screenCaptureFrequency.actual == ScreenCaptureFrequency::ON_REQUEST && frameBuffer.screenCaptureRequested)) &&
            frameBuffer.screenCaptureBuffer != nullptr)
        {
            commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(frameBuffer.renderTarget.colorOutputs[0].Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COPY_SOURCE));

            CD3DX12_TEXTURE_COPY_LOCATION sourceLocation(frameBuffer.renderTarget.colorOutputs[0].Get(), 0);
            CD3DX12_BOX sourceBox(0, 0, this->renderTargetSize[0], this->renderTargetSize[1]);

            D3D12_PLACED_SUBRESOURCE_FOOTPRINT destinationFootprint;
            destinationFootprint.Offset = 0;
            destinationFootprint.Footprint.Width = this->renderTargetSize[0];
            destinationFootprint.Footprint.Height = this->renderTargetSize[1];
            destinationFootprint.Footprint.Depth = 1;
            destinationFootprint.Footprint.Format = this->settings.colorFormat.actual;
            destinationFootprint.Footprint.RowPitch = AlignSizeUp(this->renderTargetSize[0], D3D12_TEXTURE_DATA_PITCH_ALIGNMENT) * this->frameBufferScreenCaptureColorFormatBytesPerPixel;
            CD3DX12_TEXTURE_COPY_LOCATION destinationLocation(frameBuffer.screenCaptureBuffer.Get(), destinationFootprint);

            commandList->CopyTextureRegion(&destinationLocation, 0, 0, 0, &sourceLocation, &sourceBox);

            commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(frameBuffer.renderTarget.colorOutputs[0].Get(), D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));

            frameBuffer.screenCaptureSize = this->renderTargetSize;
        }
        frameBuffer.screenCaptureRequested = false;

        if (FINJIN_CHECK_HRESULT_FAILED(commandList->Close()))
        {
            frameBuffer.ResetCommandLists();

            FINJIN_SET_ERROR(error, "Failed to close command list.");
            return;
        }

        frameBuffer.ExecuteCommandLists(this->graphicsCommandQueue.Get(), NewFenceValue());
        EmitFenceValue();
        frameBuffer.WaitForCommandLists(this->fence.Get());

    #if FINJIN_TARGET_VR_SYSTEM != FINJIN_TARGET_VR_SYSTEM_NONE
        if (AnySet(this->settings.frameDestination & GpuFrameDestination::VR_CONTEXT) && this->settings.vrContext != nullptr)
        {
            auto vrContext = static_cast<VRContext*>(this->settings.vrContext);

            MathMatrix4 vrHeadsetViewMatrix;
            vrContext->GetHeadsetViewRenderMatrix(vrHeadsetViewMatrix);
        }

        if (AnySet(this->settings.frameDestination & GpuFrameDestination::VR_CONTEXT) && this->settings.vrContext != nullptr)
        {
            auto vrContext = static_cast<VRContext*>(this->settings.vrContext);
            vrContext->SubmitEyeTextures(this, &frameBuffer);
        }
    #endif

        if (AnySet(this->settings.frameDestination & GpuFrameDestination::SWAP_CHAIN))
        {
            auto presentSyncInterval = static_cast<UINT>(presentSyncIntervalOverride != (size_t)-1 ? presentSyncIntervalOverride : this->settings.presentSyncInterval);
            auto result = this->swapChain->Present(presentSyncInterval, 0);
            if (FINJIN_CHECK_HRESULT_FAILED(result))
            {
                frameBuffer.ResetCommandLists();

                FINJIN_SET_ERROR(error, "Failed to present swap chain buffer.");
                return;
            }
        }

    #if FINJIN_TARGET_VR_SYSTEM != FINJIN_TARGET_VR_SYSTEM_NONE
        if (AnySet(this->settings.frameDestination & GpuFrameDestination::VR_CONTEXT) && this->settings.vrContext != nullptr)
        {
            auto vrContext = static_cast<VRContext*>(this->settings.vrContext);
            vrContext->OnPresentFinish();
        }
    #endif

        frameBuffer.ResetCommandLists();
    }
    else
    {
        //No rendering required
        frameBuffer.WaitForCommandLists(this->fence.Get());
        frameBuffer.ResetCommandLists();
    }
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
            case GpuCommand::Type::START_GRAPHICS_COMMANDS:
            {
                StartGraphicsCommands(frameStage, error);
                if (error)
                {
                    FINJIN_SET_ERROR(error, "Failed to start command list.");
                    return;
                }
                break;
            }
            case GpuCommand::Type::FINISH_GRAPHICS_COMMANDS:
            {
                FinishGraphicsCommands(frameStage, error);
                if (error)
                {
                    FINJIN_SET_ERROR(error, "Failed to finish command list.");
                    return;
                }
                break;
            }
            case GpuCommand::Type::EXECUTE_GRAPHICS_COMMANDS:
            {
                frameBuffer.ExecuteCommandLists(this->graphicsCommandQueue.Get(), NewFenceValue());
                EmitFenceValue();
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

                if (lastRenderTarget == GetRenderTarget(frameStage, Utf8String::GetEmpty()))
                {
                    //Main render target finished
                    auto commandListHandle = frameBuffer.GetCurrentGraphicsCommandList();
                    assert(commandListHandle != nullptr);

                    auto commandList = commandListHandle->commandList.Get();

                    auto& frameBuffer = this->frameBuffers[frameStage.frameBufferIndex];
                    this->staticMeshRenderer.UpdatePassAndMaterialConstants(frameStage.staticMeshRendererFrameState, frameStage.elapsedTime, frameStage.totalElapsedTime);
                    this->staticMeshRenderer.RenderQueued(frameStage.staticMeshRendererFrameState, commandList);
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
                auto commandListHandle = frameBuffer.GetCurrentGraphicsCommandList();
                assert(commandListHandle != nullptr);
                auto commandList = commandListHandle->commandList.Get();
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
                auto commandListHandle = frameBuffer.GetCurrentGraphicsCommandList();
                assert(commandListHandle != nullptr);
                auto commandList = commandListHandle->commandList.Get();
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
                auto commandListHandle = frameBuffer.GetCurrentGraphicsCommandList();
                assert(commandListHandle != nullptr);
                auto commandList = commandListHandle->commandList.Get();
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

                this->staticMeshRenderer.RenderEntity(frameStage.staticMeshRendererFrameState, command.sceneNodeState, command.entity, command.shaderFeatureFlags, command.sortedLights, command.ambientLight);

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
            case GpuCommand::Type::CAPTURE_SCREEN:
            {
                auto& command = static_cast<CaptureScreenGpuCommand&>(baseCommand);

                auto& frameBuffer = this->frameBuffers[frameStage.frameBufferIndex];
                frameBuffer.screenCaptureRequested = true;

                if (command.eventInfo.HasEventID())
                {
                    if (!events.push_back())
                    {
                        FINJIN_SET_ERROR(error, "Failed to record 'capture screen' GPU event.");
                        return;
                    }

                    auto& event = events.back();
                    event.type = GpuEvent::Type::CAPTURE_SCREEN_REQUESTED;
                    event.eventInfo = command.eventInfo;
                }

                break;
            }
        }
    }
}

uint64_t D3D12GpuContextImpl::FlushGpu()
{
    auto v = EmitFenceValue();
    SleepWaitForFenceValue(v);
    return v;
}

void D3D12GpuContextImpl::CreateScreenSizeDependentResources(Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    //Swap chain
    {
        //Render target and view
        for (auto& frameBuffer : this->frameBuffers)
        {
            //Get render target buffer
            Microsoft::WRL::ComPtr<ID3D12Resource> resource;
            if (FINJIN_CHECK_HRESULT_FAILED(this->swapChain->GetBuffer(static_cast<UINT>(frameBuffer.index), IID_PPV_ARGS(&resource))))
            {
                FINJIN_SET_ERROR(error, "Failed to get swap chain buffer.");
                return;
            }

            //Create view on the buffer
            this->device->CreateRenderTargetView(resource.Get(), nullptr, this->rtvDescHeap.GetCpuHeapStart(frameBuffer.index));
        }
    }

    if (this->settings.renderTargetSize.GetType() == GpuRenderTargetSizeType::WINDOW_SIZE)
    {
        auto maxRenderTargetSize = this->settings.renderTargetSize.EvaluateMax(nullptr, &this->windowPixelBounds);

        for (size_t frameBufferIndex = 0; frameBufferIndex < this->frameBuffers.size(); frameBufferIndex++)
        {
            auto& frameBuffer = this->frameBuffers[frameBufferIndex];

            //Color buffer
            frameBuffer.renderTarget.CreateColor(this->device.Get(), maxRenderTargetSize[0], maxRenderTargetSize[1], this->settings.colorFormat.actual, this->settings.multisampleCount.actual, this->settings.multisampleQuality.actual, true, GetBufferCountForFrameDestination(this->settings.frameDestination), error);
            if (error)
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create color render target for frame buffer %1%.", frameBufferIndex));
                return;
            }

            auto& output = frameBuffer.renderTarget.colorOutputs[0];
            output.rtvDescHeapIndex = CalculateOffscreenRenderTargetRTVIndex(this->frameBuffers.size(), frameBuffer.index, 0);
            output.srvDescHeapIndex = CalculateOffscreenRenderTargetSRVIndex(this->frameBuffers.size(), frameBuffer.index, 0);
            this->device->CreateRenderTargetView(output.Get(), nullptr, this->rtvDescHeap.GetCpuHeapStart(output.rtvDescHeapIndex));
            this->device->CreateShaderResourceView(output.Get(), nullptr, this->commonResources.srvTextureDescHeap.GetCpuHeapStart(output.srvDescHeapIndex));

            //Screen capture buffer
            if (this->settings.screenCaptureFrequency.actual != ScreenCaptureFrequency::NEVER)
            {
                auto byteCount = AlignSizeUp(maxRenderTargetSize[0], D3D12_TEXTURE_DATA_PITCH_ALIGNMENT) * maxRenderTargetSize[1] * this->frameBufferScreenCaptureColorFormatBytesPerPixel;
                frameBuffer.CreateScreenCaptureBuffer(this->device.Get(), byteCount, true, error);
                if (error)
                {
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create screen capture buffer for frame buffer %1%.", frameBufferIndex));
                    return;
                }
            }
        }

        //Depth buffer
        if (this->settings.depthStencilFormat.actual != DXGI_FORMAT_UNKNOWN)
        {
            this->depthStencilRenderTarget.CreateDepthStencil(this->device.Get(), maxRenderTargetSize[0], maxRenderTargetSize[1], this->settings.depthStencilFormat.actual, this->settings.maxDepthValue, this->settings.multisampleCount.actual, this->settings.multisampleQuality.actual, true, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to create depth/stencil buffer.");
                return;
            }

            this->depthStencilRenderTarget.depthStencilResource.dsvDescHeapIndex = 0;
            this->device->CreateDepthStencilView(this->depthStencilRenderTarget.depthStencilResource.Get(), nullptr, this->dsvDescHeap.GetCpuHeapStart(this->depthStencilRenderTarget.depthStencilResource.dsvDescHeapIndex));
        }
    }
}

void D3D12GpuContextImpl::DestroyScreenSizeDependentResources(bool resizing)
{
    for (auto& frameBuffer : this->frameBuffers)
        frameBuffer.DestroyScreenSizeDependentResources();

    this->depthStencilRenderTarget.DestroyScreenSizeDependentResources();
}

void D3D12GpuContextImpl::WindowResized(OSWindow* osWindow)
{
    UpdatedCachedWindowSize();

    //FINJIN_DEBUG_LOG_INFO("New resized size: %1% x %2%", this->windowPixelBounds.GetClientWidth(), this->windowPixelBounds.GetClientHeight());
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
    auto srvHandle = this->commonResources.srvTextureDescHeap.GetCpuHeapStart(d3d12Texture->textureIndex);

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

void* D3D12GpuContextImpl::CreateLightFromMainThread(FinjinSceneObjectLight* light, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    std::unique_lock<FiberSpinLock> thisLock(this->createLock);

    auto d3d12Light = this->commonResources.lights.Use();
    if (d3d12Light == nullptr)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to add light '%1%' to collection.", light->name));
        return nullptr;
    }

    d3d12Light->finjinLight = light;

    light->gpuLight = d3d12Light;

    return d3d12Light;
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

    return &d3d12Mesh;
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

ScreenCaptureResult D3D12GpuContextImpl::GetScreenCapture(ScreenCapture& screenCapture, D3D12FrameStage& frameStage)
{
    auto& frameBuffer = this->frameBuffers[frameStage.frameBufferIndex];

    if (frameBuffer.screenCaptureSize[0] > 0 && frameBuffer.screenCaptureSize[1] > 0 &&
        this->frameBufferScreenCapturePixelFormat != ScreenCapturePixelFormat::NONE)
    {
        screenCapture.image = frameBuffer.screenCaptureBufferBytes;
        screenCapture.pixelFormat = this->frameBufferScreenCapturePixelFormat;
        screenCapture.rowStride = AlignSizeUp(frameBuffer.screenCaptureSize[0], D3D12_TEXTURE_DATA_PITCH_ALIGNMENT) * this->frameBufferScreenCaptureColorFormatBytesPerPixel;
        screenCapture.width = frameBuffer.screenCaptureSize[0];
        screenCapture.height = frameBuffer.screenCaptureSize[1];

        frameBuffer.screenCaptureSize[0] = frameBuffer.screenCaptureSize[1] = 0;

        return ScreenCaptureResult::SUCCESS;
    }

    return ScreenCaptureResult::NOT_AVAILABLE;
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
        modeDesc.Width = static_cast<UINT>(this->windowPixelBounds.GetClientWidth());
        modeDesc.Height = static_cast<UINT>(this->windowPixelBounds.GetClientHeight());
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
        if (FINJIN_CHECK_HRESULT_FAILED(resizeTargetResult) && resizeTargetResult != DXGI_ERROR_NOT_CURRENTLY_AVAILABLE) //DXGI_ERROR_NOT_CURRENTLY_AVAILABLE occurs on UWP apps
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

    auto resizeWidth = static_cast<UINT>(this->windowPixelBounds.GetClientWidth());
    auto resizeHeight = static_cast<UINT>(this->windowPixelBounds.GetClientHeight());

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

    this->currentFrameBufferIndex = this->swapChain->GetCurrentBackBufferIndex();
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
        auto foundAt = this->materialMapTypeToElements.find(map.typeName);
        assert(foundAt != this->materialMapTypeToElements.end());
        if (foundAt != this->materialMapTypeToElements.end())
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

D3D12RenderTarget* D3D12GpuContextImpl::GetRenderTarget(D3D12FrameStage& frameStage, const Utf8String& name)
{
    auto& frameBuffer = this->frameBuffers[frameStage.frameBufferIndex];

    if (name.empty())
        return &frameBuffer.renderTarget;

    return nullptr;
}

void D3D12GpuContextImpl::StartGraphicsCommands(D3D12FrameStage& frameStage, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto& frameBuffer = this->frameBuffers[frameStage.frameBufferIndex];

    auto commandList = frameBuffer.NewGraphicsCommandList();
    if (commandList == nullptr)
    {
        FINJIN_SET_ERROR(error, "Failed to get an available graphics command list.");
        return;
    }
}

void D3D12GpuContextImpl::FinishGraphicsCommands(D3D12FrameStage& frameStage, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto& frameBuffer = this->frameBuffers[frameStage.frameBufferIndex];

    //Close command list
    auto commandListHandle = frameBuffer.GetCurrentGraphicsCommandList();
    auto commandList = commandListHandle->commandList.Get();
    if (FINJIN_CHECK_HRESULT_FAILED(commandList->Close()))
    {
        FINJIN_SET_ERROR(error, "Failed to close command list.");
        return;
    }
}

void D3D12GpuContextImpl::StartRenderTarget(D3D12FrameStage& frameStage, D3D12RenderTarget* renderTarget, StaticVector<D3D12RenderTarget*, EngineConstants::MAX_RENDER_TARGET_DEPENDENCIES>& dependentRenderTargets, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto& frameBuffer = this->frameBuffers[frameStage.frameBufferIndex];

    //Hold onto render targets for FinishRenderTarget()----------------------
    renderTarget->defaultViewport.resize(1);
    renderTarget->defaultViewport[0] = this->renderViewport;

    renderTarget->defaultScissorRect.resize(1);
    renderTarget->defaultScissorRect[0] = this->renderScissorRect;

    frameStage.renderTarget = renderTarget;
    frameStage.dependentRenderTargets = dependentRenderTargets;

    //Get command list---------------------
    auto commandListHandle = frameBuffer.GetCurrentGraphicsCommandList();
    auto commandList = commandListHandle->commandList.Get();

    //Indicate that the back buffer will be used as a render target
    StaticVector<CD3DX12_RESOURCE_BARRIER, EngineConstants::MAX_RENDER_TARGET_BARRIERS> barriers;
    {
        for (size_t i = 0; i < renderTarget->colorOutputs.size(); i++)
        {
            if (barriers.push_back(CD3DX12_RESOURCE_BARRIER::Transition(renderTarget->colorOutputs[i].Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET)).HasErrorOrValue(false))
            {
            }
        }
    }
    for (auto dependentRenderTarget : frameStage.dependentRenderTargets)
    {
        for (size_t i = 0; i < dependentRenderTarget->colorOutputs.size(); i++)
        {
            if (barriers.push_back(CD3DX12_RESOURCE_BARRIER::Transition(dependentRenderTarget->colorOutputs[i].Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET)).HasErrorOrValue(false))
            {
            }
        }
    }
    commandList->ResourceBarrier(static_cast<UINT>(barriers.size()), barriers.data());

    //Set render targets----------------------
    auto rtvHandle = this->rtvDescHeap.GetCpuHeapStart(renderTarget->colorOutputs[0].rtvDescHeapIndex);
    auto dsvHandle = this->dsvDescHeap.GetCpuHeapStart(this->depthStencilRenderTarget.depthStencilResource.dsvDescHeapIndex);
    commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, this->depthStencilRenderTarget.HasDepthStencil() ? &dsvHandle : nullptr);

    //Clear render target and depth stencil----------------------
    D3D12_VIEWPORT* viewports = nullptr;
    size_t viewportCount = 0;
    if (!renderTarget->viewports.empty())
    {
        viewports = renderTarget->viewports.data();
        viewportCount = renderTarget->viewports.size();
    }
    else
    {
        viewports = renderTarget->defaultViewport.data();
        viewportCount = renderTarget->defaultViewport.size();
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
        scissorRects = renderTarget->defaultScissorRect.data();
        scissorRectCount = renderTarget->defaultScissorRect.size();
    }
    commandList->RSSetScissorRects(static_cast<UINT>(scissorRectCount), scissorRects);

    commandList->ClearRenderTargetView(rtvHandle, renderTarget->clearColor.IsSet() ? renderTarget->clearColor.value.data() : this->clearColor.data(), static_cast<UINT>(scissorRectCount), scissorRects);
    if (this->depthStencilRenderTarget.HasDepthStencil())
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
        for (size_t i = 0; i < dependentRenderTarget->colorOutputs.size(); i++)
        {
            if (barriers.push_back(CD3DX12_RESOURCE_BARRIER::Transition(dependentRenderTarget->colorOutputs[i].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE)).HasErrorOrValue(false))
            {
            }
        }
    }
    {
        for (size_t i = 0; i < renderTarget->colorOutputs.size(); i++)
        {
            if (barriers.push_back(CD3DX12_RESOURCE_BARRIER::Transition(renderTarget->colorOutputs[i].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE)).HasErrorOrValue(false))
            {
            }
        }
    }
    auto commandListHandle = frameBuffer.GetCurrentGraphicsCommandList();
    auto commandList = commandListHandle->commandList.Get();
    commandList->ResourceBarrier(static_cast<UINT>(barriers.size()), barriers.data());
}

uint64_t D3D12GpuContextImpl::NewFenceValue()
{
    return this->fenceValue++;
}

uint64_t D3D12GpuContextImpl::EmitFenceValue()
{
    auto emittedValue = this->fenceValue++;
    if (FINJIN_CHECK_HRESULT_FAILED(this->graphicsCommandQueue->Signal(this->fence.Get(), emittedValue)))
    {
    }

    return emittedValue;
}

void D3D12GpuContextImpl::SleepWaitForFenceValue(uint64_t fenceValue)
{
    const uint64_t lastCompletedFenceValue = this->fence->GetCompletedValue();
    if (lastCompletedFenceValue < fenceValue)
    {
        if (FINJIN_CHECK_HRESULT_FAILED(this->fence->SetEventOnCompletion(fenceValue, this->fenceEventHandle)))
            return;

        WaitForSingleObjectEx(this->fenceEventHandle, INFINITE, FALSE);
    }
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
            static_cast<size_t>(this->windowPixelBounds.GetClientWidth()),
            static_cast<size_t>(this->windowPixelBounds.GetClientHeight()),
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
        bestWidth = static_cast<UINT>(this->windowPixelBounds.GetClientWidth());
        bestHeight = static_cast<UINT>(this->windowPixelBounds.GetClientHeight());
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

void D3D12GpuContextImpl::UpdatedCachedWindowSize()
{
    this->windowPixelBounds = this->settings.osWindow->GetWindowSize().GetSafeCurrentBounds();
    this->windowPixelBounds.Scale(this->settings.osWindow->GetDisplayDensity());

    this->windowViewport.TopLeftX = 0;
    this->windowViewport.TopLeftY = 0;
    this->windowViewport.Width = RoundToFloat(this->windowPixelBounds.GetClientWidth());
    this->windowViewport.Height = RoundToFloat(this->windowPixelBounds.GetClientHeight());
    this->windowViewport.MinDepth = 0;
    this->windowViewport.MaxDepth = this->settings.maxDepthValue;

    this->windowScissorRect.left = 0;
    this->windowScissorRect.top = 0;
    this->windowScissorRect.right = static_cast<LONG>(this->windowPixelBounds.GetClientWidth());
    this->windowScissorRect.bottom = static_cast<LONG>(this->windowPixelBounds.GetClientHeight());


    this->renderTargetSize = this->settings.renderTargetSize.Evaluate(nullptr, &this->windowPixelBounds);

    this->renderViewport.TopLeftX = 0;
    this->renderViewport.TopLeftY = 0;
    this->renderViewport.Width = RoundToFloat(this->renderTargetSize[0]);
    this->renderViewport.Height = RoundToFloat(this->renderTargetSize[1]);
    this->renderViewport.MinDepth = 0;
    this->renderViewport.MaxDepth = this->settings.maxDepthValue;

    this->renderScissorRect.left = 0;
    this->renderScissorRect.top = 0;
    this->renderScissorRect.right = static_cast<LONG>(this->renderTargetSize[0]);
    this->renderScissorRect.bottom = static_cast<LONG>(this->renderTargetSize[1]);

}

#endif
