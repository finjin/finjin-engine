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
#include "StandardGameControllerInputBindings.hpp"
#include "finjin/common/DebugLog.hpp"

using namespace Finjin::Engine;


//Implementation----------------------------------------------------------------
StandardGameControllerInputBindings::StandardGameControllerInputBindings()
{
}

const Utf8String& StandardGameControllerInputBindings::GetDefaultBindingsFileName()
{
    static const Utf8String name("standard-game-controller.cfg");
    return name;
}

void StandardGameControllerInputBindings::ProcessInputAction
    (
    StandardGameControllerEvents& outputEvents,
    InputActionID inputActionID,
    InputDeviceComponent deviceComponent,
    InputTriggerFlag trigger,
    InputBindingValue& amount,
    SimpleTimeDelta elapsedTimeSeconds
    )
{
    //FINJIN_DEBUG_LOG_INFO("Action id: %1%, amount %2%", inputActionID, amount);

    switch (inputActionID)
    {
        case ACCEPT:
        {
            outputEvents.Add(StandardGameControllerEvents::ACCEPT);
            break;
        }
        case CANCEL:
        {
            outputEvents.Add(StandardGameControllerEvents::CANCEL);
            break;
        }
        case DEFAULT:
        {
            outputEvents.Add(StandardGameControllerEvents::DEFAULT);
            break;
        }
        case CHANGE:
        {
            outputEvents.Add(StandardGameControllerEvents::CHANGE);
            break;
        }
        case SETTINGS:
        {
            outputEvents.Add(StandardGameControllerEvents::SETTINGS);
            break;
        }
        case SYSTEM_SETTINGS:
        {
            outputEvents.Add(StandardGameControllerEvents::SYSTEM_SETTINGS);
            break;
        }
        case SYSTEM_HOME:
        {
            outputEvents.Add(StandardGameControllerEvents::SYSTEM_HOME);
            break;
        }
        case SHIFT_LEFT:
        {
            outputEvents.Add(StandardGameControllerEvents::SHIFT_LEFT);
            break;
        }
        case SHIFT_RIGHT:
        {
            outputEvents.Add(StandardGameControllerEvents::SHIFT_RIGHT);
            break;
        }
        case BRAKE:
        {
            outputEvents.Add(StandardGameControllerEvents::BRAKE);
            outputEvents.brake = amount.scalar;
            break;
        }
        case GAS:
        {
            outputEvents.Add(StandardGameControllerEvents::GAS);
            outputEvents.gas = amount.scalar;
            break;
        }
        case TOGGLE_UP:
        {
            outputEvents.Add(StandardGameControllerEvents::TOGGLE);
            if (outputEvents.toggle[1] == 0)
                outputEvents.toggle[1] = amount.scalar;
            break;
        }
        case TOGGLE_DOWN:
        {
            outputEvents.Add(StandardGameControllerEvents::TOGGLE);
            if (outputEvents.toggle[1] == 0)
                outputEvents.toggle[1] = -amount.scalar;
            break;
        }
        case TOGGLE_LEFT:
        {
            outputEvents.Add(StandardGameControllerEvents::TOGGLE);
            if (outputEvents.toggle[0] == 0)
                outputEvents.toggle[0] = -amount.scalar;
            break;
        }
        case TOGGLE_RIGHT:
        {
            outputEvents.Add(StandardGameControllerEvents::TOGGLE);
            if (outputEvents.toggle[0] == 0)
                outputEvents.toggle[0] = amount.scalar;
            break;
        }
        case MOVE_UP:
        {
            outputEvents.Add(StandardGameControllerEvents::MOVE);
            if (outputEvents.move[1] == 0)
                outputEvents.move[1] += amount.scalar;
            break;
        }
        case MOVE_DOWN:
        {
            outputEvents.Add(StandardGameControllerEvents::MOVE);
            if (outputEvents.move[1] == 0)
                outputEvents.move[1] -= amount.scalar;
            break;
        }
        case MOVE_LEFT:
        {
            outputEvents.Add(StandardGameControllerEvents::MOVE);
            if (outputEvents.move[0] == 0)
                outputEvents.move[0] -= amount.scalar;
            break;
        }
        case MOVE_RIGHT:
        {
            outputEvents.Add(StandardGameControllerEvents::MOVE);
            if (outputEvents.move[0] == 0)
                outputEvents.move[0] += amount.scalar;
            break;
        }
        case MOVE_TOGGLE:
        {
            outputEvents.Add(StandardGameControllerEvents::MOVE_TOGGLE);
            break;
        }
        case LOOK_UP:
        {
            outputEvents.Add(StandardGameControllerEvents::LOOK);
            if (outputEvents.look[1] == 0)
                outputEvents.look[1] += amount.scalar;
            break;
        }
        case LOOK_DOWN:
        {
            outputEvents.Add(StandardGameControllerEvents::LOOK);
            if (outputEvents.look[1] == 0)
                outputEvents.look[1] -= amount.scalar;
            break;
        }
        case LOOK_LEFT:
        {
            outputEvents.Add(StandardGameControllerEvents::LOOK);
            if (outputEvents.look[0] == 0)
                outputEvents.look[0] -= amount.scalar;
            break;
        }
        case LOOK_RIGHT:
        {
            outputEvents.Add(StandardGameControllerEvents::LOOK);
            if (outputEvents.look[0] == 0)
                outputEvents.look[0] += amount.scalar;
            break;
        }
        case LOOK_TOGGLE:
        {
            outputEvents.Add(StandardGameControllerEvents::LOOK_TOGGLE);
            break;
        }
    }
}

const StandardGameControllerInputBindings::InputActionInfo* StandardGameControllerInputBindings::GetInputActionInfo() const
{
    static const InputActionInfo actionInfo[] =
    {
        InputActionInfo("accept", ACCEPT),
        InputActionInfo("cancel", CANCEL),
        InputActionInfo("default", DEFAULT),
        InputActionInfo("change", CHANGE),

        InputActionInfo("settings", SETTINGS),

        InputActionInfo("system-settings", SYSTEM_SETTINGS),

        InputActionInfo("system-home", SYSTEM_HOME),

        InputActionInfo("shift-left", SHIFT_LEFT),
        InputActionInfo("shift-right", SHIFT_RIGHT),

        InputActionInfo("brake", BRAKE),
        InputActionInfo("gas", GAS),

        InputActionInfo("toggle-up", TOGGLE_UP),
        InputActionInfo("toggle-down", TOGGLE_DOWN),
        InputActionInfo("toggle-left", TOGGLE_LEFT),
        InputActionInfo("toggle-right", TOGGLE_RIGHT),

        InputActionInfo("move-up", MOVE_UP),
        InputActionInfo("move-down", MOVE_DOWN),
        InputActionInfo("move-left", MOVE_LEFT),
        InputActionInfo("move-right", MOVE_RIGHT),
        InputActionInfo("move-toggle", MOVE_TOGGLE),

        InputActionInfo("look-up", LOOK_UP),
        InputActionInfo("look-down", LOOK_DOWN),
        InputActionInfo("look-left", LOOK_LEFT),
        InputActionInfo("look-right", LOOK_RIGHT),
        InputActionInfo("look-toggle", LOOK_TOGGLE),

        InputActionInfo() //Null info
    };
    return actionInfo;
}
