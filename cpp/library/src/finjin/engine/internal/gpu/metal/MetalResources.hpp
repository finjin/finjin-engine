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
#include "finjin/common/ByteBuffer.hpp"
#include "finjin/common/DynamicUnorderedMap.hpp"
#include "finjin/common/DynamicVector.hpp"
#include "finjin/common/Hash.hpp"
#include "finjin/common/UsableDynamicVector.hpp"
#include "finjin/common/Utf8String.hpp"
#include "finjin/engine/FinjinSceneAssets.hpp"
#include "MetalInputFormat.hpp"
#include "MetalLight.hpp"
#include "MetalMaterial.hpp"
#include "MetalMesh.hpp"
#include "MetalShader.hpp"
#include "MetalTexture.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    class MetalAssetResources
    {
    public:
        MetalAssetResources();

        void Destroy();

        void ValidateTextureForCreation(const Utf8String& name, Error& error);
        MetalTexture* GetTextureByName(const Utf8String& name);

        void ValidateMeshForCreation(const Utf8String& name, Error& error);
        MetalMesh* GetMeshByName(const Utf8String& name);

        void ValidateMaterialForCreation(const Utf8String& name, Error& error);
        MetalMaterial* GetMaterialByName(const Utf8String& name);

        bool ValidateShaderForCreation(MetalShaderType shaderType, const Utf8String& name, Error& error);
        MetalShader* GetShaderByName(MetalShaderType shaderType, const Utf8String& name);

        MetalInputFormat* GetInputFormatByTypeName(const Utf8String& name);

    public:
        DynamicUnorderedMap<size_t, MetalInputFormat, MapPairConstructNone<size_t, MetalInputFormat>, PassthroughHash> inputFormatsByNameHash;
        DynamicUnorderedMap<size_t, MetalTexture, MapPairConstructSecond<size_t, MetalTexture>, PassthroughHash> texturesByNameHash;
        struct TextureOffset
        {
            TextureOffset()
            {
                Reset();
            }

            void Reset()
            {
                this->baseOffset = 0;
                this->count = 0;
                this->maxCount = 0;
            }

            void SetOffsetAndCount(size_t offset, size_t maxCount)
            {
                this->baseOffset = offset;
                this->count = 0;
                this->maxCount = maxCount;
            }

            size_t GetCurrentOffset() const
            {
                return this->baseOffset + this->count;
            }

            size_t baseOffset;
            size_t count;
            size_t maxCount;
        };
        EnumArray<TextureDimension, TextureDimension::COUNT, TextureOffset> textureOffsetsByDimension;
        using ShadersByNameHash = DynamicUnorderedMap<size_t, MetalShader, MapPairConstructSecond<size_t, MetalShader>, PassthroughHash>;
        EnumArray<MetalShaderType, MetalShaderType::COUNT, ShadersByNameHash> shadersByShaderTypeAndNameHash;
        DynamicUnorderedMap<size_t, MetalMaterial, MapPairConstructSecond<size_t, MetalMaterial>, PassthroughHash> materialsByNameHash;
        DynamicUnorderedMap<size_t, MetalMesh, MapPairConstructSecond<size_t, MetalMesh>, PassthroughHash> meshesByNameHash;
    };

    class MetalFullScreenQuadMesh
    {
    private:
        enum { DEFAULT_BUFFER_INDEX = 0 };

    public:
        MetalFullScreenQuadMesh()
        {
            this->vertexDescriptor = nullptr;
            this->vertexBuffer = nullptr;
            this->vertexCount = 0;
        }

        void Create(id<MTLDevice> device, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            this->vertexDescriptor = [MTLVertexDescriptor vertexDescriptor];
            if (this->vertexDescriptor == nullptr)
            {
                FINJIN_SET_ERROR(error, "Failed to allocate vertex descriptor.");
                return;
            }

            auto layout = this->vertexDescriptor.layouts[0];
            layout.stride = 0;
            layout.stepFunction = MTLVertexStepFunctionPerVertex;
            layout.stepRate = 1;

            {
                auto metalAttribute = this->vertexDescriptor.attributes[0];
                metalAttribute.offset = layout.stride;
                metalAttribute.bufferIndex = DEFAULT_BUFFER_INDEX;
                metalAttribute.format = MTLVertexFormatFloat3;

                layout.stride += sizeof(float) * 3;
            }

            {
                auto metalAttribute = this->vertexDescriptor.attributes[1];
                metalAttribute.offset = layout.stride;
                metalAttribute.bufferIndex = DEFAULT_BUFFER_INDEX;
                metalAttribute.format = MTLVertexFormatFloat2;

                layout.stride += sizeof(float) * 2;
            }

            this->vertexCount = 4;
            this->primitiveType = MTLPrimitiveTypeTriangleStrip;

            const float vertices[] =
            {
                -1,  1, 0,  0, 0, //Upper left
                1,  1, 0,  1, 0, //Upper right
                -1, -1, 0,  0, 1, //Lower left
                1, -1, 0,  1, 1  //Lowwer right
            };

            auto vertexBufferByteSize = layout.stride * this->vertexCount;

            this->vertexBuffer = [device newBufferWithBytes:vertices length:vertexBufferByteSize options:MTLResourceCPUCacheModeDefaultCache];
        }

        void Destroy()
        {
            this->vertexDescriptor = nullptr;
            this->vertexBuffer = nullptr;
            this->vertexCount = 0;
        }

        MTLVertexDescriptor* vertexDescriptor;
        id<MTLBuffer> vertexBuffer;
        NSUInteger vertexCount;
        MTLPrimitiveType primitiveType;
    };

    class MetalCommonResources
    {
    public:
        MetalCommonResources(Allocator* allocator) : fullScreenQuadShaders(allocator)
        {
        }

        void Destroy()
        {
            this->fullScreenQuadGraphicsPipelineState = nullptr;
            this->fullScreenQuadGraphicsPipelineStateDesc = nullptr;
            this->fullScreenQuadMesh.Destroy();
            this->fullScreenQuadShaders.Destroy();
            this->commonShaderLibrary.Destroy();

            this->defaultSamplerDescriptor = nullptr;
            this->defaultSampler = nullptr;
        }

    public:
        UsableDynamicVector<MetalLight> lights;

        MTLSamplerDescriptor* defaultSamplerDescriptor;
        id<MTLSamplerState> defaultSampler;

        MetalShaderLibrary commonShaderLibrary;

        struct FullScreenQuadShaders
        {
            FullScreenQuadShaders(Allocator* allocator) : vertexShader(allocator), fragmentShader(allocator)
            {
            }

            void Destroy()
            {
                this->vertexShader.Destroy();
                this->fragmentShader.Destroy();
            }
            MetalShader vertexShader;
            MetalShader fragmentShader;
        };
        FullScreenQuadShaders fullScreenQuadShaders;
        MetalFullScreenQuadMesh fullScreenQuadMesh;
        MTLRenderPipelineDescriptor* fullScreenQuadGraphicsPipelineStateDesc;
        id<MTLRenderPipelineState> fullScreenQuadGraphicsPipelineState;
    };

} }
