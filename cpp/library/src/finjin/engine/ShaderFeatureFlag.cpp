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
#include "finjin/engine/ShaderFeatureFlag.hpp"

using namespace Finjin::Engine;


//Implementation----------------------------------------------------------------
RenderShaderFeatureFlags::RenderShaderFeatureFlags()
{
    this->environmentFlags = ShaderFeatureFlag::NONE;
    this->renderingFlags = ShaderFeatureFlag::NONE;
    this->shaderFeatureFlagsMask = ShaderFeatureFlag::ALL;
    this->additionalShaderFeatureFlags = ShaderFeatureFlag::NONE;
}

ShaderFeatureFlag RenderShaderFeatureFlags::ForMaterialMapsAndMesh(ShaderFeatureFlag materialMapFlags, ShaderFeatureFlag meshFlags) const
{
    return ((materialMapFlags | meshFlags | this->environmentFlags | this->renderingFlags) & this->shaderFeatureFlagsMask) | this->additionalShaderFeatureFlags;
}

const RenderShaderFeatureFlags& RenderShaderFeatureFlags::GetDefault()
{
    static const RenderShaderFeatureFlags value;
    return value;
}
