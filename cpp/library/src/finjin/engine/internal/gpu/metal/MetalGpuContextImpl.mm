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


//Local functions---------------------------------------------------------------
static bool IsFormatAvailable(MetalFeatureSetCapability formatCapabilities)
{
    //The presence of either RESOLVE or MSAA seems to be sufficient
    return AnySet(formatCapabilities & (MetalFeatureSetCapability::RESOLVE | MetalFeatureSetCapability::MSAA));
}

static bool IsSupportedColorFormat(MetalFeatureSet::Capabilities& caps, MTLPixelFormat format)
{
    switch (format)
    {
        case MTLPixelFormatBGRA8Unorm: return IsFormatAvailable(caps.BGRA8Unorm);
        case MTLPixelFormatBGRA8Unorm_sRGB: return IsFormatAvailable(caps.BGRA8Unorm_sRGB);
        case MTLPixelFormatRGBA8Unorm: return IsFormatAvailable(caps.RGBA8Unorm);
        case MTLPixelFormatRGBA8Unorm_sRGB: return IsFormatAvailable(caps.RGBA8Unorm_sRGB);

        default: return false;
    }
}

static bool IsSupportedDepthStencilFormat(MetalFeatureSet::Capabilities& caps, MTLPixelFormat format)
{
    switch (format)
    {
    #if FINJIN_TARGET_PLATFORM == FINJIN_TARGET_PLATFORM_MACOS
        case MTLPixelFormatDepth16Unorm: return IsFormatAvailable(caps.Depth16Unorm);
        case MTLPixelFormatDepth24Unorm_Stencil8: return IsFormatAvailable(caps.Depth24Unorm_Stencil8);
        case MTLPixelFormatX24_Stencil8: return IsFormatAvailable(caps.X24_Stencil8);
    #endif
        case MTLPixelFormatDepth32Float: return IsFormatAvailable(caps.Depth32Float);
        case MTLPixelFormatDepth32Float_Stencil8: return IsFormatAvailable(caps.Depth32Float_Stencil8);
        case MTLPixelFormatX32_Stencil8: return IsFormatAvailable(caps.X32_Stencil8);

        default: return false;
    }
}


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

