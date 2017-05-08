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
#include "finjin/common/StaticVector.hpp"
#include "finjin/engine/FinjinSceneAssets.hpp"
#include "finjin/engine/ShaderFeatureFlag.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    struct ShaderFeatures
    {
        ShaderFeatures();

        void ParseAndAddFlags(const Utf8StringView& features, Error& error);
        bool ParseAndAddFlag(const Utf8StringView& feature);

        void ParseAndAddLightTypes(const Utf8StringView& features, Error& error);
        bool ParseAndAddLightType(const Utf8StringView& feature);

        size_t GetHash() const;

        size_t inputFormatHash;
        ShaderFeatureFlag flags;
        StaticVector<LightType, EngineConstants::MAX_LIGHTS_PER_OBJECT> lights;
    };

} }
