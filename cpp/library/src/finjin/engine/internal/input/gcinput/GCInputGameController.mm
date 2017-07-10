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
#include "GCInputGameController.hpp"
#include "InputComponents.hpp"
#include "finjin/common/Convert.hpp"
#import <GameController/GameController.h>

using namespace Finjin::Engine;


//Local types-------------------------------------------------------------------
enum
{
    GCINPUT_BUTTON_A,
    GCINPUT_BUTTON_B,
    GCINPUT_BUTTON_X,
    GCINPUT_BUTTON_Y,
    GCINPUT_BUTTON_LEFT_SHOULDER,
    GCINPUT_BUTTON_RIGHT_SHOULDER,
    GCINPUT_BUTTON_MENU,

    GCINPUT_BUTTON_COUNT
};

enum
{
    GCINPUT_AXIS_DPAD_X,
    GCINPUT_AXIS_DPAD_Y,
    GCINPUT_AXIS_LEFT_THUMBSTICK_X,
    GCINPUT_AXIS_LEFT_THUMBSTICK_Y,
    GCINPUT_AXIS_RIGHT_THUMBSTICK_X,
    GCINPUT_AXIS_RIGHT_THUMBSTICK_Y,
    GCINPUT_AXIS_LEFT_TRIGGER,
    GCINPUT_AXIS_RIGHT_TRIGGER,

    GCINPUT_AXIS_EXTENDED_COUNT
};
enum {GC_INPUT_AXIS_STANDARD_COUNT = 2};


//Local functions---------------------------------------------------------------
template <size_t count>
void SetGCInputButton(StaticVector<InputButton, count>& buttons, size_t index, const char* displayName, InputComponentSemantic semantic, InputButtonProcessing processing = InputButtonProcessing::NONE)
{
    buttons[index]
        .SetIndex(index)
        .SetDisplayName(displayName)
        .SetSemantic(semantic)
        .SetProcessing(processing);
}

template <size_t count>
void SetGCInputAxis(StaticVector<InputAxis, count>& axes, size_t index, float minValue, float maxValue, const char* displayName, InputComponentSemantic semantic, InputAxisProcessing processing = InputAxisProcessing::NONE)
{
    axes[index]
        .SetIndex(index)
        .SetMinMax(minValue, maxValue)
        .SetDisplayName(displayName)
        .SetSemantic(semantic)
        .SetProcessing(processing);
}

static bool ContainsGCController(NSArray<GCController*>* gcControllers, void* gcController)
{
    for (size_t gameControllerIndex = 0; gameControllerIndex < gcControllers.count; gameControllerIndex++)
    {
        if (gcControllers[gameControllerIndex] == gcController)
            return true;
    }

    return false;
}

static bool ContainsGCController(StaticVector<GCInputGameController, GameControllerConstants::MAX_GAME_CONTROLLERS>& gameControllers, GCController* gcController)
{
    for (auto& gameController : gameControllers)
    {
        if (gcController == gameController.gcController)
            return true;
    }

    return false;
}

