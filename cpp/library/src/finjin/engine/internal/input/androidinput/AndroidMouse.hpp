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

    class AndroidMouse : public AndroidInputDevice
    {
    public:
        using Super = AndroidInputDevice;

        AndroidMouse();

        void Reset();

        bool Create(android_app* androidApp);
        void Destroy();

        void ClearChanged();

        bool IsConnected() const;
        bool ConnectionChanged() const;
        bool IsNewConnection() const;

        size_t GetAxisCount() const;
        InputAxis* GetAxis(size_t axisIndex);
        InputAxis* GetAxisByCode(int code);

        size_t GetButtonCount() const;
        InputButton* GetButton(size_t buttonIndex);

        size_t GetPovCount() const;
        InputPov* GetPov(size_t povIndex);

        bool GetCurrentPosition(float& x, float& y) const;

        static void CreateMice(StaticVector<AndroidMouse, MouseConstants::MAX_MICE>& mice, android_app* androidApp);

        static void CheckConfigurationChanged(StaticVector<AndroidMouse, MouseConstants::MAX_MICE>& mice, android_app* androidApp);
        static void RefreshMice
            (
            StaticVector<AndroidMouse, MouseConstants::MAX_MICE>& mice,
            StaticVector<AndroidMouse, MouseConstants::MAX_MICE>& miceBuffer,
            android_app* androidApp
            );

        static AndroidMouse* GetMouseByInstanceDescriptor(AndroidMouse* mice, size_t mouseCount, const Utf8String& descriptor);
        static AndroidMouse* GetMouseByID(AndroidMouse* mice, size_t mouseCount, int deviceID);

    private:
        InputDeviceState<InputButton, InputAxis, InputPov, MouseConstants::MAX_BUTTON_COUNT, MouseConstants::MAX_AXIS_COUNT> state;

        StaticVector<InputButton*, MouseConstants::MAX_BUTTON_COUNT> connectedButtons;
        StaticVector<InputAxis*, MouseConstants::MAX_AXIS_COUNT> connectedAxes;

        bool isNewConnection;
    };

} }
