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
#include "finjin/common/Math.hpp"
#include "finjin/common/Setting.hpp"
#include "D3D12Includes.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    class D3D12RenderTarget
    {
    public:
        D3D12RenderTarget();

        void CreateColor(ID3D12Device* device, UINT width, UINT height, DXGI_FORMAT colorFormat, UINT multisampleCount, UINT multisampleQuality, bool isScreenSizeDependent, size_t outputCount, Error& error);
        void CreateDepthStencil(ID3D12Device* device, UINT width, UINT height, DXGI_FORMAT depthStencilFormat, float maxDepth, UINT multisampleCount, UINT multisampleQuality, bool isScreenSizeDependent, Error& error);

        void Destroy();
        void DestroyScreenSizeDependentResources();

        bool HasDepthStencil() const;

    public:
        struct Resource
        {
            Resource();

            void Destroy();
            void DestroyScreenSizeDependentResources();

            ID3D12Resource* Get();

            Microsoft::WRL::ComPtr<ID3D12Resource> resource;
            size_t rtvDescHeapIndex;
            size_t dsvDescHeapIndex;
            size_t srvDescHeapIndex;
            bool isScreenSizeDependent;

            uint32_t nodeMask;
        };
        StaticVector<Resource, EngineConstants::MAX_RENDER_TARGET_OUTPUTS> colorOutputs;

        Resource depthStencilResource;

        StaticVector<D3D12_VIEWPORT, 1> defaultViewport;
        StaticVector<D3D12_RECT, 1> defaultScissorRect;

        StaticVector<D3D12_VIEWPORT, EngineConstants::MAX_RENDER_TARGET_VIEWPORTS> viewports;
        StaticVector<D3D12_RECT, EngineConstants::MAX_RENDER_TARGET_VIEWPORTS> scissorRects;

        Setting<MathVector4> clearColor;
    };

} }
