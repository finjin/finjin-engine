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
#include "finjin/engine/VRContextCommonSettings.hpp"

using namespace Finjin::Common;
using namespace Finjin::Engine;


//Implementation---------------------------------------------------------------
VRContextCommonSettings::VRContextCommonSettings(Allocator* allocator) : settingsFileNames(allocator)
{
    this->assetReader = nullptr;
    
    this->loadStandardRenderModels = true;

    this->maxCommandsPerUpdate = EngineConstants::DEFAULT_SUBSYSTEM_COMMANDS_PER_UPDATE;
}

void VRContextCommonSettings::HandleTopLevelSetting(const Utf8StringView& key, const Utf8StringView& value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    if (key == "max-commands-per-update")
        this->maxCommandsPerUpdate = Convert::ToNumber(value.ToString(), this->maxCommandsPerUpdate);
}