//MetalGpuContextImpl
MetalGpuContextImpl::MetalGpuContextImpl(Allocator* allocator) : AllocatedClass(allocator), settings(allocator), clearColor(0, 0, 0, 0)
{
    this->device = nullptr;

    this->metalSystem = nullptr;

    this->device = nullptr;

    this->currentFrameBufferIndex = 0;

    this->sequenceIndex = 0;

    FINJIN_ZERO_ITEM(this->viewport);
    FINJIN_ZERO_ITEM(this->scissorRect);

    this->materialMapTypeToGpuElements.insert(StandardAssetDocumentPropertyValues::TypeName::MATERIAL_MAP_DIFFUSE, MaterialMapTypeToGpuElements(MetalMaterial::MapIndex::DIFFUSE, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_DIFFUSE_TEXTURE_INDEX, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_DIFFUSE_AMOUNT, ShaderFeatureFlag::MAP_DIFFUSE), false);
    this->materialMapTypeToGpuElements.insert(StandardAssetDocumentPropertyValues::TypeName::MATERIAL_MAP_SPECULAR, MaterialMapTypeToGpuElements(MetalMaterial::MapIndex::SPECULAR, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_SPECULAR_TEXTURE_INDEX, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_SPECULAR_AMOUNT, ShaderFeatureFlag::MAP_SPECULAR), false);
    this->materialMapTypeToGpuElements.insert(StandardAssetDocumentPropertyValues::TypeName::MATERIAL_MAP_REFLECTION, MaterialMapTypeToGpuElements(MetalMaterial::MapIndex::REFLECTION, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_REFLECTION_TEXTURE_INDEX, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_REFLECTION_AMOUNT, ShaderFeatureFlag::MAP_REFLECTION), false);
    this->materialMapTypeToGpuElements.insert(StandardAssetDocumentPropertyValues::TypeName::MATERIAL_MAP_REFRACTION, MaterialMapTypeToGpuElements(MetalMaterial::MapIndex::REFRACTION, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_REFRACTION_TEXTURE_INDEX, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_REFRACTION_AMOUNT, ShaderFeatureFlag::MAP_REFRACTION), false);
    this->materialMapTypeToGpuElements.insert(StandardAssetDocumentPropertyValues::TypeName::MATERIAL_MAP_BUMP, MaterialMapTypeToGpuElements(MetalMaterial::MapIndex::BUMP, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_BUMP_TEXTURE_INDEX, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_BUMP_AMOUNT, ShaderFeatureFlag::MAP_BUMP), false);
    this->materialMapTypeToGpuElements.insert(StandardAssetDocumentPropertyValues::TypeName::MATERIAL_MAP_HEIGHT, MaterialMapTypeToGpuElements(MetalMaterial::MapIndex::HEIGHT, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_HEIGHT_TEXTURE_INDEX, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_HEIGHT_AMOUNT, ShaderFeatureFlag::MAP_HEIGHT), false);
    this->materialMapTypeToGpuElements.insert(StandardAssetDocumentPropertyValues::TypeName::MATERIAL_MAP_SELF_ILLUMINATION, MaterialMapTypeToGpuElements(MetalMaterial::MapIndex::SELF_ILLUMINATION, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_SELF_ILLUMINATION_TEXTURE_INDEX, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_SELF_ILLUMINATION_AMOUNT, ShaderFeatureFlag::MAP_SELF_ILLUMINATION), false);
    this->materialMapTypeToGpuElements.insert(StandardAssetDocumentPropertyValues::TypeName::MATERIAL_MAP_OPACITY, MaterialMapTypeToGpuElements(MetalMaterial::MapIndex::OPACITY, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_OPACITY_TEXTURE_INDEX, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_OPACITY_AMOUNT, ShaderFeatureFlag::MAP_OPACITY), false);
    this->materialMapTypeToGpuElements.insert(StandardAssetDocumentPropertyValues::TypeName::MATERIAL_MAP_ENVIRONMENT, MaterialMapTypeToGpuElements(MetalMaterial::MapIndex::ENVIRONMENT, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_ENVIRONMENT_TEXTURE_INDEX, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_ENVIRONMENT_AMOUNT, ShaderFeatureFlag::MAP_ENVIRONMENT), false);
    this->materialMapTypeToGpuElements.insert(StandardAssetDocumentPropertyValues::TypeName::MATERIAL_MAP_SHININESS, MaterialMapTypeToGpuElements(MetalMaterial::MapIndex::SHININESS, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_SHININESS_TEXTURE_INDEX, GpuStructuredAndConstantBufferStructMetadata::ElementID::MAP_SHININESS_AMOUNT, ShaderFeatureFlag::MAP_SHININESS), false);
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
        this->staticMeshRenderer.Destroy();

        this->assetResources.Destroy();

        //Destroy screen size dependent resources
        DestroyScreenSizeDependentResources(false);

        //Destroy frame buffer resources
        for (auto& frameBuffer : this->frameBuffers)
        {
            frameBuffer.ResetCommandBuffers();

            frameBuffer.commandQueue = nullptr;
        }
        this->frameStages.clear();

        //Destroy texture objects
        this->textureResources.defaultSamperDescriptor = nullptr;
        this->textureResources.defaultSampler = nullptr;

        //Destroy device
        this->device = nullptr;
    }
}

