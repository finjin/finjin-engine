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
#include "VRGameControllerInputBindings.hpp"
#include "finjin/common/DebugLog.hpp"

using namespace Finjin::Engine;


//Implementation----------------------------------------------------------------
const Utf8String& VRGameControllerInputBindings::GetDefaultBindingsFileName()
{
    static const Utf8String value("vr-game-controller.cfg");
    return value;
}

VRGameControllerInputBindings::VRGameControllerInputBindings()
{
}

void VRGameControllerInputBindings::ProcessInputAction
    (
    VRGameControllerEvents& outputEvents,
    InputActionID inputActionID,
    InputDeviceComponent deviceComponent,
    InputTriggerFlag trigger,
    InputBindingValue& amount,
    SimpleTimeDelta elapsedTimeSeconds
    )
{
    //FINJIN_DEBUG_LOG_INFO("VR Game Controller: Action ID: %1%, amount %2%", inputActionID, amount);

    switch (inputActionID)
    {
        case ACCEPT:
        {
            outputEvents.Add(VRGameControllerEvents::ACCEPT);
            break;
        }
        case CANCEL:
        {
            outputEvents.Add(VRGameControllerEvents::CANCEL);
            break;
        }
        case SETTINGS:
        {
            outputEvents.Add(VRGameControllerEvents::SETTINGS);
            break;
        }
        case SYSTEM_HOME:
        {
            outputEvents.Add(VRGameControllerEvents::SYSTEM_HOME);
            break;
        }
        case GAS:
        {
            outputEvents.Add(VRGameControllerEvents::GAS);
            outputEvents.gas = amount.scalar;
            break;
        }
        case TOGGLE_UP:
        {
            outputEvents.Add(VRGameControllerEvents::TOGGLE);
            if (outputEvents.toggle[1] == 0)
                outputEvents.toggle[1] = amount.scalar;
            break;
        }
        case TOGGLE_DOWN:
        {
            outputEvents.Add(VRGameControllerEvents::TOGGLE);
            if (outputEvents.toggle[1] == 0)
                outputEvents.toggle[1] = -amount.scalar;
            break;
        }
        case TOGGLE_LEFT:
        {
            outputEvents.Add(VRGameControllerEvents::TOGGLE);
            if (outputEvents.toggle[0] == 0)
                outputEvents.toggle[0] = -amount.scalar;
            break;
        }
        case TOGGLE_RIGHT:
        {
            outputEvents.Add(VRGameControllerEvents::TOGGLE);
            if (outputEvents.toggle[0] == 0)
                outputEvents.toggle[0] = amount.scalar;
            break;
        }
        case MOVE_UP:
        {
            outputEvents.Add(VRGameControllerEvents::MOVE);
            if (outputEvents.move[1] == 0)
                outputEvents.move[1] += amount.scalar;
            break;
        }
        case MOVE_DOWN:
        {
            outputEvents.Add(VRGameControllerEvents::MOVE);
            if (outputEvents.move[1] == 0)
                outputEvents.move[1] -= amount.scalar;
            break;
        }
        case MOVE_LEFT:
        {
            outputEvents.Add(VRGameControllerEvents::MOVE);
            if (outputEvents.move[0] == 0)
                outputEvents.move[0] -= amount.scalar;
            break;
        }
        case MOVE_RIGHT:
        {
            outputEvents.Add(VRGameControllerEvents::MOVE);
            if (outputEvents.move[0] == 0)
                outputEvents.move[0] += amount.scalar;
            break;
        }
        case MOVE_TOGGLE:
        {
            outputEvents.Add(VRGameControllerEvents::MOVE_TOGGLE);
            break;
        }
        case LOOK_UP:
        {
            outputEvents.Add(VRGameControllerEvents::LOOK);
            if (outputEvents.look[1] == 0)
                outputEvents.look[1] += amount.scalar;
            break;
        }
        case LOOK_DOWN:
        {
            outputEvents.Add(VRGameControllerEvents::LOOK);
            if (outputEvents.look[1] == 0)
                outputEvents.look[1] -= amount.scalar;
            break;
        }
        case LOOK_LEFT:
        {
            outputEvents.Add(VRGameControllerEvents::LOOK);
            if (outputEvents.look[0] == 0)
                outputEvents.look[0] -= amount.scalar;
            break;
        }
        case LOOK_RIGHT:
        {
            outputEvents.Add(VRGameControllerEvents::LOOK);
            if (outputEvents.look[0] == 0)
                outputEvents.look[0] += amount.scalar;
            break;
        }
        case LOOK_TOGGLE:
        {
            outputEvents.Add(VRGameControllerEvents::LOOK_TOGGLE);
            break;
        }
        case LOCATOR:
        {
            outputEvents.Add(VRGameControllerEvents::LOCATOR);
            FINJIN_COPY_MEMORY(&outputEvents.locator, &amount.locator, sizeof(outputEvents.locator));
            break;
        }
    }
}

const VRGameControllerInputBindings::InputActionInfo* VRGameControllerInputBindings::GetInputActionInfo() const
{
    static const InputActionInfo actionInfo[] =
    {
        InputActionInfo("accept", ACCEPT),
        InputActionInfo("cancel", CANCEL),

        InputActionInfo("settings", SETTINGS),

        InputActionInfo("system-home", SYSTEM_HOME),

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

        InputActionInfo("locator", LOCATOR),

        InputActionInfo() //Null info
    };
    return actionInfo;
}
