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
#include "finjin/common/Path.hpp"
#include "LinuxInputDevice.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    class LinuxGameController : public LinuxInputDevice
    {
    public:
        using Super = LinuxInputDevice;
        
        LinuxGameController(Allocator* allocator = nullptr);

        void SetAllocator(Allocator* allocator);
        
        void Reset();

        void Create(size_t index, int fd, const Path& devicePath);
        void Destroy();

        void Update(SimpleTimeDelta elapsedTime, bool isFirstUpdate);
        void ClearChanged();

        bool IsConnected() const;
        bool GetConnectionChanged() const;
        bool IsNewConnection() const;

        size_t GetIndex() const;

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

        InputDeviceState<InputButton, InputAxis, InputPov, GameControllerConstants::MAX_BUTTON_COUNT, GameControllerConstants::MAX_AXIS_COUNT> state;

        bool isNewConnection;
    };

} }
