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

#if FINJIN_TARGET_GPU_SYSTEM == FINJIN_TARGET_GPU_SYSTEM_METAL

#include "MetalGpuContextImpl.hpp"
#include "finjin/common/Convert.hpp"
#include "finjin/common/DebugLog.hpp"
#include "finjin/common/PNGReader.hpp"
#include "finjin/common/WrappedFileReader.hpp"
#include "finjin/engine/OSWindow.hpp"
#include "finjin/engine/OSWindowImpl.hpp"
#include "finjin/engine/StandardAssetDocumentPropertyValues.hpp"
#include "MetalSystem.hpp"
#include "MetalSystemImpl.hpp"

using namespace Finjin::Engine;


//Local types-------------------------------------------------------------------
struct MetalFullScreenQuadKnownBindings
{
    struct VertexStageBuffer
    {
        enum
        {
            VERTEX,
            MODEL_VIEW_PROJECTION_MATRIX
        };
    };

    struct FragmentStageBuffer
    {
        enum
        {
            TEXTURE
        };
    };
};


//Implementation----------------------------------------------------------------

//MetalGpuContextImpl::InternalSettings
MetalGpuContextImpl::InternalSettings::InternalSettings()
{
    this->contextImpl = nullptr;
}

void MetalGpuContextImpl::InternalSettings::ParseSettings(const ByteBufferReader& configFileBuffer, const Path& assetFilePath, Error& error)
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

void MetalGpuContextImpl::InternalSettings::Validate(Error& error)
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

GpuInputFormatStruct* MetalGpuContextImpl::InternalSettings::GetInputFormat(const Utf8String& typeName, const DynamicVector<FinjinVertexElementFormat>& elements)
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

//MetalGpuContextImpl
MetalGpuContextImpl::MetalGpuContextImpl(Allocator* allocator) :
    AllocatedClass(allocator),
    GpuContextCommonImpl(allocator),
    settings(allocator),
    materialMapTypeToElements
        (
        StandardAssetDocumentPropertyValues::TypeName::MATERIAL_MAP_DIFFUSE, GpuMaterialMapIndexToConstantBufferElements<MetalMaterial::MapIndex>(MetalMaterial::MapIndex::DIFFUSE, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_DIFFUSE_TEXTURE_INDEX, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_DIFFUSE_AMOUNT, ShaderFeatureFlag::MAP_DIFFUSE),
        StandardAssetDocumentPropertyValues::TypeName::MATERIAL_MAP_SPECULAR, GpuMaterialMapIndexToConstantBufferElements<MetalMaterial::MapIndex>(MetalMaterial::MapIndex::SPECULAR, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_SPECULAR_TEXTURE_INDEX, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_SPECULAR_AMOUNT, ShaderFeatureFlag::MAP_SPECULAR),
        StandardAssetDocumentPropertyValues::TypeName::MATERIAL_MAP_REFLECTION, GpuMaterialMapIndexToConstantBufferElements<MetalMaterial::MapIndex>(MetalMaterial::MapIndex::REFLECTION, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_REFLECTION_TEXTURE_INDEX, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_REFLECTION_AMOUNT, ShaderFeatureFlag::MAP_REFLECTION),
        StandardAssetDocumentPropertyValues::TypeName::MATERIAL_MAP_REFRACTION, GpuMaterialMapIndexToConstantBufferElements<MetalMaterial::MapIndex>(MetalMaterial::MapIndex::REFRACTION, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_REFRACTION_TEXTURE_INDEX, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_REFRACTION_AMOUNT, ShaderFeatureFlag::MAP_REFRACTION),
        StandardAssetDocumentPropertyValues::TypeName::MATERIAL_MAP_BUMP, GpuMaterialMapIndexToConstantBufferElements<MetalMaterial::MapIndex>(MetalMaterial::MapIndex::BUMP, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_BUMP_TEXTURE_INDEX, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_BUMP_AMOUNT, ShaderFeatureFlag::MAP_BUMP),
        StandardAssetDocumentPropertyValues::TypeName::MATERIAL_MAP_HEIGHT, GpuMaterialMapIndexToConstantBufferElements<MetalMaterial::MapIndex>(MetalMaterial::MapIndex::HEIGHT, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_HEIGHT_TEXTURE_INDEX, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_HEIGHT_AMOUNT, ShaderFeatureFlag::MAP_HEIGHT),
        StandardAssetDocumentPropertyValues::TypeName::MATERIAL_MAP_SELF_ILLUMINATION, GpuMaterialMapIndexToConstantBufferElements<MetalMaterial::MapIndex>(MetalMaterial::MapIndex::SELF_ILLUMINATION, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_SELF_ILLUMINATION_TEXTURE_INDEX, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_SELF_ILLUMINATION_AMOUNT, ShaderFeatureFlag::MAP_SELF_ILLUMINATION),
        StandardAssetDocumentPropertyValues::TypeName::MATERIAL_MAP_OPACITY, GpuMaterialMapIndexToConstantBufferElements<MetalMaterial::MapIndex>(MetalMaterial::MapIndex::OPACITY, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_OPACITY_TEXTURE_INDEX, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_OPACITY_AMOUNT, ShaderFeatureFlag::MAP_OPACITY),
        StandardAssetDocumentPropertyValues::TypeName::MATERIAL_MAP_ENVIRONMENT, GpuMaterialMapIndexToConstantBufferElements<MetalMaterial::MapIndex>(MetalMaterial::MapIndex::ENVIRONMENT, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_ENVIRONMENT_TEXTURE_INDEX, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_ENVIRONMENT_AMOUNT, ShaderFeatureFlag::MAP_ENVIRONMENT),
        StandardAssetDocumentPropertyValues::TypeName::MATERIAL_MAP_SHININESS, GpuMaterialMapIndexToConstantBufferElements<MetalMaterial::MapIndex>(MetalMaterial::MapIndex::SHININESS, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_SHININESS_TEXTURE_INDEX, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_SHININESS_AMOUNT, ShaderFeatureFlag::MAP_SHININESS)
        ),
    commonResources(allocator)
{
    this->device = nullptr;

    this->frameBufferScreenCaptureColorFormatBytesPerPixel = 0;
    this->frameBufferScreenCapturePixelFormat = ScreenCapturePixelFormat::NONE;

    this->metalSystem = nullptr;

    FINJIN_ZERO_ITEM(this->renderViewport);
    FINJIN_ZERO_ITEM(this->renderScissorRect);
}

