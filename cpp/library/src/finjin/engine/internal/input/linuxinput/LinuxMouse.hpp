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
#include "finjin/engine/InputComponents.hpp"
#include "finjin/engine/InputSource.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    class LinuxInputContext;

    class LinuxMouse
    {
    public:
        LinuxMouse();

        void Reset();

        bool Create(LinuxInputContext* context, size_t index);
        void Destroy();

        void Update(SimpleTimeDelta elapsedTime, bool isFirstUpdate);
        void ClearChanged();

        bool IsConnected() const;
        bool GetConnectionChanged() const;

        const Utf8String& GetDisplayName() const;
        void SetDisplayName(const Utf8String& value);

        InputDeviceSemantic GetSemantic() const;
        void SetSemantic(InputDeviceSemantic value);

        const Utf8String& GetProductDescriptor() const;
        const Utf8String& GetInstanceDescriptor() const;

        size_t GetButtonCount() const;
        InputButton* GetButton(size_t index);

        size_t GetAxisCount() const;
        InputAxis* GetAxis(size_t index);

        size_t GetPovCount() const;
        InputPov* GetPov(size_t index);

        size_t GetLocatorCount() const;
        InputLocator* GetLocator(size_t locatorIndex);

    private:
        LinuxInputContext* context;

        InputDeviceSemantic semantic;

        Utf8String displayName;

        Utf8String instanceName;
        Utf8String productName;

        Utf8String instanceDescriptor;
        Utf8String productDescriptor;

        InputDeviceState<InputButton, InputAxis, InputPov, MouseConstants::MAX_BUTTON_COUNT, MouseConstants::MAX_AXIS_COUNT> state;
    };

} }
