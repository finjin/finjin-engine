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
#include "finjin/common/Chrono.hpp"
#include "finjin/common/StaticVector.hpp"
#include "finjin/common/Utf8String.hpp"
#include "finjin/engine/InputComponents.hpp"
#include "finjin/engine/InputSource.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    class GCInputGameController
    {
    public:
        enum { MAX_GAME_CONTROLLERS = 5 }; //4 game controllers + 1 remote

    public:
        GCInputGameController();

        void Reset();

        void Destroy();

        void ClearChanged();

        bool IsConnected() const;
        bool GetConnectionChanged() const;
        bool IsNewConnection() const;

        size_t GetIndex() const;

        const Utf8String& GetDisplayName() const;
        void SetDisplayName(const Utf8String& value);

        InputDeviceSemantic GetSemantic() const;
        void SetSemantic(InputDeviceSemantic value);

        const Utf8String& GetProductDescriptor() const;
        const Utf8String& GetInstanceDescriptor() const;

        size_t GetAxisCount() const;
        InputAxis* GetAxis(size_t axisIndex);

        size_t GetButtonCount() const;
        InputButton* GetButton(size_t buttonIndex);

        size_t GetPovCount() const;
        InputPov* GetPov(size_t povIndex);

        size_t GetAccelerometerCount() const;
        InputAccelerometer* GetAccelerometer(size_t index);

        size_t GetLocatorCount() const;
        InputLocator* GetLocator(size_t locatorIndex);

        void AddHapticFeedback(const HapticFeedbackSettings* forces, size_t count);
        void StopHapticFeedback();

        static void CreateGameControllers(StaticVector<GCInputGameController, GameControllerConstants::MAX_GAME_CONTROLLERS>& gameControllers);
        static void UpdateGameControllers(StaticVector<GCInputGameController, GameControllerConstants::MAX_GAME_CONTROLLERS>& gameControllers, bool isFirstUpdate, SimpleTimeDelta elapsedTime);

    public:
        void* gcController; //Weak reference to the GCController instance

        int id;
        size_t index;

        InputDeviceSemantic semantic;

        Utf8String displayName;

        Utf8String instanceName;
        Utf8String productName;

        Utf8String instanceDescriptor;
        Utf8String productDescriptor;

        InputDeviceState<InputButton, InputAxis, InputPov, GameControllerConstants::MAX_BUTTON_COUNT, GameControllerConstants::MAX_AXIS_COUNT> state;
        InputAccelerometer accelerometer;

        bool isNewConnection;
    };

} }
