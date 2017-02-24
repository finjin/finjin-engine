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

#include "D3D12DescriptorHeap.hpp"

using namespace Finjin::Engine;


//Implementation---------------------------------------------------------------
D3D12DescriptorHeap::D3D12DescriptorHeap() : descriptorSize(0)
{
}

void D3D12DescriptorHeap::Create(ID3D12Device* device, const D3D12_DESCRIPTOR_HEAP_DESC& desc, Error& error)
{
    this->desc = desc;

    auto result = device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&this->heap));
    if (!SUCCEEDED(result))
    {
        FINJIN_SET_ERROR(error, "Failed to create descriptor heap.");
        return;
    }

    this->descriptorSize = device->GetDescriptorHandleIncrementSize(desc.Type);
}

D3D12_CPU_DESCRIPTOR_HANDLE D3D12DescriptorHeap::GetCpuHeapStart()
{
    return this->heap->GetCPUDescriptorHandleForHeapStart();
}

D3D12_GPU_DESCRIPTOR_HANDLE D3D12DescriptorHeap::GetGpuHeapStart()
{
    return this->heap->GetGPUDescriptorHandleForHeapStart();
}

#endif
