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
#include "AndroidInputDevice.hpp"
#include <dlfcn.h>

using namespace Finjin::Engine;


//Implementation----------------------------------------------------------------

//AndroidInputDevice
AndroidInputDevice::AndroidInputDevice()
{
    Reset();
}

void AndroidInputDevice::Reset()
{
    this->id = 0;

    this->semantic = InputDeviceSemantic::NONE;

    this->displayName.clear();

    this->productName.clear();
    this->instanceName.clear();

    this->productDescriptor.clear();
    this->instanceDescriptor.clear();

    this->isVirtual = false;
}

int AndroidInputDevice::GetID() const
{
    return this->id;
}

const Utf8String& AndroidInputDevice::GetDisplayName() const
{
    return this->displayName;
}

void AndroidInputDevice::SetDisplayName(const Utf8String& value)
{
    this->displayName = value;
}

InputDeviceSemantic AndroidInputDevice::GetSemantic() const
{
    return this->semantic;
}

void AndroidInputDevice::SetSemantic(InputDeviceSemantic value)
{
    this->semantic = value;
}

const Utf8String& AndroidInputDevice::GetProductName() const
{
    return this->productName;
}

void AndroidInputDevice::SetProductName(const Utf8String& value)
{
    this->productName = value;
}

const Utf8String& AndroidInputDevice::GetInstanceName() const
{
    return this->instanceName;
}

void AndroidInputDevice::SetInstanceName(const Utf8String& value)
{
    this->instanceName = value;
}

const Utf8String& AndroidInputDevice::GetProductDescriptor() const
{
    return this->productDescriptor;
}

void AndroidInputDevice::SetProductDescriptor(const Utf8String& value)
{
    this->productDescriptor = value;
}

const Utf8String& AndroidInputDevice::GetInstanceDescriptor() const
{
    return this->instanceDescriptor;
}

void AndroidInputDevice::SetInstanceDescriptor(const Utf8String& value)
{
    this->instanceDescriptor = value;
}

size_t AndroidInputDevice::GetLocatorCount() const
{
    return 0;
}

InputLocator* AndroidInputDevice::GetLocator(size_t locatorIndex)
{
    return nullptr;
}

void AndroidInputDevice::SimplifyButtonName(Utf8String& name)
{
    name.ReplaceFirst("KEYCODE_BUTTON_", "");
    name.ReplaceFirst("KEYCODE_", "");
    name.ReplaceAll('_', ' ');
}

void AndroidInputDevice::SimplifyAxisName(Utf8String& name)
{
    name.ReplaceFirst("AXIS_", "");
    name.ReplaceAll('_', ' ');
}
