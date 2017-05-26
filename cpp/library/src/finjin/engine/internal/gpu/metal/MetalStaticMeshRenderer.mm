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

#include "MetalStaticMeshRenderer.hpp"
#include "MetalGpuContextImpl.hpp"
#include "finjin/common/Convert.hpp"
#include "finjin/common/Math.hpp"
#include "finjin/engine/ShaderFeatures.hpp"
#include <mutex>

using namespace Finjin::Engine;


//Macros------------------------------------------------------------------------
#define ALLOW_LIGHT_FALLBACK_IN_FAILED_SIGNATURE_LOOKUP 1


//Implementation----------------------------------------------------------------

//MetalStaticMeshRenderer::Settings
MetalStaticMeshRenderer::Settings::Settings()
{
    this->contextImpl = nullptr;
    this->maxRenderables = 100;
}

MetalStaticMeshRenderer::Settings& MetalStaticMeshRenderer::Settings::operator = (Settings&& other)
{
    this->contextImpl = other.contextImpl;

    this->maxRenderables = other.maxRenderables;

    this->pipelineShaderFileReference = std::move(other.pipelineShaderFileReference);

    this->uniformBufferFormats = std::move(other.uniformBufferFormats);
    this->graphicsPipelineStates = std::move(other.graphicsPipelineStates);

    return *this;
}

void MetalStaticMeshRenderer::Settings::ParseSettings(const ByteBufferReader& configFileBuffer, const Path& assetFilePath, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    ConfigDocumentReader reader;
    if (reader.Start(configFileBuffer) != nullptr)
    {
        if (assetFilePath.EndsWith("buffer-formats.cfg"))
        {
            //Either I don't understand the purpose of minBufferOffsetAlignment or Apple's stated specs are incorrect
            //On iOS this is reported as 4, which results in an error
            //The macOS reported value is 256, which works correctly
            auto minBufferOffsetAlignment = std::max((size_t)16, this->contextImpl->deviceDescription.featureSet.limits.minBufferOffsetAlignment);
            Std140ConstantBufferPackingRules<GpuRenderingConstantBufferStruct> packingRules(minBufferOffsetAlignment);

            GpuRenderingConstantBufferStruct::Create
                (
                this->uniformBufferFormats,
                this->contextImpl->GetAllocator(),
                reader,
                packingRules,
                GpuRenderingConstantBufferStruct::Defines::NONE,
                error
                );
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to create uniform buffer structs.");
                return;
            }
        }
        else if (assetFilePath.EndsWith("render-states.cfg"))
        {
            this->pipelineShaderFileReference.Create(this->contextImpl->GetAllocator());
            SimpleUri tempUri(this->contextImpl->GetAllocator());
            MetalPipelineStateDescriptor::Create(this->pipelineShaderFileReference, this->graphicsPipelineStates, this->contextImpl->GetAllocator(), reader, tempUri, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to create Metal pipeline state settings.");
                return;
            }
        }
        else
        {
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

                            if (key == "max-renderables")
                                this->maxRenderables = Convert::ToInteger(value, this->maxRenderables);
                        }
                        break;
                    }
                    default: break;
                }
            }
        }
    }
}

//MetalStaticMeshRenderer
MetalStaticMeshRenderer::MetalStaticMeshRenderer() : knownUniformBufferDescriptions(nullptr)
{
    this->graphicsPipelineStateDesc = nullptr;
}

