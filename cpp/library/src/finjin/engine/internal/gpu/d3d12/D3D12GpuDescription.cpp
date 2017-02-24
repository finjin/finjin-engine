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

#include "D3D12GpuDescription.hpp"

using namespace Finjin::Engine;


//Implementation---------------------------------------------------------------
//D3D12GpuDescription
D3D12GpuDescription::D3D12GpuDescription()
{
    this->index = 0;
    FINJIN_ZERO_ITEM(this->desc);
}

D3D12GpuDescription::D3D12GpuDescription(UINT index, const DXGI_ADAPTER_DESC1& desc)
{
    this->index = index;
    this->desc = desc;
}

D3D12GpuID D3D12GpuDescription::GetGpuID() const
{
    return this->desc.AdapterLuid;
}

uint64_t D3D12GpuDescription::GetDedicatedGpuMemorySize() const
{
    return this->desc.DedicatedVideoMemory;
}

uint64_t D3D12GpuDescription::GetDedicatedSystemMemorySize() const
{
    return this->desc.DedicatedSystemMemory;
}

uint64_t D3D12GpuDescription::GetSharedMemorySize() const
{
    return this->desc.SharedSystemMemory;
}

uint64_t D3D12GpuDescription::GetTotalMemorySize() const
{
    return this->desc.DedicatedVideoMemory + this->desc.SharedSystemMemory;
}

//D3D12HardwareGpuDescriptions
void D3D12HardwareGpuDescriptions::SortBestToWorst()
{
    //NOTE: Should probably take other factors into account

    //Order by the various GPU memory types/sizes
    std::sort(begin(), end(), [](const D3D12GpuDescription& a, const D3D12GpuDescription& b) {
        if (a.GetDedicatedGpuMemorySize() != b.GetDedicatedGpuMemorySize())
            return a.GetDedicatedGpuMemorySize() < b.GetDedicatedGpuMemorySize();
        if (a.GetSharedMemorySize() != b.GetSharedMemorySize())
            return a.GetSharedMemorySize() < b.GetSharedMemorySize();
        return a.GetTotalMemorySize() < b.GetTotalMemorySize();
    });
}

const D3D12GpuDescription* D3D12HardwareGpuDescriptions::GetByGpuID(const D3D12GpuID& gpuID) const
{
    for (const auto& desc : *this)
    {
        if (gpuID == desc.desc.AdapterLuid)
            return &desc;
    }

    return nullptr;
}

#endif
