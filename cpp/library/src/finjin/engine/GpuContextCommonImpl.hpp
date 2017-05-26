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
#include "finjin/common/AllocatedClass.hpp"
#include "finjin/common/ByteBuffer.hpp"
#include "finjin/common/DynamicVector.hpp"
#include "finjin/common/Error.hpp"
#include "finjin/common/FiberSpinLock.hpp"
#include "finjin/engine/GpuContextCommonSettings.hpp"
#include "finjin/common/OperationStatus.hpp"
#include "finjin/common/StaticVector.hpp"
#include "finjin/engine/AssetClassFileReader.hpp"
#include "finjin/engine/Camera.hpp"
#include "finjin/engine/GenericGpuNumericStructs.hpp"
#include "finjin/engine/OSWindowEventListener.hpp"
#include "finjin/engine/ShaderFeatureFlag.hpp"
#include "finjin/engine/WindowSize.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    class GpuContextCommonImpl
    {
    public:
        GpuContextCommonImpl(Allocator* allocator);

        virtual ~GpuContextCommonImpl();

    public:
        AssetClassFileReader settingsAssetClassFileReader;
        AssetClassFileReader shaderAssetClassFileReader;

        std::array<uint32_t, 2> renderTargetSize;
        WindowBounds windowPixelBounds;

        ByteBuffer readBuffer;

        Camera camera;
        MathVector4 clearColor;

        AssetReference workingAssetReference;
        SimpleUri workingUri;

        size_t sequenceIndex;

        size_t currentFrameBufferIndex;

        FiberSpinLock createLock;
    };

    template <typename RendererMaterialMapIndex>
    struct GpuMaterialMapIndexToConstantBufferElements
    {
        GpuMaterialMapIndexToConstantBufferElements() {}
        GpuMaterialMapIndexToConstantBufferElements(RendererMaterialMapIndex gpuMaterialMapIndex, GpuStructuredAndConstantBufferStructMetadata::ElementID gpuBufferTextureIndexElementID, ShaderFeatureFlag gpuMaterialMapFlag)
        {
            this->gpuMaterialMapIndex = gpuMaterialMapIndex;
            this->gpuBufferTextureIndexElementID = gpuBufferTextureIndexElementID;
            this->gpuBufferAmountElementID = GpuStructuredAndConstantBufferStructMetadata::ElementID::NONE;
            this->gpuMaterialMapFlag = gpuMaterialMapFlag;
        }
        GpuMaterialMapIndexToConstantBufferElements(RendererMaterialMapIndex gpuMaterialMapIndex, GpuStructuredAndConstantBufferStructMetadata::ElementID gpuBufferTextureIndexElementID, GpuStructuredAndConstantBufferStructMetadata::ElementID gpuBufferAmountElementID, ShaderFeatureFlag gpuMaterialMapFlag)
        {
            this->gpuMaterialMapIndex = gpuMaterialMapIndex;
            this->gpuBufferTextureIndexElementID = gpuBufferTextureIndexElementID;
            this->gpuBufferAmountElementID = gpuBufferAmountElementID;
            this->gpuMaterialMapFlag = gpuMaterialMapFlag;
        }

        RendererMaterialMapIndex gpuMaterialMapIndex; //Index into VulkanMaterial/D3D12Material/MetalMaterial/etc maps
        GpuStructuredAndConstantBufferStructMetadata::ElementID gpuBufferTextureIndexElementID; //ElementID (index) into shader constant or structured buffer for 'texture index'
        GpuStructuredAndConstantBufferStructMetadata::ElementID gpuBufferAmountElementID; //ElementID (index) into shader constant or structured buffer for 'amount'
        ShaderFeatureFlag gpuMaterialMapFlag; //Single flag identifying map's shader usage
    };

} }
