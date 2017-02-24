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

#include "D3D12RenderTarget.hpp"
#include "D3D12Utilities.hpp"

using namespace Finjin::Common;
using namespace Finjin::Engine;


//Implementation---------------------------------------------------------------
D3D12RenderTarget::D3D12RenderTarget() :
    depthStencilResource(nullptr),
    renderTargetResourceHeapIndex((size_t)-1),
    depthStencilResourceHeapIndex((size_t)-1),
    clearColor(MathVector4(0, 0, 0, 0))
{
}

void D3D12RenderTarget::CreateDepthStencil(ID3D12Device* device, UINT width, UINT height, DXGI_FORMAT depthStencilFormat, float maxDepth, UINT multisampleCount, UINT multisampleQuality, Error& error)
{
    //Create depth stencil resource
    D3D12_HEAP_PROPERTIES heapProperties = {};
    heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
    heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    heapProperties.CreationNodeMask = 1;
    heapProperties.VisibleNodeMask = 1;

    D3D12_CLEAR_VALUE depthOptimizedClearValue = {};
    depthOptimizedClearValue.Format = depthStencilFormat;
    depthOptimizedClearValue.DepthStencil.Depth = maxDepth;
    depthOptimizedClearValue.DepthStencil.Stencil = 0;

    D3D12_RESOURCE_DESC depthStencilBufferDesc = {};
    depthStencilBufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    depthStencilBufferDesc.Width = width;
    depthStencilBufferDesc.Height = height;
    depthStencilBufferDesc.DepthOrArraySize = 1;
    depthStencilBufferDesc.Format = depthStencilFormat;
    depthStencilBufferDesc.SampleDesc.Count = multisampleCount;
    depthStencilBufferDesc.SampleDesc.Quality = multisampleQuality;
    depthStencilBufferDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    depthStencilBufferDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

    auto result = device->CreateCommittedResource
        (
        &heapProperties,
        D3D12_HEAP_FLAG_NONE,
        &depthStencilBufferDesc,
        D3D12_RESOURCE_STATE_DEPTH_WRITE,
        &depthOptimizedClearValue,
        IID_PPV_ARGS(&this->depthStencilResource)
        );
    if (FINJIN_CHECK_HRESULT_FAILED(result))
    {
        FINJIN_SET_ERROR(error, "Failed to create depth buffer.");
        return;
    }
}

void D3D12RenderTarget::DestroySceenSizeDependentResources()
{
    for (auto& renderTargetResource : this->renderTargetOutputResources)
        renderTargetResource.Reset();
    
    this->depthStencilResource.Reset();

    this->renderTargetResourceHeapIndex = (size_t)-1;
    this->depthStencilResourceHeapIndex = (size_t)-1;
}

bool D3D12RenderTarget::HasDepthStencil() const
{
    return this->depthStencilResourceHeapIndex != (size_t)-1;
}

#endif
