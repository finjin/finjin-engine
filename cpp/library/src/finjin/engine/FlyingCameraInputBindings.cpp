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
#include "FlyingCameraInputBindings.hpp"
#include "finjin/common/DebugLog.hpp"

using namespace Finjin::Engine;


//Static initialization--------------------------------------------------------
const Utf8String FlyingCameraInputBindings::DEFAULT_BINDINGS_FILE_NAME = "flying-camera.cfg";


//Implementation---------------------------------------------------------------
FlyingCameraInputBindings::FlyingCameraInputBindings()
{   
}

void FlyingCameraInputBindings::ProcessInputAction
    (
    FlyingCameraEvents& outputEvents,
    InputActionID inputActionID,
    InputDeviceComponent deviceComponent,
    InputTriggerFlag trigger,
    InputBindingValue& amount,
    SimpleTimeDelta elapsedTimeSeconds
    )
{
    //std::cout << Utf8StringFormatter::Format("Action id: %1%, amount %2%", inputActionID, amount.scalar) << std::endl;
    
    outputEvents.usedKeyboard |= InputDeviceComponentUtilities::IsKeyboard(deviceComponent);
    outputEvents.usedMouse |= InputDeviceComponentUtilities::IsMouse(deviceComponent);
    outputEvents.usedGameController |= InputDeviceComponentUtilities::IsGameController(deviceComponent);
    outputEvents.usedTouchScreen |= InputDeviceComponentUtilities::IsTouchScreen(deviceComponent);

    switch (inputActionID)
    {
        case TAP_DOWN_X:
        {
            outputEvents.Add(FlyingCameraEvents::TAP);
            outputEvents.tap[0] = amount.scalar;
            break;
        }
        case TAP_DOWN_Y:
        {
            outputEvents.Add(FlyingCameraEvents::TAP);
            outputEvents.tap[1] = amount.scalar;
            break;
        }

        case SWIPE_UP:
        {
            outputEvents.Add(FlyingCameraEvents::SWIPE);
            if (outputEvents.swipe[1] == 0)
                outputEvents.swipe[1] -= amount.scalar;
            break;
        }
        case SWIPE_DOWN:
        {
            outputEvents.Add(FlyingCameraEvents::SWIPE);
            if (outputEvents.swipe[1] == 0)
                outputEvents.swipe[1] += amount.scalar;
            break;
        }
        case SWIPE_LEFT:
        {
            outputEvents.Add(FlyingCameraEvents::SWIPE);
            if (outputEvents.swipe[0] == 0)
                outputEvents.swipe[0] -= amount.scalar;
            break;
        }
        case SWIPE_RIGHT:
        {
            outputEvents.Add(FlyingCameraEvents::SWIPE);
            if (outputEvents.swipe[0] == 0)
                outputEvents.swipe[0] += amount.scalar;
            break;
        }

        case MOUSE_X:
        {
            outputEvents.Add(FlyingCameraEvents::MOUSE_X);
            if (outputEvents.mouse[0] == 0)
                outputEvents.mouse[0] = amount.scalar;
            break;
        }
        case MOUSE_Y:
        {
            outputEvents.Add(FlyingCameraEvents::MOUSE_Y);
            if (outputEvents.mouse[1] == 0)
                outputEvents.mouse[1] = amount.scalar;
            break;
        }
        case MOUSE_LEFT_BUTTON:
        {
            if (AnySet(trigger & InputTriggerFlag::PRESSED))
                outputEvents.Add(FlyingCameraEvents::MOUSE_LEFT_BUTTON_DOWN);
            else if (AnySet(trigger & InputTriggerFlag::RELEASED))
                outputEvents.Add(FlyingCameraEvents::MOUSE_LEFT_BUTTON_UP);
            break;
        }
        case MOUSE_RIGHT_BUTTON:
        {
            if (AnySet(trigger & InputTriggerFlag::PRESSED))
                outputEvents.Add(FlyingCameraEvents::MOUSE_RIGHT_BUTTON_DOWN);
            else if (AnySet(trigger & InputTriggerFlag::RELEASED))
                outputEvents.Add(FlyingCameraEvents::MOUSE_RIGHT_BUTTON_UP);
            break;
        }
        
        case MOVE_FORWARD:
        {
            outputEvents.Add(FlyingCameraEvents::MOVE);
            if (outputEvents.move[1] == 0)
                outputEvents.move[1] += amount.scalar;
            break;
        }
        case MOVE_BACKWARD:
        {
            outputEvents.Add(FlyingCameraEvents::MOVE);
            if (outputEvents.move[1] == 0)
                outputEvents.move[1] -= amount.scalar;
            break;
        }
        case MOVE_LEFT:
        {
            outputEvents.Add(FlyingCameraEvents::MOVE);
            if (outputEvents.move[0] == 0)
                outputEvents.move[0] -= amount.scalar;
            break;
        }
        case MOVE_RIGHT:
        {
            outputEvents.Add(FlyingCameraEvents::MOVE);
            if (outputEvents.move[0] == 0)
                outputEvents.move[0] += amount.scalar;
            break;
        }
        
        case PAN_ORBIT_UP:
        {
            outputEvents.Add(FlyingCameraEvents::PAN_ORBIT);
            if (outputEvents.panOrbit[1] == 0)
                outputEvents.panOrbit[1] += amount.scalar;
            break;
        }
        case PAN_ORBIT_DOWN:
        {
            outputEvents.Add(FlyingCameraEvents::PAN_ORBIT);
            if (outputEvents.panOrbit[1] == 0)
                outputEvents.panOrbit[1] -= amount.scalar;
            break;
        }
        case PAN_ORBIT_LEFT:
        {
            outputEvents.Add(FlyingCameraEvents::PAN_ORBIT);
            if (outputEvents.panOrbit[0] == 0)
                outputEvents.panOrbit[0] -= amount.scalar;
            break;
        }
        case PAN_ORBIT_RIGHT:
        {
            outputEvents.Add(FlyingCameraEvents::PAN_ORBIT);
            if (outputEvents.panOrbit[0] == 0)
                outputEvents.panOrbit[0] += amount.scalar;
            break;
        }
        
        case LOOK_UP:
        {
            outputEvents.Add(FlyingCameraEvents::LOOK);
            if (outputEvents.look[1] == 0)
                outputEvents.look[1] += amount.scalar;
            break;
        }
        case LOOK_DOWN:
        {
            outputEvents.Add(FlyingCameraEvents::LOOK);
            if (outputEvents.look[1] == 0)
                outputEvents.look[1] -= amount.scalar;
            break;
        }
        case LOOK_LEFT:
        {
            outputEvents.Add(FlyingCameraEvents::LOOK);
            if (outputEvents.look[0] == 0)
                outputEvents.look[0] -= amount.scalar;
            break;
        }
        case LOOK_RIGHT:
        {
            outputEvents.Add(FlyingCameraEvents::LOOK);
            if (outputEvents.look[0] == 0)
                outputEvents.look[0] += amount.scalar;
            break;
        }
        
        case SELECT_OBJECT:
        {
            outputEvents.Add(FlyingCameraEvents::SELECT_OBJECT);
            break;
        }
        case DESELECT_OBJECT:
        {
            outputEvents.Add(FlyingCameraEvents::DESELECT_OBJECT);
            break;
        }
        
        case SET_DOLLY_MODE:
        {
            outputEvents.Add(FlyingCameraEvents::SET_DOLLY_MODE); 
            break;
        }
        
        case SET_PAN_OR_ORBIT_MODE:
        {
            outputEvents.Add(FlyingCameraEvents::SET_PAN_OR_ORBIT_MODE); 
            break;
        }
        
        case SET_FREE_LOOK_MODE:
        {
            outputEvents.Add(FlyingCameraEvents::SET_FREE_LOOK_MODE); 
            break;
        }
        
        case ZOOM_TO_FIT:
        {
            outputEvents.Add(FlyingCameraEvents::ZOOM_TO_FIT); 
            break;
        }

        case SCREENSHOT: 
        {
            outputEvents.Add(FlyingCameraEvents::SCREENSHOT); 
            break;
        }

        case ESCAPE: 
        {
            outputEvents.Add(FlyingCameraEvents::ESCAPE); 
            break;
        }
        case MENU: 
        {
            outputEvents.Add(FlyingCameraEvents::MENU); 
            break;
        }
    }
}

