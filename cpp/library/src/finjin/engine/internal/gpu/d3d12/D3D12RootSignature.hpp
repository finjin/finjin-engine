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
#include "finjin/common/Hash.hpp"
#include "D3D12Includes.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    class D3D12RootSignatureDescriptor
    {
    public:
        D3D12RootSignatureDescriptor(Allocator* allocator);

        void CreateRootSignature(Microsoft::WRL::ComPtr<ID3D12RootSignature>& result, ID3D12Device* device, Error& error);

        template <typename Collection, typename T>
        static D3D12RootSignatureDescriptor* GetByTypeName(Collection& collection, const T& name)
        {
            for (auto& item : collection)
            {
                if (item.typeName == name)
                    return &item;
            }
            return nullptr;
        }

        struct DescriptorTableRangeElement
        {
            DescriptorTableRangeElement(Allocator* allocator) : id(allocator)
            {
                this->type = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
                this->shaderRegister = 0;
                this->registerSpace = 0;
                this->descriptorCount = 0;
            }

            size_t GetHash() const
            {
                size_t values[] = { (size_t)this->type, this->id.GetHash(), this->shaderRegister, this->registerSpace, this->descriptorCount };
                return Hash::Bytes(values, sizeof(values));
            }

            template <typename Collection>
            static size_t Hash(Collection& c)
            {
                StaticVector<size_t, 50> values;
                for (auto& item : c)
                {
                    if (values.push_back(item.GetHash()).HasErrorOrValue(false))
                        break;
                }

                assert(values.size() == c.size());
                assert(!values.full());
                values.push_back(c.size());

                return Hash::Bytes(values.data(), sizeof(size_t) * values.size());
            }

            D3D12_DESCRIPTOR_RANGE_TYPE type;
            Utf8String id;
            size_t shaderRegister;
            size_t registerSpace;
            size_t descriptorCount;
        };

        template <typename T>
        size_t GetIndexOfElement(const T& name) const
        {
            for (size_t index = 0; index < this->elements.size(); index++)
            {
                if (this->elements[index].id == name)
                    return index;
            }

            return (size_t)-1;
        }

        size_t GetHash() const
        {
            size_t values[] = { this->typeName.GetHash(), RootSignatureElement::Hash(this->elements) };
            return Hash::Bytes(values, sizeof(values));
        }

        struct RootSignatureElement
        {
            RootSignatureElement(Allocator* allocator) : id(allocator)
            {
                this->type = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
                this->shaderRegister = 0;
                this->registerSpace = 0;
                this->visibility = D3D12_SHADER_VISIBILITY_ALL;
            }

            size_t GetHash() const
            {
                size_t values[] = { (size_t)this->type, this->id.GetHash(), this->shaderRegister, this->registerSpace, (size_t)this->visibility, DescriptorTableRangeElement::Hash(this->descriptorTable) };
                return Hash::Bytes(values, sizeof(values));
            }

            template <typename Collection>
            static size_t Hash(Collection& c)
            {
                StaticVector<size_t, 50> values;
                for (auto& item : c)
                {
                    if (values.push_back(item.GetHash()).HasErrorOrValue(false))
                        break;
                }

                assert(values.size() == c.size());
                assert(!values.full());
                values.push_back(c.size());

                return Hash::Bytes(values.data(), sizeof(size_t) * values.size());
            }

            D3D12_ROOT_PARAMETER_TYPE type;
            Utf8String id;
            size_t shaderRegister;
            size_t registerSpace;
            D3D12_SHADER_VISIBILITY visibility;
            DynamicVector<DescriptorTableRangeElement> descriptorTable;
        };

        static void Create
            (
            DynamicVector<D3D12RootSignatureDescriptor>& rootSignatures,
            Allocator* allocator,
            const ByteBuffer& readBuffer,
            Error& error
            );

        static void Create
            (
            DynamicVector<D3D12RootSignatureDescriptor>& rootSignatures,
            Allocator* allocator,
            ConfigDocumentReader& reader,
            Error& error
            );

        static void CreateFromScope
            (
            D3D12RootSignatureDescriptor& rootSignature,
            Allocator* allocator,
            ConfigDocumentReader& reader,
            Error& error
            );

    public:
        Utf8String typeName;
        DynamicVector<RootSignatureElement> elements;
    };

} }
