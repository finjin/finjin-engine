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
#include "finjin/common/Error.hpp"
#include "OpenSLESContext.hpp"
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    class OpenSLESUtilities
    {
    public:
        static void SetVolume(SLVolumeItf volumeInterface, float volume)
        {
            //Sound scaling from https://code.google.com/p/opensl-soundpool/issues/detail?id=1
            //Whether or not this type of scaling matches up with the other versions of the sound library is up for debate/testing

            if (volumeInterface != nullptr)
            {
                if (volume <= 0.0f)
                    volume = 0.001f; // Can't actually take the log of 0 so bump it up a bit

                auto androidVolume = (SLmillibel)RoundToFloat(1000.0f * log(volume));
                (*volumeInterface)->SetVolumeLevel(volumeInterface, androidVolume);
            }
        }

        static void SetPanning(SLVolumeItf volumeInterface, float panning)
        {
            if (volumeInterface != nullptr)
            {
                auto androidStereoPosition = (SLmillibel)RoundToFloat(1000.0f * panning);
                (*volumeInterface)->SetStereoPosition(volumeInterface, androidStereoPosition);
            }
        }

        static void EnableStereoPanning(SLVolumeItf volumeInterface, bool enable)
        {
            if (volumeInterface != nullptr)
            {
                (*volumeInterface)->EnableStereoPosition(volumeInterface, enable ? SL_BOOLEAN_TRUE : SL_BOOLEAN_FALSE);
            }
        }

        static void SetPlaybackRate(SLPlaybackRateItf playbackRateInterface, float playbackRate)
        {
            if (playbackRateInterface != nullptr)
            {
                auto androidRate = (SLmillibel)RoundToFloat(1000.0f * playbackRate);
                (*playbackRateInterface)->SetRate(playbackRateInterface, androidRate);
            }
        }
    };

} }
