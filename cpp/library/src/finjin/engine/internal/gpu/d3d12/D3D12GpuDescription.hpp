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
#include "finjin/common/StaticVector.hpp"
#include "finjin/common/Version.hpp"
#include "D3D12GpuOutput.hpp"
#include "D3D12GpuID.hpp"
#include <dxgi1_4.h>
#include <d3d12.h>


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    //Feature-level defined limits, as specified in https://msdn.microsoft.com/en-us/library/windows/desktop/ff476876(v=vs.85).aspx
    //It is possible some of these values duplicate what's in D3D12_FEATURE_DATA_D3D12_OPTIONS
    struct D3D12GpuStandardFeatureLevelFeatures
    {
        enum class ShaderModel : uint32_t
        {
            MODEL_2_0_LEVEL_9_1,
            MODEL_2_0_LEVEL_9_3,
            MODEL_4_0,
            MODEL_4_X,
            MODEL_5_1,
            MODEL_6_0
        };

        enum class Tier : uint32_t
        {
            NONE,
            TIER_1,
            TIER_2,
            TIER_3,
            NOT_REQUIRED //Optional
        };

        enum class FeatureBoolPlus : uint32_t
        {
            NO,
            YES,
            NOT_REQUIRED, //Optional
            NOT_AVAILABLE
        };

        enum class FeatureBoolYesOrNotRequired : uint32_t
        {
            YES,
            NOT_REQUIRED //Optional
        };

        D3D_FEATURE_LEVEL featureLevel;
        Version featureLevelVersion;
        ShaderModel shaderModel;
        Version shaderModelVersion;
        Tier tiledResourcesTier;
        Tier conservativeRasterizationTier;
        FeatureBoolPlus rasterizerOrderViews;
        FeatureBoolPlus minMaxFilters;
        FeatureBoolPlus mapDefaultBuffer;
        FeatureBoolPlus shaderSpecifiedStencilReferenceValue;
        uint32_t typedUAVLoadCount;
        bool geometryShader;
        bool streamOut;
        FeatureBoolPlus directComputeShader;
        bool hullAndDomainShader;
        bool textureResourceArrays;
        bool cubeMapResourceArrays;
        bool bc4bc5Compression;
        bool bc6bc7Compression;
        bool alphaToCoverage;
        FeatureBoolPlus extendedFormats;
        FeatureBoolPlus xr10bitHighColorFormat;
        FeatureBoolPlus logicOperationsOutputMerger;
        bool targetIndependentRasterization;
        FeatureBoolPlus multipleRenderTargetWithForcedSampleCount1;
        uint32_t uavSlotCount;
        FeatureBoolPlus uavsAtEveryStage;
        uint32_t maxForcedSampleCountForUAVOnlyRendering;
        FeatureBoolPlus constantBufferOffsettingAndPartialUpdates;
        FeatureBoolYesOrNotRequired pixelFormats16BitsPerPixel;
        uint32_t maxTextureDimension;
        uint32_t maxCubeMapDimension;
        uint32_t maxVolumeExtent;
        uint32_t maxTextureRepeat;
        uint32_t maxAnisotrophy;
        uint32_t maxPrimitiveCount;
        uint32_t maxVertexIndex;
        uint32_t maxInputSlots;
        uint32_t maxSimultaneousRenderTargets;
        bool occlusionQueries;
        bool separateAlphaBlend;
        bool mirrorOnce;
        bool overlappingVertexElements;
        bool independentWriteMasks;
        bool instancing;
        bool nonPowersOf2Conditionally;
        bool nonPowersOf2Unconditionally;
    };

    struct D3D12GpuFeatures
    {
        static size_t GetStandardFeatureLevelsLowestToHighest(D3D_FEATURE_LEVEL* featureLevels, size_t maxCount);

        bool SetStandardFeatures(D3D_FEATURE_LEVEL featureLevel);

        //Initialized in D3D12SystemImpl
        D3D_SHADER_MODEL maxShaderModel;
        D3D12_FEATURE_DATA_D3D12_OPTIONS options;
        D3D12_FEATURE_DATA_D3D12_OPTIONS1 options1;
        D3D12_FEATURE_DATA_GPU_VIRTUAL_ADDRESS_SUPPORT gpuVirtualAddressSupport;
        StaticVector<D3D12_FEATURE_DATA_ARCHITECTURE, 10> dataArchitectures;
        StaticVector<D3D12_FEATURE_DATA_FORMAT_SUPPORT, 144> formatSupport;
        D3D12GpuStandardFeatureLevelFeatures standardFeatures;

        //Initialized in D3D12GpuContextImpl
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

        uint64_t GetDedicatedDeviceMemorySize() const;
        uint64_t GetDedicatedSystemMemorySize() const;
        uint64_t GetTotalDedicatedMemorySize() const;
    };
    class D3D12HardwareGpuDescriptions : public StaticVector<D3D12GpuDescription, EngineConstants::MAX_GPU_ADAPTERS>
    {
    public:
        void SortBestToWorst();

        const D3D12GpuDescription* GetByGpuID(const D3D12GpuID& gpuID) const;
    };

    using D3D12SoftwareGpuDescriptions = StaticVector<D3D12GpuDescription, 1>;

} }
