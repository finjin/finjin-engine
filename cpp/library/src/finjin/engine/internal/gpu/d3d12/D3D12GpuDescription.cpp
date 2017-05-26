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


//Local data--------------------------------------------------------------------

//This cuts off at feature level 11.0. Anything lower isn't going to be supported by this engine
static const D3D12GpuStandardFeatureLevelFeatures standardFeatureLevelFeatures[] =
{
    {
        D3D_FEATURE_LEVEL_12_1, Version(12, 1),
        D3D12GpuStandardFeatureLevelFeatures::ShaderModel::MODEL_5_1, Version(5, 1),
        D3D12GpuStandardFeatureLevelFeatures::Tier::TIER_2,
        D3D12GpuStandardFeatureLevelFeatures::Tier::TIER_1,
        D3D12GpuStandardFeatureLevelFeatures::FeatureBoolPlus::YES,
        D3D12GpuStandardFeatureLevelFeatures::FeatureBoolPlus::YES,
        D3D12GpuStandardFeatureLevelFeatures::FeatureBoolPlus::NOT_REQUIRED,
        D3D12GpuStandardFeatureLevelFeatures::FeatureBoolPlus::NOT_REQUIRED,
        18,
        true,
        true,
        D3D12GpuStandardFeatureLevelFeatures::FeatureBoolPlus::YES,
        true,
        true,
        true,
        true,
        true,
        true,
        D3D12GpuStandardFeatureLevelFeatures::FeatureBoolPlus::YES,
        D3D12GpuStandardFeatureLevelFeatures::FeatureBoolPlus::YES,
        D3D12GpuStandardFeatureLevelFeatures::FeatureBoolPlus::YES,
        true,
        D3D12GpuStandardFeatureLevelFeatures::FeatureBoolPlus::YES,
        64,
        D3D12GpuStandardFeatureLevelFeatures::FeatureBoolPlus::YES,
        16,
        D3D12GpuStandardFeatureLevelFeatures::FeatureBoolPlus::YES,
        D3D12GpuStandardFeatureLevelFeatures::FeatureBoolYesOrNotRequired::YES,
        16384,
        16384,
        2048,
        16384,
        16,
        4294967295,
        4294967295,
        32,
        8,
        true,
        true,
        true,
        true,
        true,
        true,
        false,
        true,
    },
    {
        D3D_FEATURE_LEVEL_12_0, Version(12, 0),
        D3D12GpuStandardFeatureLevelFeatures::ShaderModel::MODEL_5_1, Version(5, 1),
        D3D12GpuStandardFeatureLevelFeatures::Tier::TIER_2,
        D3D12GpuStandardFeatureLevelFeatures::Tier::NOT_REQUIRED,
        D3D12GpuStandardFeatureLevelFeatures::FeatureBoolPlus::NOT_REQUIRED,
        D3D12GpuStandardFeatureLevelFeatures::FeatureBoolPlus::YES,
        D3D12GpuStandardFeatureLevelFeatures::FeatureBoolPlus::NOT_REQUIRED,
        D3D12GpuStandardFeatureLevelFeatures::FeatureBoolPlus::NOT_REQUIRED,
        18,
        true,
        true,
        D3D12GpuStandardFeatureLevelFeatures::FeatureBoolPlus::YES,
        true,
        true,
        true,
        true,
        true,
        true,
        D3D12GpuStandardFeatureLevelFeatures::FeatureBoolPlus::YES,
        D3D12GpuStandardFeatureLevelFeatures::FeatureBoolPlus::YES,
        D3D12GpuStandardFeatureLevelFeatures::FeatureBoolPlus::YES,
        true,
        D3D12GpuStandardFeatureLevelFeatures::FeatureBoolPlus::YES,
        64,
        D3D12GpuStandardFeatureLevelFeatures::FeatureBoolPlus::YES,
        16,
        D3D12GpuStandardFeatureLevelFeatures::FeatureBoolPlus::YES,
        D3D12GpuStandardFeatureLevelFeatures::FeatureBoolYesOrNotRequired::YES,
        16384,
        16384,
        2048,
        16384,
        16,
        4294967295,
        4294967295,
        32,
        8,
        true,
        true,
        true,
        true,
        true,
        true,
        false,
        true,
    },
    {
        D3D_FEATURE_LEVEL_11_1, Version(11, 1),
        D3D12GpuStandardFeatureLevelFeatures::ShaderModel::MODEL_5_1, Version(5, 1),
        D3D12GpuStandardFeatureLevelFeatures::Tier::NOT_REQUIRED,
        D3D12GpuStandardFeatureLevelFeatures::Tier::NOT_REQUIRED,
        D3D12GpuStandardFeatureLevelFeatures::FeatureBoolPlus::NOT_REQUIRED,
        D3D12GpuStandardFeatureLevelFeatures::FeatureBoolPlus::NOT_REQUIRED,
        D3D12GpuStandardFeatureLevelFeatures::FeatureBoolPlus::NOT_REQUIRED,
        D3D12GpuStandardFeatureLevelFeatures::FeatureBoolPlus::NOT_REQUIRED,
        3,
        true,
        true,
        D3D12GpuStandardFeatureLevelFeatures::FeatureBoolPlus::YES,
        true,
        true,
        true,
        true,
        true,
        true,
        D3D12GpuStandardFeatureLevelFeatures::FeatureBoolPlus::YES,
        D3D12GpuStandardFeatureLevelFeatures::FeatureBoolPlus::YES,
        D3D12GpuStandardFeatureLevelFeatures::FeatureBoolPlus::YES,
        true,
        D3D12GpuStandardFeatureLevelFeatures::FeatureBoolPlus::YES,
        64,
        D3D12GpuStandardFeatureLevelFeatures::FeatureBoolPlus::YES,
        16,
        D3D12GpuStandardFeatureLevelFeatures::FeatureBoolPlus::YES,
        D3D12GpuStandardFeatureLevelFeatures::FeatureBoolYesOrNotRequired::YES,
        16384,
        16384,
        2048,
        16384,
        16,
        4294967295,
        4294967295,
        32,
        8,
        true,
        true,
        true,
        true,
        true,
        true,
        false,
        true,
    },
    {
        D3D_FEATURE_LEVEL_11_0, Version(11, 0),
        D3D12GpuStandardFeatureLevelFeatures::ShaderModel::MODEL_5_1, Version(5, 1),
        D3D12GpuStandardFeatureLevelFeatures::Tier::NOT_REQUIRED,
        D3D12GpuStandardFeatureLevelFeatures::Tier::NONE,
        D3D12GpuStandardFeatureLevelFeatures::FeatureBoolPlus::NO,
        D3D12GpuStandardFeatureLevelFeatures::FeatureBoolPlus::NO,
        D3D12GpuStandardFeatureLevelFeatures::FeatureBoolPlus::NOT_REQUIRED,
        D3D12GpuStandardFeatureLevelFeatures::FeatureBoolPlus::NO,
        3,
        true,
        true,
        D3D12GpuStandardFeatureLevelFeatures::FeatureBoolPlus::YES,
        true,
        true,
        true,
        true,
        true,
        true,
        D3D12GpuStandardFeatureLevelFeatures::FeatureBoolPlus::YES,
        D3D12GpuStandardFeatureLevelFeatures::FeatureBoolPlus::YES,
        D3D12GpuStandardFeatureLevelFeatures::FeatureBoolPlus::NOT_REQUIRED,
        false,
        D3D12GpuStandardFeatureLevelFeatures::FeatureBoolPlus::NOT_REQUIRED,
        8,
        D3D12GpuStandardFeatureLevelFeatures::FeatureBoolPlus::NO,
        8,
        D3D12GpuStandardFeatureLevelFeatures::FeatureBoolPlus::NOT_REQUIRED,
        D3D12GpuStandardFeatureLevelFeatures::FeatureBoolYesOrNotRequired::NOT_REQUIRED,
        16384,
        16384,
        2048,
        16384,
        16,
        4294967295,
        4294967295,
        32,
        8,
        true,
        true,
        true,
        true,
        true,
        true,
        false,
        true,
    },
};


