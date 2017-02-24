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
#include "DInputDeviceConfiguration.hpp"
#include <dinput.h>

using namespace Finjin::Common;
using namespace Finjin::Engine;


//Implementation---------------------------------------------------------------
DInputDeviceConfiguration::DInputDeviceConfiguration()
{    
}

DInputDeviceConfiguration::DInputDeviceConfiguration(const void* voidDDI)
{
    auto ddi = (const DIDEVICEINSTANCE*)voidDDI;

    this->productGuid = ddi->guidProduct;
    this->instanceGuid = ddi->guidInstance;

    this->productDescriptor = this->productGuid.ToString();
    this->instanceDescriptor = this->instanceGuid.ToString();
    
    this->instanceName = ddi->tszInstanceName;
    this->productName = ddi->tszProductName;
    
    this->type = ddi->dwDevType;
}

uint8_t DInputDeviceConfiguration::GetDeviceType() const
{
    return (BYTE)(this->type & 0xff);
}

uint8_t DInputDeviceConfiguration::GetDeviceSubtype() const
{
    return (BYTE)((this->type && 0xff00) >> 8);
}

const Utf8String& DInputDeviceConfiguration::GetProductDescriptor() const
{
    return this->productDescriptor;
}

const Utf8String& DInputDeviceConfiguration::GetInstanceDescriptor() const
{
    return this->instanceDescriptor;
}

const Utf8String& DInputDeviceConfiguration::GetDebugName() const
{
    return this->productName;
}