void MetalGpuContextImpl::Create(const MetalGpuContextSettings& settings, Error& error)
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
        FINJIN_SET_ERROR(error, "Failed to find a suitable Metal device.");
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

void MetalGpuContextImpl::Destroy()
{
    this->settings.osWindow->RemoveWindowEventListener(this);

    if (this->device != nullptr)
    {
        for (auto& frameBuffer : this->frameBuffers)
            frameBuffer.Destroy();

        for (auto& frameStage : this->frameStages)
            frameStage.Destroy();

        this->depthStencilRenderTarget.Destroy();

        this->staticMeshRenderer.Destroy();

        this->assetResources.Destroy();
        this->commonResources.Destroy();

        this->device = nullptr;
    }
}

void MetalGpuContextImpl::CreateDevice(Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    //Find device
    this->device = this->metalSystem->GetImpl()->GetBestMetalDevice(this->settings.gpuID, this->deviceDescription, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to find best device.");
        return;
    }

    //Determine color format
    if (!MetalUtilities::GetBestColorFormat(this->settings.colorFormat, this->deviceDescription.featureSet.capabilities))
    {
        FINJIN_SET_ERROR(error, "Failed to determine supported color format.");
        return;
    }
    this->frameBufferScreenCaptureColorFormatBytesPerPixel = MetalUtilities::GetBitsPerPixel(static_cast<MTLPixelFormat>(this->settings.colorFormat.actual)) / 8;
    switch (static_cast<MTLPixelFormat>(this->settings.colorFormat.actual))
    {
        case MTLPixelFormatBGRA8Unorm: this->frameBufferScreenCapturePixelFormat = ScreenCapturePixelFormat::BGRA8_UNORM; break;
        case MTLPixelFormatBGRA8Unorm_sRGB: this->frameBufferScreenCapturePixelFormat = ScreenCapturePixelFormat::BGRA8_UNORM_SRGB; break;
        case MTLPixelFormatRGBA16Float: this->frameBufferScreenCapturePixelFormat = ScreenCapturePixelFormat::RGBA16_FLOAT; break;
        default: assert(0); this->frameBufferScreenCapturePixelFormat = ScreenCapturePixelFormat::NONE; break;
    }

    //Determine depth/stencil format
    if (!MetalUtilities::GetBestDepthStencilFormat(this->settings.depthStencilFormat, this->settings.stencilRequired, this->deviceDescription.featureSet.capabilities))
    {
        FINJIN_SET_ERROR(error, "Failed to determine supported depth/stencil format.");
        return;
    }

    //Initialize Metal layer
    auto metalLayer = this->settings.osWindow->GetImpl()->GetMetalLayer();
    metalLayer.device = this->device;
    metalLayer.pixelFormat = static_cast<MTLPixelFormat>(this->settings.colorFormat.actual);

    //Determine some settings-------------------------------------------
    UpdatedCachedWindowSize();
    auto maxRenderTargetSize = this->settings.renderTargetSize.EvaluateMax(nullptr, &this->windowPixelBounds);

    this->settings.screenCaptureFrequency.actual = this->settings.screenCaptureFrequency.requested;

    this->settings.frameBufferCount.actual = this->settings.frameBufferCount.requested;
    this->frameBuffers.resize(this->settings.frameBufferCount.actual);
    for (size_t frameBufferIndex = 0; frameBufferIndex < this->frameBuffers.size(); frameBufferIndex++)
    {
        auto& frameBuffer = this->frameBuffers[frameBufferIndex];

        frameBuffer.SetIndex(frameBufferIndex);

        frameBuffer.commandQueue = [this->device newCommandQueue];

        frameBuffer.CreateCommandBuffers(this->settings.maxGpuCommandListsPerStage, GetAllocator(), error);
        if (error)
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create command buffer objects for frame buffer %1%.", frameBufferIndex));
            return;
        }

        if (this->settings.renderTargetSize.GetType() == GpuRenderTargetSizeType::EXPLICIT_SIZE)
        {
            //Create color buffer
            frameBuffer.renderTarget.CreateColor(this->device, maxRenderTargetSize[0], maxRenderTargetSize[1], static_cast<MTLPixelFormat>(this->settings.colorFormat.actual), false, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create color render target for frame buffer %1%.", frameBufferIndex));
                return;
            }

            //Create screen capture buffer
            if (this->settings.screenCaptureFrequency.actual != ScreenCaptureFrequency::NEVER)
            {
                auto byteCount = this->frameBufferScreenCaptureColorFormatBytesPerPixel * maxRenderTargetSize[0] * maxRenderTargetSize[1];
                frameBuffer.CreateScreenCaptureBuffer(this->device, byteCount, false, error);
                if (error)
                {
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create screen capture buffer for frame buffer %1%.", frameBufferIndex));
                    return;
                }
            }
        }
    }

    if (this->settings.renderTargetSize.GetType() == GpuRenderTargetSizeType::EXPLICIT_SIZE &&
        this->settings.depthStencilFormat.actual != MTLPixelFormatInvalid)
    {
        this->depthStencilRenderTarget.CreateDepthStencil(this->device, maxRenderTargetSize[0], maxRenderTargetSize[1], static_cast<MTLPixelFormat>(this->settings.depthStencilFormat.actual), false, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to create depth/stencil buffer.");
            return;
        }
    }

    this->settings.jobProcessingPipelineSize = GpuContextCommonSettings::CalculateJobPipelineSize(this->settings.frameBufferCount.actual);
    this->frameStages.resize(this->settings.jobProcessingPipelineSize);
    for (size_t frameStageIndex = 0; frameStageIndex < this->frameStages.size(); frameStageIndex++)
    {
        auto& frameStage = this->frameStages[frameStageIndex];

        frameStage.SetIndex(frameStageIndex);
    }
}

