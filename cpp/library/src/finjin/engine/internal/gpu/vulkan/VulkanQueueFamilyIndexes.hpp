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
#include "VulkanIncludes.hpp"
#include "VulkanGpuDescription.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    class VulkanQueueFamilyIndexes
    {
    public:
        VulkanQueueFamilyIndexes();

        void Enumerate(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, VulkanInstanceFunctions& vk, VulkanQueueFamilyProperties& queueProperties, Error& error);

        bool IsGraphicsValid() const { return this->graphics != UINT32_MAX; }
        bool IsComputeValid() const { return this->compute != UINT32_MAX; }
        bool IsTransferValid() const { return this->transfer != UINT32_MAX; }
        bool IsSparseBindingValid() const { return this->sparseBinding != UINT32_MAX; }
        bool IsPresentValid() const { return this->present != UINT32_MAX; }

        size_t GetValidCount() const;

        enum { MAX_VALID_COUNT = 5 };

    private:
        void Reset();

    public:
        uint32_t graphics;
        uint32_t compute;
        uint32_t transfer;
        uint32_t sparseBinding;
        uint32_t present; //Not necessarily a distinct queue. Might be the same as one of the other queues, most likely graphics
    };

} }
