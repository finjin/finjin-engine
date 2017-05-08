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
    this->depthStencilTextureDesc = nullptr;
    this->depthStencilState = nullptr;
    this->depthStencilTexture = nullptr;

    this->renderPassDescriptor = nullptr;
    this->renderCommandEncoder = nullptr;
}

void MetalRenderTarget::CreateDepthStencil(id<MTLDevice> device, size_t width, size_t height, MTLPixelFormat depthStencilFormat, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    this->depthStencilTextureDesc = [[MTLTextureDescriptor alloc] init];
    this->depthStencilTextureDesc.textureType = MTLTextureType2D;
    this->depthStencilTextureDesc.width = width;
    this->depthStencilTextureDesc.height = height;
    this->depthStencilTextureDesc.pixelFormat = depthStencilFormat;
    this->depthStencilTextureDesc.mipmapLevelCount = 1;
    this->depthStencilTextureDesc.resourceOptions = MTLResourceStorageModePrivate;
    this->depthStencilTextureDesc.usage = MTLTextureUsageRenderTarget;

    this->depthStencilTexture = [device newTextureWithDescriptor:this->depthStencilTextureDesc];
    if (this->depthStencilTexture == nullptr)
        FINJIN_SET_ERROR(error, "Failed to create depth/stencil texture.");

    MTLDepthStencilDescriptor* depthStencilStateDesc = [[MTLDepthStencilDescriptor alloc] init];
    depthStencilStateDesc.depthCompareFunction = MTLCompareFunctionLess;
    depthStencilStateDesc.depthWriteEnabled = YES;
    this->depthStencilState = [device newDepthStencilStateWithDescriptor:depthStencilStateDesc];
}

bool MetalRenderTarget::HasDepthStencil() const
{
    return this->depthStencilTexture != nullptr;
}

void MetalRenderTarget::DestroySceenSizeDependentResources()
{
    for (auto& renderTargetResource : this->renderTargetOutputResources)
        renderTargetResource = nullptr;
    this->renderTargetOutputResources.clear();

    this->depthStencilTextureDesc = nullptr;
    this->depthStencilState = nullptr;
    this->depthStencilTexture = nullptr;
}

#endif