void MetalGpuContextImpl::CreateDeviceSupportObjects(Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    //Set more selector settings-----------

    //Shader language version used to compile shaders
    this->settings.initialAssetFileSelector.Set(AssetPathComponent::GPU_SHADER_MODEL, "1.2");

    //Feature level
    auto featureLevel = this->deviceDescription.GetFeatureSetString();
    this->settings.initialAssetFileSelector.Set(AssetPathComponent::GPU_FEATURE_LEVEL, featureLevel);

    //Preferred texture format
    if (this->deviceDescription.featureSet.availability.astcPixelFormats)
        this->settings.initialAssetFileSelector.Set(AssetPathComponent::GPU_PREFERRED_TEXTURE_FORMAT, "astc");
    else if (this->deviceDescription.featureSet.availability.eacAndEtcPixelFormats)
        this->settings.initialAssetFileSelector.Set(AssetPathComponent::GPU_PREFERRED_TEXTURE_FORMAT, "etc2");
    else if (this->deviceDescription.featureSet.availability.bcPixelFormats)
        this->settings.initialAssetFileSelector.Set(AssetPathComponent::GPU_PREFERRED_TEXTURE_FORMAT, "bc");

    //GPU model
    Utf8String deviceDescription(this->device.description.UTF8String);
    auto deviceNameFoundAt = deviceDescription.find("name = ");
    if (deviceNameFoundAt != Utf8String::npos)
    {
        deviceDescription.pop_front(deviceNameFoundAt + 7); //strlen("name = ") = 7
        this->settings.initialAssetFileSelector.Set(AssetPathComponent::GPU_MODEL, deviceDescription);
    }

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
    MetalInputFormat::CreateUnorderedMap(this->assetResources.inputFormatsByNameHash, GetAllocator(), this->internalSettings.inputFormats, error);
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

    //Create texture samplers
    {
        this->commonResources.defaultSamplerDescriptor = [[MTLSamplerDescriptor alloc] init];
        this->commonResources.defaultSamplerDescriptor.minFilter = MTLSamplerMinMagFilterLinear;
        this->commonResources.defaultSamplerDescriptor.magFilter = MTLSamplerMinMagFilterLinear;
        this->commonResources.defaultSamplerDescriptor.sAddressMode = MTLSamplerAddressModeRepeat;
        this->commonResources.defaultSamplerDescriptor.tAddressMode = MTLSamplerAddressModeRepeat;
        this->commonResources.defaultSamplerDescriptor.rAddressMode = MTLSamplerAddressModeRepeat;
        this->commonResources.defaultSamplerDescriptor.mipFilter = MTLSamplerMipFilterLinear;
        this->commonResources.defaultSamplerDescriptor.maxAnisotropy = 1;
        this->commonResources.defaultSamplerDescriptor.normalizedCoordinates = YES;
        this->commonResources.defaultSamplerDescriptor.lodMinClamp = 0.0f;
        this->commonResources.defaultSamplerDescriptor.lodMaxClamp = FLT_MAX;
        this->commonResources.defaultSampler = [this->device newSamplerStateWithDescriptor:this->commonResources.defaultSamplerDescriptor];
    }

    //Create full screen shader resources
    {
        this->workingAssetReference.ForUriString("common-shaders.metallib", this->workingUri, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to parse common shaders asset reference.");
            return;
        }

        this->commonResources.commonShaderLibrary.Create(this->device, this->workingAssetReference, this->shaderAssetClassFileReader, this->readBuffer, GetAllocator(), error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to create common shader library.");
            return;
        }

        this->commonResources.fullScreenQuadShaders.vertexShader.Create(this->commonResources.commonShaderLibrary.metalLibrary, "fullscreen_quad_vertex_main", error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to create vertex shader 'fullscreen_quad_vertex_main'.");
            return;
        }

        this->commonResources.fullScreenQuadShaders.fragmentShader.Create(this->commonResources.commonShaderLibrary.metalLibrary, "fullscreen_quad_fragment_main", error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to create fragment shader 'fullscreen_quad_fragment_main'.");
            return;
        }

        this->commonResources.fullScreenQuadMesh.Create(this->device, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to create full screen quad mesh.");
            return;
        }

        this->commonResources.fullScreenQuadGraphicsPipelineStateDesc = [[MTLRenderPipelineDescriptor alloc] init];
        this->commonResources.fullScreenQuadGraphicsPipelineStateDesc.sampleCount = 1;
        this->commonResources.fullScreenQuadGraphicsPipelineStateDesc.vertexFunction = this->commonResources.fullScreenQuadShaders.vertexShader.metalFunction;
        this->commonResources.fullScreenQuadGraphicsPipelineStateDesc.fragmentFunction = this->commonResources.fullScreenQuadShaders.fragmentShader.metalFunction;
        this->commonResources.fullScreenQuadGraphicsPipelineStateDesc.vertexDescriptor = this->commonResources.fullScreenQuadMesh.vertexDescriptor;
        this->commonResources.fullScreenQuadGraphicsPipelineStateDesc.colorAttachments[0].pixelFormat = static_cast<MTLPixelFormat>(this->settings.colorFormat.actual);
        {
            NSError* nserror = nullptr;
            this->commonResources.fullScreenQuadGraphicsPipelineState = [this->device newRenderPipelineStateWithDescriptor:this->commonResources.fullScreenQuadGraphicsPipelineStateDesc error:&nserror];
            if (this->commonResources.fullScreenQuadGraphicsPipelineState == nullptr)
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create full screen quad Metal pipeline: %1%", nserror.localizedDescription.UTF8String));
                return;
            }
        }
    }

    CreateScreenSizeDependentResources(error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to create GPU core assets.");
        return;
    }
}

