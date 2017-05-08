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


//Includes----------------------------------------------------------------------
#include "FinjinPrecompiled.hpp"
#include "AndroidAccelerometer.hpp"
#include "finjin/common/Convert.hpp"
#include "finjin/common/DebugLog.hpp"

#define ACCELEROMETER_UPDATES_PER_SECOND 60 //Desired updates per second for the accelerometer

using namespace Finjin::Engine;


//Implementation----------------------------------------------------------------
AndroidAccelerometer::AndroidAccelerometer()
{
    this->androidSensorManager = nullptr;
    this->androidAccelerometerSensor = nullptr;
    this->androidSensorEventQueue = nullptr;

    InputAccelerometer::Enable(false);
}

void AndroidAccelerometer::Initialize(android_app* androidApp, int accelerometerAlooperID)
{
    this->androidSensorManager = ASensorManager_getInstance();
    this->androidAccelerometerSensor = this->androidSensorManager == nullptr ? nullptr : ASensorManager_getDefaultSensor(this->androidSensorManager, ASENSOR_TYPE_ACCELEROMETER);
    if (this->androidAccelerometerSensor != nullptr)
    {
        this->androidSensorEventQueue = ASensorManager_createEventQueue(this->androidSensorManager, androidApp->looper, accelerometerAlooperID, 0, 0);

        SetDisplayName("Default accelerometer");

        //Leave sensor in disabled state
    }
}

void AndroidAccelerometer::Destroy()
{
    Reset();

    this->androidSensorManager = nullptr;
    this->androidAccelerometerSensor = nullptr;
    this->androidSensorEventQueue = nullptr;

    InputAccelerometer::Enable(false);
}

bool AndroidAccelerometer::IsInitialized() const
{
    return this->androidAccelerometerSensor != nullptr;
}

void AndroidAccelerometer::Enable(bool value)
{
    if (value)
    {
        if (this->androidAccelerometerSensor != nullptr && this->androidSensorEventQueue != nullptr)
        {
            ASensorEventQueue_enableSensor(this->androidSensorEventQueue, this->androidAccelerometerSensor);

            int32_t eventRate = std::max(ASensor_getMinDelay(this->androidAccelerometerSensor), (1000 / ACCELEROMETER_UPDATES_PER_SECOND) * 1000);
            ASensorEventQueue_setEventRate(this->androidSensorEventQueue, this->androidAccelerometerSensor, eventRate);
        }
    }
    else
    {
        if (this->androidAccelerometerSensor != nullptr && this->androidSensorEventQueue != nullptr)
            ASensorEventQueue_disableSensor(this->androidSensorEventQueue, this->androidAccelerometerSensor);
    }

    InputAccelerometer::Enable(value);
}

void AndroidAccelerometer::ProcessQueue()
{
    ASensorEvent event;
    while (ASensorEventQueue_getEvents(this->androidSensorEventQueue, &event, 1) > 0)
    {
        Update(event.acceleration.x, event.acceleration.y, event.acceleration.z);
        //FINJIN_DEBUG_LOG_INFO("accelerometer: x=%f y=%f z=%f", event.acceleration.x, event.acceleration.y, event.acceleration.z);
    }
}
