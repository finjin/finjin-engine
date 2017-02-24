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
#include "DInputDevice.hpp"
#include "Win32InputContext.hpp"

using namespace Finjin::Common;
using namespace Finjin::Engine;


//Implementation---------------------------------------------------------------
DInputDevice::DInputDevice()
{
    this->context = nullptr;

    this->dinputDevice = nullptr;    

    FINJIN_ZERO_ITEM(this->dinputCapabilities);
    this->dinputCapabilities.dwSize = sizeof(this->dinputCapabilities);

    this->semantic = InputDeviceSemantic::NONE;
}

DInputDevice::~DInputDevice()
{
}

void DInputDevice::Create(Win32InputContext* context, const DInputDeviceConfiguration& config, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto dinput = (IDInput)context->GetDInput();

    this->context = context;
    this->config = config;  
    this->displayName = config.productName;

    LPDIRECTINPUTDEVICE8 device;    
    if (FAILED(dinput->CreateDevice(config.instanceGuid, &device, nullptr)))
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create DirectInput device '%1%'", config.GetDebugName()));
        return;
    }
    
    if (FAILED(device->QueryInterface(IID_DirectInputDevice, (void**)&this->dinputDevice)))
    {
        device->Release();

        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to query DirectInput8 interface for device '%1%'", config.GetDebugName()));
        return;
    }

    device->Release();

    //Get device capabilities    
    if (FAILED(this->dinputDevice->GetCapabilities(&this->dinputCapabilities)))
    {
        Destroy();

        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to get DirectInput capabilities for device '%1%'", config.GetDebugName()));
        return;
    }    
}

void DInputDevice::Destroy()  
{
    if (this->dinputDevice != nullptr)
    {
        this->dinputDevice->Unacquire();                    
        this->dinputDevice->Release();
        this->dinputDevice = nullptr;
    }
}

const Utf8String& DInputDevice::GetDisplayName() const
{
    return this->displayName;
}

void DInputDevice::SetDisplayName(const Utf8String& value)
{
    this->displayName = value;
}

HRESULT DInputDevice::SetPropertyDWord(const GUID& guidProperty, DWORD object, DWORD how, DWORD value)
{
    DIPROPDWORD propWord = {};
    propWord.diph.dwSize = sizeof(propWord);
    propWord.diph.dwHeaderSize = sizeof(propWord.diph);
    propWord.diph.dwObj = object;
    propWord.diph.dwHow = how;
    propWord.dwData = value;
    return this->dinputDevice->SetProperty(guidProperty, &propWord.diph);
}

HRESULT DInputDevice::SetPropertyRange(const GUID& guidProperty, DWORD object, DWORD how, LONG minValue, LONG maxValue)
{
    DIPROPRANGE propRange = {};
    propRange.diph.dwSize = sizeof(propRange);
    propRange.diph.dwHeaderSize = sizeof(propRange.diph);
    propRange.diph.dwObj = object;
    propRange.diph.dwHow = how;
    propRange.lMin = minValue;
    propRange.lMax = maxValue;    
    return this->dinputDevice->SetProperty(guidProperty, &propRange.diph);
}