void MetalGpuContextImpl::CreateRenderers(Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    //Static mesh renderer
    {
        //Settings
        MetalStaticMeshRenderer::Settings staticMeshRendererSettings;
        staticMeshRendererSettings.contextImpl = this;
        for (auto fileName : { "gpu-context-static-mesh-renderer-render-states.cfg" })
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

MetalFrameStage& MetalGpuContextImpl::StartFrameStage(size_t index, SimpleTimeDelta elapsedTime, SimpleTimeCounter totalElapsedTime)
{
    auto& frameStage = this->frameStages[index];

    auto frameBufferIndex = this->currentFrameBufferIndex++ % this->frameBuffers.size();

    auto& frameBuffer = this->frameBuffers[frameBufferIndex];

    //Wait for previous frame to complete
    frameBuffer.WaitForNotInUse();
    frameStage.frameBufferIndex = frameBufferIndex;

    //Update state
    frameStage.elapsedTime = elapsedTime;
    frameStage.totalElapsedTime = totalElapsedTime;
    frameStage.sequenceIndex = this->sequenceIndex++;

    frameBuffer.CreateFreeCommandBuffers();

    return frameStage;
}

void MetalGpuContextImpl::PresentFrameStage(MetalFrameStage& frameStage, const RenderStatus& renderStatus, size_t presentSyncIntervalOverride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto& frameBuffer = this->frameBuffers[frameStage.frameBufferIndex];

    //Process update
    if (renderStatus.IsRenderingRequired())
    {
        //Get next drawable/back buffer
        auto metalLayer = this->settings.osWindow->GetImpl()->GetMetalLayer();
        auto drawable = metalLayer.nextDrawable;
        assert(drawable != nullptr);
        if (drawable == nullptr)
        {
            frameBuffer.WaitForCommandBuffers();
            frameBuffer.ResetCommandBuffers();

            FINJIN_SET_ERROR(error, "Failed to get next drawable.");
            return;
        }

        //Encode commands to draw the off screen render target onto a full screen quad on the back buffer----------------
        auto commandBuffer = frameBuffer.NewGraphicsCommandBuffer();
        assert(commandBuffer != nullptr);

        //Color attachment
        auto colorAttachment = frameBuffer.renderTarget.renderPassDescriptor.colorAttachments[0];
        colorAttachment.texture = drawable.texture;
        colorAttachment.loadAction = MTLLoadActionLoad;
        colorAttachment.storeAction = MTLStoreActionStore;

        //Depth/stencil attachments
        auto depthAttachment = frameBuffer.renderTarget.renderPassDescriptor.depthAttachment;
        depthAttachment.texture = nullptr;
        depthAttachment.loadAction = MTLLoadActionLoad;
        depthAttachment.storeAction = MTLStoreActionStore;

        if (MetalUtilities::IsDepthStencilFormat(static_cast<MTLPixelFormat>(this->settings.depthStencilFormat.actual)))
        {
            auto stencilAttachment = frameBuffer.renderTarget.renderPassDescriptor.stencilAttachment;
            stencilAttachment.texture = nullptr;
            stencilAttachment.loadAction = MTLLoadActionLoad;
            stencilAttachment.storeAction = MTLStoreActionStore;
        }

        auto renderCommandEncoder = [commandBuffer renderCommandEncoderWithDescriptor:frameBuffer.renderTarget.renderPassDescriptor];

        [renderCommandEncoder setViewport:this->windowViewport];
        [renderCommandEncoder setScissorRect:this->windowScissorRect];

        [renderCommandEncoder setCullMode:MTLCullModeNone];

        [renderCommandEncoder setRenderPipelineState:this->commonResources.fullScreenQuadGraphicsPipelineState];

        [renderCommandEncoder setVertexBuffer:this->commonResources.fullScreenQuadMesh.vertexBuffer offset:0 atIndex:MetalFullScreenQuadKnownBindings::VertexStageBuffer::VERTEX];

        MathMatrix4 modelViewProjectionMatrix;
        {
            modelViewProjectionMatrix.setIdentity();
            /*auto translation = MathAffineTransform(MathTranslation(0.0f, 0.0f, 0.0f)); //Translation is in normalized device coordinates [-1, 1]
             auto scale = MathAffineTransform(MathScaling(1.0f, 1.0f, 1.0f));
             modelViewProjectionMatrix = (translation * scale).matrix();*/
        }
        MathMatrix4Values metalMatrix;
        GetColumnOrderMatrixData(metalMatrix, modelViewProjectionMatrix);
        [renderCommandEncoder setVertexBytes:metalMatrix.a length:sizeof(metalMatrix) atIndex:MetalFullScreenQuadKnownBindings::VertexStageBuffer::MODEL_VIEW_PROJECTION_MATRIX];

        //MathVector4 multiplyColor(1,1,1,1);
        //[renderCommandEncoder setFragmentBytes:multiplyColor.data() length:4*sizeof(float) atIndex:MetalFullScreenQuadKnownBindings::FragmentStage::MULTIPLY_COLOR];

        //MathVector4 addColor(0,0,0,0);
        //[renderCommandEncoder setFragmentBytes:addColor.data() length:4*sizeof(float) atIndex:MetalFullScreenQuadKnownBindings::FragmentStage::ADD_COLOR];

        [renderCommandEncoder setFragmentTexture:frameBuffer.renderTarget.colorOutputs[0].GetTexture() atIndex:MetalFullScreenQuadKnownBindings::FragmentStageBuffer::TEXTURE];

        [renderCommandEncoder drawPrimitives:this->commonResources.fullScreenQuadMesh.primitiveType vertexStart:0 vertexCount:this->commonResources.fullScreenQuadMesh.vertexCount];

        [renderCommandEncoder endEncoding];
        renderCommandEncoder = nullptr;

        if ((this->settings.screenCaptureFrequency.actual == ScreenCaptureFrequency::EVERY_FRAME ||
            (this->settings.screenCaptureFrequency.actual == ScreenCaptureFrequency::ON_REQUEST && frameBuffer.screenCaptureRequested)) &&
            frameBuffer.screenCaptureBuffer != nullptr)
        {
            auto blitCommandEncoder = [commandBuffer blitCommandEncoder];

            [blitCommandEncoder copyFromTexture:frameBuffer.renderTarget.colorOutputs[0].GetTexture()
                sourceSlice:0
                sourceLevel:0
                sourceOrigin:MTLOriginMake(0, 0, 0)
                sourceSize:MTLSizeMake(this->renderTargetSize[0], this->renderTargetSize[1], 1)
                toBuffer:frameBuffer.screenCaptureBuffer
                destinationOffset:0
                destinationBytesPerRow:(this->renderTargetSize[0] * static_cast<uint32_t>(this->frameBufferScreenCaptureColorFormatBytesPerPixel))
                destinationBytesPerImage:(this->renderTargetSize[0] * this->renderTargetSize[1] * static_cast<uint32_t>(this->frameBufferScreenCaptureColorFormatBytesPerPixel))];

            frameBuffer.screenCaptureSize = this->renderTargetSize;

            [blitCommandEncoder endEncoding];
        }
        frameBuffer.screenCaptureRequested = false;

        //Encode the "present" command
        [commandBuffer presentDrawable:drawable];

        //Commit the encoded commands
        frameBuffer.CommitCommandBuffers();
        frameBuffer.WaitForCommandBuffers();
        frameBuffer.ResetCommandBuffers();
        colorAttachment.texture = nullptr;
    }
    else
    {
        //No rendering required
        frameBuffer.WaitForCommandBuffers();
        frameBuffer.ResetCommandBuffers();
    }
}

void MetalGpuContextImpl::Execute(MetalFrameStage& frameStage, GpuEvents& events, GpuCommands& commands, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto& frameBuffer = this->frameBuffers[frameStage.frameBufferIndex];

    MetalRenderTarget* lastRenderTarget = nullptr;

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
                frameBuffer.CommitCommandBuffers();
                break;
            }
            case GpuCommand::Type::START_RENDER_TARGET:
            {
                auto& command = static_cast<StartRenderTargetGpuCommand&>(baseCommand);

                auto renderTarget = GetRenderTarget(frameStage, command.renderTargetName);
                StaticVector<MetalRenderTarget*, EngineConstants::MAX_RENDER_TARGET_DEPENDENCIES> dependentRenderTargets;
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
                //auto& command = static_cast<FinishRenderTargetGpuCommand&>(baseCommand);

                if (lastRenderTarget == nullptr)
                {
                    FINJIN_SET_ERROR(error, "Failed to finish render target. There was no render target to finish.");
                    return;
                }

                if (lastRenderTarget == GetRenderTarget(frameStage, Utf8String::GetEmpty()))
                {
                    //Main render target finished
                    auto commandBuffer = frameBuffer.GetCurrentGraphicsCommandBuffer();
                    assert(commandBuffer != nullptr);

                    //auto& frameBuffer = this->frameBuffers[frameStage.frameBufferIndex];
                    this->staticMeshRenderer.UpdatePassAndMaterialConstants(frameStage.staticMeshRendererFrameState, frameStage.elapsedTime, frameStage.totalElapsedTime);
                    this->staticMeshRenderer.RenderQueued(frameStage.staticMeshRendererFrameState, commandBuffer, lastRenderTarget->renderCommandEncoder);
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

                //auto& frameBuffer = this->frameBuffers[frameStage.frameBufferIndex];

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
            default: break;
        }
    }
}

void MetalGpuContextImpl::FlushGpu()
{
    //Do nothing. No special handling necessary for now
}

void MetalGpuContextImpl::CreateScreenSizeDependentResources(Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    //Update layer drawable size
    auto drawableSize = CGSizeMake(this->windowPixelBounds.GetClientWidth(), this->windowPixelBounds.GetClientHeight());
    auto metalLayer = this->settings.osWindow->GetImpl()->GetMetalLayer();
    metalLayer.drawableSize = drawableSize;

    //Create render targets
    if (this->settings.renderTargetSize.GetType() == GpuRenderTargetSizeType::WINDOW_SIZE)
    {
        auto maxRenderTargetSize = this->settings.renderTargetSize.EvaluateMax(nullptr, &this->windowPixelBounds);

        //Create frame buffer objects
        for (size_t frameBufferIndex = 0; frameBufferIndex < this->frameBuffers.size(); frameBufferIndex++)
        {
            auto& frameBuffer = this->frameBuffers[frameBufferIndex];

            //Create color buffer
            frameBuffer.renderTarget.CreateColor(this->device, maxRenderTargetSize[0], maxRenderTargetSize[1], static_cast<MTLPixelFormat>(this->settings.colorFormat.actual), true, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create color render target for frame buffer %1%.", frameBufferIndex));
                return;
            }

            //Create screen capture buffer
            if (this->settings.screenCaptureFrequency.actual != ScreenCaptureFrequency::NEVER)
            {
                auto byteCount = this->frameBufferScreenCaptureColorFormatBytesPerPixel * maxRenderTargetSize[0] * maxRenderTargetSize[1];
                frameBuffer.CreateScreenCaptureBuffer(this->device, byteCount, true, error);
                if (error)
                {
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create screen capture buffer for frame buffer %1%.", frameBufferIndex));
                    return;
                }
            }
        }

        //Create depth buffer
        if (this->settings.depthStencilFormat.actual != MTLPixelFormatInvalid)
        {
            this->depthStencilRenderTarget.CreateDepthStencil(this->device, maxRenderTargetSize[0], maxRenderTargetSize[1], static_cast<MTLPixelFormat>(this->settings.depthStencilFormat.actual), true, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to create depth/stencil buffer.");
                return;
            }
        }
    }
}

void MetalGpuContextImpl::DestroyScreenSizeDependentResources(bool resizing)
{
    for (auto& frameBuffer : this->frameBuffers)
        frameBuffer.DestroyScreenSizeDependentResources();

    this->depthStencilRenderTarget.DestroyScreenSizeDependentResources();
}

void MetalGpuContextImpl::WindowResized(OSWindow* osWindow)
{
    UpdatedCachedWindowSize();
}

MetalTexture* MetalGpuContextImpl::CreateTextureFromMainThread(FinjinTexture* texture, Error& error)
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

    auto isPNG = false;
    auto foundWrappedFormat = false;

    WrappedFileReader wrappedFileReader;
    auto wrappedReadHeaderResult = wrappedFileReader.ReadHeader(reader);
    if (wrappedReadHeaderResult == WrappedFileReader::ReadHeaderResult::SUCCESS)
    {
        if (wrappedFileReader.GetHeader().fileExtension == "png")
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


    MetalTexture* metalTexture = nullptr;

    if (metalTexture == nullptr && (isPNG || !foundWrappedFormat))
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

            metalTexture = addedTexture.value;

            metalTexture->textureDimension = TextureDimension::DIMENSION_2D;

            metalTexture->CreateFromPNG
                (
                pngReader,
                this->device,
                this->readBuffer.data(),
                this->readBuffer.size(),
                0,
                error
                );
            if (error)
            {
                metalTexture->HandleCreationFailure();
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

    if (metalTexture == nullptr)
    {
        FINJIN_SET_ERROR(error, "The texture file contained an invalid signature.");
        return nullptr;
    }

    auto metalTextureIndex = this->assetResources.textureOffsetsByDimension[metalTexture->textureDimension].GetCurrentOffset();
    metalTexture->textureIndex = metalTextureIndex;
    if (metalTexture->name.assign(texture->name).HasError())
    {
        metalTexture->HandleCreationFailure();
        this->assetResources.texturesByNameHash.remove(texture->name.GetHash());

        FINJIN_SET_ERROR(error, "Failed to assign texture name.");
        return nullptr;
    }

    this->assetResources.textureOffsetsByDimension[metalTexture->textureDimension].count++;

    texture->gpuTexture = metalTexture;

    return metalTexture;
}

void* MetalGpuContextImpl::CreateMaterialFromMainThread(FinjinMaterial* material, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    std::unique_lock<FiberSpinLock> thisLock(this->createLock);

    this->assetResources.ValidateMaterialForCreation(material->name, error);
    if (error)
    {
        FINJIN_SET_ERROR_NO_MESSAGE(error);
        return nullptr;
    }

    auto metalMaterialIndex = this->assetResources.materialsByNameHash.size();

    auto addedMaterial = this->assetResources.materialsByNameHash.GetOrAdd(material->name.GetHash());
    if (addedMaterial.HasErrorOrValue(nullptr))
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to add material '%1%' to collection.", material->name));
        return nullptr;
    }

    auto& metalMaterial = *addedMaterial.value;
    metalMaterial.finjinMaterial = material;
    metalMaterial.gpuBufferIndex = metalMaterialIndex;

    material->gpuMaterial = &metalMaterial;

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

            metalMaterial.maps[map.gpuMaterialMapIndex].finjinMap = &map;
            material->gpuMapFlags |= map.gpuMaterialMapFlag;
        }
    }
    ResolveMaterialMaps(*material);

    return &metalMaterial;
}

