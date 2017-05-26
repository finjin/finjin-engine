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

using namespace Finjin::Engine;


//Implementation----------------------------------------------------------------

//D3D12RenderTarget::Resource
D3D12RenderTarget::Resource::Resource()
{
    this->rtvDescHeapIndex = (size_t)-1;
    this->dsvDescHeapIndex = (size_t)-1;
    this->srvDescHeapIndex = (size_t)-1;
    this->isScreenSizeDependent = false;
}

void D3D12RenderTarget::Resource::Destroy()
{
    this->resource = nullptr;
    this->rtvDescHeapIndex = (size_t)-1;
    this->dsvDescHeapIndex = (size_t)-1;
    this->srvDescHeapIndex = (size_t)-1;
    this->isScreenSizeDependent = false;
}

void D3D12RenderTarget::Resource::DestroyScreenSizeDependentResources()
{
    if (this->isScreenSizeDependent)
        this->resource = nullptr;
}

ID3D12Resource* D3D12RenderTarget::Resource::Get()
{
    return this->resource.Get();
}

//D3D12RenderTarget
D3D12RenderTarget::D3D12RenderTarget() : clearColor(MathVector4(0, 0, 0, 0))
{
}

void D3D12RenderTarget::CreateColor
    (
    ID3D12Device* device,
    UINT width,
    UINT height,
    DXGI_FORMAT colorFormat,
    UINT multisampleCount,
    UINT multisampleQuality,
    bool isScreenSizeDependent,
    Error& error
    )
{
    FINJIN_ERROR_METHOD_START(error);

    for (auto& colorOutput : this->colorOutputs)
        colorOutput.Destroy();
    this->colorOutputs.resize(1);

    D3D12_HEAP_PROPERTIES heapProperties = {};
    heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
    heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    heapProperties.CreationNodeMask = 1;
    heapProperties.VisibleNodeMask = 1;

    D3D12_CLEAR_VALUE colorClearValue = {};
    colorClearValue.Format = colorFormat;

    D3D12_RESOURCE_DESC colorBufferDesc = {};
    colorBufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    colorBufferDesc.Width = width;
    colorBufferDesc.Height = height;
    colorBufferDesc.DepthOrArraySize = 1;
    colorBufferDesc.Format = colorFormat;
    colorBufferDesc.SampleDesc.Count = multisampleCount;
    colorBufferDesc.SampleDesc.Quality = multisampleQuality;
    colorBufferDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    colorBufferDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

    auto result = device->CreateCommittedResource
        (
        &heapProperties,
        D3D12_HEAP_FLAG_NONE,
        &colorBufferDesc,
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
        &colorClearValue,
        IID_PPV_ARGS(&this->colorOutputs[0].resource)
        );
    if (FINJIN_CHECK_HRESULT_FAILED(result))
    {
        this->colorOutputs.clear();

        FINJIN_SET_ERROR(error, "Failed to create color resource.");
        return;
    }

    this->colorOutputs[0].isScreenSizeDependent = isScreenSizeDependent;
}

void D3D12RenderTarget::CreateDepthStencil
    (
    ID3D12Device* device,
    UINT width,
    UINT height,
    DXGI_FORMAT depthStencilFormat,
    float maxDepth,
    UINT multisampleCount,
    UINT multisampleQuality,
    bool isScreenSizeDependent,
    Error& error
    )
{
    FINJIN_ERROR_METHOD_START(error);

    this->depthStencilResource.resource = nullptr;

    D3D12_HEAP_PROPERTIES heapProperties = {};
    heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
    heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    heapProperties.CreationNodeMask = 1;
    heapProperties.VisibleNodeMask = 1;

    D3D12_CLEAR_VALUE depthStencilClearValue = {};
    depthStencilClearValue.Format = depthStencilFormat;
    depthStencilClearValue.DepthStencil.Depth = maxDepth;
    depthStencilClearValue.DepthStencil.Stencil = 0;

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
        &depthStencilClearValue,
        IID_PPV_ARGS(&this->depthStencilResource.resource)
        );
    if (FINJIN_CHECK_HRESULT_FAILED(result))
    {
        FINJIN_SET_ERROR(error, "Failed to create depth resource.");
        return;
    }

    this->depthStencilResource.isScreenSizeDependent = isScreenSizeDependent;
}

void D3D12RenderTarget::Destroy()
{
    for (auto& colorOutput : this->colorOutputs)
        colorOutput.Destroy();
    this->colorOutputs.clear();

    this->depthStencilResource.Destroy();
}

void D3D12RenderTarget::DestroyScreenSizeDependentResources()
{
    for (auto& colorOutput : this->colorOutputs)
        colorOutput.DestroyScreenSizeDependentResources();

    this->depthStencilResource.DestroyScreenSizeDependentResources();
}

bool D3D12RenderTarget::HasDepthStencil() const
{
    return this->depthStencilResource.dsvDescHeapIndex != (size_t)-1;
}

#endif