static void Create(GCInputGameController& gameController, size_t index, GCController* gcController)
{
    gameController.gcController = (__bridge void*)gcController; //Keep a weak reference

    gameController.index = index;

    Utf8String vendorName = gcController.vendorName.UTF8String;

    gameController.instanceName = vendorName;
    gameController.instanceName += Convert::ToString(index + 1);

    gameController.displayName = vendorName;
    gameController.productName = vendorName;

    gameController.instanceDescriptor = "gcinput-";
    gameController.instanceDescriptor += Convert::ToString(index);

    gameController.state.isConnected = true;

    auto gcExtendedGamepad = gcController.extendedGamepad;
    auto gcGamepad = gcController.gamepad;
#if TARGET_OS_TV
    auto gcMicroGamepad = gcController.microGamepad;
#endif
    auto gcMotion = gcController.motion;

    if (gcExtendedGamepad != nullptr)
    {
        gameController.productDescriptor = "extended-gamepad";

        //Buttons
        gameController.state.buttons.resize(GCINPUT_BUTTON_COUNT);
        SetGCInputButton(gameController.state.buttons, GCINPUT_BUTTON_A, "A", InputComponentSemantic::ACCEPT);
        SetGCInputButton(gameController.state.buttons, GCINPUT_BUTTON_B, "B", InputComponentSemantic::CANCEL);
        SetGCInputButton(gameController.state.buttons, GCINPUT_BUTTON_X, "X", InputComponentSemantic::HANDBRAKE);
        SetGCInputButton(gameController.state.buttons, GCINPUT_BUTTON_Y, "Y", InputComponentSemantic::CHANGE_VIEW);
        SetGCInputButton(gameController.state.buttons, GCINPUT_BUTTON_LEFT_SHOULDER, "Left shoulder", InputComponentSemantic::SHIFT_LEFT);
        SetGCInputButton(gameController.state.buttons, GCINPUT_BUTTON_RIGHT_SHOULDER, "Right shoulder", InputComponentSemantic::SHIFT_RIGHT);
        SetGCInputButton(gameController.state.buttons, GCINPUT_BUTTON_MENU, "Menu", InputComponentSemantic::SETTINGS, InputButtonProcessing::REST_ON_CLEAR_CHANGES);

        //Axes
        gameController.state.axes.resize(GCINPUT_AXIS_EXTENDED_COUNT);
        SetGCInputAxis(gameController.state.axes, GCINPUT_AXIS_DPAD_X, -1, 1, "D-pad X", InputComponentSemantic::TOGGLE_LEFT_RIGHT, InputAxisProcessing::ROUND_UPDATE_TO_ONE);
        SetGCInputAxis(gameController.state.axes, GCINPUT_AXIS_DPAD_Y, -1, 1, "D-pad Y", InputComponentSemantic::TOGGLE_UP_DOWN, InputAxisProcessing::ROUND_UPDATE_TO_ONE);
        SetGCInputAxis(gameController.state.axes, GCINPUT_AXIS_LEFT_THUMBSTICK_X, -1, 1, "Left thumbstick X", InputComponentSemantic::MOVE_LEFT_RIGHT);
        SetGCInputAxis(gameController.state.axes, GCINPUT_AXIS_LEFT_THUMBSTICK_Y, -1, 1, "Left thumbstick Y", InputComponentSemantic::MOVE_UP_DOWN);
        SetGCInputAxis(gameController.state.axes, GCINPUT_AXIS_RIGHT_THUMBSTICK_X, -1, 1, "Right thumbstick X", InputComponentSemantic::LOOK_LEFT_RIGHT);
        SetGCInputAxis(gameController.state.axes, GCINPUT_AXIS_RIGHT_THUMBSTICK_Y, -1, 1, "Right thumbstick Y", InputComponentSemantic::LOOK_UP_DOWN);
        SetGCInputAxis(gameController.state.axes, GCINPUT_AXIS_LEFT_TRIGGER, 0, 1, "Left trigger", InputComponentSemantic::BRAKE);
        SetGCInputAxis(gameController.state.axes, GCINPUT_AXIS_RIGHT_TRIGGER, 0, 1, "Right trigger", InputComponentSemantic::GAS);
    }
    else if (gcGamepad != nullptr)
    {
        gameController.productDescriptor = "gamepad";

        //Buttons
        gameController.state.buttons.resize(GCINPUT_BUTTON_COUNT);
        SetGCInputButton(gameController.state.buttons, GCINPUT_BUTTON_A, "A", InputComponentSemantic::ACCEPT);
        SetGCInputButton(gameController.state.buttons, GCINPUT_BUTTON_B, "B", InputComponentSemantic::CANCEL);
        SetGCInputButton(gameController.state.buttons, GCINPUT_BUTTON_X, "X", InputComponentSemantic::HANDBRAKE);
        SetGCInputButton(gameController.state.buttons, GCINPUT_BUTTON_Y, "Y", InputComponentSemantic::CHANGE_VIEW);
        SetGCInputButton(gameController.state.buttons, GCINPUT_BUTTON_LEFT_SHOULDER, "Left shoulder", InputComponentSemantic::SHIFT_LEFT);
        SetGCInputButton(gameController.state.buttons, GCINPUT_BUTTON_RIGHT_SHOULDER, "Right shoulder", InputComponentSemantic::SHIFT_RIGHT);
        SetGCInputButton(gameController.state.buttons, GCINPUT_BUTTON_MENU, "Menu", InputComponentSemantic::SETTINGS, InputButtonProcessing::REST_ON_CLEAR_CHANGES);

        //Axes
        gameController.state.axes.resize(GC_INPUT_AXIS_STANDARD_COUNT);
        SetGCInputAxis(gameController.state.axes, GCINPUT_AXIS_DPAD_X, -1, 1, "D-pad X", InputComponentSemantic::MOVE_LEFT_RIGHT, InputAxisProcessing::ROUND_UPDATE_TO_ONE);
        SetGCInputAxis(gameController.state.axes, GCINPUT_AXIS_DPAD_Y, -1, 1, "D-pad Y", InputComponentSemantic::MOVE_UP_DOWN, InputAxisProcessing::ROUND_UPDATE_TO_ONE);
    }
#if TARGET_OS_TV
    else if (gcMicroGamepad != nullptr)
    {
        gameController.state.buttons.resize(GCINPUT_BUTTON_COUNT);
        SetGCInputButton(gameController.state.buttons, GCINPUT_BUTTON_A, "Accept", InputComponentSemantic::ACCEPT);
        SetGCInputButton(gameController.state.buttons, GCINPUT_BUTTON_B, "Cancel", InputComponentSemantic::CANCEL); //The microgamepad profile calls this "X"

        //Axes
        gameController.state.axes.resize(GC_INPUT_AXIS_STANDARD_COUNT);
        gcMicroGamepad.reportsAbsoluteDpadValues = YES;
        if (gcMicroGamepad.reportsAbsoluteDpadValues)
        {
            //In this mode the user can press down and values offsets are registered
            gameController.state.axes[0].SetIndex(0).SetSemantic(InputComponentSemantic::MOVE_LEFT_RIGHT).SetProcessing(InputAxisProcessing::IS_ABSOLUTE).SetDisplayName("Touch Pad X");
            gameController.state.axes[1].SetIndex(1).SetSemantic(InputComponentSemantic::MOVE_UP_DOWN).SetProcessing(InputAxisProcessing::IS_ABSOLUTE).SetDisplayName("Touch Pad Y");
        }
        else
        {
            //In this mode the user must move from the position where they initially touch
            SetGCInputAxis(gameController.state.axes, GCINPUT_AXIS_DPAD_X, -1, 1, "Touch Pad X", InputComponentSemantic::MOVE_LEFT_RIGHT, InputAxisProcessing::ROUND_UPDATE_TO_ONE);
            SetGCInputAxis(gameController.state.axes, GCINPUT_AXIS_DPAD_Y, -1, 1, "Touch Pad Y", InputComponentSemantic::MOVE_UP_DOWN, InputAxisProcessing::ROUND_UPDATE_TO_ONE);
        }
    }
#endif

    if (gcMotion != nullptr)
    {
        gameController.accelerometer.Enable(true);
        gameController.accelerometer.SetDisplayName("Accelerometer");
    }

    gcController.controllerPausedHandler = ^(GCController* gcPausedController)
    {
        gameController.state.buttons[GCINPUT_BUTTON_MENU].Update(true, false);
    };
}