void* MetalGpuContextImpl::CreateLightFromMainThread(FinjinSceneObjectLight* light, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    std::unique_lock<FiberSpinLock> thisLock(this->createLock);

    auto metalLight = this->commonResources.lights.Use();
    if (metalLight == nullptr)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to add light '%1%' to collection.", light->name));
        return nullptr;
    }

    metalLight->finjinLight = light;

    light->gpuLight = metalLight;

    return metalLight;
}

void* MetalGpuContextImpl::CreateMeshFromMainThread(FinjinMesh* mesh, Error& error)
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

    auto& metalMesh = *addedMesh.value;

    metalMesh.sharedIndexBuffer.CreateIndexBuffer(mesh->indexBuffer, GetAllocator(), this->device, error);
    if (error)
    {
        metalMesh.HandleCreationFailure();
        this->assetResources.meshesByNameHash.remove(mesh->name.GetHash());

        FINJIN_SET_ERROR(error, "Failed to create main mesh index buffer.");
        return nullptr;
    }

    if (!mesh->vertexBuffers.empty())
    {
        if (!metalMesh.sharedVertexBuffers.Create(mesh->vertexBuffers.size(), GetAllocator()))
        {
            metalMesh.HandleCreationFailure();
            this->assetResources.meshesByNameHash.remove(mesh->name.GetHash());

            FINJIN_SET_ERROR(error, "Failed to create shared vertex buffers collection.");
            return nullptr;
        }

        for (size_t vertexBufferIndex = 0; vertexBufferIndex < mesh->vertexBuffers.size(); vertexBufferIndex++)
        {
            auto inputFormat = this->internalSettings.GetInputFormat(mesh->vertexBuffers[vertexBufferIndex].formatName, mesh->vertexBuffers[vertexBufferIndex].formatElements);
            metalMesh.sharedVertexBuffers[vertexBufferIndex].CreateVertexBuffer(mesh->vertexBuffers[vertexBufferIndex], inputFormat, GetAllocator(), this->device, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to create shared vertex buffer.");
                return nullptr;
            }
        }
    }

    if (!metalMesh.submeshes.Create(mesh->submeshes.size(), GetAllocator(), &metalMesh))
    {
        metalMesh.HandleCreationFailure();
        this->assetResources.meshesByNameHash.remove(mesh->name.GetHash());

        FINJIN_SET_ERROR(error, "Failed to create submeshes collection.");
        return nullptr;
    }

    for (size_t submeshIndex = 0; submeshIndex < mesh->submeshes.size(); submeshIndex++)
    {
        auto& metalSubmesh = metalMesh.submeshes[submeshIndex];
        auto& submesh = mesh->submeshes[submeshIndex];

        metalSubmesh.vertexBufferIndex = submesh.vertexBufferRange.bufferIndex;
        metalSubmesh.startIndexLocation = submesh.indexBufferRange.startIndex;
        metalSubmesh.indexCount = submesh.indexBufferRange.count;
        metalSubmesh.baseVertexLocation = submesh.vertexBufferRange.startIndex;
        metalSubmesh.vertexCount = submesh.vertexBufferRange.count;

        auto inputFormat = this->internalSettings.GetInputFormat(submesh.vertexBuffer.formatName, submesh.vertexBuffer.formatElements);
        metalSubmesh.vertexBuffer.CreateVertexBuffer(submesh.vertexBuffer, inputFormat, GetAllocator(), this->device, error);
        if (error)
        {
            metalMesh.HandleCreationFailure();
            this->assetResources.meshesByNameHash.remove(mesh->name.GetHash());

            FINJIN_SET_ERROR(error, "Failed to create submesh vertex buffer.");
            return nullptr;
        }

        metalSubmesh.indexBuffer.CreateIndexBuffer(submesh.indexBuffer, GetAllocator(), this->device, error);
        if (error)
        {
            metalMesh.HandleCreationFailure();
            this->assetResources.meshesByNameHash.remove(mesh->name.GetHash());

            FINJIN_SET_ERROR(error, "Failed to create submesh index buffer.");
            return nullptr;
        }
    }

    mesh->gpuMesh = &metalMesh;
    
    return &metalMesh;
}

