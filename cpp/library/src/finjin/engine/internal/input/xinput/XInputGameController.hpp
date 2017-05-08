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
#include "finjin/common/Utf8String.hpp"
#include "finjin/engine/InputComponents.hpp"
#include "finjin/engine/InputSource.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    class XInputGameController
    {
    public:
        XInputGameController();
        ~XInputGameController();

        void Reset();

        bool Create(size_t index);
        void Destroy();

        void Update(SimpleTimeDelta elapsedTime, bool isFirstUpdate);

        void ClearChanged();

        bool IsConnected() const;
        bool GetConnectionChanged() const;

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

        void AddHapticFeedback(const HapticFeedbackSettings* forces, size_t count);
        void StopHapticFeedback();

        static size_t CreateGameControllers(XInputGameController* gameControllers, size_t gameControllerCount);
        static size_t UpdateGameControllers(SimpleTimeDelta elapsedTime, XInputGameController* gameControllers, size_t gameControllerCount);

        static XInputGameController* GetGameControllerByInstanceDescriptor(XInputGameController* gameControllers, size_t gameControllerCount, const Utf8String& descriptor);
        static XInputGameController* GetGameControllerByIndex(XInputGameController* gameControllers, size_t gameControllerCount, size_t deviceIndex);

    private:
        void _AcceptUpdate(bool isFirstUpdate);

        void _SetForce();

    private:
        struct Impl;
        std::unique_ptr<Impl> impl;
    };

} }
