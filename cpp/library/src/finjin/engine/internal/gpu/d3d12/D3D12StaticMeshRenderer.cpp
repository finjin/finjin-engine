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

#include "D3D12StaticMeshRenderer.hpp"
#include "D3D12GpuContextImpl.hpp"
#include "finjin/common/Convert.hpp"
#include "finjin/common/Math.hpp"
#include "finjin/engine/ShaderFeatures.hpp"
#include <mutex>

using namespace Finjin::Engine;


//Macros------------------------------------------------------------------------
#define MAX_HEAP_DESCRIPTOR_COMBINATIONS 10
#define ALLOW_LIGHT_FALLBACK_IN_FAILED_SIGNATURE_LOOKUP 1


//Implementation----------------------------------------------------------------

//D3D12StaticMeshRenderer::Settings
D3D12StaticMeshRenderer::Settings::Settings()
{
    this->contextImpl = nullptr;
    this->maxRenderables = 100;
}

D3D12StaticMeshRenderer::Settings& D3D12StaticMeshRenderer::Settings::operator = (Settings&& other)
{
    this->contextImpl = other.contextImpl;

    this->maxRenderables = other.maxRenderables;

    this->pipelineShaderFileReference = std::move(other.pipelineShaderFileReference);

    this->constantBufferFormats = std::move(other.constantBufferFormats);
    this->rootSignatures = std::move(other.rootSignatures);
    this->graphicsPipelineStates = std::move(other.graphicsPipelineStates);

    return *this;
}

