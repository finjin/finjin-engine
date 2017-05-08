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

    struct D3D12RenderTarget
    {
        D3D12RenderTarget();

        void CreateDepthStencil(ID3D12Device* device, UINT width, UINT height, DXGI_FORMAT depthStencilFormat, float maxDepth, UINT multisampleCount, UINT multisampleQuality, Error& error);

        void DestroySceenSizeDependentResources();

        bool HasDepthStencil() const;

        StaticVector<Microsoft::WRL::ComPtr<ID3D12Resource>, EngineConstants::MAX_RENDER_TARGET_OUTPUTS> renderTargetOutputResources;

        Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource;

        size_t renderTargetResourceHeapIndex;
        size_t depthStencilResourceHeapIndex;

        StaticVector<D3D12_VIEWPORT, EngineConstants::MAX_RENDER_TARGET_VIEWPORTS> viewports;
        StaticVector<D3D12_RECT, EngineConstants::MAX_RENDER_TARGET_VIEWPORTS> scissorRects;

        Setting<MathVector4> clearColor;
    };

} }