void MetalStaticMeshRenderer::Create(Settings&& settings, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    this->settings = std::move(settings);

    if (this->settings.uniformBufferFormats.empty())
    {
        FINJIN_SET_ERROR(error, "No uniform buffer formats were specified.");
        return;
    }

    //Look up various uniform buffer descriptions
    {
        EnumArray<MetalStaticMeshRendererKnownUniformBuffer, MetalStaticMeshRendererKnownUniformBuffer::COUNT, const char*> constantBufferDescriptionTypeNames;
        constantBufferDescriptionTypeNames[MetalStaticMeshRendererKnownUniformBuffer::PASS] = "finjin.static-mesh-renderer.buffer.pass";
        constantBufferDescriptionTypeNames[MetalStaticMeshRendererKnownUniformBuffer::OBJECT] = "finjin.static-mesh-renderer.buffer.object";
        constantBufferDescriptionTypeNames[MetalStaticMeshRendererKnownUniformBuffer::MATERIAL] = "finjin.static-mesh-renderer.buffer.material";
        constantBufferDescriptionTypeNames[MetalStaticMeshRendererKnownUniformBuffer::LIGHT] = "finjin.static-mesh-renderer.buffer.light";
        for (size_t descriptionIndex = 0; descriptionIndex < (size_t)MetalStaticMeshRendererKnownUniformBuffer::COUNT; descriptionIndex++)
        {
            this->knownUniformBufferDescriptions[descriptionIndex] = GpuRenderingConstantBufferStruct::GetByTypeName(this->settings.uniformBufferFormats, constantBufferDescriptionTypeNames[descriptionIndex]);
            if (this->knownUniformBufferDescriptions[descriptionIndex] == nullptr)
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to find '%1%' uniform buffer description.", constantBufferDescriptionTypeNames[descriptionIndex]));
                return;
            }
        }
    }

    //Create shaders
    if (!this->settings.pipelineShaderFileReference.IsValid())
    {
        FINJIN_SET_ERROR(error, "Pipeline shader file reference is not set.");
        return;
    }

    {
        this->shaderLibrary.Create(this->settings.contextImpl->device, this->settings.pipelineShaderFileReference, this->settings.contextImpl->shaderAssetClassFileReader, this->settings.contextImpl->readBuffer, this->settings.contextImpl->GetAllocator(), error);
        if (error)
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create Metal shader library for '%1%'.", this->settings.pipelineShaderFileReference.ToUriString()));
            return;
        }

        for (auto& pipelineState : this->settings.graphicsPipelineStates)
        {
            for (size_t shaderType = 0; shaderType < (size_t)MetalShaderType::COUNT; shaderType++)
            {
                auto& functionName = pipelineState.shaderFunctionNames[shaderType];
                if (!functionName.empty())
                {
                    this->settings.contextImpl->CreateShader(static_cast<MetalShaderType>(shaderType), functionName, this->shaderLibrary.metalLibrary, error);
                    if (error)
                    {
                        FINJIN_SET_ERROR_NO_MESSAGE(error);
                        return;
                    }
                }
            }
        }
    }

    //Create pipeline state objects
    {
        if (!this->graphicsPipelineStates.Create(this->settings.graphicsPipelineStates.size(), this->settings.contextImpl->GetAllocator()))
        {
            FINJIN_SET_ERROR(error, "Failed to allocate pipeline state objects collection.");
            return;
        }

        if (!this->graphicsPipelineStatesByHash.Create(this->settings.graphicsPipelineStates.size(), this->settings.graphicsPipelineStates.size(), this->settings.contextImpl->GetAllocator()))
        {
            FINJIN_SET_ERROR(error, "Failed to create root pipeline state lookup.");
            return;
        }

        this->graphicsPipelineStateDesc = [[MTLRenderPipelineDescriptor alloc] init];

        for (size_t i = 0; i < this->settings.graphicsPipelineStates.size(); i++)
        {
            auto& pipelineState = this->settings.graphicsPipelineStates[i];

            if (pipelineState.inputFormatType.empty())
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Pipeline state '%1%' does not have an input type.", pipelineState.typeName));
                return;
            }

            //Set up base graphics pipeline state
            [this->graphicsPipelineStateDesc reset];

            this->graphicsPipelineStateDesc.sampleCount = 1;

            auto colorAttachment = this->graphicsPipelineStateDesc.colorAttachments[0];
            colorAttachment.pixelFormat = static_cast<MTLPixelFormat>(this->settings.contextImpl->settings.colorFormat.actual);

            this->graphicsPipelineStateDesc.depthAttachmentPixelFormat = static_cast<MTLPixelFormat>(this->settings.contextImpl->settings.depthStencilFormat.actual);

            if (MetalUtilities::IsDepthStencilFormat(static_cast<MTLPixelFormat>(this->settings.contextImpl->settings.depthStencilFormat.actual)))
                this->graphicsPipelineStateDesc.stencilAttachmentPixelFormat = static_cast<MTLPixelFormat>(this->settings.contextImpl->settings.depthStencilFormat.actual);

            auto metalInputFormat = this->settings.contextImpl->assetResources.GetInputFormatByTypeName(pipelineState.inputFormatType);
            if (metalInputFormat == nullptr)
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to find input format '%1%'.", pipelineState.inputFormatType));
                return;
            }
            pipelineState.graphicsState.shaderFeatures.inputFormatHash = metalInputFormat->inputFormatHash;
            this->graphicsPipelineStateDesc.vertexDescriptor = metalInputFormat->vertexDescriptor;

            for (size_t shaderType = 0; shaderType < (size_t)MetalShaderType::COUNT; shaderType++)
            {
                auto& functionName = pipelineState.shaderFunctionNames[shaderType];
                if (!functionName.empty())
                {
                    auto shader = this->settings.contextImpl->assetResources.GetShaderByName(static_cast<MetalShaderType>(shaderType), functionName);
                    if (shader == nullptr)
                    {
                        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to find shader function name '%1%'.", functionName));
                        return;
                    }

                    switch (static_cast<MetalShaderType>(shaderType))
                    {
                        case MetalShaderType::VERTEX: this->graphicsPipelineStateDesc.vertexFunction = shader->metalFunction; break;
                        case MetalShaderType::FRAGMENT: this->graphicsPipelineStateDesc.fragmentFunction = shader->metalFunction; break;
                        default:
                        {
                            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Shader type '%1%' improperly specified for a graphics pipeline.", MetalShaderTypeUtilities::ToString(static_cast<MetalShaderType>(shaderType))));
                            return;
                        }
                    }
                }
            }

        #if FINJIN_TARGET_PLATFORM == FINJIN_TARGET_PLATFORM_MACOS
            if (pipelineState.graphicsState.primitiveTopology.IsSet())
                this->graphicsPipelineStateDesc.inputPrimitiveTopology = pipelineState.graphicsState.primitiveTopology;
        #endif

            {
                NSError* nserror = nullptr;
                this->graphicsPipelineStates[i] = [this->settings.contextImpl->device newRenderPipelineStateWithDescriptor:this->graphicsPipelineStateDesc error:&nserror];
                if (this->graphicsPipelineStates[i] == nullptr)
                {
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create pipeline state for '%1%': %2%.", pipelineState.typeName, nserror.localizedDescription.UTF8String));
                    return;
                }
            }

            auto renderStateHash = pipelineState.graphicsState.GetHash();

            if (this->graphicsPipelineStatesByHash.contains(renderStateHash))
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Render state for pipeline state object '%1%' already exists in the lookup.", pipelineState.typeName));
                return;
            }

            auto rootSignatureAndPipelineStateResult = this->graphicsPipelineStatesByHash.GetOrAdd(renderStateHash);
            if (rootSignatureAndPipelineStateResult.HasError() || rootSignatureAndPipelineStateResult.value == nullptr)
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to add pipeline state object '%1%' to lookup.", pipelineState.typeName));
                return;
            }
            rootSignatureAndPipelineStateResult.value->pipelineState = this->graphicsPipelineStates[i];
        }
    }

    //Build frame resources
    for (auto& frameStage : this->settings.contextImpl->frameStages)
    {
        auto& staticMeshRendererFrameState = frameStage.staticMeshRendererFrameState;

        //Uniform buffers
        {
            EnumArray<MetalStaticMeshRendererKnownUniformBuffer, MetalStaticMeshRendererKnownUniformBuffer::COUNT, size_t> instanceCounts;
            instanceCounts[MetalStaticMeshRendererKnownUniformBuffer::PASS] = 1;
            instanceCounts[MetalStaticMeshRendererKnownUniformBuffer::OBJECT] = this->settings.maxRenderables;
            instanceCounts[MetalStaticMeshRendererKnownUniformBuffer::MATERIAL] = this->settings.contextImpl->settings.maxMaterials;
            instanceCounts[MetalStaticMeshRendererKnownUniformBuffer::LIGHT] = this->settings.contextImpl->settings.maxLights;

            for (size_t i = 0; i < (size_t)MetalStaticMeshRendererKnownUniformBuffer::COUNT; i++)
            {
                staticMeshRendererFrameState.uniformBuffers[i].Create(this->settings.contextImpl->device, this->settings.contextImpl->deviceDescription, *this->knownUniformBufferDescriptions[i], instanceCounts[i], error);
                if (error)
                {
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create known uniform buffer '%1%' for frame stage '%2%'.", i, frameStage.index));
                    return;
                }
            }
        }

        if (!staticMeshRendererFrameState.opaqueMaterials.Create(this->settings.contextImpl->settings.maxMaterials, this->settings.contextImpl->settings.maxMaterials, this->settings.contextImpl->GetAllocator()))
        {
            FINJIN_SET_ERROR(error, "Failed to create 'opaque' materials collection.");
            return;
        }

        if (!staticMeshRendererFrameState.lights.Create(this->settings.contextImpl->settings.maxLights, this->settings.contextImpl->settings.maxLights, this->settings.contextImpl->GetAllocator()))
        {
            FINJIN_SET_ERROR(error, "Failed to create lights collection.");
            return;
        }

        for (auto& renderTargetState : staticMeshRendererFrameState.renderTargetRenderStates)
        {
            if (!renderTargetState.opaqueEntities.CreateEmpty(this->settings.maxRenderables, this->settings.contextImpl->GetAllocator()))
            {
                FINJIN_SET_ERROR(error, "Failed to create 'opaque' renderables collection.");
                return;
            }

            if (!renderTargetState.pipelineStateSlots.Create(this->settings.graphicsPipelineStates.size(), this->settings.graphicsPipelineStates.size(), this->settings.contextImpl->GetAllocator()))
            {
                FINJIN_SET_ERROR(error, "Failed to create 'opaque' pipeline state slots lookup.");
                return;
            }
            for (auto& pipelineSlot : renderTargetState.pipelineStateSlots.GetValueEntries())
            {
                if (!pipelineSlot.value.second.opaqueEntities.CreateEmpty(this->settings.maxRenderables, this->settings.contextImpl->GetAllocator()))
                {
                    FINJIN_SET_ERROR(error, "Failed to create 'opaque' pipeline lookup.");
                    return;
                }
            }
            renderTargetState.pipelineStateSlots.clear();
        }
    }
}

