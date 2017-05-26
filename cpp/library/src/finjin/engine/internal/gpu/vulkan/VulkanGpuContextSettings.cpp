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

#if FINJIN_TARGET_GPU_SYSTEM == FINJIN_TARGET_GPU_SYSTEM_VULKAN

#include "VulkanGpuContextSettings.hpp"

using namespace Finjin::Engine;


//Implementation----------------------------------------------------------------
VulkanGpuContextSettings::VulkanGpuContextSettings(Allocator* initialAllocator) :
    GpuContextCommonSettings(initialAllocator),
    colorSpace(VK_COLOR_SPACE_SRGB_NONLINEAR_KHR),
    colorFormat(VK_FORMAT_UNDEFINED),
    depthStencilFormat(VK_FORMAT_UNDEFINED)
{
    this->stencilRequired = false;
    this->maxFenceTimeout = TimeDuration::Milliseconds(100);
    this->deviceAllocationCallbacks = nullptr;
}

#endif
