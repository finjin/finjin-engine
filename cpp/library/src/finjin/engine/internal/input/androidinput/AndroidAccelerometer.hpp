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
#include <android/sensor.h>
#include <android/input.h>


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    class AndroidAccelerometer : public InputAccelerometer
    {
    public:
        AndroidAccelerometer();

        void Initialize(android_app* androidApp, int accelerometerAlooperID);
        void Destroy();

        bool IsInitialized() const;

        void Enable(bool value);

        void ProcessQueue();

    public:
        ASensorManager* androidSensorManager;
        const ASensor* androidAccelerometerSensor;
        ASensorEventQueue* androidSensorEventQueue;
    };

} }
