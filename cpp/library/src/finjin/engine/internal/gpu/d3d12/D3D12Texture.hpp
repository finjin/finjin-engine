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
#include "finjin/common/DynamicVector.hpp"
#include "finjin/common/Error.hpp"
#include "finjin/common/PNGReader.hpp"
#include "finjin/engine/DDSReader.hpp"
#include "finjin/engine/TextureDimension.hpp"
#include "D3D12Includes.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    class D3D12Texture
    {
    public:
        D3D12Texture(Allocator* allocator);

        void CreateFromDDS
            (
            const DDSReader& ddsReader,
            ID3D12Device* device,
            const void* bytes,
            size_t byteCount,
            DynamicVector<D3D12_SUBRESOURCE_DATA>& preallocatedSubresourceData,
            ByteBuffer& preallocatedFootprintSubresourceData,
            size_t maxDimension,
            Error& error
            );

        void CreateFromPNG
            (
            const PNGReader& pngReader,
            ID3D12Device* device,
            const void* bytes,
            size_t byteCount,
            DynamicVector<D3D12_SUBRESOURCE_DATA>& preallocatedSubresourceData,
            ByteBuffer& preallocatedFootprintSubresourceData,
            size_t maxDimension,
            Error& error
            );

        void Destroy();

        void HandleCreationFailure();
        void ReleaseUploaders();

        bool IsResidentOnGpu() const;
        void UpdateResidentOnGpuStatus();

    public:
        Microsoft::WRL::ComPtr<ID3D12Resource> resource;
        Microsoft::WRL::ComPtr<ID3D12Resource> uploadHeapResource;

        DynamicVector<D3D12_SUBRESOURCE_DATA> subresourceData;
        ByteBuffer footprintSubresourceData;

        ByteBuffer textureData;

        Utf8String name;
        uint32_t textureIndex;

        TextureDimension textureDimension;

        size_t isResidentCountdown;
        D3D12Texture* waitingToBeResidentNext;
    };

} }