static void Update(GCInputGameController& gameController, SimpleTimeDelta elapsedTime, bool isFirstUpdate, GCController* gcController)
{
    auto gcExtendedGamepad = gcController.extendedGamepad;
    auto gcGamepad = gcController.gamepad;
#if TARGET_OS_TV
    auto gcMicroGamepad = gcController.microGamepad;
#endif
    auto gcMotion = gcController.motion;

    if (gcExtendedGamepad != nullptr)
    {
        //Buttons
        gameController.state.buttons[GCINPUT_BUTTON_A].Update(gcExtendedGamepad.buttonA.isPressed, isFirstUpdate);
        gameController.state.buttons[GCINPUT_BUTTON_B].Update(gcExtendedGamepad.buttonB.isPressed, isFirstUpdate);
        gameController.state.buttons[GCINPUT_BUTTON_X].Update(gcExtendedGamepad.buttonX.isPressed, isFirstUpdate);
        gameController.state.buttons[GCINPUT_BUTTON_Y].Update(gcExtendedGamepad.buttonY.isPressed, isFirstUpdate);
        gameController.state.buttons[GCINPUT_BUTTON_LEFT_SHOULDER].Update(gcExtendedGamepad.leftShoulder.isPressed, isFirstUpdate);
        gameController.state.buttons[GCINPUT_BUTTON_RIGHT_SHOULDER].Update(gcExtendedGamepad.rightShoulder.isPressed, isFirstUpdate);

        //Axes
        gameController.state.axes[GCINPUT_AXIS_DPAD_X].Update(gcExtendedGamepad.dpad.xAxis.value, isFirstUpdate);
        gameController.state.axes[GCINPUT_AXIS_DPAD_Y].Update(gcExtendedGamepad.dpad.yAxis.value, isFirstUpdate);
        gameController.state.axes[GCINPUT_AXIS_LEFT_THUMBSTICK_X].Update(gcExtendedGamepad.leftThumbstick.xAxis.value, isFirstUpdate);
        gameController.state.axes[GCINPUT_AXIS_LEFT_THUMBSTICK_Y].Update(gcExtendedGamepad.leftThumbstick.yAxis.value, isFirstUpdate);
        gameController.state.axes[GCINPUT_AXIS_RIGHT_THUMBSTICK_X].Update(gcExtendedGamepad.rightThumbstick.xAxis.value, isFirstUpdate);
        gameController.state.axes[GCINPUT_AXIS_RIGHT_THUMBSTICK_Y].Update(gcExtendedGamepad.rightThumbstick.yAxis.value, isFirstUpdate);
        gameController.state.axes[GCINPUT_AXIS_LEFT_TRIGGER].Update(gcExtendedGamepad.leftTrigger.value, isFirstUpdate);
        gameController.state.axes[GCINPUT_AXIS_RIGHT_TRIGGER].Update(gcExtendedGamepad.rightTrigger.value, isFirstUpdate);
    }
    else if (gcGamepad != nullptr)
    {
        //Buttons
        gameController.state.buttons[GCINPUT_BUTTON_A].Update(gcExtendedGamepad.buttonA.isPressed, isFirstUpdate);
        gameController.state.buttons[GCINPUT_BUTTON_B].Update(gcExtendedGamepad.buttonB.isPressed, isFirstUpdate);
        gameController.state.buttons[GCINPUT_BUTTON_X].Update(gcExtendedGamepad.buttonX.isPressed, isFirstUpdate);
        gameController.state.buttons[GCINPUT_BUTTON_Y].Update(gcExtendedGamepad.buttonY.isPressed, isFirstUpdate);
        gameController.state.buttons[GCINPUT_BUTTON_LEFT_SHOULDER].Update(gcExtendedGamepad.leftShoulder.isPressed, isFirstUpdate);
        gameController.state.buttons[GCINPUT_BUTTON_RIGHT_SHOULDER].Update(gcExtendedGamepad.rightShoulder.isPressed, isFirstUpdate);

        //Axes
        gameController.state.axes[GCINPUT_AXIS_DPAD_X].Update(gcExtendedGamepad.dpad.xAxis.value, isFirstUpdate);
        gameController.state.axes[GCINPUT_AXIS_DPAD_Y].Update(gcExtendedGamepad.dpad.yAxis.value, isFirstUpdate);
    }
#if TARGET_OS_TV
    else if (gcMicroGamepad != nullptr)
    {
        //Butons
        gameController.state.buttons[GCINPUT_BUTTON_A].Update(gcMicroGamepad.buttonA.isPressed, isFirstUpdate);
        gameController.state.buttons[GCINPUT_BUTTON_B].Update(gcMicroGamepad.buttonX.isPressed, isFirstUpdate);

        //Axes
        gameController.state.axes[GCINPUT_AXIS_DPAD_X].Update(gcMicroGamepad.dpad.xAxis.value, isFirstUpdate);
        gameController.state.axes[GCINPUT_AXIS_DPAD_Y].Update(gcMicroGamepad.dpad.yAxis.value, isFirstUpdate);
    }
#endif

    if (gcMotion != nullptr)
    {
        auto gravity = gcMotion.gravity;
        gameController.accelerometer.Update(gravity.x, gravity.y, gravity.z, isFirstUpdate);
    }
}