//Implementation----------------------------------------------------------------

//D3D12GpuFeatures
size_t D3D12GpuFeatures::GetStandardFeatureLevelsLowestToHighest(D3D_FEATURE_LEVEL* featureLevels, size_t maxCount)
{
    if (featureLevels == nullptr)
        return FINJIN_COUNT_OF(standardFeatureLevelFeatures);
    else
    {
        size_t count = 0;
        for (size_t featureLevelIndex = FINJIN_COUNT_OF(standardFeatureLevelFeatures) - 1; featureLevelIndex != (size_t)-1 && count < maxCount; featureLevelIndex--)
            featureLevels[count++] = standardFeatureLevelFeatures[featureLevelIndex].featureLevel;
        return count;
    }
}

bool D3D12GpuFeatures::SetStandardFeatures(D3D_FEATURE_LEVEL featureLevel)
{
    for (size_t featureLevelIndex = 0; featureLevelIndex < FINJIN_COUNT_OF(standardFeatureLevelFeatures); featureLevelIndex++)
    {
        if (standardFeatureLevelFeatures[featureLevelIndex].featureLevel == featureLevel)
        {
            this->standardFeatures = standardFeatureLevelFeatures[featureLevelIndex];
            return true;
        }
    }

    return false;
}

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

Luid D3D12GpuDescription::GetGpuID() const
{
    return this->desc.AdapterLuid;
}

uint64_t D3D12GpuDescription::GetDedicatedDeviceMemorySize() const
{
    return this->desc.DedicatedVideoMemory;
}

uint64_t D3D12GpuDescription::GetDedicatedSystemMemorySize() const
{
    return this->desc.DedicatedSystemMemory;
}

uint64_t D3D12GpuDescription::GetTotalDedicatedMemorySize() const
{
    return this->desc.DedicatedVideoMemory + this->desc.DedicatedSystemMemory;
}

//D3D12HardwareGpuDescriptions
void D3D12HardwareGpuDescriptions::SortBestToWorst()
{
    //NOTE: Should probably take other factors into account

    //Order by the various GPU memory types/sizes
    std::sort(begin(), end(), [](const D3D12GpuDescription& a, const D3D12GpuDescription& b)
    {
        if (a.GetDedicatedDeviceMemorySize() != b.GetDedicatedDeviceMemorySize())
            return a.GetDedicatedDeviceMemorySize() > b.GetDedicatedDeviceMemorySize();
        if (a.GetDedicatedSystemMemorySize() != b.GetDedicatedSystemMemorySize())
            return a.GetDedicatedSystemMemorySize() > b.GetDedicatedSystemMemorySize();
        return a.GetTotalDedicatedMemorySize() > b.GetTotalDedicatedMemorySize();
    });
}

const D3D12GpuDescription* D3D12HardwareGpuDescriptions::GetByGpuID(const Luid& gpuID) const
{
    for (const auto& desc : *this)
    {
        if (gpuID == desc.desc.AdapterLuid)
            return &desc;
    }

    return nullptr;
}

#endif
