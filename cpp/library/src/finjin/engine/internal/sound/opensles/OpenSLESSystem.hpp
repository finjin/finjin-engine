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
#include "finjin/engine/SoundSystemCommonSettings.hpp"
#include "OpenSLESContext.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    class OpenSLESSystem
    {
    public:
        OpenSLESSystem();
        ~OpenSLESSystem();

        struct Settings : SoundSystemCommonSettings
        {
        };

        void Create(const Settings& settings, Error& error);
        void Destroy();

        OpenSLESContext* CreateContext(const OpenSLESContext::Settings& settings, Error& error);
        void DestroyContext(OpenSLESContext* context);

        static const Utf8String& GetSystemInternalName();

    private:
        struct Impl;
        std::unique_ptr<Impl> impl;
    };

} }
