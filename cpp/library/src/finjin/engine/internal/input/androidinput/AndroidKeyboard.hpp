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
#include "finjin/common/StaticVector.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    class AndroidKeyboard : public AndroidInputDevice
    {
    public:
        AndroidKeyboard();

        void Reset();

        bool Create(android_app* androidApp);
        void Destroy();

        void ClearChanged();

        bool IsConnected() const;
        bool ConnectionChanged() const;
        bool IsNewConnection() const;

        size_t GetAxisCount() const;
        InputAxis* GetAxis(size_t axisIndex);

        size_t GetButtonCount() const;
        InputButton* GetButton(size_t buttonIndex);

        size_t GetPovCount() const;
        InputPov* GetPov(size_t povIndex);

        static void CreateKeyboards(StaticVector<AndroidKeyboard, KeyboardConstants::MAX_KEYBOARDS>& keyboards, android_app* androidApp);

        static void CheckConfigurationChanged(StaticVector<AndroidKeyboard, KeyboardConstants::MAX_KEYBOARDS>& keyboards, android_app* androidApp);
        static void RefreshKeyboards
            (
            StaticVector<AndroidKeyboard, KeyboardConstants::MAX_KEYBOARDS>& keyboards,
            StaticVector<AndroidKeyboard, KeyboardConstants::MAX_KEYBOARDS>& keyboardsBuffer,
            android_app* androidApp
            );

        static AndroidKeyboard* GetKeyboardByInstanceDescriptor(AndroidKeyboard* keyboards, size_t keyboardCount, const Utf8String& descriptor);
        static AndroidKeyboard* GetKeyboardByID(AndroidKeyboard* keyboards, size_t keyboardCount, int deviceID);

    private:
        InputDeviceState<InputButton, InputAxis, InputPov, KeyboardConstants::MAX_BUTTON_COUNT> state;

        bool isNewConnection;
    };

} }
