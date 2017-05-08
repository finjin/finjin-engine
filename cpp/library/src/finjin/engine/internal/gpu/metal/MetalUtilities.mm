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

#include "MetalUtilities.hpp"

using namespace Finjin::Engine;


//Implementation----------------------------------------------------------------
bool MetalUtilities::IsDepthStencilFormat(MTLPixelFormat format)
{
    switch (format)
    {
    #if FINJIN_TARGET_PLATFORM == FINJIN_TARGET_PLATFORM_MACOS
        case MTLPixelFormatDepth24Unorm_Stencil8: return true;
        case MTLPixelFormatX24_Stencil8: return true;
    #endif
        case MTLPixelFormatDepth32Float_Stencil8: return true;
        case MTLPixelFormatX32_Stencil8: return true;

        default: return false;
    }
}

size_t MetalUtilities::GetUniformBufferAlignedSize(size_t paddedSize, size_t size, size_t count, size_t minBufferOffsetAlignment)
{
    auto byteSize = paddedSize * (count - 1) + size;
    return std::max(byteSize, minBufferOffsetAlignment);
}

#endif
