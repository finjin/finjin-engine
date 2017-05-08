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
#include "finjin/common/StaticVector.hpp"
#include "MetalIncludes.hpp"


//Macros------------------------------------------------------------------------
#if FINJIN_DEBUG
    #define FINJIN_METAL_SET_RESOURCE_LABEL(obj, name) obj.label = name
#else
    #define FINJIN_METAL_SET_RESOURCE_LABEL(obj, name)
#endif


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    class MetalUtilities
    {
    public:
        static bool IsDepthStencilFormat(MTLPixelFormat format);

        static size_t GetUniformBufferAlignedSize(size_t paddedSize, size_t size, size_t count, size_t minBufferOffsetAlignment);
    };

} }
