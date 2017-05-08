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
#include "finjin/common/Path.hpp"
#include "finjin/common/StaticVector.hpp"
#include "finjin/common/Utf8String.hpp"
#include "finjin/engine/InputComponents.hpp"
#include "finjin/engine/InputSource.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    class LinuxGameController
    {
    public:
        LinuxGameController();

        void Reset();

        void Create(size_t index, int fd, const Path& devicePath);
        void Destroy();

        void Update(SimpleTimeDelta elapsedTime, bool isFirstUpdate);
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

        size_t GetLocatorCount() const;
        InputLocator* GetLocator(size_t locatorIndex);

        static void CreateGameControllers(StaticVector<LinuxGameController, GameControllerConstants::MAX_GAME_CONTROLLERS>& gameControllers);
        static void UpdateGameControllers
            (
            StaticVector<LinuxGameController, GameControllerConstants::MAX_GAME_CONTROLLERS>& gameControllers,
            SimpleTimeDelta elapsedTime,
            bool isFirstUpdate
            );
        static bool CheckForNewGameControllers(StaticVector<LinuxGameController, GameControllerConstants::MAX_GAME_CONTROLLERS>& gameControllers);

    public:
        int fd;
        Path devicePath;

        size_t index;

        InputDeviceSemantic semantic;

        Utf8String displayName;

        Utf8String instanceName;
        Utf8String productName;

        Utf8String instanceDescriptor;
        Utf8String productDescriptor;

        InputDeviceState<InputButton, InputAxis, InputPov, GameControllerConstants::MAX_BUTTON_COUNT, GameControllerConstants::MAX_AXIS_COUNT> state;

        bool isNewConnection;
    };

} }
