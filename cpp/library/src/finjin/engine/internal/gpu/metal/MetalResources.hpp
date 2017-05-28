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

        template <typename T>
        void ValidateTextureForCreation(const T& name, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);
            
            if (this->texturesByNameHash.full())
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Unable to create texture '%1%'. Texture lookup is full.", name));
                return;
            }
            
            Utf8StringHash hash;
            if (this->texturesByNameHash.contains(hash(name)))
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Unable to create texture '%1%'. The name already exists.", name));
                return;
            }
        }
        
        template <typename T>
        MetalTexture* GetTextureByName(const T& name)
        {
            Utf8StringHash hash;
            auto foundAt = this->texturesByNameHash.find(hash(name));
            if (foundAt != this->texturesByNameHash.end())
                return &foundAt->second;
            return nullptr;
        }

        template <typename T>
        void ValidateMeshForCreation(const T& name, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);
            
            if (this->meshesByNameHash.full())
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Unable to create mesh '%1%'. Mesh lookup is full.", name));
                return;
            }
            
            Utf8StringHash hash;
            if (this->meshesByNameHash.contains(hash(name)))
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Unable to create mesh '%1%'. The name already exists.", name));
                return;
            }
        }
        
        template <typename T>
        MetalMesh* GetMeshByName(const T& name)
        {
            Utf8StringHash hash;
            auto foundAt = this->meshesByNameHash.find(hash(name));
            if (foundAt != this->meshesByNameHash.end())
                return &foundAt->second;
            return nullptr;
        }

        template <typename T>
        void ValidateMaterialForCreation(const T& name, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);
            
            if (this->materialsByNameHash.full())
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Unable to create material '%1%'. Material lookup is full.", name));
                return;
            }
            
            Utf8StringHash hash;
            if (this->materialsByNameHash.contains(hash(name)))
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Unable to create material '%1%'. The name already exists.", name));
                return;
            }
        }
        
        template <typename T>
        MetalMaterial* GetMaterialByName(const T& name)
        {
            Utf8StringHash hash;
            auto foundAt = this->materialsByNameHash.find(hash(name));
            if (foundAt != this->materialsByNameHash.end())
                return &foundAt->second;
            return nullptr;
        }

        template <typename T>
        bool ValidateShaderForCreation(MetalShaderType shaderType, const T& name, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);
            
            auto& shadersByNameHash = this->shadersByShaderTypeAndNameHash[shaderType];
            
            if (shadersByNameHash.full())
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Unable to create shader '%1%'. Shader lookup is full.", name));
                return false;
            }
            
            //Encountering a duplicate shader isn't an error condition
            Utf8StringHash hash;
            if (shadersByNameHash.contains(hash(name)))
                return false;
            
            return true;
        }
        
        template <typename T>
        MetalShader* GetShaderByName(MetalShaderType shaderType, const T& name)
        {
            auto& shadersByNameHash = this->shadersByShaderTypeAndNameHash[shaderType];
            
            Utf8StringHash hash;
            auto foundAt = shadersByNameHash.find(hash(name));
            if (foundAt != shadersByNameHash.end())
                return &foundAt->second;
            return nullptr;
        }
        
        template <typename T>
        MetalInputFormat* GetInputFormatByTypeName(const T& name)
        {
            Utf8StringHash hash;
            auto foundAt = this->inputFormatsByNameHash.find(hash(name));
            if (foundAt != this->inputFormatsByNameHash.end())
                return &foundAt->second;
            return nullptr;
        }

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
        MetalFullScreenQuadMesh();

        void Create(id<MTLDevice> device, Error& error);
        void Destroy();

        MTLVertexDescriptor* vertexDescriptor;
        id<MTLBuffer> vertexBuffer;
        NSUInteger vertexCount;
        MTLPrimitiveType primitiveType;
    };

    class MetalCommonResources
    {
    public:
        MetalCommonResources(Allocator* allocator);

        void Destroy();

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
