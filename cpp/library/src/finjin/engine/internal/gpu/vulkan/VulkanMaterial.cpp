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

#include "VulkanMaterial.hpp"

using namespace Finjin::Engine;


//Implementation----------------------------------------------------------------

//VulkanMaterial
VulkanMaterial::VulkanMaterial(Allocator* allocator)
{
    this->finjinMaterial = nullptr;

    this->gpuBufferIndex = (size_t)-1;

    this->isFullyResolved = false;
}

//VulkanMaterial::Map
VulkanMaterial::Map::Map()
{
    this->finjinMap = nullptr;
    this->texture = nullptr;
}

#endif