void MetalGpuContextImpl::CreateShader(MetalShaderType shaderType, const Utf8String& name, id<MTLLibrary> shaderLibrary, Error& error)
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

    auto& metalShader = *addedShader.value;

    metalShader.Create(shaderLibrary, name, error);
    if (error)
    {
        metalShader.HandleCreationFailed();
        shadersByNameHash.remove(name.GetHash());

        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create shader '%1%'.", name));
        return;
    }
}

bool MetalGpuContextImpl::ResolveMeshRef(FinjinMesh& mesh, const AssetReference& assetRef)
{
    return mesh.gpuMesh != nullptr;
}

bool MetalGpuContextImpl::ResolveMaterialRef(FinjinMaterial& material, const AssetReference& assetRef)
{
    return material.gpuMaterial != nullptr;
}

bool MetalGpuContextImpl::ResolveMaterialMaps(FinjinMaterial& material)
{
    auto metalMaterial = static_cast<MetalMaterial*>(material.gpuMaterial);

    if (!metalMaterial->isFullyResolved)
    {
        auto isFullyResolved = true; //Assume resolution will succeed
        for (auto& map : metalMaterial->finjinMaterial->maps)
        {
            if (map.gpuMaterialMapIndex != (size_t)-1)
            {
                auto& metalMap = metalMaterial->maps[map.gpuMaterialMapIndex];
                if (metalMap.texture == nullptr)
                {
                    metalMap.texture = this->assetResources.GetTextureByName(map.textureHandle.assetRef.objectName);
                    if (metalMap.texture == nullptr)
                        isFullyResolved = false; //Failed to find the texture
                }
            }
        }
        metalMaterial->isFullyResolved = isFullyResolved;
    }

    return metalMaterial->isFullyResolved;
}

