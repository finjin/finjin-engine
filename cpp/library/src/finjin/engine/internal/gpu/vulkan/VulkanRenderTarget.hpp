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
#include "VulkanIncludes.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    struct VulkanRenderTarget
    {
        VulkanRenderTarget();

        VkImage image;
        VkDeviceMemory mem;
        VkImageView view;

        struct Viewport
        {
            Viewport()
            {
                this->width = this->height = 0;
            }

            float width;
            float height;
        };
        StaticVector<Viewport, EngineConstants::MAX_RENDER_TARGET_VIEWPORTS> viewports;

        struct ScissorRect
        {
            ScissorRect()
            {
                this->width = this->height = 0;
            }

            uint32_t width;
            uint32_t height;
        };
        StaticVector<ScissorRect, EngineConstants::MAX_RENDER_TARGET_VIEWPORTS> scissorRects;

        Setting<MathVector4> clearColor;
    };

} }
