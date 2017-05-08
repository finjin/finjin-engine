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

#include "VulkanPrimaryQueues.hpp"

using namespace Finjin::Engine;


//Implementation----------------------------------------------------------------
VulkanPrimaryQueues::VulkanPrimaryQueues()
{
    this->graphics = VK_NULL_HANDLE;
    this->compute = VK_NULL_HANDLE;
    this->transfer = VK_NULL_HANDLE;
    this->sparseBinding = VK_NULL_HANDLE;
    this->present = VK_NULL_HANDLE;
}

void VulkanPrimaryQueues::SetPresent(const VulkanQueueFamilyIndexes& indexes)
{
    if (indexes.present == indexes.graphics)
        this->present = this->graphics;
    else if (indexes.present == indexes.compute)
        this->present = this->compute;
    else if (indexes.present == indexes.transfer)
        this->present = this->transfer;
    else if (indexes.present == indexes.sparseBinding)
        this->present = this->sparseBinding;
}

#endif
