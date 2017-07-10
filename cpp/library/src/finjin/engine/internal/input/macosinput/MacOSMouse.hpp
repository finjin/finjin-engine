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
#include "finjin/engine/GenericInputDevice.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    class MacOSMouse : public GenericInputDeviceImpl
    {
    public:
        using Super = GenericInputDeviceImpl;

        MacOSMouse(Allocator* allocator);

        void Reset();

        bool Create(size_t index);
        void Destroy();

        void Update(SimpleTimeDelta elapsedTime, bool isFirstUpdate);
        void ClearChanged();

        bool IsConnected() const;
        bool GetConnectionChanged() const;

        size_t GetButtonCount() const;
        InputButton* GetButton(size_t index);

        size_t GetAxisCount() const;
        InputAxis* GetAxis(size_t index);

        size_t GetPovCount() const;
        InputPov* GetPov(size_t index);

        size_t GetLocatorCount() const;
        InputLocator* GetLocator(size_t locatorIndex);

    private:
        InputDeviceState<InputButton, InputAxis, InputPov, MouseConstants::MAX_BUTTON_COUNT, MouseConstants::MAX_AXIS_COUNT> state;
    };

} }
