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


//Includes---------------------------------------------------------------------
#include "D3D12Includes.hpp"
#include "finjin/common/Math.hpp"
#include "finjin/common/UsableAllocatedVector.hpp"
#include "finjin/engine/FinjinSceneAssets.hpp"
#include "finjin/engine/ShaderFeatures.hpp"


//Classes----------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    class D3D12Light
    {
    public:
        D3D12Light(Allocator* allocator);
                
    public:
        FinjinSceneObjectLight* finjinLight;

        size_t gpuBufferIndex; //Index into constant buffer or structured buffer corresponding to this light
    };

    class D3D12LightTable
    {
    public:
        void Create(size_t count, Allocator* allocator, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            if (!this->lights.Create(count, allocator, allocator))
            {
                FINJIN_SET_ERROR(error, "Failed to allocate light lookup.");
                return;
            }
            for (size_t i = 0; i < this->lights.items.size(); i++)
                this->lights.items[i].gpuBufferIndex = i;
        }

        D3D12Light* Add(FinjinSceneObjectLight* light)
        {
            auto d3d12Light = this->lights.Use();
            if (d3d12Light == nullptr)
                return nullptr;

            d3d12Light->finjinLight = light;
            /*light->EvaluateLightState(d3d12Light->frameStates[0].lightState);
            for (size_t i = 1; i < d3d12Light->frameStates.size(); i++)
                d3d12Light->frameStates[i] = d3d12Light->frameStates[0];*/

            light->gpuLight = d3d12Light;
        }

        void Remove(D3D12Light* d3d12Light)
        {
            this->lights.Unuse(d3d12Light);
        }

    public:
        UsableAllocatedVector<D3D12Light> lights;
    };

} }