ScreenCaptureResult MetalGpuContextImpl::GetScreenCapture(ScreenCapture& screenCapture, MetalFrameStage& frameStage)
{
    auto& frameBuffer = this->frameBuffers[frameStage.frameBufferIndex];

    if (frameBuffer.screenCaptureSize[0] > 0 && frameBuffer.screenCaptureSize[1] > 0 &&
        this->frameBufferScreenCapturePixelFormat != ScreenCapturePixelFormat::NONE)
    {
        screenCapture.image = frameBuffer.screenCaptureBuffer.contents;
        screenCapture.pixelFormat = this->frameBufferScreenCapturePixelFormat;
        screenCapture.rowStride = frameBuffer.screenCaptureSize[0] * static_cast<uint32_t>(this->frameBufferScreenCaptureColorFormatBytesPerPixel);
        screenCapture.width = frameBuffer.screenCaptureSize[0];
        screenCapture.height = frameBuffer.screenCaptureSize[1];

        frameBuffer.screenCaptureSize[0] = frameBuffer.screenCaptureSize[1] = 0;

        return ScreenCaptureResult::SUCCESS;
    }

    return ScreenCaptureResult::NOT_AVAILABLE;
}

MetalRenderTarget* MetalGpuContextImpl::GetRenderTarget(MetalFrameStage& frameStage, const Utf8String& name)
{
    auto& frameBuffer = this->frameBuffers[frameStage.frameBufferIndex];

    if (name.empty())
        return &frameBuffer.renderTarget;

    return nullptr;
}

