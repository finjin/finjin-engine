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
#include "finjin/common/Math.hpp"
#include "finjin/engine/FinjinSceneAssets.hpp"
#include "VulkanPushConstant.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    struct VulkanRenderableEntity
    {
        VulkanRenderableEntity()
        {
            this->entity = nullptr;
            this->subentityIndex = 0;
            this->gpuBufferIndex = 0;
            //this->pushConstantStageFlags = (VkShaderStageFlagBits)0;
        }

        MathMatrix4 worldMatrix;
        MathMatrix4 worldInverseMatrix;
        MathMatrix4 worldInverseTransposeMatrix;
        MathVector4 ambientLightColor;
        std::array<uint32_t, EngineConstants::MAX_LIGHTS_PER_OBJECT> lightIndex;
        FinjinSceneObjectEntity* entity;
        size_t subentityIndex;
        uint32_t gpuBufferIndex; //Index into uniform buffer corresponding to this renderable
        //VulkanStaticPushConstants pushConstants;
        //VkShaderStageFlagBits pushConstantStageFlags; //TODO: Remove this?
    };

} }
