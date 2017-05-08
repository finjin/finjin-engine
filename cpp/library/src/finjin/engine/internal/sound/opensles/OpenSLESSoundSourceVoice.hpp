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
#include "finjin/common/Angle.hpp"
#include "finjin/common/Chrono.hpp"
#include "finjin/engine/SoundContextCommonSettings.hpp"
#include "finjin/engine/SoundFormat.hpp"
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    class OpenSLESSoundSourceVoice
    {
    public:
        SLObjectItf playerInterface;
        SLPlayItf playerPlayInterface;
        SLAndroidSimpleBufferQueueItf bufferQueueInterface;
        SLEffectSendItf effectSendInterface;
        SLVolumeItf volumeInterface;
        SLPlaybackRateItf playbackRateInterface;

        OpenSLESSoundSourceVoice()
        {
            this->playerInterface = nullptr;
            this->playerPlayInterface = nullptr;
            this->bufferQueueInterface = nullptr;
            this->effectSendInterface = nullptr;
            this->volumeInterface = nullptr;
            this->playbackRateInterface = nullptr;
        }

        void Stop()
        {
            (*this->playerPlayInterface)->SetPlayState(this->playerPlayInterface, SL_PLAYSTATE_STOPPED);
            (*this->bufferQueueInterface)->Clear(this->bufferQueueInterface);
        }

        bool Restore()
        {
            SLObjectItf obj = this->playerInterface;

            SLresult result = -1;

            SLuint32 state;
            SLint32 priority;
            SLboolean preemptable;

            result = (*obj)->GetState(obj, &state);
            if (state == SL_OBJECT_STATE_SUSPENDED)
            {
                //Resuming state synchronously
                result = (*obj)->Resume(obj, SL_BOOLEAN_FALSE);
                while (result == SL_RESULT_RESOURCE_ERROR)
                {
                    //Not enough resources. Increasing object priority.
                    //result = (*obj)->GetPriority(obj, &priority, &preemptable);
                    result = (*obj)->SetPriority(obj, INT_MAX, SL_BOOLEAN_FALSE);

                    //Try again
                    result = (*obj)->Resume(obj, SL_BOOLEAN_FALSE);
                }
            }
            else if (result == SL_OBJECT_STATE_UNREALIZED)
            {
                //Realizing state synchronously
                result = (*obj)->Realize(obj, SL_BOOLEAN_FALSE);
                while (result = SL_RESULT_RESOURCE_ERROR)
                {
                    //Not enough resources. Increasing object priority.
                    //result = (*obj)->GetPriority(obj, &priority, &preemptable);
                    result = (*obj)->SetPriority(obj, INT_MAX, SL_BOOLEAN_FALSE);

                    //Try again
                    result = (*obj)->Realize(obj, SL_BOOLEAN_FALSE);
                }
            }

            return result == SL_RESULT_SUCCESS;
        }

    };

} }