void MetalGpuContextImpl::CreateDevice(Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    //Find device---------------
    this->device = this->metalSystem->GetImpl()->GetBestMetalDevice(this->settings.gpuID, this->deviceDescription, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to find best device.");
        return;
    }

    //Determine color format--------------
    {
        StaticVector<MTLPixelFormat, 10> formats;

        if (this->settings.colorFormat.requested == MTLPixelFormatInvalid)
            formats.push_back(static_cast<MTLPixelFormat>(this->settings.colorFormat.requested));

        formats.push_back(MTLPixelFormatBGRA8Unorm);
        formats.push_back(MTLPixelFormatBGRA8Unorm_sRGB);
        formats.push_back(MTLPixelFormatRGBA8Unorm);
        formats.push_back(MTLPixelFormatRGBA8Unorm_sRGB);

        //Get first supported format
        for (auto format : formats)
        {
            if (IsSupportedColorFormat(this->deviceDescription.featureSet.capabilities, format))
            {
                this->settings.colorFormat.actual = format;
                break;
            }
        }

        if (this->settings.colorFormat.actual == MTLPixelFormatInvalid)
        {
            FINJIN_SET_ERROR(error, "Failed to find a supported color format.");
            return;
        }
    }

    //Determine depth/stencil format---------------
    {
        StaticVector<MTLPixelFormat, 10> formats;

        if (this->settings.depthStencilFormat.requested != MTLPixelFormatInvalid)
        {
            if (!this->settings.stencilRequired || MetalUtilities::IsDepthStencilFormat(static_cast<MTLPixelFormat>(this->settings.depthStencilFormat.requested)))
                formats.push_back(static_cast<MTLPixelFormat>(this->settings.depthStencilFormat.requested));
        }

        if (this->settings.stencilRequired)
        {
        #if FINJIN_TARGET_PLATFORM == FINJIN_TARGET_PLATFORM_MACOS
            formats.push_back(MTLPixelFormatDepth24Unorm_Stencil8);
        #endif
            formats.push_back(MTLPixelFormatDepth32Float_Stencil8);
        }
        else
        {
            formats.push_back(MTLPixelFormatDepth32Float);
        #if FINJIN_TARGET_PLATFORM == FINJIN_TARGET_PLATFORM_MACOS
            formats.push_back(MTLPixelFormatDepth24Unorm_Stencil8);
            formats.push_back(MTLPixelFormatDepth32Float_Stencil8);
            formats.push_back(MTLPixelFormatDepth16Unorm);
        #endif
            formats.push_back(MTLPixelFormatDepth32Float_Stencil8);
        }

        //Get first supported format
        for (auto format : formats)
        {
            if (IsSupportedDepthStencilFormat(this->deviceDescription.featureSet.capabilities, format))
            {
                this->settings.depthStencilFormat.actual = format;
                break;
            }
        }

        if (this->settings.depthStencilFormat.actual == MTLPixelFormatInvalid)
        {
            FINJIN_SET_ERROR(error, "Failed to find a supported depth/stencil format.");
            return;
        }
    }

    //Initialize Metal layer
    auto metalLayer = this->settings.osWindow->GetImpl()->GetMetalLayer();
    metalLayer.device = this->device;
    metalLayer.pixelFormat = static_cast<MTLPixelFormat>(this->settings.colorFormat.actual);

    //Determine some settings-------------------------------------------
    UpdateCachedFrameBufferSize();

    this->settings.frameBufferCount.actual = this->settings.frameBufferCount.requested;
    this->frameBuffers.resize(this->settings.frameBufferCount.actual);
    for (size_t i = 0; i < this->frameBuffers.size(); i++)
    {
        auto& frameBuffer = this->frameBuffers[i];
        frameBuffer.SetIndex(i);

        frameBuffer.commandQueue = [this->device newCommandQueue];
    }

    this->settings.jobProcessingPipelineSize = GpuContextCommonSettings::CalculateJobPipelineSize(this->settings.frameBufferCount.actual);
    this->frameStages.resize(this->settings.jobProcessingPipelineSize);
    for (size_t i = 0; i < this->frameStages.size(); i++)
    {
        auto& frameStage = this->frameStages[i];
        frameStage.index = i;
    }

    //Create other resources
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

    if (!this->lights.Create(this->settings.maxLights, GetAllocator(), GetAllocator()))
    {
        FINJIN_SET_ERROR(error, "Failed to allocate light lookup.");
        return;
    }
    for (size_t i = 0; i < this->lights.items.size(); i++)
        this->lights.items[i].gpuBufferIndex = i;

    //Create texture samplers
    {
        this->textureResources.defaultSamperDescriptor = [[MTLSamplerDescriptor alloc] init];
        this->textureResources.defaultSamperDescriptor.minFilter = MTLSamplerMinMagFilterLinear;
        this->textureResources.defaultSamperDescriptor.magFilter = MTLSamplerMinMagFilterLinear;
        this->textureResources.defaultSamperDescriptor.sAddressMode = MTLSamplerAddressModeRepeat;
        this->textureResources.defaultSamperDescriptor.tAddressMode = MTLSamplerAddressModeRepeat;
        this->textureResources.defaultSamperDescriptor.mipFilter = MTLSamplerMipFilterLinear;
        this->textureResources.defaultSamperDescriptor.maxAnisotropy = 1;
        this->textureResources.defaultSamperDescriptor.normalizedCoordinates = YES;
        this->textureResources.defaultSamperDescriptor.lodMinClamp = 0.0f;
        this->textureResources.defaultSamperDescriptor.lodMaxClamp = FLT_MAX;
        this->textureResources.defaultSampler = [this->device newSamplerStateWithDescriptor:this->textureResources.defaultSamperDescriptor];
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

    frameStage.frameBufferIndex = this->currentFrameBufferIndex++ % this->frameBuffers.size();

    auto metalLayer = this->settings.osWindow->GetImpl()->GetMetalLayer();
    auto& frameBuffer = this->frameBuffers[frameStage.frameBufferIndex];

    frameBuffer.WaitForCommandBuffers();
    frameBuffer.ResetCommandBuffers();

    auto drawable = metalLayer.nextDrawable;
    //assert(drawable != nullptr);
    while (drawable == nullptr)
    {
        drawable = metalLayer.nextDrawable;
    }
    if (drawable != nullptr)
    {
        frameBuffer.renderTarget.renderTargetOutputResources.resize(1);
        frameBuffer.renderTarget.renderTargetOutputResources[0] = drawable;
    }

    frameStage.elapsedTime = elapsedTime;

    frameStage.totalElapsedTime = totalElapsedTime;

    frameStage.sequenceIndex = this->sequenceIndex++;

    return frameStage;
}

void MetalGpuContextImpl::FinishBackFrameBufferRender(MetalFrameStage& frameStage, bool continueRendering, bool modifyingRenderTarget, size_t presentSyncIntervalOverride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto& frameBuffer = this->frameBuffers[frameStage.frameBufferIndex];

    if (!frameBuffer.renderTarget.renderTargetOutputResources.empty())
    {
        auto commandBuffer = frameBuffer.GetCurrentGraphicsCommandBuffer();
        [commandBuffer presentDrawable:frameBuffer.renderTarget.renderTargetOutputResources[0]];
        frameBuffer.CommitCommandBuffers();

        auto renderTarget = &frameBuffer.renderTarget;
        auto colorAttachment = renderTarget->renderPassDescriptor.colorAttachments[0];
        colorAttachment.texture = nullptr;

        frameBuffer.renderTarget.renderTargetOutputResources[0] = nullptr;
        frameBuffer.renderTarget.renderTargetOutputResources.clear();
    }
}

MetalRenderTarget* MetalGpuContextImpl::GetRenderTarget(MetalFrameStage& frameStage, const Utf8String& name)
{
    auto& frameBuffer = this->frameBuffers[frameStage.frameBufferIndex];

    if (name.empty())
        return &frameBuffer.renderTarget;

    return nullptr;
}

void MetalGpuContextImpl::StartGraphicsCommandList(MetalFrameStage& frameStage, Error& error)
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

void MetalGpuContextImpl::FinishGraphicsCommandList(MetalFrameStage& frameStage, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    //auto& frameBuffer = this->frameBuffers[frameStage.frameBufferIndex];
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
                auto& command = static_cast<FinishRenderTargetGpuCommand&>(baseCommand);

                if (lastRenderTarget == nullptr)
                {
                    FINJIN_SET_ERROR(error, "Failed to finish render target. There was no render target to finish.");
                    return;
                }

                if (lastRenderTarget == GetRenderTarget(frameStage, Utf8String::Empty()))
                {
                    //Main render target finished
                    auto commandBuffer = frameBuffer.GetCurrentGraphicsCommandBuffer();
                    assert(commandBuffer != nullptr);

                    auto& frameBuffer = this->frameBuffers[frameStage.frameBufferIndex];
                    this->staticMeshRenderer.UpdatePassAndMaterialConstants(frameBuffer.staticMeshRendererFrameState, frameStage.elapsedTime, frameStage.totalElapsedTime);
                    this->staticMeshRenderer.RenderQueued
                        (
                        frameBuffer.staticMeshRendererFrameState,
                        commandBuffer,
                        lastRenderTarget->renderCommandEncoder
                        );
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
            default: break;
        }
    }
}

