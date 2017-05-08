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
#include "finjin/common/Chrono.hpp"
#include "VulkanIncludes.hpp"
#include "VulkanRenderTarget.hpp"
#include "VulkanTexture.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    class VulkanFrameStage
    {
    public:
        VulkanFrameStage();

    public:
        size_t index;
        uint32_t frameBufferIndex;

        size_t sequenceIndex;

        SimpleTimeDelta elapsedTime;
        SimpleTimeCounter totalElapsedTime;

        VulkanRenderTarget* renderTarget;
        StaticVector<VulkanRenderTarget*, EngineConstants::MAX_RENDER_TARGET_DEPENDENCIES> dependentRenderTargets;
    };

} }
