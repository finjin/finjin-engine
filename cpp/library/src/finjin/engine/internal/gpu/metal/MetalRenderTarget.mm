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

#if FINJIN_TARGET_GPU_SYSTEM == FINJIN_TARGET_GPU_SYSTEM_METAL

#include "MetalRenderTarget.hpp"
#include "MetalUtilities.hpp"

using namespace Finjin::Engine;


//Implementation----------------------------------------------------------------
MetalRenderTarget::MetalRenderTarget() : clearColor(MathVector4(0,0,0,0))
{
    this->colorTextureDesc = nullptr;
    this->isColorScreenSizeDependent = false;

    this->depthStencilTextureDesc = nullptr;
    this->depthStencilTexture = nullptr;
    this->depthStencilStateDesc = nullptr;
    this->depthStencilState = nullptr;
    this->isDepthStencilScreenSizeDependent = false;

    this->renderPassDescriptor = nullptr;
    this->renderCommandEncoder = nullptr;
}

void MetalRenderTarget::CreateColor
    (
    id<MTLDevice> device,
    size_t width,
    size_t height,
    MTLPixelFormat colorFormat,
    bool isScreenSizeDependent,
    Error& error
    )
{
    FINJIN_ERROR_METHOD_START(error);

    this->isColorScreenSizeDependent = isScreenSizeDependent;

    if (this->colorTextureDesc == nullptr)
    {
        this->colorTextureDesc = [[MTLTextureDescriptor alloc] init];
        if (this->colorTextureDesc == nullptr)
        {
            FINJIN_SET_ERROR(error, "Failed to create color texture descriptor.");
            return;
        }
    }
    this->colorTextureDesc.textureType = MTLTextureType2D;
    this->colorTextureDesc.width = width;
    this->colorTextureDesc.height = height;
    this->colorTextureDesc.pixelFormat = colorFormat;
    this->colorTextureDesc.mipmapLevelCount = 1;
    this->colorTextureDesc.resourceOptions = MTLResourceStorageModePrivate;
    this->colorTextureDesc.usage = MTLTextureUsageRenderTarget | MTLTextureUsageShaderRead;

    for (auto& renderTargetResource : this->colorOutputs)
        renderTargetResource.Destroy();
    this->colorOutputs.resize(1);
    this->colorOutputs[0].texture = [device newTextureWithDescriptor:this->colorTextureDesc];
    if (this->colorOutputs[0].texture == nullptr)
    {
        this->colorOutputs.clear();

        FINJIN_SET_ERROR(error, "Failed to create color texture.");
        return;
    }
}

void MetalRenderTarget::CreateDepthStencil
    (
    id<MTLDevice> device,
    size_t width,
    size_t height,
    MTLPixelFormat depthStencilFormat,
    bool isScreenSizeDependent,
    Error& error
    )
{
    FINJIN_ERROR_METHOD_START(error);

    this->isDepthStencilScreenSizeDependent = isScreenSizeDependent;

    if (this->depthStencilTextureDesc == nullptr)
    {
        this->depthStencilTextureDesc = [[MTLTextureDescriptor alloc] init];
        if (this->depthStencilTextureDesc == nullptr)
        {
            FINJIN_SET_ERROR(error, "Failed to create depth/stencil texture descriptor.");
            return;
        }

        this->depthStencilTextureDesc.textureType = MTLTextureType2D;
        this->depthStencilTextureDesc.mipmapLevelCount = 1;
        this->depthStencilTextureDesc.resourceOptions = MTLResourceStorageModePrivate;
        this->depthStencilTextureDesc.usage = MTLTextureUsageRenderTarget;
    }
    this->depthStencilTextureDesc.width = width;
    this->depthStencilTextureDesc.height = height;
    this->depthStencilTextureDesc.pixelFormat = depthStencilFormat;

    this->depthStencilTexture = [device newTextureWithDescriptor:this->depthStencilTextureDesc];
    if (this->depthStencilTexture == nullptr)
    {
        FINJIN_SET_ERROR(error, "Failed to create depth/stencil texture.");
        return;
    }

    if (this->depthStencilStateDesc == nullptr)
    {
        this->depthStencilStateDesc = [[MTLDepthStencilDescriptor alloc] init];
        if (this->depthStencilStateDesc == nullptr)
        {
            FINJIN_SET_ERROR(error, "Failed to create depth/stencil state descriptor.");
            return;
        }
    }
    this->depthStencilStateDesc.depthCompareFunction = MTLCompareFunctionLess;
    this->depthStencilStateDesc.depthWriteEnabled = YES;
    this->depthStencilState = [device newDepthStencilStateWithDescriptor:depthStencilStateDesc];
    if (this->depthStencilState == nullptr)
    {
        FINJIN_SET_ERROR(error, "Failed to create depth/stencil state.");
        return;
    }
}

void MetalRenderTarget::Destroy()
{
    this->colorTextureDesc = nullptr;
    for (auto& colorOutput : this->colorOutputs)
        colorOutput.Destroy();
    this->colorOutputs.clear();

    this->depthStencilTextureDesc = nullptr;
    this->depthStencilTexture = nullptr;
    this->depthStencilStateDesc = nullptr;
    this->depthStencilState = nullptr;
}

void MetalRenderTarget::DestroyScreenSizeDependentResources()
{
    if (this->isColorScreenSizeDependent)
    {
        this->colorTextureDesc = nullptr;
        for (auto& colorOutput : this->colorOutputs)
            colorOutput.Destroy();
        this->colorOutputs.clear();
    }

    if (this->isDepthStencilScreenSizeDependent)
    {
        this->depthStencilTextureDesc = nullptr;
        this->depthStencilTexture = nullptr;
        this->depthStencilStateDesc = nullptr;
        this->depthStencilState = nullptr;
    }
}

bool MetalRenderTarget::HasDepthStencil() const
{
    return this->depthStencilTexture != nullptr;
}

#endif