MetalFrameBuffer& MetalGpuContextImpl::GetFrameBuffer(size_t index)
{
    return this->frameBuffers[index];
}

void MetalGpuContextImpl::FlushGpu()
{
    //Do nothing. No special handling necessary for now
}

void MetalGpuContextImpl::StartRenderTarget(MetalFrameStage& frameStage, MetalRenderTarget* renderTarget, StaticVector<MetalRenderTarget*, EngineConstants::MAX_RENDER_TARGET_DEPENDENCIES>& dependentRenderTargets, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto& frameBuffer = this->frameBuffers[frameStage.frameBufferIndex];

    //Hold onto render targets for FinishRenderTarget()----------------------
    if (renderTarget == nullptr)
        renderTarget = &frameBuffer.renderTarget;
    frameStage.renderTarget = renderTarget;
    frameStage.dependentRenderTargets = dependentRenderTargets;

    //Set up render pass descriptor-----------------------------

    //Create render pass descriptor if necessary
    if (renderTarget->renderPassDescriptor == nullptr)
        renderTarget->renderPassDescriptor = [MTLRenderPassDescriptor renderPassDescriptor];

    //Color attachment
    auto colorAttachment = renderTarget->renderPassDescriptor.colorAttachments[0];
    colorAttachment.texture = renderTarget->renderTargetOutputResources[0].texture;
    colorAttachment.loadAction = MTLLoadActionClear;
    colorAttachment.storeAction = MTLStoreActionStore;
    auto& clearColor = renderTarget->clearColor.IsSet() ? renderTarget->clearColor.value : this->clearColor;
    colorAttachment.clearColor = MTLClearColorMake(clearColor(0), clearColor(1), clearColor(2), clearColor(3));

    //Depth/stencil attachments
    auto depthAttachment = renderTarget->renderPassDescriptor.depthAttachment;
    depthAttachment.texture = this->depthStencilRenderTarget.depthStencilTexture;
    depthAttachment.loadAction = MTLLoadActionClear;
    depthAttachment.storeAction = MTLStoreActionStore;
    depthAttachment.clearDepth = 1.0;

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
            [renderTarget->renderCommandEncoder setViewport:this->viewport];

        if (!renderTarget->scissorRects.empty())
            [renderTarget->renderCommandEncoder setScissorRect:renderTarget->scissorRects[0]];
        else
            [renderTarget->renderCommandEncoder setScissorRect:this->scissorRect];

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

void MetalGpuContextImpl::CreateScreenSizeDependentResources(Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto metalLayer = this->settings.osWindow->GetImpl()->GetMetalLayer();

    auto drawableSize = CGSizeMake(this->renderingPixelBounds.GetClientWidth(), this->renderingPixelBounds.GetClientHeight());
    metalLayer.drawableSize = drawableSize;

    this->viewport.originX = 0;
    this->viewport.originY = 0;
    this->viewport.width = drawableSize.width;
    this->viewport.height = drawableSize.height;
    this->viewport.znear = 0;
    this->viewport.zfar = this->settings.maxDepthValue;

    this->scissorRect.x = 0;
    this->scissorRect.y = 0;
    this->scissorRect.width = drawableSize.width;
    this->scissorRect.height = drawableSize.height;

    //Create depth buffer
    if (this->settings.depthStencilFormat.actual != MTLPixelFormatInvalid)
    {
        this->depthStencilRenderTarget.CreateDepthStencil(this->device, drawableSize.width, drawableSize.height, static_cast<MTLPixelFormat>(this->settings.depthStencilFormat.actual), error);
        if (error)
            FINJIN_SET_ERROR(error, "Failed to create depth/stencil buffer.");
    }
}

void MetalGpuContextImpl::DestroyScreenSizeDependentResources(bool resizing)
{
    this->depthStencilRenderTarget.DestroyScreenSizeDependentResources();
}

void MetalGpuContextImpl::WindowResized(OSWindow* osWindow)
{
    UpdateCachedFrameBufferSize();
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
        auto foundAt = this->materialMapTypeToGpuElements.find(map.typeName);
        assert(foundAt != this->materialMapTypeToGpuElements.end());
        if (foundAt != this->materialMapTypeToGpuElements.end())
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

    auto metalLight = this->lights.Use();
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
    metalMesh.finjinMesh = mesh;

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

    metalShader.Create(GetAllocator(), name, shaderLibrary, error);
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

void MetalGpuContextImpl::UpdateCachedFrameBufferSize()
{
    this->renderingPixelBounds = this->settings.osWindow->GetWindowSize().GetSafeCurrentBounds();
    this->renderingPixelBounds.Scale(this->settings.osWindow->GetDisplayDensity());
}

#endif
