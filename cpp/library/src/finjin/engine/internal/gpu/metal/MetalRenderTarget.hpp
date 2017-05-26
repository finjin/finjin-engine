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
#include "MetalIncludes.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    class MetalRenderTarget
    {
    public:
        MetalRenderTarget();

        void CreateColor
            (
            id<MTLDevice> device,
            size_t width,
            size_t height,
            MTLPixelFormat colorFormat,
            bool isScreenSizeDependent,
            Error& error
            );

        void CreateDepthStencil
            (
            id<MTLDevice> device,
            size_t width,
            size_t height,
            MTLPixelFormat depthStencilFormat,
            bool isScreenSizeDependent,
            Error& error
            );

        void Destroy();
        void DestroyScreenSizeDependentResources();

        bool HasDepthStencil() const;

    public:
        struct ColorOutput
        {
            ColorOutput()
            {
                this->texture = nullptr;
                this->drawable = nullptr;
            }

            void Destroy()
            {
                this->texture = nullptr;
                this->drawable = nullptr;
            }

            id<MTLTexture> GetTexture()
            {
                if (this->drawable != nullptr)
                    return this->drawable.texture;
                return this->texture;
            }

            id<MTLTexture> texture;
            id<CAMetalDrawable> drawable;
        };
        StaticVector<ColorOutput, EngineConstants::MAX_RENDER_TARGET_OUTPUTS> colorOutputs;

        MTLTextureDescriptor* colorTextureDesc;
        bool isColorScreenSizeDependent;

        MTLTextureDescriptor* depthStencilTextureDesc;
        id<MTLTexture> depthStencilTexture;
        MTLDepthStencilDescriptor* depthStencilStateDesc;
        id<MTLDepthStencilState> depthStencilState;
        bool isDepthStencilScreenSizeDependent;

        MTLRenderPassDescriptor* renderPassDescriptor;
        id<MTLRenderCommandEncoder> renderCommandEncoder;

        StaticVector<MTLViewport, 1> viewports;
        StaticVector<MTLScissorRect, 1> scissorRects;

        StaticVector<MTLViewport, 1> defaultViewport;
        StaticVector<MTLScissorRect, 1> defaultScissorRect;

        Setting<MathVector4> clearColor;
    };

} }
