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
#include "D3D12Includes.hpp"
#include "D3D12ShaderType.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    class D3D12PipelineStateDescriptor
    {
    public:
        D3D12PipelineStateDescriptor(Allocator* allocator);

        template <typename Collection, typename T>
        static D3D12PipelineStateDescriptor* GetByTypeName(Collection& collection, const T& name)
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
            DynamicVector<D3D12PipelineStateDescriptor>& pipelineStates,
            Allocator* allocator,
            const ByteBuffer& readBuffer,
            SimpleUri& tempUri,
            Error& error
            );

        static void Create
            (
            AssetReference& shaderFileRef,
            DynamicVector<D3D12PipelineStateDescriptor>& pipelineStates,
            Allocator* allocator,
            ConfigDocumentReader& reader,
            SimpleUri& tempUri,
            Error& error
            );

        static void CreateFromScope
            (
            D3D12PipelineStateDescriptor& pipelineState,
            Allocator* allocator,
            ConfigDocumentReader& reader,
            SimpleUri& tempUri,
            D3D12PipelineStateDescriptor* otherPipelineStates,
            size_t otherPipelineStateCount,
            Error& error
            );

    public:
        Utf8String typeName;

        struct GraphicsState
        {
            GraphicsState() : primitiveTopology(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE)
            {
            }

            size_t GetHash() const
            {
                size_t values[] = { (size_t)this->primitiveTopology.value, this->shaderFeatures.GetHash() };
                return Hash::Bytes(values, sizeof(values));
            }

            Setting<D3D12_PRIMITIVE_TOPOLOGY_TYPE> primitiveTopology;
            ShaderFeatures shaderFeatures;
        };

        Utf8String rootSignatureType;
        Utf8String inputFormatType;
        struct ShaderOffset
        {
            ShaderOffset() { this->offset = this->size = (size_t)-1; }

            bool IsValid() const
            {
                return this->offset != (size_t)-1 && this->size != (size_t)-1;
            }

            ValueOrError<bool> Parse(const Utf8StringView& value)
            {
                size_t valueIndex = 0;
                return Split(value, ' ', [this, &valueIndex](Utf8StringView& item)
                {
                    if (valueIndex > 1)
                        return ValueOrError<bool>::CreateError();

                    if (valueIndex == 0)
                        this->offset = Convert::ToInteger(item, this->offset);
                    else
                        this->size = Convert::ToInteger(item, this->size);
                    valueIndex++;

                    return ValueOrError<bool>(true);
                });
            }

            size_t offset;
            size_t size;
        };
        EnumArray<D3D12ShaderType, D3D12ShaderType::COUNT, ShaderOffset> shaderOffsets;
        EnumArray<D3D12ShaderType, D3D12ShaderType::COUNT, AssetReference> shaderRefs;
        GraphicsState graphicsState;
    };

} }
