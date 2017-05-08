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
#include "finjin/common/Math.hpp"
#include "finjin/common/Utf8String.hpp"
#include "finjin/engine/EventCollection.hpp"
#include "finjin/engine/InputBindings.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    class FlyingCameraEvents : public EventCollectionFlags<>
    {
    public:
        enum //EventID
        {
            TAP,

            SWIPE,

            MOUSE_X,
            MOUSE_Y,
            MOUSE_LEFT_BUTTON_DOWN,
            MOUSE_LEFT_BUTTON_UP,
            MOUSE_RIGHT_BUTTON_DOWN,
            MOUSE_RIGHT_BUTTON_UP,

            MOVE,

            PAN_ORBIT,

            LOOK,

            SELECT_OBJECT,
            DESELECT_OBJECT,

            CHANGE_CAMERA,
            TOGGLE_VIEW_CAMERA_LOCK,

            SET_DOLLY_MODE,
            POP_DOLLY,

            SET_PAN_OR_ORBIT_MODE,
            POP_PAN_OR_ORBIT,

            SET_FREE_LOOK_MODE,
            POP_FREE_LOOK,

            ZOOM_TO_FIT,

            SCREENSHOT,

            ESCAPE,
            MENU
        };

        FlyingCameraEvents()
        {
            Reset();
        }

        void Reset()
        {
            ClearFlags();

            this->tap = MathVector2::Zero();
            this->swipe = MathVector2::Zero();
            this->mouse = MathVector2::Zero();
            this->move = MathVector2::Zero();
            this->panOrbit = MathVector2::Zero();
            this->look = MathVector2::Zero();

            this->usedKeyboard = false;
            this->usedMouse = false;
            this->usedGameController = false;
            this->usedTouchScreen = false;
        }

    public:
        //An absolute tap position on the screen, in DIPS
        MathVector2 tap;

        //A relative swipe on the screen, in DIPS
        MathVector2 swipe;

        //Absolute mouse coordiantes
        MathVector2 mouse;

        /**
         * The components represent the amount of movement where:
         *  +x is right, -x is left,
         *  +y is forward, -y is backward
         * The MOVE event must be set for this to be used
         */
        MathVector2 move;

        MathVector2 panOrbit;

        /**
         * The components represent the amount of rotation where:
         *  +x is turn right, -x is turn left
         *  +y is tilt upward, -y is tilt downward
         * The LOOK event must be set for this to be used
         */
        MathVector2 look;

        bool usedKeyboard;
        bool usedMouse;
        bool usedGameController;
        bool usedTouchScreen;
    };

    class FlyingCameraInputBindings : public InputBindings<FlyingCameraEvents>
    {
    public:
        enum //InputActionID
        {
            TAP_DOWN_X,
            TAP_DOWN_Y,

            SWIPE_UP,
            SWIPE_DOWN,
            SWIPE_LEFT,
            SWIPE_RIGHT,

            MOUSE_X,
            MOUSE_Y,
            MOUSE_LEFT_BUTTON,
            MOUSE_RIGHT_BUTTON,

            MOVE_FORWARD,
            MOVE_BACKWARD,
            MOVE_LEFT,
            MOVE_RIGHT,

            PAN_ORBIT_UP,
            PAN_ORBIT_DOWN,
            PAN_ORBIT_LEFT,
            PAN_ORBIT_RIGHT,

            LOOK_UP,
            LOOK_DOWN,
            LOOK_LEFT,
            LOOK_RIGHT,

            SET_DOLLY_MODE,
            SET_PAN_OR_ORBIT_MODE,
            SET_FREE_LOOK_MODE,

            ZOOM_TO_FIT,

            SELECT_OBJECT,
            DESELECT_OBJECT,

            SCREENSHOT,

            ESCAPE,
            MENU
        };

    public:
        FlyingCameraInputBindings();

        static const Utf8String& GetDefaultBindingsFileName();

    protected:
        void ProcessInputAction
            (
            FlyingCameraEvents& outputEvents,
            InputActionID inputActionID,
            InputDeviceComponent deviceComponent,
            InputTriggerFlag trigger,
            InputBindingValue& amount,
            SimpleTimeDelta elapsedTimeSeconds
            ) override;

        const InputActionInfo* GetInputActionInfo() const override;
    };

} }
