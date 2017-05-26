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
#include "finjin/common/Error.hpp"
#include "D3D12Includes.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    class D3D12DescriptorHeap
    {
    public:
        D3D12DescriptorHeap();

        void Create(ID3D12Device* device, const D3D12_DESCRIPTOR_HEAP_DESC& desc, Error& error);
        void Destroy();

        CD3DX12_CPU_DESCRIPTOR_HANDLE GetCpuHeapStart(size_t descriptorOffset = 0);
        CD3DX12_GPU_DESCRIPTOR_HANDLE GetGpuHeapStart(size_t descriptorOffset = 0);

    public:
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> heap;
        UINT descriptorSize;
        D3D12_DESCRIPTOR_HEAP_DESC desc;
    };

} }
