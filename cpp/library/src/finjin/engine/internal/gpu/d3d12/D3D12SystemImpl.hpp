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
#include "D3D12Texture.hpp"
#include "D3D12DescriptorHeap.hpp"
#include "D3D12GpuContext.hpp"
#include "D3D12SystemSettings.hpp"
#include "D3D12Includes.hpp"
#include "D3D12GpuDescription.hpp"
#include "finjin/common/Error.hpp"
#include "finjin/common/StaticVector.hpp"
#include "finjin/engine/WindowSize.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    class D3D12SystemImpl : public AllocatedClass
    {
    public:
        D3D12SystemImpl(Allocator* allocator);

        const D3D12GpuOutputs* GetAdapterOutputs(const Luid& gpuID) const;

        bool IsDebugEnabled() const;

        void EnumerateAdapterOutputs(IDXGIAdapter* adapter, D3D12GpuOutputs& outputs);

        void EnumerateHardwareGpus();
        void EnumerateSoftwareGpus();
        void GetHardwareD3DAdapter(Luid& gpuID, IDXGIAdapter1** ppAdapter, D3D12GpuDescription& desc, Error& error);

    private:
        void EnumerateDeviceFeatures(ID3D12Device* device, D3D12GpuFeatures& features);

    public:
        D3D12SystemSettings settings;
        D3D12HardwareGpuDescriptions hardwareGpuDescriptions;
        D3D12SoftwareGpuDescriptions softwareGpuDescriptions;

        Microsoft::WRL::ComPtr<IDXGIFactory4> dxgiFactory;

        StaticVector<D3D12GpuContext*, EngineConstants::MAX_APPLICATION_VIEWPORTS> contexts;
    };

} }
