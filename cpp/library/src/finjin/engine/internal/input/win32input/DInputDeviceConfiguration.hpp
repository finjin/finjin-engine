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
#include "finjin/common/Utf8String.hpp"
#include "finjin/common/Uuid.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    class DInputDeviceConfiguration
    {
    public:
        DInputDeviceConfiguration();
        DInputDeviceConfiguration(const void* ddi);

        uint8_t GetDeviceType() const;
        uint8_t GetDeviceSubtype() const;

        const Utf8String& GetProductDescriptor() const;
        const Utf8String& GetInstanceDescriptor() const;

        const Utf8String& GetDebugName() const; //Name used in error messages

    public:
        int id;

        Utf8String productDescriptor;
        Utf8String instanceDescriptor;

        Uuid productGuid;
        Uuid instanceGuid;

        Utf8String productName;
        Utf8String instanceName;

        //The device type specifier
        uint32_t type;
    };

} }
