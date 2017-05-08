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

#include "D3D12SystemImpl.hpp"

using namespace Finjin::Engine;


//Implementation----------------------------------------------------------------
D3D12SystemImpl::D3D12SystemImpl(Allocator* allocator) : AllocatedClass(allocator)
{
}

const D3D12GpuOutputs* D3D12SystemImpl::GetAdapterOutputs(const D3D12GpuID& gpuID) const
{
    //Check software adapter first
    for (auto& desc : this->softwareGpuDescriptions)
    {
        if (gpuID == desc.desc.AdapterLuid)
            return &desc.outputs;
    }

    //Check hardware adapters
    for (auto& desc : this->hardwareGpuDescriptions)
    {
        if (gpuID == desc.desc.AdapterLuid)
            return &desc.outputs;
    }

    return nullptr;
}

bool D3D12SystemImpl::IsDebugEnabled() const
{
    return this->settings.enableDebug;
}

void D3D12SystemImpl::EnumerateHardwareGpus()
{
    Microsoft::WRL::ComPtr<IDXGIAdapter1> adapter;
    for (UINT adapterIndex = 0;
        !this->hardwareGpuDescriptions.full() && this->dxgiFactory->EnumAdapters1(adapterIndex, &adapter) != DXGI_ERROR_NOT_FOUND;
        adapterIndex++)
    {
        DXGI_ADAPTER_DESC1 dxgiDesc;
        adapter->GetDesc1(&dxgiDesc);

        //Hardware devices only
        if ((dxgiDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) == 0)
        {
            //Ensure adapter supports minimum feature level
            Microsoft::WRL::ComPtr<ID3D12Device> device;
            if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), this->settings.d3dMinimumFeatureLevel, IID_PPV_ARGS(&device))))
            {
                auto gpuIndex = this->hardwareGpuDescriptions.size();
                this->hardwareGpuDescriptions.push_back();
                auto& desc = this->hardwareGpuDescriptions[gpuIndex];

                desc.index = adapterIndex;
                desc.desc = dxgiDesc;

                EnumerateAdapterOutputs(adapter.Get(), desc.outputs);

                EnumerateDeviceFeatures(device.Get(), desc.features);
            }
        }
    }
    this->hardwareGpuDescriptions.SortBestToWorst();
}

void D3D12SystemImpl::EnumerateSoftwareGpus()
{
    Microsoft::WRL::ComPtr<IDXGIAdapter> adapter;
    if (SUCCEEDED(this->dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&adapter))))
    {
        DXGI_ADAPTER_DESC dxgiDesc;
        adapter->GetDesc(&dxgiDesc);

        //Ensure adapter supports minimum feature level
        Microsoft::WRL::ComPtr<ID3D12Device> device;
        if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), this->settings.d3dMinimumFeatureLevel, IID_PPV_ARGS(&device))))
        {
            this->softwareGpuDescriptions.resize(1);
            auto& desc = this->softwareGpuDescriptions[0];
            FINJIN_ZERO_ITEM(desc.desc);
            FINJIN_COPY_MEMORY(&desc.desc, &dxgiDesc, sizeof(desc.desc));

            //Note: Software GPUs don't have outputs, so no need to enumerate them

            EnumerateDeviceFeatures(device.Get(), desc.features);
        }
    }
}

void D3D12SystemImpl::EnumerateAdapterOutputs(IDXGIAdapter* adapter, D3D12GpuOutputs& outputs)
{
    //Output count
    size_t outputCount = 0;
    Microsoft::WRL::ComPtr<IDXGIOutput> output;
    for (UINT i = 0; SUCCEEDED(adapter->EnumOutputs(i, &output)); i++)
        outputCount++;

    //Outputs
    outputs.resize(outputCount);
    for (UINT i = 0; i < outputs.size() && SUCCEEDED(adapter->EnumOutputs(i, &output)); i++)
    {
        output->GetDesc(&outputs[i].desc);

        //Display modes
        outputs[i].displayModeFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
        UINT displayModeCount = 0;
        if (SUCCEEDED(output->GetDisplayModeList(outputs[i].displayModeFormat, 0, &displayModeCount, nullptr)))
        {
            displayModeCount = static_cast<UINT>(outputs[i].displayModes.resize(displayModeCount));
            if (SUCCEEDED(output->GetDisplayModeList(outputs[i].displayModeFormat, 0, &displayModeCount, &outputs[i].displayModes[0])))
            {
                //Do nothing
            }
        }
    }
}