//Implementation----------------------------------------------------------------

//GCInputGameController
GCInputGameController::GCInputGameController(Allocator* allocator) : Super(allocator)
{
    this->index = 0;
    this->isNewConnection = false;
}

void GCInputGameController::Reset()
{
    Super::Reset();

    this->index = 0;
    this->state.Reset();
    this->isNewConnection = false;
}

void GCInputGameController::Destroy()
{
    Reset();
}

void GCInputGameController::ClearChanged()
{
    this->state.ClearChanged();
    this->accelerometer.ClearChanged();

    this->isNewConnection = false;
}

bool GCInputGameController::IsConnected() const
{
    return this->state.isConnected;
}

bool GCInputGameController::GetConnectionChanged() const
{
    return this->state.connectionChanged;
}

bool GCInputGameController::IsNewConnection() const
{
    return this->isNewConnection;
}

size_t GCInputGameController::GetIndex() const
{
    return this->index;
}

size_t GCInputGameController::GetAxisCount() const
{
    return this->state.axes.size();
}

InputAxis* GCInputGameController::GetAxis(size_t axisIndex)
{
    return &this->state.axes[axisIndex];
}

size_t GCInputGameController::GetButtonCount() const
{
    return this->state.buttons.size();
}

InputButton* GCInputGameController::GetButton(size_t buttonIndex)
{
    return &this->state.buttons[buttonIndex];
}

