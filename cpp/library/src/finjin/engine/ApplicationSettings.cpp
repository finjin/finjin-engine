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
#include "ApplicationSettings.hpp"
#include "finjin/common/Convert.hpp"

using namespace Finjin::Engine;


//Implementation----------------------------------------------------------------
ApplicationSettings::ApplicationSettings() :
    windowWidth((OSWindowDimension)640),
    windowHeight((OSWindowDimension)480),
    fullScreenWidth((OSWindowDimension)1280),
    fullScreenHeight((OSWindowDimension)720),
    isFullScreen(false),
    isFullScreenExclusive(false),
    vsync(true),
    useAccelerometer(true),
    useSystemBackButton(true),
    updateWhenNotFocused(false),
    checkSystemMemoryFree(true),
    vrRequired(false),
    startInVR(false),
    mirrorVR(true) //false does not work yet
{
}

void ApplicationSettings::ReadCommandLineSettings(CommandLineArgsProcessor& argsProcessor, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    for (size_t argIndex = 0; argIndex < argsProcessor.GetCount(); argIndex++)
    {
        auto& arg = argsProcessor[argIndex];

        if (arg == "-additional-read-application-assets-directory" && argIndex < argsProcessor.GetCount() - 1)
        {
            this->additionalReadApplicationAssetsDirectory = argsProcessor[argIndex + 1];

            argIndex++;
        }
        else if (arg == "-full-screen" && argIndex < argsProcessor.GetCount() - 1)
        {
            this->isFullScreen = Convert::ToBool(argsProcessor[argIndex + 1]);

            argIndex++;
        }
        if (arg == "-full-screen-exclusive" && argIndex < argsProcessor.GetCount() - 1)
        {
            this->isFullScreen = true;
            this->isFullScreenExclusive = Convert::ToBool(argsProcessor[argIndex + 1]);

            argIndex++;
        }
        else if (arg == "-vsync" && argIndex < argsProcessor.GetCount() - 1)
        {
            this->vsync = Convert::ToBool(argsProcessor[argIndex + 1]);;

            argIndex++;
        }
        else if (arg =="-window-width" && argIndex < argsProcessor.GetCount() - 1)
        {
            uint32_t width;
            Convert::ToInteger(width, argsProcessor[argIndex + 1], error);
            if (error)
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to parse 'window-width' value '%1%'.", argsProcessor[argIndex + 1]));
                return;
            }

            this->windowWidth = static_cast<OSWindowDimension>(width);

            argIndex++;
        }
        else if (arg == "-window-height" && argIndex < argsProcessor.GetCount() - 1)
        {
            uint32_t height;
            Convert::ToInteger(height, argsProcessor[argIndex + 1], error);
            if (error)
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to parse 'window-height' value '%1%'.", argsProcessor[argIndex + 1]));
                return;
            }

            this->windowHeight = static_cast<OSWindowDimension>(height);

            argIndex++;
        }
        else if (arg == "-full-screen-width" && argIndex < argsProcessor.GetCount() - 1)
        {
            uint32_t width;
            Convert::ToInteger(width, argsProcessor[argIndex + 1], error);
            if (error)
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to parse 'full-screen-width' value '%1%'.", argsProcessor[argIndex + 1]));
                return;
            }

            this->fullScreenWidth = static_cast<OSWindowDimension>(width);

            argIndex++;
        }
        else if (arg == "-full-screen-height" && argIndex < argsProcessor.GetCount() - 1)
        {
            uint32_t height;
            Convert::ToInteger(height, argsProcessor[argIndex + 1], error);
            if (error)
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to parse 'full-screen-height' value '%1%'.", argsProcessor[argIndex + 1]));
                return;
            }

            this->fullScreenHeight = static_cast<OSWindowDimension>(height);

            argIndex++;
        }
        else if (arg == "-update-when-not-focused" && argIndex < argsProcessor.GetCount() - 1)
        {
            this->updateWhenNotFocused = Convert::ToBool(argsProcessor[argIndex + 1]);

            argIndex++;
        }
        else if (arg == "-no-check-system-memory-free" && argIndex < argsProcessor.GetCount() - 1)
        {
            this->checkSystemMemoryFree = Convert::ToBool(argsProcessor[argIndex + 1]);

            argIndex++;
        }
        else if (arg == "-vr-required" && argIndex < argsProcessor.GetCount() - 1)
        {
            this->vrRequired = Convert::ToBool(argsProcessor[argIndex + 1]);

            argIndex++;
        }
        else if (arg == "-start-in-vr" && argIndex < argsProcessor.GetCount() - 1)
        {
            this->startInVR = Convert::ToBool(argsProcessor[argIndex + 1]);

            argIndex++;
        }
        else if (arg == "-mirror-vr" && argIndex < argsProcessor.GetCount() - 1)
        {
            this->mirrorVR = Convert::ToBool(argsProcessor[argIndex + 1]);

            argIndex++;
        }
    }
}
