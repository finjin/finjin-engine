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
    updateWhenNotFocused(false)
{
}

void ApplicationSettings::ReadCommandLineSettings(CommandLineArgsProcessor& argsProcessor, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    for (size_t index = 0; index < argsProcessor.GetCount(); index++)
    {
        auto& arg = argsProcessor[index];

        if (arg == "-additional-read-application-assets-directory" && index < argsProcessor.GetCount() - 1)
        {
            this->additionalReadApplicationAssetsDirectory = argsProcessor[index + 1];

            index++;
        }
        else if (arg == "-full-screen")
        {
            this->isFullScreen = true;
        }
        if (arg == "-full-screen-exclusive")
        {
            this->isFullScreen = true;
            this->isFullScreenExclusive = true;
        }
        else if (arg == "-vsync")
            this->vsync = true;
        else if (arg =="-window-width" && index < argsProcessor.GetCount() - 1)
        {
            uint32_t width;
            Convert::ToInteger(width, argsProcessor[index + 1], error);
            if (error)
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to parse 'window-width' value '%1%'.", argsProcessor[index + 1]));
                return;
            }

            this->windowWidth = static_cast<OSWindowDimension>(width);

            index++;
        }
        else if (arg == "-window-height" && index < argsProcessor.GetCount() - 1)
        {
            uint32_t height;
            Convert::ToInteger(height, argsProcessor[index + 1], error);
            if (error)
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to parse 'window-height' value '%1%'.", argsProcessor[index + 1]));
                return;
            }

            this->windowHeight = static_cast<OSWindowDimension>(height);

            index++;
        }
        else if (arg == "-full-screen-width" && index < argsProcessor.GetCount() - 1)
        {
            uint32_t width;
            Convert::ToInteger(width, argsProcessor[index + 1], error);
            if (error)
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to parse 'full-screen-width' value '%1%'.", argsProcessor[index + 1]));
                return;
            }

            this->fullScreenWidth = static_cast<OSWindowDimension>(width);

            index++;
        }
        else if (arg == "-full-screen-height" && index < argsProcessor.GetCount() - 1)
        {
            uint32_t height;
            Convert::ToInteger(height, argsProcessor[index + 1], error);
            if (error)
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to parse 'full-screen-height' value '%1%'.", argsProcessor[index + 1]));
                return;
            }

            this->fullScreenHeight = static_cast<OSWindowDimension>(height);

            index++;
        }
    }
}
