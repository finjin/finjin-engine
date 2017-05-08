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
#include "finjin/common/EnumBitwise.hpp"
#include "finjin/common/StaticVector.hpp"
#include "finjin/common/Utf8String.hpp"
#include "finjin/common/Version.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    enum class MetalFeatureSetCapability
    {
        NONE = 0,
        FILTER = 1 << 0,
        WRITE = 1 << 1,
        COLOR = 1 << 2,
        BLEND = 1 << 3,
        MSAA = 1 << 4,
        RESOLVE = 1 << 5,
        ALL = FILTER | WRITE | COLOR | BLEND | MSAA | RESOLVE
    };
    FINJIN_ENUM_BITWISE_OPERATIONS(MetalFeatureSetCapability)

    //Feature set info: https://developer.apple.com/library/content/documentation/Miscellaneous/Conceptual/MetalProgrammingGuide/MetalFeatureSetTables/MetalFeatureSetTables.html
    struct MetalFeatureSet
    {
        uint32_t gpuFamily; //MTLFeatureSet_iOS_GPUFamily2_v1, etc...
        Version version;

        struct Availability
        {
            bool metalKit;
            bool metalPerformanceShaders;
            bool programmableBlending;
            bool pvrtcPixelFormats;
            bool eacAndEtcPixelFormats;
            bool astcPixelFormats;
            bool bcPixelFormats;
            bool msaaDepthResolve;
            bool countingOcclusionQuery;
            bool baseVertexInstanceDrawing;
            bool indirectBuffers;
            bool cubeMapArrays;
            bool textureBarriers;
            bool layeredRendering;
            bool tessellation;
            bool resourceHeaps;
            bool memorylessRenderTargets;
            bool functionSpecialization;
            bool functionBufferReadWrites;
            bool functionTextureReadWrites;
            bool arrayOfTextures;
            bool stencilTextureViews;
            bool depth16PixelFormat;
            bool extendedRangePixelFormats;
            bool combinedMsaaStoreAndResolveAction;
            bool deferredStoreAction;
            bool msaaBits;
            bool srgbWrites;
            bool unsignedInt16Coordinates;
            bool extractInsertAndReverseBits;
            bool simdBarrier;
            bool samplerMaxAnisotrophy;
            bool samplerLodClamp;
            bool borderColor;
            bool dualSourceBlending;
        };
        Availability availability;

        struct Limits
        {
            //Function arguments
            size_t maxVertexAttributesPerVertexDescriptor;
            size_t maxEntriesInTheBufferArgumentTablePerRenderOrComputeCommandEncoder;
            size_t maxEntriesInTheTextureArgumentTablePerRenderOrComputeCommandEncoder;
            size_t maxEntriesInTheSamplerStateArgumentTablePerRenderOrComputeCommandEncoder;
            size_t maxEntriesInTheThreadgroupMemoryArgumentTablePerComputeCommandEncoder;
            size_t maxLengthOfADataBlockForAFunctionPerRenderOrComputeCommandEncoder;
            size_t maxThreadsPerThreadgroup;
            size_t maxTotalThreadgroupMemoryAllocation;
            size_t threadgroupMemoryLengthAlignment;
            size_t maxFunctionMemoryAllocationForABufferInTheConstantAddressSpace;
            size_t maxInputsToAFragmentFunctionDeclaredWithTheStageInQualifier;
            size_t maxInputComponentsToAFragmentFunctionDeclaredWithTheStageInQualifier;
            size_t maxFunctionConstants;
            size_t maxTessellationFactor;

            //Resources
            size_t maxBufferLength;
            size_t minBufferOffsetAlignment;
            size_t max1DTextureWidth;
            size_t max2DTextureWidthAndHeight;
            size_t maxCubeMapTextureWidthAndHeight;
            size_t max3DTextureWidthHeightAndDepth;
            size_t maxLayersPer1DTextureArray2DTextureArrayOr3DTexture;
            size_t bufferAlignmentForCopyingAnExistingTextureToABuffer;
            size_t bufferAlignmentForCreatingANewTextureFromABuffer;

            //Render targets
            size_t maxColorRenderTargetsPerRenderPassDescriptor;
            size_t maxSizeOfAPointPrimitive;
            size_t maxTotalRenderTargetSizePerPixelWhenUsingMultipleColorRenderTargets;
            size_t maxVisibilityQueryOffset;
        };
        Limits limits;

        struct Capabilities
        {
            //Ordinary 8-bit pixel formats
            MetalFeatureSetCapability A8Unorm;
            MetalFeatureSetCapability R8Unorm;
            MetalFeatureSetCapability R8Unorm_sRGB;
            MetalFeatureSetCapability R8Snorm;
            union
            {
                MetalFeatureSetCapability R8Uint;
                MetalFeatureSetCapability R8Sint;
            };

            //Ordinary 16-bit pixel formats
            union
            {
                MetalFeatureSetCapability R16Unorm;
                MetalFeatureSetCapability R16Snorm;
            };
            union
            {
                MetalFeatureSetCapability R16Uint;
                MetalFeatureSetCapability R16Sint;
            };
            MetalFeatureSetCapability R16Float;
            MetalFeatureSetCapability RG8Unorm;
            MetalFeatureSetCapability RG8Unorm_sRGB;
            MetalFeatureSetCapability RG8Snorm;
            union
            {
                MetalFeatureSetCapability RG8Uint;
                MetalFeatureSetCapability RG8Sint;
            };

            //Packed 16-bit pixel formats
            union
            {
                MetalFeatureSetCapability B5G6R5Unorm;
                MetalFeatureSetCapability A1BGR5Unorm;
                MetalFeatureSetCapability ABGR4Unorm;
                MetalFeatureSetCapability BGR5A1Unorm;
            };

            //Ordinary 32-bit pixel formats
            union
            {
                MetalFeatureSetCapability R32Uint;
                MetalFeatureSetCapability R32Sint;
            };
            MetalFeatureSetCapability R32Float;
            union
            {
                MetalFeatureSetCapability RG16Unorm;
                MetalFeatureSetCapability RG16Snorm;
            };
            union
            {
                MetalFeatureSetCapability RG16Uint;
                MetalFeatureSetCapability RG16Sint;
            };
            MetalFeatureSetCapability RG16Float;
            MetalFeatureSetCapability RGBA8Unorm;
            MetalFeatureSetCapability RGBA8Unorm_sRGB;
            MetalFeatureSetCapability RGBA8Snorm;
            union
            {
                MetalFeatureSetCapability RGBA8Uint;
                MetalFeatureSetCapability RGBA8Sint;
            };
            MetalFeatureSetCapability BGRA8Unorm;
            MetalFeatureSetCapability BGRA8Unorm_sRGB;

            //Packed 32-bit pixel formats
            MetalFeatureSetCapability RGB10A2Unorm;
            MetalFeatureSetCapability RGB10A2Uint;
            MetalFeatureSetCapability RG11B10Float;
            MetalFeatureSetCapability RGB9E5Float;

            //Ordinary 64-bit pixel formats
            union
            {
                MetalFeatureSetCapability RG32Uint;
                MetalFeatureSetCapability RG32Sint;
            };
            MetalFeatureSetCapability RG32Float;
            union
            {
                MetalFeatureSetCapability RGBA16Unorm;
                MetalFeatureSetCapability RGBA16Snorm;
            };
            union
            {
                MetalFeatureSetCapability RGBA16Uint;
                MetalFeatureSetCapability RGBA16Sint;
            };
            MetalFeatureSetCapability RGBA16Float;

            //Ordinary 128-bit pixel formats
            union
            {
                MetalFeatureSetCapability RGBA32Uint;
                MetalFeatureSetCapability RGBA32Sint;
            };
            MetalFeatureSetCapability RGBA32Float;

            //Compressed pixel formats
            MetalFeatureSetCapability pvrtcPixelFormats;
            MetalFeatureSetCapability eacAndEtcPixelFormats;
            MetalFeatureSetCapability astcPixelFormats;
            MetalFeatureSetCapability bcPixelFormats;

            //YUV pixel formats
            union
            {
                MetalFeatureSetCapability GBGR422;
                MetalFeatureSetCapability BGRG422;
            };

            //Depth and stencil pixel formats
            MetalFeatureSetCapability Depth16Unorm;
            MetalFeatureSetCapability Depth32Float;
            MetalFeatureSetCapability Stencil8;
            MetalFeatureSetCapability Depth24Unorm_Stencil8;
            MetalFeatureSetCapability Depth32Float_Stencil8;
            MetalFeatureSetCapability X24_Stencil8;
            MetalFeatureSetCapability X32_Stencil8;

            //Extended range pixel formats
            union
            {
                MetalFeatureSetCapability BGRA10_XR;
                MetalFeatureSetCapability BGRA10_XR_sRGB;
                MetalFeatureSetCapability BGR10_XR;
                MetalFeatureSetCapability BGR10_XR_sRGB;
            };
        };
        Capabilities capabilities;
    };

    class MetalGpuDescription
    {
    public:
        MetalGpuDescription();

        const Utf8String& GetGpuID() const;

        uint64_t GetDedicatedDeviceMemorySize() const;
        uint64_t GetDedicatedSystemMemorySize() const;
        uint64_t GetTotalDedicatedMemorySize() const;

        bool SetStandardFeatureSet(uint32_t gpuFamily);
        Utf8String GetFeatureSetString() const;

    public:
        Utf8String gpuID;

        bool isLowPower;
        bool isHeadless;
        StaticVector<size_t, 8> supportedTextureSampleCounts;
        std::array<unsigned long, 3> maxThreadsPerThreadgroup; //Width, height, depth
        uint64_t recommendedMaxWorkingSetSize; //Will be 0 on iOS/tvOS

        MetalFeatureSet featureSet;
        bool supportsReadWriteTexturesTier2;
    };
    class MetalHardwareGpuDescriptions : public StaticVector<MetalGpuDescription, EngineConstants::MAX_GPU_ADAPTERS>
    {
    public:
        void SortBestToWorst();

        const MetalGpuDescription* GetByGpuID(const Utf8String& gpuID) const;
    };

    using MetalSoftwareGpuDescriptions = StaticVector<MetalGpuDescription, 1>;

} }