const FlyingCameraInputBindings::InputActionInfo* FlyingCameraInputBindings::GetInputActionInfo() const
{
    static const InputActionInfo actionInfo[] =
    {
        InputActionInfo("tap-down-x", TAP_DOWN_X, InputActionInfoFlag::PRIVATE),
        InputActionInfo("tap-down-y", TAP_DOWN_Y, InputActionInfoFlag::PRIVATE),

        InputActionInfo("swipe-up", SWIPE_UP, InputActionInfoFlag::PRIVATE),
        InputActionInfo("swipe-down", SWIPE_DOWN, InputActionInfoFlag::PRIVATE),
        InputActionInfo("swipe-left", SWIPE_LEFT, InputActionInfoFlag::PRIVATE),
        InputActionInfo("swipe-right", SWIPE_RIGHT, InputActionInfoFlag::PRIVATE),

        InputActionInfo("mouse-x", MOUSE_X, InputActionInfoFlag::PRIVATE),
        InputActionInfo("mouse-y", MOUSE_Y, InputActionInfoFlag::PRIVATE),
        InputActionInfo("mouse-left-button", MOUSE_LEFT_BUTTON, InputActionInfoFlag::PRIVATE),
        InputActionInfo("mouse-right-button", MOUSE_RIGHT_BUTTON, InputActionInfoFlag::PRIVATE),
        
        InputActionInfo("move-forward", MOVE_FORWARD),
        InputActionInfo("move-backward", MOVE_BACKWARD),
        InputActionInfo("move-left", MOVE_LEFT),
        InputActionInfo("move-right", MOVE_RIGHT),
        
        InputActionInfo("pan-orbit-up", PAN_ORBIT_UP),
        InputActionInfo("pan-orbit-down", PAN_ORBIT_DOWN),
        InputActionInfo("pan-orbit-left", PAN_ORBIT_LEFT),
        InputActionInfo("pan-orbit-right", PAN_ORBIT_RIGHT),
        
        InputActionInfo("look-up", LOOK_UP),
        InputActionInfo("look-down", LOOK_DOWN),
        InputActionInfo("look-left", LOOK_LEFT),
        InputActionInfo("look-right", LOOK_RIGHT),
        
        InputActionInfo("set-dolly-mode", SET_DOLLY_MODE),
        InputActionInfo("set-pan-or-orbit-mode", SET_PAN_OR_ORBIT_MODE),
        InputActionInfo("set-free-look-mode", SET_FREE_LOOK_MODE),

        InputActionInfo("zoom-to-fit", ZOOM_TO_FIT),

        InputActionInfo("select-object", SELECT_OBJECT),
        InputActionInfo("deselect-object", DESELECT_OBJECT),

        InputActionInfo("screenshot", SCREENSHOT),
        
        InputActionInfo("escape", ESCAPE),
        InputActionInfo("menu", MENU),

        InputActionInfo() //Null info
    };
    return actionInfo;
}