size_t GCInputGameController::GetPovCount() const
{
    return 0;
}

InputPov* GCInputGameController::GetPov(size_t povIndex)
{
    return nullptr;
}

size_t GCInputGameController::GetAccelerometerCount() const
{
    return this->accelerometer.IsEnabled() ? 1 : 0;
}

InputAccelerometer* GCInputGameController::GetAccelerometer(size_t index)
{
    return this->accelerometer.IsEnabled() ? &this->accelerometer : nullptr;
}

size_t GCInputGameController::GetLocatorCount() const
{
    return 0;
}

InputLocator* GCInputGameController::GetLocator(size_t locatorIndex)
{
    return nullptr;
}

void GCInputGameController::AddHapticFeedback(const HapticFeedback* forces, size_t count)
{
}

void GCInputGameController::StopHapticFeedback()
{
}

void GCInputGameController::CreateGameControllers(StaticVector<GCInputGameController, GameControllerConstants::MAX_GAME_CONTROLLERS>& gameControllers)
{
    auto gcControllers = [GCController controllers];

    auto gameControllerCount = std::min(gameControllers.max_size(), (size_t)gcControllers.count);
    gameControllers.resize(gameControllerCount);

    for (size_t gameControllerIndex = 0; gameControllerIndex < gameControllerCount; gameControllerIndex++)
        Create(gameControllers[gameControllerIndex], gameControllerIndex, gcControllers[gameControllerIndex]);
}

void GCInputGameController::UpdateGameControllers(StaticVector<GCInputGameController, GameControllerConstants::MAX_GAME_CONTROLLERS>& gameControllers, bool isFirstUpdate, SimpleTimeDelta elapsedTime)
{
    auto gcControllers = [GCController controllers];

    //Update existing controllers
    for (size_t gameControllerIndex = 0; gameControllerIndex < gameControllers.size(); gameControllerIndex++)
    {
        auto& gameController = gameControllers[gameControllerIndex];

        if (gameController.state.isConnected)
        {
            //Connected, so see if it was disconnected
            if (!ContainsGCController(gcControllers, gameController.gcController))
            {
                //Disconnected
                gameController.state.isConnected = false;
                gameController.state.connectionChanged = true;

                for (auto& button : gameController.state.buttons)
                    button.Update(false, isFirstUpdate);

                for (auto& axis : gameController.state.axes)
                    axis.Update(0, isFirstUpdate);
            }
            else
            {
                //Still connected
                Update(gameController, elapsedTime, isFirstUpdate, gcControllers[gameControllerIndex]);
            }
        }
        else
        {
            //Disconnected, so see if it was connected
            if (ContainsGCController(gcControllers, gameController.gcController))
            {
                //Reconnected
                gameController.state.isConnected = true;
                gameController.state.connectionChanged = true;

                Update(gameController, 0, true, gcControllers[gameControllerIndex]);
            }
        }
    }

    //Figure out which controllers were newly connected and add those
    if (!gameControllers.full())
    {
        for (size_t gameControllerIndex = 0; gameControllerIndex < gcControllers.count; gameControllerIndex++)
        {
            if (!ContainsGCController(gameControllers, gcControllers[gameControllerIndex]))
            {
                auto newGameControllerIndex = gameControllers.size();
                gameControllers.push_back();

                Create(gameControllers[newGameControllerIndex], newGameControllerIndex, gcControllers[gameControllerIndex]);

                Update(gameControllers[newGameControllerIndex], 0, true, gcControllers[gameControllerIndex]);

                gameControllers[newGameControllerIndex].isNewConnection = true;
            }
        }
    }
}
