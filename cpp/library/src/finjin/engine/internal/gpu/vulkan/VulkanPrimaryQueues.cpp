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

void VulkanPrimaryQueues::Create(VulkanDeviceFunctions& vk, const VulkanQueueFamilyIndexes& indexes, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    if (indexes.IsGraphicsValid())
    {
        vk.GetDeviceQueue(vk.device, indexes.graphics, 0, &this->graphics);
        if (this->graphics == VK_NULL_HANDLE)
        {
            FINJIN_SET_ERROR(error, "Failed to get graphics queue.");
            return;
        }
    }

    if (indexes.IsComputeValid())
    {
        vk.GetDeviceQueue(vk.device, indexes.compute, 0, &this->compute);
        if (this->compute == VK_NULL_HANDLE)
        {
            FINJIN_SET_ERROR(error, "Failed to get compute queue.");
            return;
        }
    }

    if (indexes.IsTransferValid())
    {
        vk.GetDeviceQueue(vk.device, indexes.transfer, 0, &this->transfer);
        if (this->transfer == VK_NULL_HANDLE)
        {
            FINJIN_SET_ERROR(error, "Failed to get transfer queue.");
            return;
        }
    }

    if (indexes.IsSparseBindingValid())
    {
        vk.GetDeviceQueue(vk.device, indexes.sparseBinding, 0, &this->sparseBinding);
        if (this->sparseBinding == VK_NULL_HANDLE)
        {
            FINJIN_SET_ERROR(error, "Failed to get sparse binding queue.");
            return;
        }
    }

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
