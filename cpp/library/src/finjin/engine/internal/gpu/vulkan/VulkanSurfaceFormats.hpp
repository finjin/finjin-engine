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
#include "finjin/common/StaticVector.hpp"
#include "VulkanUtilities.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    class VulkanSurfaceFormats : public StaticVector<VkSurfaceFormatKHR, 32> //Way more than necessary
    {
    public:
        using Super = StaticVector<VkSurfaceFormatKHR, 32>;

        using iterator = typename Super::iterator;
        using const_iterator = typename Super::const_iterator;

        const_iterator FindFormat(VkFormat format) const;
        const_iterator FindFormat(VkFormat format, VkColorSpaceKHR colorSpace) const;

        void Enumerate(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, VulkanInstanceFunctions& vk, Error& error);
    };

} }