void MetalStaticMeshRenderer::Destroy()
{
}

void MetalStaticMeshRenderer::UpdatePassAndMaterialConstants(MetalStaticMeshRendererFrameState& frameState, SimpleTimeDelta elapsedTime, SimpleTimeCounter totalElapsedTime)
{
    //Pass
    {
        auto windowBounds = this->settings.contextImpl->windowPixelBounds;

        auto clientWidth = RoundToFloat(windowBounds.GetClientWidth());
        auto clientHeight = RoundToFloat(windowBounds.GetClientHeight());

        this->settings.contextImpl->camera.SetLens(this->settings.contextImpl->camera.GetFovY(), clientWidth / clientHeight, this->settings.contextImpl->camera.GetNearZ(), this->settings.contextImpl->camera.GetFarZ());
        this->settings.contextImpl->camera.Update();

        auto viewMatrix = this->settings.contextImpl->camera.GetViewMatrix();
        auto projectionMatrix = this->settings.contextImpl->camera.GetProjectionMatrix();

        MathMatrix4 viewProjectionMatrix = projectionMatrix * viewMatrix;
        MathMatrix4 viewInverseMatrix = viewMatrix.inverse();
        MathMatrix4 viewInverseTransposeMatrix = viewInverseMatrix.transpose();
        MathMatrix4 projectionInverseMatrix = projectionMatrix.inverse();
        MathMatrix4 viewProjectionInverseMatrix = viewInverseMatrix * projectionInverseMatrix; //viewProjectionMatrix.inverse();

        MathMatrix4Values metalMatrix;

        auto& framePassConstantBuffer = frameState.uniformBuffers[MetalStaticMeshRendererKnownUniformBuffer::PASS];
        framePassConstantBuffer.StartWrites();
        {
            size_t offset;

            //offset = framePassConstantBuffer.Set(metalWorldEnvironmentMatrix, 0, GpuStructuredAndConstantBufferStructMetadata::ElementID::ENVIRONMENT_MATRIX);

            GetColumnOrderMatrixData(metalMatrix, viewMatrix);
            offset = framePassConstantBuffer.Set(metalMatrix, 0, GpuStructuredAndConstantBufferStructMetadata::ElementID::VIEW_MATRIX);

            GetColumnOrderMatrixData(metalMatrix, viewInverseMatrix);
            offset = framePassConstantBuffer.Set(metalMatrix, 0, GpuStructuredAndConstantBufferStructMetadata::ElementID::INVERSE_VIEW_MATRIX);

            GetColumnOrderMatrixData(metalMatrix, viewInverseTransposeMatrix);
            offset = framePassConstantBuffer.Set(metalMatrix, 0, GpuStructuredAndConstantBufferStructMetadata::ElementID::INVERSE_TRANSPOSE_VIEW_MATRIX);

            GetColumnOrderMatrixData(metalMatrix, projectionMatrix);
            offset = framePassConstantBuffer.Set(metalMatrix, 0, GpuStructuredAndConstantBufferStructMetadata::ElementID::PROJECTION_MATRIX);

            GetColumnOrderMatrixData(metalMatrix, projectionInverseMatrix);
            offset = framePassConstantBuffer.Set(metalMatrix, 0, GpuStructuredAndConstantBufferStructMetadata::ElementID::INVERSE_PROJECTION_MATRIX);

            GetColumnOrderMatrixData(metalMatrix, viewProjectionMatrix);
            offset = framePassConstantBuffer.Set(metalMatrix, 0, GpuStructuredAndConstantBufferStructMetadata::ElementID::VIEW_PROJECTION_MATRIX);

            GetColumnOrderMatrixData(metalMatrix, viewProjectionInverseMatrix);
            offset = framePassConstantBuffer.Set(metalMatrix, 0, GpuStructuredAndConstantBufferStructMetadata::ElementID::INVERSE_VIEW_PROJECTION_MATRIX);

            auto metalEyePosition = this->settings.contextImpl->camera.GetPosition();
            offset = framePassConstantBuffer.Set(metalEyePosition, 0, GpuStructuredAndConstantBufferStructMetadata::ElementID::EYE_POSITION);

            auto metalRenderTargetSize = MathVector2(clientWidth, clientHeight);
            offset = framePassConstantBuffer.Set(metalRenderTargetSize, 0, GpuStructuredAndConstantBufferStructMetadata::ElementID::RENDER_TARGET_SIZE);

            auto metalRenderTargetInverseSize = MathVector2(1.0f / clientWidth, 1.0f / clientHeight);
            offset = framePassConstantBuffer.Set(metalRenderTargetInverseSize, 0, GpuStructuredAndConstantBufferStructMetadata::ElementID::INVERSE_RENDER_TARGET_SIZE);

            float metalNearZ = this->settings.contextImpl->camera.GetNearZ();
            offset = framePassConstantBuffer.Set(metalNearZ, 0, GpuStructuredAndConstantBufferStructMetadata::ElementID::Z_NEAR);

            float metalFarZ = this->settings.contextImpl->camera.GetFarZ();
            offset = framePassConstantBuffer.Set(metalFarZ, 0, GpuStructuredAndConstantBufferStructMetadata::ElementID::Z_FAR);

            auto metalTotalTime = SimpleTimeCounterToFloat(totalElapsedTime);
            offset = framePassConstantBuffer.Set(metalTotalTime, 0, GpuStructuredAndConstantBufferStructMetadata::ElementID::TOTAL_ELAPSED_TIME_SECONDS);

            auto metalDeltaTime = SimpleTimeDeltaToFloat(elapsedTime);
            offset = framePassConstantBuffer.Set(metalDeltaTime, 0, GpuStructuredAndConstantBufferStructMetadata::ElementID::FRAME_ELAPSED_TIME_SECONDS);
        }
        framePassConstantBuffer.FinishWrites();
    }

    //Materials
    {
        //auto textureOffsetsByDimension = this->settings.contextImpl->assetResources.textureOffsetsByDimension; //Make a copy

        MathMatrix4 textureTransform;
        MathMatrix4Values metalMatrix;

        auto& frameMaterialConstantBuffer = frameState.uniformBuffers[MetalStaticMeshRendererKnownUniformBuffer::MATERIAL];
        frameMaterialConstantBuffer.StartWrites();
        for (auto material : frameState.opaqueMaterials)
        {
            auto metalMaterial = static_cast<MetalMaterial*>(material->gpuMaterial);

            auto offset = frameMaterialConstantBuffer.Set(material->diffuseColor, metalMaterial->gpuBufferIndex, GpuStructuredAndConstantBufferStructMetadata::ElementID::MATERIAL_DIFFUSE_COLOR);

            offset = frameMaterialConstantBuffer.Set(material->ambientColor, metalMaterial->gpuBufferIndex, GpuStructuredAndConstantBufferStructMetadata::ElementID::MATERIAL_AMBIENT_COLOR);

            offset = frameMaterialConstantBuffer.Set(material->specularColor, metalMaterial->gpuBufferIndex, GpuStructuredAndConstantBufferStructMetadata::ElementID::MATERIAL_SPECULAR_COLOR);

            offset = frameMaterialConstantBuffer.Set(material->selfIlluminationColor, metalMaterial->gpuBufferIndex, GpuStructuredAndConstantBufferStructMetadata::ElementID::MATERIAL_SELF_ILLUMINATION_COLOR);

            offset = frameMaterialConstantBuffer.Set(material->shininess, metalMaterial->gpuBufferIndex, GpuStructuredAndConstantBufferStructMetadata::ElementID::MATERIAL_SHININESS);

            offset = frameMaterialConstantBuffer.Set(material->opacity, metalMaterial->gpuBufferIndex, GpuStructuredAndConstantBufferStructMetadata::ElementID::MATERIAL_OPACITY);

            if (metalMaterial->maps[MetalMaterial::MapIndex::DIFFUSE].finjinMap != nullptr)
            {
                auto map = static_cast<FinjinMaterial::Map*>(metalMaterial->maps[MetalMaterial::MapIndex::DIFFUSE].finjinMap);
                textureTransform = (MathTranslation(map->textureOffset) * MathAngleAxis(map->textureRotation.ToRadiansValue(), MathVector3(1, 0, 0)) * MathScaling(map->textureScale)).matrix();
            }
            else
                textureTransform.setIdentity();
            GetColumnOrderMatrixData(metalMatrix, textureTransform);
            offset = frameMaterialConstantBuffer.Set(metalMatrix, metalMaterial->gpuBufferIndex, GpuStructuredAndConstantBufferStructMetadata::ElementID::MATERIAL_TEXTURE_COORDINATE_MATRIX);

            for (auto& metalMap : metalMaterial->maps)
            {
                if (metalMap.texture != nullptr)
                {
                    auto metalTextureIndex = static_cast<uint32_t>(metalMap.texture->textureIndex);// - textureOffsetsByDimension[MetalMap.texture->type].baseOffset); //This commented code is for setting the index back to its zero base with respect to the appropriate sampler table
                    offset = frameMaterialConstantBuffer.Set(metalTextureIndex, metalMaterial->gpuBufferIndex, (GpuStructuredAndConstantBufferStructMetadata::ElementID)metalMap.finjinMap->gpuBufferTextureIndexElementID);

                    offset = frameMaterialConstantBuffer.Set(metalMap.finjinMap->amount, metalMaterial->gpuBufferIndex, (GpuStructuredAndConstantBufferStructMetadata::ElementID)metalMap.finjinMap->gpuBufferAmountElementID);
                }
            }
        }
        frameMaterialConstantBuffer.FinishWrites();
        frameState.opaqueMaterials.clear();
    }

    //Lights
    {
        SceneNodeState sceneNodeState;

        MathVector4 position4;

        MathVector4 direction4Temp;
        MathVector3 direction3;

        auto& frameLightBuffer = frameState.uniformBuffers[MetalStaticMeshRendererKnownUniformBuffer::LIGHT];
        frameLightBuffer.StartWrites();
        for (auto& lightItem : frameState.lights)
        {
            auto light = lightItem.first;
            auto lightState = lightItem.second;

            auto metalLight = static_cast<MetalLight*>(light->gpuLight);

            auto offset = frameLightBuffer.Set(lightState->color, metalLight->gpuBufferIndex, GpuStructuredAndConstantBufferStructMetadata::ElementID::LIGHT_COLOR);

            lightState->GetWorldPosition(position4);
            offset = frameLightBuffer.Set(position4, metalLight->gpuBufferIndex, GpuStructuredAndConstantBufferStructMetadata::ElementID::LIGHT_POSITION);

            lightState->GetWorldDirection(direction3, direction4Temp);
            offset = frameLightBuffer.Set(direction3, metalLight->gpuBufferIndex, GpuStructuredAndConstantBufferStructMetadata::ElementID::LIGHT_DIRECTION);

            offset = frameLightBuffer.Set(lightState->range, metalLight->gpuBufferIndex, GpuStructuredAndConstantBufferStructMetadata::ElementID::LIGHT_RANGE);

            offset = frameLightBuffer.Set(lightState->power, metalLight->gpuBufferIndex, GpuStructuredAndConstantBufferStructMetadata::ElementID::LIGHT_POWER);

            float coneRange[2] = { lightState->coneRange[0].ToRadiansValue(), lightState->coneRange[1].ToRadiansValue() };
            offset = frameLightBuffer.Set(coneRange, metalLight->gpuBufferIndex, GpuStructuredAndConstantBufferStructMetadata::ElementID::LIGHT_CONE_RANGE);

            float sinConeRange[2] = { sinf(coneRange[0] * 0.5f), sinf(coneRange[1] * 0.5f) };
            offset = frameLightBuffer.Set(sinConeRange, metalLight->gpuBufferIndex, GpuStructuredAndConstantBufferStructMetadata::ElementID::LIGHT_SIN_CONE_RANGE);
        }
        frameLightBuffer.FinishWrites();
        frameState.lights.clear();
    }
}

