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


//Includes---------------------------------------------------------------------
#include "finjin/engine/EventCollection.hpp"
#include "finjin/engine/InputBindings.hpp"


//Classes----------------------------------------------------------------------
namespace Finjin { namespace Engine {

    class StandardGameControllerEvents : public EventCollectionFlags<>
    {
    public:
        enum //EventID
        {
            ACCEPT,
            CANCEL,
            DEFAULT,
            CHANGE,

            SETTINGS,
            
            SYSTEM_SETTINGS,

            SYSTEM_HOME,

            SHIFT_LEFT,
            SHIFT_RIGHT,

            BRAKE,
            GAS,

            TOGGLE,

            MOVE,
            MOVE_TOGGLE,

            LOOK,
            LOOK_TOGGLE
        };
        
        StandardGameControllerEvents()
        {
            Reset();
        }

        void Reset()
        {
            ClearFlags();

            this->brake = 0;
            this->gas = 0;
            this->toggle[0] = this->toggle[1] = 0;
            this->move[0] = this->move[1] = 0;
            this->look[0] = this->look[1] = 0;
        }
        
    public:
        float brake; //Left trigger, if there is one
        float gas; //Right trigger, if there is one
        float toggle[2]; //D-pad x/y, if there's a d-pad and an analog stick
        float move[2]; //Left analog stick x/y, if there's a d-pad and a left analog stick. Otherwise, it's the d-pad
        float look[2]; //Right analog stick x/y, if there is one
    };

    class StandardGameControllerInputBindings : public InputBindings<StandardGameControllerEvents>
    {
    public:
        static const Utf8String DEFAULT_BINDINGS_FILE_NAME;
        
        enum //InputActionID
        {
            ACCEPT,
            CANCEL,
            DEFAULT,
            CHANGE,

            SETTINGS,
            
            SYSTEM_SETTINGS,

            SYSTEM_HOME,

            SHIFT_LEFT,
            SHIFT_RIGHT,

            BRAKE,
            GAS,

            TOGGLE_UP,
            TOGGLE_DOWN,
            TOGGLE_LEFT,
            TOGGLE_RIGHT,

            MOVE_UP,
            MOVE_DOWN,
            MOVE_LEFT,
            MOVE_RIGHT,
            MOVE_TOGGLE,

            LOOK_UP,
            LOOK_DOWN,
            LOOK_LEFT,
            LOOK_RIGHT,
            LOOK_TOGGLE
        };
        
    public:
        StandardGameControllerInputBindings();

    protected:
        void ProcessInputAction
            (
            StandardGameControllerEvents& outputEvents,
            InputActionID inputActionID,
            InputDeviceComponent deviceComponent,
            InputTriggerFlag trigger,
            InputBindingValue& amount,
            SimpleTimeDelta elapsedTimeSeconds
            ) override;

        const InputActionInfo* GetInputActionInfo() const override;
    };

} }
