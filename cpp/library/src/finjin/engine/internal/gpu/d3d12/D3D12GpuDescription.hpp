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


//Includes---------------------------------------------------------------------
#include "finjin/common/Vector.hpp"
#include "D3D12GpuOutput.hpp"
#include "D3D12GpuID.hpp"
#include <dxgi1_4.h>
#include <d3d12.h>


//Classes----------------------------------------------------------------------
namespace Finjin { namespace Engine {
    
    using namespace Finjin::Common;

    struct D3D12GpuFeatures
    {
        //Initialized in D3D12System
        D3D_FEATURE_LEVEL d3dMaximumFeatureLevel;
        D3D12_FEATURE_DATA_D3D12_OPTIONS options;
        D3D12_FEATURE_DATA_GPU_VIRTUAL_ADDRESS_SUPPORT gpuVirtualSupport;
        StaticVector<D3D12_FEATURE_DATA_ARCHITECTURE, 10> dataArchitectures;
        StaticVector<D3D12_FEATURE_DATA_FORMAT_SUPPORT, 144> formatSupport;

        //Initialized in D3D12GpuContext
        StaticVector<D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS, D3D12_MAX_MULTISAMPLE_SAMPLE_COUNT> multisampleQualityLevels;        
    };
    
    struct D3D12GpuDescription
    {
        D3D12GpuDescription();
        D3D12GpuDescription(UINT index, const DXGI_ADAPTER_DESC1& desc);

        UINT index; //Adapter index
        DXGI_ADAPTER_DESC1 desc; //Adapter description
        D3D12GpuFeatures features;

        D3D12GpuOutputs outputs;

        D3D12GpuID GetGpuID() const;

        uint64_t GetDedicatedGpuMemorySize() const;
        uint64_t GetDedicatedSystemMemorySize() const;
        uint64_t GetSharedMemorySize() const;
        uint64_t GetTotalMemorySize() const;
    };
    class D3D12HardwareGpuDescriptions : public StaticVector<D3D12GpuDescription, EngineConstants::MAX_GPU_ADAPTERS>
    {
    public:
        void SortBestToWorst();

        const D3D12GpuDescription* GetByGpuID(const D3D12GpuID& gpuID) const;
    };

    using D3D12SoftwareGpuDescriptions = StaticVector<D3D12GpuDescription, 1>;

} }
