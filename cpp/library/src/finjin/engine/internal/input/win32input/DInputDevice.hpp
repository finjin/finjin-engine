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
#include "finjin/common/Chrono.hpp"
#include "finjin/common/Path.hpp"
#include "finjin/common/Uuid.hpp"
#include "finjin/common/VirtualFileSystem.hpp"
#include "finjin/engine/InputSource.hpp"
#include "DInputDeviceConfiguration.hpp"
#include "DInputDeviceComponents.hpp"
#include <dinput.h>

#define IID_DirectInputDevice IID_IDirectInputDevice8
using IDInput = IDirectInput8*;
using IDInputDevice = IDirectInputDevice8*;


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    template <typename State, typename Button, typename Axis, typename Pov, size_t maxButtonCount, size_t maxAxisCount = 1, size_t maxPovCount = 1>
    struct DInputDeviceState : public InputDeviceState<Button, Axis, Pov, maxButtonCount, maxAxisCount, maxPovCount>
    {
        DInputDeviceState()
        {
        }

        template <typename DeviceType>
        bool GetState(DeviceType device, State& state)
        {
            FINJIN_ZERO_MEMORY(&state, sizeof(State));

            auto result = false;
            if (device != nullptr)
            {
                if (SUCCEEDED(device->GetDeviceState(sizeof(state), &state)))
                {
                    //Succeeded on first try
                    result = true;
                }
                else if (SUCCEEDED(device->Acquire()) && SUCCEEDED(device->GetDeviceState(sizeof(state), &state)))
                {
                    //Succeeded on second try after reacquiring device
                    result = true;
                }
            }

            return result;
        }

        template <typename DeviceType>
        void Update(DeviceType device, InputDeviceComponentClass componentClasses, bool isFirstUpdate = false)
        {
            State state;
            auto gotState = GetState(device, state);

            Update(state, gotState, componentClasses, isFirstUpdate);
        }

        void Update(State& state, bool gotState, InputDeviceComponentClass componentClasses, bool isFirstUpdate = false)
        {
            //Track insertion and removals
            auto wasPreviouslyConnected = this->isConnected;
            if (isFirstUpdate)
            {
                this->isConnected = true; //Treat it as though it's connected even if acquiring the state failed
                this->connectionChanged = false;
            }
            else
            {
                this->isConnected = gotState;
                this->connectionChanged = wasPreviouslyConnected != this->isConnected;
            }

            if (gotState)
            {
                auto stateBytes = reinterpret_cast<uint8_t*>(&state);

                if (AnySet(componentClasses & InputDeviceComponentClass::AXIS))
                {
                    for (auto& axis : this->axes)
                        axis.Update(RoundToFloat(*reinterpret_cast<LONG*>(stateBytes + axis.dinputOffset)), isFirstUpdate);
                }

                if (AnySet(componentClasses & InputDeviceComponentClass::BUTTON))
                {
                    for (auto& button : this->buttons)
                        button.Update(*reinterpret_cast<BYTE*>(stateBytes + button.dinputOffset) ? true : false, isFirstUpdate);
                }

                if (AnySet(componentClasses & InputDeviceComponentClass::POV))
                {
                    for (auto& pov : this->povs)
                        pov.Update(*reinterpret_cast<LONG*>(stateBytes + pov.dinputOffset), isFirstUpdate);
                }
            }
        }
    };

    class Win32InputContext;

    class DInputDevice
    {
    public:
        DInputDevice();
        ~DInputDevice();

        void Create(Win32InputContext* context, const DInputDeviceConfiguration& config, Error& error);
        void Destroy();

        const Utf8String& GetDisplayName() const;
        void SetDisplayName(const Utf8String& value);

        HRESULT SetPropertyDWord(const GUID& guidProp, DWORD object, DWORD how, DWORD value);
        HRESULT SetPropertyRange(const GUID& guidProperty, DWORD object, DWORD how, LONG minValue, LONG maxValue);

    public:
        Win32InputContext* context;
        IDInputDevice dinputDevice;
        DIDEVCAPS dinputCapabilities;
        DInputDeviceConfiguration config;
        Utf8String displayName;
        InputDeviceSemantic semantic;
    };

} }
