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
#if FINJIN_TARGET_GPU_SYSTEM == FINJIN_TARGET_GPU_SYSTEM_D3D12
    #include "finjin/engine/internal/gpu/d3d12/D3D12System.hpp"
    namespace Finjin { namespace Engine {
        using GpuSystem = D3D12System;
    } }
#elif FINJIN_TARGET_GPU_SYSTEM == FINJIN_TARGET_GPU_SYSTEM_VULKAN
    #include "finjin/engine/internal/gpu/vulkan/VulkanSystem.hpp"
    namespace Finjin { namespace Engine {
        using GpuSystem = VulkanSystem;
    } }
#endif
