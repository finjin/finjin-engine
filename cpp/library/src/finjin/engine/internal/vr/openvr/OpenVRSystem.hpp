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
#include "OpenVRContext.hpp"
#include "OpenVRSystemSettings.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    class OpenVRSystemImpl;

    class OpenVRSystem
    {
    public:
        OpenVRSystem();
        ~OpenVRSystem();

        using Settings = OpenVRSystemSettings;

        void Create(const Settings& settings, Error& error);
        void Destroy();

        OpenVRContext* CreateContext(const OpenVRContext::Settings& settings, Error& error);
        void DestroyContext(OpenVRContext* context);

        static const Utf8String& GetSystemInternalName();

    private:
        struct Impl;
        std::unique_ptr<Impl> impl;
    };

} }
