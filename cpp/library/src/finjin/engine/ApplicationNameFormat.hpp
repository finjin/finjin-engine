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


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    enum class ApplicationNameFormat
    {
        DISPLAY, //Example: "My Application"
        UPPER_CAMEL_CASE, //Example: "MyApplication"
        LOWER_CASE, //Example: 'my-application'
        DOTTED_WITH_ORGANIZATION_PREFIX, //Example: "org.myorganization.myapplication"

        COUNT
    };

} }
