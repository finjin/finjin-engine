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
#include "finjin/common/ConfigDocumentReader.hpp"
#include "finjin/common/DynamicVector.hpp"
#include "finjin/common/EnumArray.hpp"
#include "finjin/common/Setting.hpp"
#include "finjin/engine/AssetReference.hpp"
#include "finjin/engine/ShaderFeatures.hpp"
#include "MetalIncludes.hpp"
#include "MetalShaderType.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    class MetalPipelineStateDescriptor
    {
    public:
        MetalPipelineStateDescriptor(Allocator* allocator);

        template <typename Collection, typename T>
        static MetalPipelineStateDescriptor* GetByTypeName(Collection& collection, const T& name)
        {
            for (auto& item : collection)
            {
                if (item.typeName == name)
                    return &item;
            }
            return nullptr;
        }

        static void Create
            (
            AssetReference& shaderFileRef,
            DynamicVector<MetalPipelineStateDescriptor>& pipelineStates,
            Allocator* allocator,
            const ByteBuffer& readBuffer,
            SimpleUri& tempUri,
            Error& error
            );

        static void Create
            (
            AssetReference& shaderFileRef,
            DynamicVector<MetalPipelineStateDescriptor>& pipelineStates,
            Allocator* allocator,
            ConfigDocumentReader& reader,
            SimpleUri& tempUri,
            Error& error
            );

        static void CreateFromScope
            (
            MetalPipelineStateDescriptor& pipelineState,
            Allocator* allocator,
            ConfigDocumentReader& reader,
            SimpleUri& tempUri,
            MetalPipelineStateDescriptor* otherPipelineStates,
            size_t otherPipelineStateCount,
            Error& error
            );

    public:
        Utf8String typeName;

        struct GraphicsState
        {
            GraphicsState()
            {
            #if FINJIN_TARGET_PLATFORM == FINJIN_TARGET_PLATFORM_MACOS
                this->primitiveTopology.Reset(MTLPrimitiveTopologyClassTriangle);
            #endif
            }

            size_t GetHash() const
            {
            #if FINJIN_TARGET_PLATFORM == FINJIN_TARGET_PLATFORM_MACOS
                size_t values[] = { (size_t)this->primitiveTopology.value, this->shaderFeatures.GetHash() };
                return Hash::Bytes(values, sizeof(values));
            #else
                return this->shaderFeatures.GetHash();
            #endif
            }

        #if FINJIN_TARGET_PLATFORM == FINJIN_TARGET_PLATFORM_MACOS
            Setting<MTLPrimitiveTopologyClass> primitiveTopology;
        #endif
            ShaderFeatures shaderFeatures;
        };

        Utf8String inputFormatType;
        EnumArray<MetalShaderType, MetalShaderType::COUNT, Utf8String> shaderFunctionNames;
        GraphicsState graphicsState;
    };

} }
