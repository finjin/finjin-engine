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
#include "VulkanIncludes.hpp"
#include "finjin/common/Math.hpp"
#include "finjin/common/UsableDynamicVector.hpp"
#include "finjin/engine/FinjinSceneAssets.hpp"
#include "finjin/engine/ShaderFeatures.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    class VulkanLight
    {
    public:
        VulkanLight(Allocator* allocator);

    public:
        FinjinSceneObjectLight* finjinLight;

        size_t gpuBufferIndex; //Index into constant buffer or structured buffer corresponding to this light
    };

} }
