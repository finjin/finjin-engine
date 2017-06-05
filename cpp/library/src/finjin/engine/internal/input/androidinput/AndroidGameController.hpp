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
#include "AndroidInputDevice.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    struct AndroidGameControllerConstants
    {
        enum { MAX_NATIVE_GAME_CONTROLLERS = 16 }; //Android has no inherent limit but we impose one
    };

    class AndroidGameController : public AndroidInputDevice
    {
    public:
        using Super = AndroidInputDevice;

        AndroidGameController();

        void Reset();

        bool Create(android_app* androidApp);
        void Destroy();

        void ClearChanged();

        bool IsConnected() const;
        bool ConnectionChanged() const;
        bool IsNewConnection() const;

        int GetControllerNumber() const;

        size_t GetAxisCount() const;
        InputAxis* GetAxis(size_t axisIndex);

        size_t GetButtonCount() const;
        InputButton* GetButton(size_t buttonIndex);

        size_t GetPovCount() const;
        InputPov* GetPov(size_t povIndex);

        static void CreateGameControllers(StaticVector<AndroidGameController, AndroidGameControllerConstants::MAX_NATIVE_GAME_CONTROLLERS>& gameControllers, android_app* androidApp);

        static void CheckConfigurationChanged(StaticVector<AndroidGameController, AndroidGameControllerConstants::MAX_NATIVE_GAME_CONTROLLERS>& gameControllers, android_app* androidApp);
        static void RefreshGameControllers
            (
            StaticVector<AndroidGameController, AndroidGameControllerConstants::MAX_NATIVE_GAME_CONTROLLERS>& gameControllers,
            StaticVector<AndroidGameController, AndroidGameControllerConstants::MAX_NATIVE_GAME_CONTROLLERS>& gameControllersBuffer,
            android_app* androidApp
            );

        static AndroidGameController* GetGameControllerByInstanceDescriptor(AndroidGameController* gameControllers, size_t gameControllerCount, const Utf8String& descriptor);
        static AndroidGameController* GetGameControllerByID(AndroidGameController* gameControllers, size_t gameControllerCount, int deviceID);

    private:
        InputDeviceState<InputButton, InputAxis, InputPov, GameControllerConstants::MAX_BUTTON_COUNT, GameControllerConstants::MAX_AXIS_COUNT> state;

        StaticVector<InputButton*, GameControllerConstants::MAX_BUTTON_COUNT> connectedButtons;
        StaticVector<InputAxis*, GameControllerConstants::MAX_AXIS_COUNT> connectedAxes;

        int controllerNumber;

        bool isNewConnection;
    };

} }