void MetalGpuContextImpl::StartGraphicsCommands(MetalFrameStage& frameStage, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto& frameBuffer = this->frameBuffers[frameStage.frameBufferIndex];

    auto commandBuffer = frameBuffer.NewGraphicsCommandBuffer();
    if (commandBuffer == nullptr)
    {
        FINJIN_SET_ERROR(error, "Failed to get an available graphics command buffer.");
        return;
    }
}

void MetalGpuContextImpl::FinishGraphicsCommands(MetalFrameStage& frameStage, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    //auto& frameBuffer = this->frameBuffers[frameStage.frameBufferIndex];
}

void MetalGpuContextImpl::StartRenderTarget(MetalFrameStage& frameStage, MetalRenderTarget* renderTarget, StaticVector<MetalRenderTarget*, EngineConstants::MAX_RENDER_TARGET_DEPENDENCIES>& dependentRenderTargets, Error& error)
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

    //Set up render pass descriptor-----------------------------

    //Create render pass descriptor if necessary
    if (renderTarget->renderPassDescriptor == nullptr)
        renderTarget->renderPassDescriptor = [MTLRenderPassDescriptor renderPassDescriptor];

    //Color attachment
    auto colorAttachment = renderTarget->renderPassDescriptor.colorAttachments[0];
    colorAttachment.texture = renderTarget->colorOutputs[0].GetTexture();
    colorAttachment.loadAction = MTLLoadActionClear;
    colorAttachment.storeAction = MTLStoreActionStore;
    auto& clearColor = renderTarget->clearColor.IsSet() ? renderTarget->clearColor.value : this->clearColor;
    colorAttachment.clearColor = MTLClearColorMake(clearColor(0), clearColor(1), clearColor(2), clearColor(3));

    //Depth/stencil attachments
    auto depthAttachment = renderTarget->renderPassDescriptor.depthAttachment;
    depthAttachment.texture = this->depthStencilRenderTarget.depthStencilTexture;
    depthAttachment.loadAction = MTLLoadActionClear;
    depthAttachment.storeAction = MTLStoreActionStore;
    depthAttachment.clearDepth = this->settings.maxDepthValue;

    if (MetalUtilities::IsDepthStencilFormat(static_cast<MTLPixelFormat>(this->settings.depthStencilFormat.actual)))
    {
        auto stencilAttachment = renderTarget->renderPassDescriptor.stencilAttachment;
        stencilAttachment.texture = this->depthStencilRenderTarget.depthStencilTexture;
        stencilAttachment.loadAction = MTLLoadActionClear;
        stencilAttachment.storeAction = MTLStoreActionStore;
        stencilAttachment.clearStencil = 0;
    }

    //Get render encoder------------------------------
    auto commandBuffer = frameBuffer.GetCurrentGraphicsCommandBuffer();

    renderTarget->renderCommandEncoder = [commandBuffer renderCommandEncoderWithDescriptor:renderTarget->renderPassDescriptor];
    if (renderTarget->renderCommandEncoder != nullptr)
    {
        if (!renderTarget->viewports.empty())
            [renderTarget->renderCommandEncoder setViewport:renderTarget->viewports[0]];
        else
            [renderTarget->renderCommandEncoder setViewport:renderTarget->defaultViewport[0]];

        if (!renderTarget->scissorRects.empty())
            [renderTarget->renderCommandEncoder setScissorRect:renderTarget->scissorRects[0]];
        else
            [renderTarget->renderCommandEncoder setScissorRect:renderTarget->defaultScissorRect[0]];

        [renderTarget->renderCommandEncoder setDepthStencilState:this->depthStencilRenderTarget.depthStencilState];
        [renderTarget->renderCommandEncoder setCullMode:MTLCullModeBack];
        [renderTarget->renderCommandEncoder setFrontFacingWinding:MTLWindingCounterClockwise];
    }
}

void MetalGpuContextImpl::FinishRenderTarget(MetalFrameStage& frameStage, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    //auto& frameBuffer = this->frameBuffers[frameStage.frameBufferIndex];

    auto renderTarget = frameStage.renderTarget;

    if (renderTarget->renderCommandEncoder != nullptr)
    {
        [renderTarget->renderCommandEncoder endEncoding];
        renderTarget->renderCommandEncoder = nullptr;
    }
}

MetalFrameBuffer& MetalGpuContextImpl::GetFrameBuffer(size_t index)
{
    return this->frameBuffers[index];
}

void MetalGpuContextImpl::UpdatedCachedWindowSize()
{
    this->windowPixelBounds = this->settings.osWindow->GetWindowSize().GetSafeCurrentBounds();
    this->windowPixelBounds.Scale(this->settings.osWindow->GetDisplayDensity());

    this->windowViewport.originX = 0;
    this->windowViewport.originY = 0;
    this->windowViewport.width = this->windowPixelBounds.GetClientWidth();
    this->windowViewport.height = this->windowPixelBounds.GetClientHeight();
    this->windowViewport.znear = 0;
    this->windowViewport.zfar = this->settings.maxDepthValue;

    this->windowScissorRect.x = 0;
    this->windowScissorRect.y = 0;
    this->windowScissorRect.width = this->windowPixelBounds.GetClientWidth();
    this->windowScissorRect.height = this->windowPixelBounds.GetClientHeight();


    this->renderTargetSize = this->settings.renderTargetSize.Evaluate(nullptr, &this->windowPixelBounds);

    this->renderViewport.originX = 0;
    this->renderViewport.originY = 0;
    this->renderViewport.width = this->renderTargetSize[0];
    this->renderViewport.height = this->renderTargetSize[1];
    this->renderViewport.znear = 0;
    this->renderViewport.zfar = this->settings.maxDepthValue;

    this->renderScissorRect.x = 0;
    this->renderScissorRect.y = 0;
    this->renderScissorRect.width = this->renderTargetSize[0];
    this->renderScissorRect.height = this->renderTargetSize[1];
}

#endif