void D3D12SystemImpl::GetHardwareD3DAdapter(D3D12GpuID& gpuID, IDXGIAdapter1** ppAdapter, D3D12GpuDescription& desc, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    *ppAdapter = nullptr;

    Microsoft::WRL::ComPtr<IDXGIAdapter1> adapter;
    for (const auto& gpuDesc : this->hardwareGpuDescriptions)
    {
        if (gpuID.IsZero() || gpuID == gpuDesc.desc.AdapterLuid)
        {
            if (SUCCEEDED(this->dxgiFactory->EnumAdapters1(gpuDesc.index, &adapter)) &&
                SUCCEEDED(D3D12CreateDevice(adapter.Get(), this->settings.d3dMinimumFeatureLevel, _uuidof(ID3D12Device), nullptr)))
            {
                gpuID = gpuDesc.desc.AdapterLuid; //In case a zero ID was specified
                *ppAdapter = adapter.Detach();
                desc = gpuDesc;
                return;
            }

            if (!gpuID.IsZero())
            {
                //If we got this far something failed
                FINJIN_SET_ERROR(error, "Failed to initialize specified adapter.");
                return;
            }
        }
    }
}

void D3D12SystemImpl::EnumerateDeviceFeatures(ID3D12Device* device, D3D12GpuFeatures& features)
{
    //Feature level---------------------------
    StaticVector<D3D_FEATURE_LEVEL, 32> d3dFeatureLevelsList;
    {
        d3dFeatureLevelsList.maximize();
        auto count = D3D12GpuFeatures::GetStandardFeatureLevelsLowestToHighest(d3dFeatureLevelsList.data(), d3dFeatureLevelsList.max_size());
        d3dFeatureLevelsList.resize(count);
    }

    D3D_FEATURE_LEVEL maxFeatureLevel = d3dFeatureLevelsList.front();
    D3D12_FEATURE_DATA_FEATURE_LEVELS d3dFeatureLevels;
    d3dFeatureLevels.NumFeatureLevels = static_cast<UINT>(d3dFeatureLevelsList.size());
    d3dFeatureLevels.pFeatureLevelsRequested = d3dFeatureLevelsList.data();
    if (SUCCEEDED(device->CheckFeatureSupport(D3D12_FEATURE_FEATURE_LEVELS, &d3dFeatureLevels, sizeof(d3dFeatureLevels))))
        maxFeatureLevel = d3dFeatureLevels.MaxSupportedFeatureLevel;
    features.SetStandardFeatures(maxFeatureLevel);

    //Max shader model--------------------------
    features.maxShaderModel = (D3D_SHADER_MODEL)0;
    for (auto shaderModel : { D3D_SHADER_MODEL_6_0, D3D_SHADER_MODEL_5_1 })
    {
        D3D12_FEATURE_DATA_SHADER_MODEL maxShaderModel;
        maxShaderModel.HighestShaderModel = shaderModel;

        //Note: This does not appear to ever succeed on my desktop Win10/GTX 980 machine
        if (SUCCEEDED(device->CheckFeatureSupport(D3D12_FEATURE_SHADER_MODEL, &maxShaderModel, sizeof(maxShaderModel))))
        {
            features.maxShaderModel = shaderModel;
            break;
        }
    }
    if (features.maxShaderModel == (D3D_SHADER_MODEL)0)
        features.maxShaderModel = D3D_SHADER_MODEL_5_1; //D3D12 = 5.1 minimum

    //Options-------------------------------

    //General
    if (SUCCEEDED(device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS, &features.options, sizeof(features.options))))
    {
        //This will always succeed
    }
    //More
    if (SUCCEEDED(device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS1, &features.options1, sizeof(features.options1))))
    {
        //This will always succeed
    }

    //Virtual address support--------------
    if (SUCCEEDED(device->CheckFeatureSupport(D3D12_FEATURE_GPU_VIRTUAL_ADDRESS_SUPPORT, &features.gpuVirtualAddressSupport, sizeof(features.gpuVirtualAddressSupport))))
    {
        //This will always succeed
    }

    //Architecture---------------------
    D3D12_FEATURE_DATA_ARCHITECTURE architecture;
    for (architecture.NodeIndex = 0; architecture.NodeIndex < features.dataArchitectures.max_size(); architecture.NodeIndex++)
    {
        if (SUCCEEDED(device->CheckFeatureSupport(D3D12_FEATURE_ARCHITECTURE, &architecture, sizeof(architecture))))
            features.dataArchitectures.push_back(architecture);
        else
            break;
    }

    //Supported formats--------------------
    D3D12_FEATURE_DATA_FORMAT_SUPPORT formatSupport;
    for (formatSupport.Format = static_cast<DXGI_FORMAT>(DXGI_FORMAT_UNKNOWN + 1);
        formatSupport.Format <= static_cast<DXGI_FORMAT>(features.formatSupport.max_size());
        formatSupport.Format = static_cast<DXGI_FORMAT>(formatSupport.Format + 1))
    {
        if (SUCCEEDED(device->CheckFeatureSupport(D3D12_FEATURE_FORMAT_SUPPORT, &formatSupport, sizeof(formatSupport))))
            features.formatSupport.push_back(formatSupport);
    }
}

#endif