void D3D12StaticMeshRenderer::Settings::ParseSettings(const ByteBufferReader& configFileBuffer, const Path& assetFilePath, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    ConfigDocumentReader reader;
    if (reader.Start(configFileBuffer) != nullptr)
    {
        if (assetFilePath.EndsWith("buffer-formats.cfg"))
        {
            Std140ConstantBufferPackingRules<GpuRenderingConstantBufferStruct> packingRules(D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);

            GpuRenderingConstantBufferStruct::Create
                (
                this->constantBufferFormats,
                this->contextImpl->GetAllocator(),
                reader,
                packingRules,
                GpuRenderingConstantBufferStruct::Defines::NONE,
                error
                );
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to create constant buffer structs.");
                return;
            }
        }
        else if (assetFilePath.EndsWith("root-signatures.cfg"))
        {
            D3D12RootSignatureDescriptor::Create(this->rootSignatures, this->contextImpl->GetAllocator(), reader, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to create D3D12 root signature settings.");
                return;
            }
        }
        else if (assetFilePath.EndsWith("render-states.cfg"))
        {
            this->pipelineShaderFileReference.Create(this->contextImpl->GetAllocator());
            SimpleUri tempUri(this->contextImpl->GetAllocator());
            D3D12PipelineStateDescriptor::Create(this->pipelineShaderFileReference, this->graphicsPipelineStates, this->contextImpl->GetAllocator(), reader, tempUri, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to create D3D12 pipeline state settings.");
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

//D3D12StaticMeshRenderer
D3D12StaticMeshRenderer::D3D12StaticMeshRenderer() : knownConstantBufferDescriptions(nullptr)
{
}

void D3D12StaticMeshRenderer::Create(Settings&& settings, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    this->settings = std::move(settings);

    if (this->settings.constantBufferFormats.empty())
    {
        FINJIN_SET_ERROR(error, "No constant buffer formats were specified.");
        return;
    }

    //Look up various uniform buffer descriptions
    {
        EnumArray<D3D12StaticMeshRendererKnownConstantBuffer, D3D12StaticMeshRendererKnownConstantBuffer::COUNT, const char*> constantBufferDescriptionTypeNames;
        constantBufferDescriptionTypeNames[D3D12StaticMeshRendererKnownConstantBuffer::PASS] = "finjin.static-mesh-renderer.buffer.pass";
        constantBufferDescriptionTypeNames[D3D12StaticMeshRendererKnownConstantBuffer::OBJECT] = "finjin.static-mesh-renderer.buffer.object";
        constantBufferDescriptionTypeNames[D3D12StaticMeshRendererKnownConstantBuffer::MATERIAL] = "finjin.static-mesh-renderer.buffer.material";
        constantBufferDescriptionTypeNames[D3D12StaticMeshRendererKnownConstantBuffer::LIGHT] = "finjin.static-mesh-renderer.buffer.light";
        for (size_t descriptionIndex = 0; descriptionIndex < (size_t)D3D12StaticMeshRendererKnownConstantBuffer::COUNT; descriptionIndex++)
        {
            this->knownConstantBufferDescriptions[descriptionIndex] = GpuRenderingConstantBufferStruct::GetByTypeName(this->settings.constantBufferFormats, constantBufferDescriptionTypeNames[descriptionIndex]);
            if (this->knownConstantBufferDescriptions[descriptionIndex] == nullptr)
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to find '%1%' constant buffer description.", constantBufferDescriptionTypeNames[descriptionIndex]));
                return;
            }
        }
    }

    //Set up base graphics pipeline builder
    FINJIN_ZERO_ITEM(this->baseGraphicsPipelineStateDesc);
    this->baseGraphicsPipelineStateDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    this->baseGraphicsPipelineStateDesc.RasterizerState.FrontCounterClockwise = TRUE;
    this->baseGraphicsPipelineStateDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
    this->baseGraphicsPipelineStateDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    this->baseGraphicsPipelineStateDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    this->baseGraphicsPipelineStateDesc.SampleMask = UINT_MAX;
    this->baseGraphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    this->baseGraphicsPipelineStateDesc.NumRenderTargets = 1;
    this->baseGraphicsPipelineStateDesc.RTVFormats[0] = this->settings.contextImpl->settings.colorFormat.actual;
    this->baseGraphicsPipelineStateDesc.SampleDesc.Count = this->settings.contextImpl->settings.multisampleCount.actual;
    this->baseGraphicsPipelineStateDesc.SampleDesc.Quality = this->settings.contextImpl->settings.multisampleQuality.actual;
    this->baseGraphicsPipelineStateDesc.DSVFormat = this->settings.contextImpl->settings.depthStencilFormat.actual;

    //Create shaders
    if (this->settings.pipelineShaderFileReference.IsValid())
    {
        auto readResult = this->settings.contextImpl->shaderAssetClassFileReader.ReadAsset(this->settings.contextImpl->readBuffer, this->settings.pipelineShaderFileReference);
        if (readResult == FileOperationResult::SUCCESS)
        {
            if (!this->shaderFileBytes.Create(this->settings.contextImpl->readBuffer.size(), this->settings.contextImpl->GetAllocator()))
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to allocate shader buffer for '%1%'.", this->settings.pipelineShaderFileReference.ToUriString()));
                return;
            }

            FINJIN_COPY_MEMORY(this->shaderFileBytes.data(), this->settings.contextImpl->readBuffer.data(), this->settings.contextImpl->readBuffer.size());
        }

        for (auto& pipelineState : this->settings.graphicsPipelineStates)
        {
            for (size_t shaderType = 0; shaderType < (size_t)D3D12ShaderType::COUNT; shaderType++)
            {
                auto& assetOffset = pipelineState.shaderOffsets[shaderType];
                if (assetOffset.IsValid())
                {
                    this->settings.contextImpl->CreateShader(static_cast<D3D12ShaderType>(shaderType), pipelineState.typeName, this->shaderFileBytes.data() + assetOffset.offset, assetOffset.size, false, error);
                    if (error)
                    {
                        FINJIN_SET_ERROR_NO_MESSAGE(error);
                        return;
                    }
                }
            }
        }
    }
    else
    {
        for (auto& pipelineState : this->settings.graphicsPipelineStates)
        {
            for (size_t shaderType = 0; shaderType < (size_t)D3D12ShaderType::COUNT; shaderType++)
            {
                auto& assetRef = pipelineState.shaderRefs[shaderType];
                if (assetRef.IsValid())
                {
                    this->settings.contextImpl->CreateShader(static_cast<D3D12ShaderType>(shaderType), assetRef, error);
                    if (error)
                    {
                        FINJIN_SET_ERROR_NO_MESSAGE(error);
                        return;
                    }
                }
            }
        }
    }

    //Create root signatures
    if (!this->rootSignatures.Create(this->settings.rootSignatures.size(), this->settings.contextImpl->GetAllocator()))
    {
        FINJIN_SET_ERROR(error, "Failed to allocate root signatures collection.");
        return;
    }

    for (size_t i = 0; i < this->settings.rootSignatures.size(); i++)
    {
        this->settings.rootSignatures[i].CreateRootSignature(this->rootSignatures[i], this->settings.contextImpl->device.Get(), error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to create root signature.");
            return;
        }
    }

    //Create pipeline state objects
    {
        if (!this->graphicsPipelineStates.Create(this->settings.graphicsPipelineStates.size(), this->settings.contextImpl->GetAllocator()))
        {
            FINJIN_SET_ERROR(error, "Failed to allocate pipeline state objects collection.");
            return;
        }

        if (!this->graphicsRootSignatureAndPipelineStates.Create(this->settings.graphicsPipelineStates.size(), this->settings.graphicsPipelineStates.size(), this->settings.contextImpl->GetAllocator()))
        {
            FINJIN_SET_ERROR(error, "Failed to create root signature/pipeline state lookup.");
            return;
        }

        for (size_t i = 0; i < this->settings.graphicsPipelineStates.size(); i++)
        {
            auto& pipelineState = this->settings.graphicsPipelineStates[i];

            if (pipelineState.inputFormatType.empty())
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Pipeline state '%1%' does not have an input type.", pipelineState.typeName));
                return;
            }

            if (pipelineState.rootSignatureType.empty())
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Pipeline state '%1%' does not a root signature type.", pipelineState.typeName));
                return;
            }

            auto graphicsPipelineStateDesc = this->baseGraphicsPipelineStateDesc; //Make a copy

            auto d3d12InputFormat = this->settings.contextImpl->assetResources.GetInputFormatByTypeName(pipelineState.inputFormatType);
            if (d3d12InputFormat == nullptr)
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to find input format '%1%'.", pipelineState.inputFormatType));
                return;
            }
            pipelineState.graphicsState.shaderFeatures.inputFormatHash = d3d12InputFormat->inputFormatHash;
            graphicsPipelineStateDesc.InputLayout = { d3d12InputFormat->elements.data(), static_cast<UINT>(d3d12InputFormat->elements.size()) };

            auto rootSignatureDescriptor = D3D12RootSignatureDescriptor::GetByTypeName(this->settings.rootSignatures, pipelineState.rootSignatureType);
            if (rootSignatureDescriptor == nullptr)
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to find root signature '%1%'.", pipelineState.rootSignatureType));
                return;
            }
            auto& rootSignature = this->rootSignatures[rootSignatureDescriptor - this->settings.rootSignatures.data()];
            graphicsPipelineStateDesc.pRootSignature = rootSignature.Get();

            for (size_t shaderType = 0; shaderType < (size_t)D3D12ShaderType::COUNT; shaderType++)
            {
                D3D12Shader* shader = nullptr;

                auto& shaderOffset = pipelineState.shaderOffsets[shaderType];
                auto& shaderRef = pipelineState.shaderRefs[shaderType];

                if (shaderOffset.IsValid())
                {
                    shader = this->settings.contextImpl->assetResources.GetShaderByName(static_cast<D3D12ShaderType>(shaderType), pipelineState.typeName);
                    if (shader == nullptr)
                    {
                        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to find shader '%1%'.", pipelineState.typeName));
                        return;
                    }
                }
                else if (shaderRef.IsValid())
                {
                    shader = this->settings.contextImpl->assetResources.GetShaderByName(static_cast<D3D12ShaderType>(shaderType), shaderRef.objectName);
                    if (shader == nullptr)
                    {
                        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to find shader '%1%'.", shaderRef.ToUriString()));
                        return;
                    }
                }

                if (shader != nullptr)
                {
                    switch (static_cast<D3D12ShaderType>(shaderType))
                    {
                        case D3D12ShaderType::VERTEX: graphicsPipelineStateDesc.VS = { shader->bytes.data_start(), shader->bytes.max_size() }; break;
                        case D3D12ShaderType::PIXEL: graphicsPipelineStateDesc.PS = { shader->bytes.data_start(), shader->bytes.max_size() }; break;
                        case D3D12ShaderType::DOMAIN: graphicsPipelineStateDesc.DS = { shader->bytes.data_start(), shader->bytes.max_size() }; break;
                        case D3D12ShaderType::HULL: graphicsPipelineStateDesc.HS = { shader->bytes.data_start(), shader->bytes.max_size() }; break;
                        case D3D12ShaderType::GEOMETRY: graphicsPipelineStateDesc.GS = { shader->bytes.data_start(), shader->bytes.max_size() }; break;
                        default:
                        {
                            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Shader type '%1%' improperly specified for a graphics pipeline.", D3D12ShaderTypeUtilities::ToString(static_cast<D3D12ShaderType>(shaderType))));
                            return;
                        }
                    }
                }
            }

            if (AnySet(pipelineState.graphicsState.shaderFeatures.flags & ShaderFeatureFlag::RENDERING_FILL_WIREFRAME))
                graphicsPipelineStateDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;

            if (pipelineState.graphicsState.primitiveTopology.IsSet())
                graphicsPipelineStateDesc.PrimitiveTopologyType = pipelineState.graphicsState.primitiveTopology;

            auto createGraphicsPipelineStateResult = this->settings.contextImpl->device->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&this->graphicsPipelineStates[i]));
            if (FINJIN_CHECK_HRESULT_FAILED(createGraphicsPipelineStateResult))
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create pipeline state object for '%1%'.", pipelineState.typeName));
                return;
            }

            auto renderStateHash = pipelineState.graphicsState.GetHash();

            if (this->graphicsRootSignatureAndPipelineStates.contains(renderStateHash))
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Render state for pipeline state object '%1%' already exists in the lookup.", pipelineState.typeName));
                return;
            }

            auto rootSignatureAndPipelineStateResult = this->graphicsRootSignatureAndPipelineStates.GetOrAdd(renderStateHash);
            if (rootSignatureAndPipelineStateResult.HasError() || rootSignatureAndPipelineStateResult.value == nullptr)
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to add pipeline state object '%1%' to lookup.", pipelineState.typeName));
                return;
            }
            rootSignatureAndPipelineStateResult.value->rootSignature = graphicsPipelineStateDesc.pRootSignature;
            rootSignatureAndPipelineStateResult.value->pipelineState = this->graphicsPipelineStates[i].Get();
        }
    }

    //Build frame resources
    for (auto& frameStage : this->settings.contextImpl->frameStages)
    {
        auto& staticMeshRendererFrameState = frameStage.staticMeshRendererFrameState;

        //Constant buffers
        {
            EnumArray<D3D12StaticMeshRendererKnownConstantBuffer, D3D12StaticMeshRendererKnownConstantBuffer::COUNT, size_t> instanceCounts;
            instanceCounts[D3D12StaticMeshRendererKnownConstantBuffer::PASS] = 1;
            instanceCounts[D3D12StaticMeshRendererKnownConstantBuffer::OBJECT] = this->settings.maxRenderables;
            instanceCounts[D3D12StaticMeshRendererKnownConstantBuffer::MATERIAL] = this->settings.contextImpl->settings.maxMaterials;
            instanceCounts[D3D12StaticMeshRendererKnownConstantBuffer::LIGHT] = this->settings.contextImpl->settings.maxLights;

            for (size_t i = 0; i < (size_t)D3D12StaticMeshRendererKnownConstantBuffer::COUNT; i++)
            {
                staticMeshRendererFrameState.constantBuffers[i].Create(this->settings.contextImpl->device.Get(), *this->knownConstantBufferDescriptions[i], instanceCounts[i], error);
                if (error)
                {
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create known constant buffer '%1%' for frame '%2%'.", i, frameStage.index));
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

            if (!renderTargetState.rootSignatureSlots.Create(this->settings.rootSignatures.size(), this->settings.rootSignatures.size(), this->settings.contextImpl->GetAllocator()))
            {
                FINJIN_SET_ERROR(error, "Failed to create 'opaque' root signature lookup.");
                return;
            }
            for (auto& rootSignatureSlot : renderTargetState.rootSignatureSlots.GetValueEntries())
            {
                if (!rootSignatureSlot.value.second.pipelineStateSlots.Create(this->settings.graphicsPipelineStates.size(), this->settings.graphicsPipelineStates.size(), this->settings.contextImpl->GetAllocator()))
                {
                    FINJIN_SET_ERROR(error, "Failed to create 'opaque' pipeline state lookup.");
                    return;
                }
            }

            if (!renderTargetState.pipelineStateSlots.Create(this->settings.graphicsPipelineStates.size(), this->settings.graphicsPipelineStates.size(), this->settings.contextImpl->GetAllocator()))
            {
                FINJIN_SET_ERROR(error, "Failed to create 'opaque' pipeline state lookup.");
                return;
            }

            for (auto& pipelineSlot : renderTargetState.pipelineStateSlots.GetValueEntries())
            {
                if (!pipelineSlot.value.second.heapRefs.Create(MAX_HEAP_DESCRIPTOR_COMBINATIONS, this->settings.contextImpl->GetAllocator()))
                {
                    FINJIN_SET_ERROR(error, "Failed to create heap refs for render pipeline state.");
                    return;
                }
                for (auto& heapRef : pipelineSlot.value.second.heapRefs)
                {
                    if (!heapRef.opaqueEntities.CreateEmpty(this->settings.maxRenderables, this->settings.contextImpl->GetAllocator()))
                    {
                        FINJIN_SET_ERROR(error, "Failed to create 'opaque' renderables collection for pipeline state.");
                        return;
                    }
                }
                pipelineSlot.value.second.heapRefs.clear();
            }
        }
    }
}

void D3D12StaticMeshRenderer::Destroy()
{
    this->graphicsRootSignatureAndPipelineStates.Destroy();

    this->shaderFileBytes.Destroy();

    this->graphicsPipelineStates.Destroy();

    this->rootSignatures.Destroy();
}

void D3D12StaticMeshRenderer::UpdatePassAndMaterialConstants(D3D12StaticMeshRendererFrameState& frameState, SimpleTimeDelta elapsedTime, SimpleTimeCounter totalElapsedTime)
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

        MathMatrix4Values d3d12Matrix;

        auto& framePassConstantBuffer = frameState.constantBuffers[D3D12StaticMeshRendererKnownConstantBuffer::PASS];
        framePassConstantBuffer.StartWrites();
        {
            size_t offset;

            //offset = framePassConstantBuffer.Set(d3d12WorldEnvironmentMatrix, 0, GpuStructuredAndConstantBufferStructMetadata::ElementID::ENVIRONMENT_MATRIX);

            GetRowOrderMatrixData(d3d12Matrix, viewMatrix);
            offset = framePassConstantBuffer.Set(d3d12Matrix, 0, GpuStructuredAndConstantBufferStructMetadata::ElementID::VIEW_MATRIX);

            GetRowOrderMatrixData(d3d12Matrix, viewInverseMatrix);
            offset = framePassConstantBuffer.Set(d3d12Matrix, 0, GpuStructuredAndConstantBufferStructMetadata::ElementID::INVERSE_VIEW_MATRIX);

            GetRowOrderMatrixData(d3d12Matrix, viewInverseTransposeMatrix);
            offset = framePassConstantBuffer.Set(d3d12Matrix, 0, GpuStructuredAndConstantBufferStructMetadata::ElementID::INVERSE_TRANSPOSE_VIEW_MATRIX);

            GetRowOrderMatrixData(d3d12Matrix, projectionMatrix);
            offset = framePassConstantBuffer.Set(d3d12Matrix, 0, GpuStructuredAndConstantBufferStructMetadata::ElementID::PROJECTION_MATRIX);

            GetRowOrderMatrixData(d3d12Matrix, projectionInverseMatrix);
            offset = framePassConstantBuffer.Set(d3d12Matrix, 0, GpuStructuredAndConstantBufferStructMetadata::ElementID::INVERSE_PROJECTION_MATRIX);

            GetRowOrderMatrixData(d3d12Matrix, viewProjectionMatrix);
            offset = framePassConstantBuffer.Set(d3d12Matrix, 0, GpuStructuredAndConstantBufferStructMetadata::ElementID::VIEW_PROJECTION_MATRIX);

            GetRowOrderMatrixData(d3d12Matrix, viewProjectionInverseMatrix);
            offset = framePassConstantBuffer.Set(d3d12Matrix, 0, GpuStructuredAndConstantBufferStructMetadata::ElementID::INVERSE_VIEW_PROJECTION_MATRIX);

            auto d3d12EyePosition = this->settings.contextImpl->camera.GetPosition();
            offset = framePassConstantBuffer.Set(d3d12EyePosition, 0, GpuStructuredAndConstantBufferStructMetadata::ElementID::EYE_POSITION);

            auto d3d12RenderTargetSize = MathVector2(clientWidth, clientHeight);
            offset = framePassConstantBuffer.Set(d3d12RenderTargetSize, 0, GpuStructuredAndConstantBufferStructMetadata::ElementID::RENDER_TARGET_SIZE);

            auto d3d12RenderTargetInverseSize = MathVector2(1.0f / clientWidth, 1.0f / clientHeight);
            offset = framePassConstantBuffer.Set(d3d12RenderTargetInverseSize, 0, GpuStructuredAndConstantBufferStructMetadata::ElementID::INVERSE_RENDER_TARGET_SIZE);

            float d3d12NearZ = this->settings.contextImpl->camera.GetNearZ();
            offset = framePassConstantBuffer.Set(d3d12NearZ, 0, GpuStructuredAndConstantBufferStructMetadata::ElementID::Z_NEAR);

            float d3d12FarZ = this->settings.contextImpl->camera.GetFarZ();
            offset = framePassConstantBuffer.Set(d3d12FarZ, 0, GpuStructuredAndConstantBufferStructMetadata::ElementID::Z_FAR);

            auto d3d12TotalTime = SimpleTimeCounterToFloat(totalElapsedTime);
            offset = framePassConstantBuffer.Set(d3d12TotalTime, 0, GpuStructuredAndConstantBufferStructMetadata::ElementID::TOTAL_ELAPSED_TIME_SECONDS);

            auto d3d12DeltaTime = SimpleTimeDeltaToFloat(elapsedTime);
            offset = framePassConstantBuffer.Set(d3d12DeltaTime, 0, GpuStructuredAndConstantBufferStructMetadata::ElementID::FRAME_ELAPSED_TIME_SECONDS);
        }
        framePassConstantBuffer.FinishWrites();
    }

    //Materials
    {
        //auto textureOffsetsByDimension = this->settings.contextImpl->assetResources.textureOffsetsByDimension; //Make a copy

        MathMatrix4 textureTransform;
        MathMatrix4Values d3d12Matrix;

        auto& frameMaterialConstantBuffer = frameState.constantBuffers[D3D12StaticMeshRendererKnownConstantBuffer::MATERIAL];
        frameMaterialConstantBuffer.StartWrites();
        for (auto material : frameState.opaqueMaterials)
        {
            auto d3d12Material = static_cast<D3D12Material*>(material->gpuMaterial);

            auto offset = frameMaterialConstantBuffer.Set(material->diffuseColor, d3d12Material->gpuBufferIndex, GpuStructuredAndConstantBufferStructMetadata::ElementID::MATERIAL_DIFFUSE_COLOR);

            offset = frameMaterialConstantBuffer.Set(material->ambientColor, d3d12Material->gpuBufferIndex, GpuStructuredAndConstantBufferStructMetadata::ElementID::MATERIAL_AMBIENT_COLOR);

            offset = frameMaterialConstantBuffer.Set(material->specularColor, d3d12Material->gpuBufferIndex, GpuStructuredAndConstantBufferStructMetadata::ElementID::MATERIAL_SPECULAR_COLOR);

            offset = frameMaterialConstantBuffer.Set(material->selfIlluminationColor, d3d12Material->gpuBufferIndex, GpuStructuredAndConstantBufferStructMetadata::ElementID::MATERIAL_SELF_ILLUMINATION_COLOR);

            offset = frameMaterialConstantBuffer.Set(material->shininess, d3d12Material->gpuBufferIndex, GpuStructuredAndConstantBufferStructMetadata::ElementID::MATERIAL_SHININESS);

            offset = frameMaterialConstantBuffer.Set(material->opacity, d3d12Material->gpuBufferIndex, GpuStructuredAndConstantBufferStructMetadata::ElementID::MATERIAL_OPACITY);

            if (d3d12Material->maps[D3D12Material::MapIndex::DIFFUSE].finjinMap != nullptr)
            {
                auto map = static_cast<FinjinMaterial::Map*>(d3d12Material->maps[D3D12Material::MapIndex::DIFFUSE].finjinMap);
                textureTransform = (MathTranslation(map->textureOffset) * MathAngleAxis(map->textureRotation.ToRadiansValue(), MathVector3(1, 0, 0)) * MathScaling(map->textureScale)).matrix();
            }
            else
                textureTransform.setIdentity();
            GetRowOrderMatrixData(d3d12Matrix, textureTransform);
            offset = frameMaterialConstantBuffer.Set(d3d12Matrix, d3d12Material->gpuBufferIndex, GpuStructuredAndConstantBufferStructMetadata::ElementID::MATERIAL_TEXTURE_COORDINATE_MATRIX);

            for (auto& d3d12Map : d3d12Material->maps)
            {
                if (d3d12Map.texture != nullptr)
                {
                    auto d3d12TextureIndex = static_cast<uint32_t>(d3d12Map.texture->textureIndex);// - textureOffsetsByDimension[d3d12Map.texture->type].baseOffset); //This commented code is for setting the index back to its zero base with respect to the appropriate sampler table
                    offset = frameMaterialConstantBuffer.Set(d3d12TextureIndex, d3d12Material->gpuBufferIndex, (GpuStructuredAndConstantBufferStructMetadata::ElementID)d3d12Map.finjinMap->gpuBufferTextureIndexElementID);

                    offset = frameMaterialConstantBuffer.Set(d3d12Map.finjinMap->amount, d3d12Material->gpuBufferIndex, (GpuStructuredAndConstantBufferStructMetadata::ElementID)d3d12Map.finjinMap->gpuBufferAmountElementID);
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

        auto& frameLightBuffer = frameState.constantBuffers[D3D12StaticMeshRendererKnownConstantBuffer::LIGHT];
        frameLightBuffer.StartWrites();
        for (auto& lightItem : frameState.lights)
        {
            auto light = lightItem.first;
            auto lightState = lightItem.second;

            auto d3d12Light = static_cast<D3D12Light*>(light->gpuLight);

            auto offset = frameLightBuffer.Set(lightState->color, d3d12Light->gpuBufferIndex, GpuStructuredAndConstantBufferStructMetadata::ElementID::LIGHT_COLOR);

            lightState->GetWorldPosition(position4);
            offset = frameLightBuffer.Set(position4, d3d12Light->gpuBufferIndex, GpuStructuredAndConstantBufferStructMetadata::ElementID::LIGHT_POSITION);

            lightState->GetWorldDirection(direction3, direction4Temp);
            offset = frameLightBuffer.Set(direction3, d3d12Light->gpuBufferIndex, GpuStructuredAndConstantBufferStructMetadata::ElementID::LIGHT_DIRECTION);

            offset = frameLightBuffer.Set(lightState->range, d3d12Light->gpuBufferIndex, GpuStructuredAndConstantBufferStructMetadata::ElementID::LIGHT_RANGE);

            offset = frameLightBuffer.Set(lightState->power, d3d12Light->gpuBufferIndex, GpuStructuredAndConstantBufferStructMetadata::ElementID::LIGHT_POWER);

            float coneRange[2] = { lightState->coneRange[0].ToRadiansValue(), lightState->coneRange[1].ToRadiansValue() };
            offset = frameLightBuffer.Set(coneRange, d3d12Light->gpuBufferIndex, GpuStructuredAndConstantBufferStructMetadata::ElementID::LIGHT_CONE_RANGE);

            float sinConeRange[2] = { sinf(coneRange[0] * 0.5f), sinf(coneRange[1] * 0.5f) };
            offset = frameLightBuffer.Set(sinConeRange, d3d12Light->gpuBufferIndex, GpuStructuredAndConstantBufferStructMetadata::ElementID::LIGHT_SIN_CONE_RANGE);
        }
        frameLightBuffer.FinishWrites();
        frameState.lights.clear();
    }
}

void D3D12StaticMeshRenderer::RenderEntity(D3D12StaticMeshRendererFrameState& frameState, SceneNodeState& sceneNodeState, FinjinSceneObjectEntity* entity, const RenderShaderFeatureFlags& entityShaderFeatureFlags, const GpuCommandLights& sortedLights, const MathVector4& ambientLight)
{
    auto& renderTargetState = frameState.renderTargetRenderStates[0];

    assert(!renderTargetState.opaqueEntities.full() && "The entities list is full. Try reconfiguring the system with a higher maximum.");

    auto mesh = entity->meshHandle.asset;
    if (mesh == nullptr)
        return;

    if (!this->settings.contextImpl->ResolveMeshRef(*mesh, entity->meshHandle.assetRef))
        return;

    auto d3d12Mesh = static_cast<D3D12Mesh*>(mesh->gpuMesh);
    if (d3d12Mesh->submeshes.empty())
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
        auto& d3d12Submesh = d3d12Mesh->submeshes[subentityIndex];
        auto& submesh = mesh->submeshes[subentityIndex];
        auto& subentity = entity->subentities[subentityIndex];
        auto material = subentity.materialHandle.asset;
        if (material != nullptr &&
            this->settings.contextImpl->ResolveMaterialRef(*material, subentity.materialHandle.assetRef) &&
            this->settings.contextImpl->ResolveMaterialMaps(*material))
        {
            auto d3d12Material = static_cast<D3D12Material*>(material->gpuMaterial);
            auto inputFormat = d3d12Submesh.GetInputFormat();
            if (inputFormat != nullptr)
            {
                D3D12PipelineStateDescriptor::GraphicsState graphicsState;
                graphicsState.primitiveTopology = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
                graphicsState.shaderFeatures.inputFormatHash = inputFormat->GetElementHash();
                graphicsState.shaderFeatures.flags = entityShaderFeatureFlags.ForMaterialMapsAndMesh(material->gpuMapFlags, mesh->gpuMeshFlags);
                graphicsState.shaderFeatures.lights = sortedLightTypes;
                auto renderStateHash = graphicsState.GetHash();

                auto rootSignatureAndPipelineStateFoundAt = this->graphicsRootSignatureAndPipelineStates.find(renderStateHash);

            #if ALLOW_LIGHT_FALLBACK_IN_FAILED_SIGNATURE_LOOKUP
                //If the lookup failed, keep removing lights in an attempt to find a match
                while (rootSignatureAndPipelineStateFoundAt == this->graphicsRootSignatureAndPipelineStates.end() &&
                    !graphicsState.shaderFeatures.lights.empty())
                {
                    graphicsState.shaderFeatures.lights.pop_back();
                    renderStateHash = graphicsState.GetHash();
                    rootSignatureAndPipelineStateFoundAt = this->graphicsRootSignatureAndPipelineStates.find(renderStateHash);
                }
            #endif

                //Continue if there's a matching root signature/pipeline state
                if (rootSignatureAndPipelineStateFoundAt != this->graphicsRootSignatureAndPipelineStates.end())
                {
                    auto& foundRootSignatureAndPipelineState = rootSignatureAndPipelineStateFoundAt->second;

                    //Get/add root signature
                    auto foundRootSignatureSlot = renderTargetState.RecordRootSignature(foundRootSignatureAndPipelineState.rootSignature);
                    if (foundRootSignatureSlot != nullptr)
                    {
                        //Get/add pipeline state
                        auto foundPipelineStateSlot = renderTargetState.RecordPipelineState(foundRootSignatureSlot, foundRootSignatureAndPipelineState.pipelineState);
                        if (foundPipelineStateSlot != nullptr)
                        {
                            //Get/add heaps
                            ID3D12DescriptorHeap* heaps[] = { this->settings.contextImpl->commonResources.srvTextureDescHeap.heap.Get() };
                            auto heapRef = foundPipelineStateSlot->Record(heaps, FINJIN_COUNT_OF(heaps));
                            if (heapRef != nullptr)
                            {
                                if (!renderTargetState.opaqueEntities.full())
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

                                    for (size_t lightIndex = 0; lightIndex < graphicsState.shaderFeatures.lights.size(); lightIndex++)
                                    {
                                        auto& light = sortedLights[lightIndex];
                                        auto d3d12Light = static_cast<D3D12Light*>(light.light->gpuLight);
                                        instance.lightIndex[lightIndex] = d3d12Light->gpuBufferIndex;
                                    }

                                    heapRef->opaqueEntities.push_back(&instance);

                                    frameState.opaqueMaterials.insert(material);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void D3D12StaticMeshRenderer::RenderQueued(D3D12StaticMeshRendererFrameState& frameState, ID3D12GraphicsCommandList* commandList)
{
    auto& renderTargetState = frameState.renderTargetRenderStates[0];

    {
        MathMatrix4Values d3d12Matrix;

        //Update object CBs
        auto& frameObjectBuffer = frameState.constantBuffers[D3D12StaticMeshRendererKnownConstantBuffer::OBJECT];
        frameObjectBuffer.StartWrites();
        for (auto& item : renderTargetState.opaqueEntities)
        {
            GetRowOrderMatrixData(d3d12Matrix, item.worldMatrix);
            auto offset = frameObjectBuffer.Set(d3d12Matrix, item.gpuBufferIndex, GpuStructuredAndConstantBufferStructMetadata::ElementID::MODEL_MATRIX);

            GetRowOrderMatrixData(d3d12Matrix, item.worldInverseMatrix);
            offset = frameObjectBuffer.Set(d3d12Matrix, item.gpuBufferIndex, GpuStructuredAndConstantBufferStructMetadata::ElementID::INVERSE_MODEL_MATRIX);

            GetRowOrderMatrixData(d3d12Matrix, item.worldInverseTransposeMatrix);
            offset = frameObjectBuffer.Set(d3d12Matrix, item.gpuBufferIndex, GpuStructuredAndConstantBufferStructMetadata::ElementID::INVERSE_TRANSPOSE_MODEL_MATRIX);

            offset = frameObjectBuffer.Set(item.ambientLightColor, item.gpuBufferIndex, GpuStructuredAndConstantBufferStructMetadata::ElementID::AMBIENT_LIGHT_COLOR);

            auto material = item.entity->subentities[item.subentityIndex].materialHandle.asset;
            auto d3d12Material = static_cast<D3D12Material*>(material->gpuMaterial);

            auto materialIndex = static_cast<uint32_t>(d3d12Material->gpuBufferIndex);
            offset = frameObjectBuffer.Set(materialIndex, item.gpuBufferIndex, GpuStructuredAndConstantBufferStructMetadata::ElementID::MATERIAL_INDEX);

            //frameObjectBuffer.SetArray(item.lightIndex, item.gpuBufferIndex, GpuStructuredAndConstantBufferStructMetadata::ElementID::LIGHT_INDEX);
            offset = frameObjectBuffer.Set(item.lightIndex, item.gpuBufferIndex, GpuStructuredAndConstantBufferStructMetadata::ElementID::LIGHT_INDEX);
        }
        frameObjectBuffer.FinishWrites();
        renderTargetState.opaqueEntities.clear();
    }

    {
        auto objectBufferInstanceByteSize = frameState.constantBuffers[D3D12StaticMeshRendererKnownConstantBuffer::OBJECT].GetPaddedTotalSize();
        auto objectBufferResource = frameState.constantBuffers[D3D12StaticMeshRendererKnownConstantBuffer::OBJECT].GetResource();

        for (auto& rootSignatureSlot : renderTargetState.rootSignatureSlots)
        {
            commandList->SetGraphicsRootSignature(rootSignatureSlot.second.rootSignature);

            for (auto& pipelineStateSlot : rootSignatureSlot.second.pipelineStateSlots)
            {
                commandList->SetPipelineState(pipelineStateSlot.second->pipelineState);

                for (auto& heapRefs : pipelineStateSlot.second->heapRefs)
                {
                    commandList->SetDescriptorHeaps(static_cast<UINT>(heapRefs.descriptorHeaps.size()), heapRefs.descriptorHeaps.data());

                    //Pass constant buffer
                    commandList->SetGraphicsRootConstantBufferView
                        (
                        static_cast<UINT>(D3D12StaticMeshRendererKnownRootSignatureBindings::PASS),
                        frameState.constantBuffers[D3D12StaticMeshRendererKnownConstantBuffer::PASS].GetResource()->GetGPUVirtualAddress()
                        );

                    //Material constant buffer
                    commandList->SetGraphicsRootConstantBufferView
                        (
                        static_cast<UINT>(D3D12StaticMeshRendererKnownRootSignatureBindings::MATERIAL),
                        frameState.constantBuffers[D3D12StaticMeshRendererKnownConstantBuffer::MATERIAL].GetResource()->GetGPUVirtualAddress()
                        );

                    //Light constant buffer
                    commandList->SetGraphicsRootConstantBufferView
                        (
                        static_cast<UINT>(D3D12StaticMeshRendererKnownRootSignatureBindings::LIGHT),
                        frameState.constantBuffers[D3D12StaticMeshRendererKnownConstantBuffer::LIGHT].GetResource()->GetGPUVirtualAddress()
                        );

                    //Textures
                    commandList->SetGraphicsRootDescriptorTable
                        (
                        static_cast<UINT>(D3D12StaticMeshRendererKnownRootSignatureBindings::TEXTURE),
                        this->settings.contextImpl->commonResources.srvTextureDescHeap.GetGpuHeapStart()
                        );

                    for (auto item : heapRefs.opaqueEntities)
                    {
                        auto& subentity = item->entity->subentities[item->subentityIndex];

                        auto mesh = item->entity->meshHandle.asset;
                        auto& submesh = mesh->submeshes[item->subentityIndex];

                        auto d3d12Mesh = static_cast<D3D12Mesh*>(mesh->gpuMesh);
                        auto& d3d12Submesh = d3d12Mesh->submeshes[item->subentityIndex];

                        //Object constant buffer
                        commandList->SetGraphicsRootConstantBufferView
                            (
                            static_cast<UINT>(D3D12StaticMeshRendererKnownRootSignatureBindings::OBJECT),
                            objectBufferResource->GetGPUVirtualAddress() + item->gpuBufferIndex * objectBufferInstanceByteSize
                            );

                        //Primitive type
                        commandList->IASetPrimitiveTopology(d3d12Submesh.primitiveType);

                        //Vertex buffer
                        auto vertexBufferView = d3d12Submesh.GetVertexBufferView();
                        commandList->IASetVertexBuffers(0, 1, &vertexBufferView);

                        //Index buffer + draw, or just draw
                        auto indexBufferView = d3d12Submesh.GetIndexBufferView();
                        if (indexBufferView.SizeInBytes > 0)
                        {
                            commandList->IASetIndexBuffer(&indexBufferView);
                            commandList->DrawIndexedInstanced(d3d12Submesh.indexCount, 1, d3d12Submesh.startIndexLocation, d3d12Submesh.baseVertexLocation, 0);
                        }
                        else
                        {

                            commandList->DrawInstanced(d3d12Submesh.vertexCount, 1, 0, 0);
                        }
                    }
                }
            }
        }
        renderTargetState.ClearState();
    }
}

#endif