void MetalStaticMeshRenderer::RenderEntity(MetalStaticMeshRendererFrameState& frameState, SceneNodeState& sceneNodeState, FinjinSceneObjectEntity* entity, const RenderShaderFeatureFlags& entityShaderFeatureFlags, const GpuCommandLights& sortedLights, const MathVector4& ambientLight)
{
    auto& renderTargetState = frameState.renderTargetRenderStates[0];

    assert(!renderTargetState.opaqueEntities.full() && "The entities list is full. Try reconfiguring the system with a higher maximum.");

    auto mesh = entity->meshHandle.asset;
    if (mesh == nullptr)
        return;

    if (!this->settings.contextImpl->ResolveMeshRef(*mesh, entity->meshHandle.assetRef))
        return;

    auto metalMesh = static_cast<MetalMesh*>(mesh->gpuMesh);
    if (metalMesh->submeshes.empty())
        return;

    MathMatrix4 entityWorldMatrix = sceneNodeState.worldMatrix * entity->transform;
    MathMatrix4 entityInverseWorldMatrix = entityWorldMatrix.inverse();
    MathMatrix4 entityInverseTransposeWorldMatrix = entityInverseWorldMatrix.transpose();

    StaticVector<LightType, EngineConstants::MAX_LIGHTS_PER_OBJECT> sortedLightTypes;
    for (auto& light : sortedLights)
    {
        sortedLightTypes.push_back(light.light->lightType);
        frameState.lights.insert(light.light, light.state, false);
    }

    for (size_t subentityIndex = 0; subentityIndex < entity->subentities.size() && !renderTargetState.opaqueEntities.full(); subentityIndex++)
    {
        auto& metalSubmesh = metalMesh->submeshes[subentityIndex];
        //auto& submesh = mesh->submeshes[subentityIndex];
        auto& subentity = entity->subentities[subentityIndex];
        auto material = subentity.materialHandle.asset;
        if (material != nullptr &&
            this->settings.contextImpl->ResolveMaterialRef(*material, subentity.materialHandle.assetRef) &&
            this->settings.contextImpl->ResolveMaterialMaps(*material))
        {
            //auto metalMaterial = static_cast<MetalMaterial*>(material->gpuMaterial);
            auto inputFormat = metalSubmesh.GetInputFormat();
            if (inputFormat != nullptr)
            {
                MetalPipelineStateDescriptor::GraphicsState graphicsState;
            #if FINJIN_TARGET_PLATFORM == FINJIN_TARGET_PLATFORM_MACOS
                graphicsState.primitiveTopology = MTLPrimitiveTopologyClassTriangle;
            #endif
                graphicsState.shaderFeatures.inputFormatHash = inputFormat->GetElementHash();
                graphicsState.shaderFeatures.flags = entityShaderFeatureFlags.ForMaterialMapsAndMesh(material->gpuMapFlags, mesh->gpuMeshFlags);
                graphicsState.shaderFeatures.lights = sortedLightTypes;
                auto renderStateHash = graphicsState.GetHash();

                auto foundPipelineState = this->graphicsPipelineStatesByHash.find(renderStateHash);

            #if ALLOW_LIGHT_FALLBACK_IN_FAILED_SIGNATURE_LOOKUP
                //If the lookup failed, keep removing lights in an attempt to find a match
                while (foundPipelineState == this->graphicsPipelineStatesByHash.end() &&
                    !graphicsState.shaderFeatures.lights.empty())
                {
                    graphicsState.shaderFeatures.lights.pop_back();
                    renderStateHash = graphicsState.GetHash();
                    foundPipelineState = this->graphicsPipelineStatesByHash.find(renderStateHash);
                }
            #endif

                //Continue if there's a matching pipeline state
                if (foundPipelineState != this->graphicsPipelineStatesByHash.end())
                {
                    auto& pipelineStateByHash = foundPipelineState->second;
                    auto pipelineStateSlot = renderTargetState.RecordPipelineState(pipelineStateByHash.pipelineState);
                    if (pipelineStateSlot != nullptr && !pipelineStateSlot->opaqueEntities.full() && !renderTargetState.opaqueEntities.full())
                    {
                        auto gpuBufferIndex = renderTargetState.opaqueEntities.size();
                        renderTargetState.opaqueEntities.push_back();
                        auto& instance = renderTargetState.opaqueEntities.back();

                        instance.entity = entity;
                        instance.subentityIndex = subentityIndex;
                        instance.gpuBufferIndex = gpuBufferIndex;
                        instance.worldMatrix = entityWorldMatrix;
                        instance.worldInverseMatrix = entityInverseWorldMatrix;
                        instance.worldInverseTransposeMatrix = entityInverseTransposeWorldMatrix;
                        instance.ambientLightColor = ambientLight;

                        instance.lights.clear();
                        for (size_t lightIndex = 0; lightIndex < graphicsState.shaderFeatures.lights.size(); lightIndex++)
                        {
                            auto& light = sortedLights[lightIndex];
                            auto metalLight = static_cast<MetalLight*>(light.light->gpuLight);
                            instance.lightIndex[lightIndex] = static_cast<uint32_t>(metalLight->gpuBufferIndex);
                            instance.lights.push_back(metalLight);
                        }

                        frameState.opaqueMaterials.insert(material);

                        pipelineStateSlot->opaqueEntities.push_back(&instance);
                    }
                }
            }
        }
    }
}

