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
#include "finjin/common/StaticVector.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    //4 bytes
    union VulkanPushConstant
    {
        uint32_t ui;
        int32_t i;
        float f;
    };

    //Vulkan guarantees a minimum of 128 bytes. NVidia GTX 980 reports 256 bytes supported
    using VulkanStaticPushConstants = StaticVector<VulkanPushConstant, 256 / sizeof(VulkanPushConstant)>;

} }
