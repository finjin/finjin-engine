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
#include "finjin/common/DynamicVector.hpp"
#include "finjin/common/Error.hpp"
#include "finjin/common/StaticVector.hpp"
#include "VulkanIncludes.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    struct VulkanLayerPropertyExtensions
    {
        VkLayerProperties properties;
        StaticVector<VkExtensionProperties, 16> extensions;
    };

    class VulkanLayerProperties : public DynamicVector<VulkanLayerPropertyExtensions>
    {
    public:
        void Create(VulkanGlobalFunctions& vulkanGlobalFunctions, Allocator* allocator, Error& error);

        const VulkanLayerPropertyExtensions* GetByName(const Utf8String& layerName) const;

    private:
        void EnumerateExtensions(VulkanGlobalFunctions& vulkanGlobalFunctions, VulkanLayerPropertyExtensions& layerExtensions, Error& error);
    };

} }