void MetalStaticMeshRenderer::RenderQueued(MetalStaticMeshRendererFrameState& frameState, id<MTLCommandBuffer> commandBuffer, id<MTLRenderCommandEncoder> renderCommandEncoder)
{
    auto& renderTargetState = frameState.renderTargetRenderStates[0];

    {
        MathMatrix4Values metalMatrix;

        //Update object CBs
        auto& frameObjectBuffer = frameState.uniformBuffers[MetalStaticMeshRendererKnownUniformBuffer::OBJECT];
        frameObjectBuffer.StartWrites();
        for (auto& item : renderTargetState.opaqueEntities)
        {
            GetColumnOrderMatrixData(metalMatrix, item.worldMatrix);
            auto offset = frameObjectBuffer.Set(metalMatrix, item.gpuBufferIndex, GpuStructuredAndConstantBufferStructMetadata::ElementID::MODEL_MATRIX);

            GetColumnOrderMatrixData(metalMatrix, item.worldInverseMatrix);
            offset = frameObjectBuffer.Set(metalMatrix, item.gpuBufferIndex, GpuStructuredAndConstantBufferStructMetadata::ElementID::INVERSE_MODEL_MATRIX);

            GetColumnOrderMatrixData(metalMatrix, item.worldInverseTransposeMatrix);
            offset = frameObjectBuffer.Set(metalMatrix, item.gpuBufferIndex, GpuStructuredAndConstantBufferStructMetadata::ElementID::INVERSE_TRANSPOSE_MODEL_MATRIX);

            offset = frameObjectBuffer.Set(item.ambientLightColor, item.gpuBufferIndex, GpuStructuredAndConstantBufferStructMetadata::ElementID::AMBIENT_LIGHT_COLOR);

            auto material = item.entity->subentities[item.subentityIndex].materialHandle.asset;
            auto metalMaterial = static_cast<MetalMaterial*>(material->gpuMaterial);

            auto materialIndex = static_cast<uint32_t>(metalMaterial->gpuBufferIndex);
            offset = frameObjectBuffer.Set(materialIndex, item.gpuBufferIndex, GpuStructuredAndConstantBufferStructMetadata::ElementID::MATERIAL_INDEX);

            //frameObjectBuffer.SetArray(item.lightIndex, item.gpuBufferIndex, GpuStructuredAndConstantBufferStructMetadata::ElementID::LIGHT_INDEX);
            offset = frameObjectBuffer.Set(item.lightIndex, item.gpuBufferIndex, GpuStructuredAndConstantBufferStructMetadata::ElementID::LIGHT_INDEX);
        }
        frameObjectBuffer.FinishWrites();
        renderTargetState.opaqueEntities.clear();
    }

    {
        //auto objectBufferInstanceByteSize = frameState.uniformBuffers[MetalStaticMeshRendererKnownUniformBuffer::OBJECT].GetPaddedTotalSize();
        //auto objectBufferResource = frameState.uniformBuffers[MetalStaticMeshRendererKnownUniformBuffer::OBJECT].resource;

        //Pass buffer
        [renderCommandEncoder setVertexBuffer:frameState.uniformBuffers[MetalStaticMeshRendererKnownUniformBuffer::PASS].resource offset:0 atIndex:MetalStaticMeshRenderKnownBindings::VertexAndFragmentStageBuffer::PASS];
        [renderCommandEncoder setFragmentBuffer:frameState.uniformBuffers[MetalStaticMeshRendererKnownUniformBuffer::PASS].resource offset:0 atIndex:MetalStaticMeshRenderKnownBindings::VertexAndFragmentStageBuffer::PASS];

        //Object buffer
        [renderCommandEncoder setVertexBuffer:frameState.uniformBuffers[MetalStaticMeshRendererKnownUniformBuffer::OBJECT].resource offset:0 atIndex:MetalStaticMeshRenderKnownBindings::VertexAndFragmentStageBuffer::OBJECT];
        [renderCommandEncoder setFragmentBuffer:frameState.uniformBuffers[MetalStaticMeshRendererKnownUniformBuffer::OBJECT].resource offset:0 atIndex:MetalStaticMeshRenderKnownBindings::VertexAndFragmentStageBuffer::OBJECT];

        for (auto& pipelineStateSlot : renderTargetState.pipelineStateSlots)
        {
            [renderCommandEncoder setRenderPipelineState:pipelineStateSlot.second.pipelineState];

            for (auto item : pipelineStateSlot.second.opaqueEntities)
            {
                //auto& subentity = item->entity->subentities[item->subentityIndex];

                auto mesh = item->entity->meshHandle.asset;
                auto& submesh = mesh->submeshes[item->subentityIndex];
                auto material = submesh.materialHandle.asset;

                auto metalMesh = static_cast<MetalMesh*>(mesh->gpuMesh);
                auto& metalSubmesh = metalMesh->submeshes[item->subentityIndex];
                auto metalMaterial = static_cast<MetalMaterial*>(material->gpuMaterial);

                //Object buffer offset
                {
                    auto bufferDescription = this->knownUniformBufferDescriptions[MetalStaticMeshRendererKnownUniformBuffer::OBJECT];
                    auto offset = static_cast<NSUInteger>(item->gpuBufferIndex * bufferDescription->paddedTotalSize);

                    [renderCommandEncoder setVertexBufferOffset:offset atIndex:MetalStaticMeshRenderKnownBindings::VertexAndFragmentStageBuffer::OBJECT];
                    [renderCommandEncoder setFragmentBufferOffset:offset atIndex:MetalStaticMeshRenderKnownBindings::VertexAndFragmentStageBuffer::OBJECT];
                }

                //Material buffer
                {
                    auto bufferDescription = this->knownUniformBufferDescriptions[MetalStaticMeshRendererKnownUniformBuffer::MATERIAL];
                    auto offset = static_cast<NSUInteger>(metalMaterial->gpuBufferIndex * bufferDescription->paddedTotalSize);

                    [renderCommandEncoder setVertexBuffer:frameState.uniformBuffers[MetalStaticMeshRendererKnownUniformBuffer::MATERIAL].resource offset:offset atIndex:MetalStaticMeshRenderKnownBindings::VertexAndFragmentStageBuffer::MATERIAL];
                    [renderCommandEncoder setFragmentBuffer:frameState.uniformBuffers[MetalStaticMeshRendererKnownUniformBuffer::MATERIAL].resource offset:offset atIndex:MetalStaticMeshRenderKnownBindings::VertexAndFragmentStageBuffer::MATERIAL];
                }

                //Light buffers
                {
                    NSUInteger bindingIndex = MetalStaticMeshRenderKnownBindings::VertexAndFragmentStageBuffer::LIGHT_BASE;

                    auto bufferDescription = this->knownUniformBufferDescriptions[MetalStaticMeshRendererKnownUniformBuffer::LIGHT];

                    for (auto metalLight : item->lights)
                    {
                        auto offset = static_cast<NSUInteger>(metalLight->gpuBufferIndex * bufferDescription->paddedTotalSize);

                        [renderCommandEncoder setVertexBuffer:frameState.uniformBuffers[MetalStaticMeshRendererKnownUniformBuffer::LIGHT].resource offset:offset atIndex:bindingIndex];
                        [renderCommandEncoder setFragmentBuffer:frameState.uniformBuffers[MetalStaticMeshRendererKnownUniformBuffer::LIGHT].resource offset:offset atIndex:bindingIndex];

                        bindingIndex++;
                    }
                }

                //Vertex buffer
                {
                    auto vertexBufferView = metalSubmesh.GetVertexBufferView();

                    [renderCommandEncoder setVertexBuffer:vertexBufferView.buffer offset:0 atIndex:MetalStaticMeshRenderKnownBindings::VertexAndFragmentStageBuffer::VERTEX];
                }

                //Sampler
                {
                    auto defaultSampler = this->settings.contextImpl->commonResources.defaultSampler;

                    [renderCommandEncoder setVertexSamplerState:defaultSampler atIndex:MetalStaticMeshRenderKnownBindings::Sampler::DEFAULT_SAMPLER];
                    [renderCommandEncoder setFragmentSamplerState:defaultSampler atIndex:MetalStaticMeshRenderKnownBindings::Sampler::DEFAULT_SAMPLER];
                }

                //Textures
                for (size_t mapIndex = 0; mapIndex < metalMaterial->maps.size(); mapIndex++)
                {
                    auto& metalMap = metalMaterial->maps[mapIndex];
                    if (metalMap.texture != nullptr)
                    {
                        auto metalMapTextureBindingIndex = static_cast<NSUInteger>(mapIndex);
                        [renderCommandEncoder setVertexTexture:metalMap.texture->resource atIndex:metalMapTextureBindingIndex];
                        [renderCommandEncoder setFragmentTexture:metalMap.texture->resource atIndex:metalMapTextureBindingIndex];
                    }
                }

                //[renderCommandEncoder setTriangleFillMode:MTLTriangleFillModeFill]; //MTLTriangleFillModeLines

                //Index buffer + draw, or just draw
                auto indexBufferView = metalSubmesh.GetIndexBufferView();
                if (indexBufferView.byteSize > 0)
                {
                    auto indexBufferOffset = metalSubmesh.startIndexLocation * indexBufferView.strideInBytes;

                    //Works on MTLFeatureSet_iOS_GPUFamily3_v1 hardware
                    //[renderCommandEncoder drawIndexedPrimitives:metalSubmesh.primitiveType indexCount:metalSubmesh.indexCount indexType:indexBufferView.indexType indexBuffer:indexBufferView.buffer indexBufferOffset:indexBufferOffset instanceCount:1 baseVertex:metalSubmesh.baseVertexLocation baseInstance:0];

                    //Works on iPhone 6/Plus
                    [renderCommandEncoder drawIndexedPrimitives:metalSubmesh.primitiveType indexCount:metalSubmesh.indexCount indexType:indexBufferView.indexType indexBuffer:indexBufferView.buffer indexBufferOffset:indexBufferOffset instanceCount:1];
                }
                else
                {
                    [renderCommandEncoder drawPrimitives:metalSubmesh.primitiveType vertexStart:0 vertexCount:metalSubmesh.vertexCount];
                }
            }
        }
        renderTargetState.ClearState();
    }
}

#endif
