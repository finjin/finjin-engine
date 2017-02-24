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

    class VRGameControllerEvents : public EventCollectionFlags<>
    {
    public:
        enum //EventID
        {
            ACCEPT,
            CANCEL,
            
            SETTINGS,
            
            SYSTEM_HOME,

            GAS,

            TOGGLE,
            
            MOVE,
            MOVE_TOGGLE,

            LOOK,
            LOOK_TOGGLE,

            LOCATOR
        };
        
        VRGameControllerEvents()
        {
            Reset();
        }

        void Reset()
        {
            ClearFlags();

            this->gas = 0;
            this->toggle[0] = this->toggle[1] = 0;
            this->move[0] = this->move[1] = 0;
            this->look[0] = this->look[1] = 0;
            FINJIN_ZERO_ITEM(this->locator);
        }
        
    public:
        float gas; //Trigger
        float toggle[2]; //D-pad x/y, if there's a d-pad and an analog stick
        float move[2]; //Touchpad movement
        float look[2]; //Right analog stick x/y, if there is one
        InputBindingValue::Locator locator;
    };

    class VRGameControllerInputBindings : public InputBindings<VRGameControllerEvents>
    {
    public:
        static const Utf8String DEFAULT_BINDINGS_FILE_NAME;
        
        enum //InputActionID
        {
            ACCEPT,
            CANCEL,
            
            SETTINGS,
            
            SYSTEM_HOME,

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
            LOOK_TOGGLE,

            LOCATOR
        };
        
    public:
        VRGameControllerInputBindings();

    protected:
        void ProcessInputAction
            (
            VRGameControllerEvents& outputEvents,
            InputActionID inputActionID,
            InputDeviceComponent deviceComponent,
            InputTriggerFlag trigger,
            InputBindingValue& amount,
            SimpleTimeDelta elapsedTimeSeconds
            ) override;

        const InputActionInfo* GetInputActionInfo() const override;
    };

} }
