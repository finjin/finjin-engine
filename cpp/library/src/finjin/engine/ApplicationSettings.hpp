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
#include "finjin/common/CommandLineArgsProcessor.hpp"
#include "finjin/common/Error.hpp"
#include "finjin/common/Path.hpp"
#include "finjin/common/Settings.hpp"
#include "ScreenCapture.hpp"
#include "OSWindowDefs.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    class ApplicationSettings
    {
    public:
        ApplicationSettings();

        void ReadCommandLineSettings(CommandLineArgsProcessor& argsProcessor, Error& error);

        bool IsVRRequested() const { return true; }
        bool IsVRRequired() const { return this->vrRequired; }
        bool StartInVR() const { return this->startInVR; }

    public:
        Setting<Path> additionalReadApplicationAssetsDirectory; //Additional directory for ApplicationFileSystem::READ_APPLICATION_ASSETS
        Setting<OSWindowDimension> windowWidth;
        Setting<OSWindowDimension> windowHeight;
        Setting<OSWindowDimension> fullScreenWidth;
        Setting<OSWindowDimension> fullScreenHeight;
        Setting<bool> isFullScreen;
        Setting<bool> isFullScreenExclusive;
        Setting<bool> vsync;
        Setting<bool> useAccelerometer; //Not read on command line. Typically set in derived settings class, depending on needs of application
        Setting<bool> useSystemBackButton; //Not read on command line. Typically set in derived settings class, depending on needs of application
        Setting<bool> updateWhenNotFocused; //Not read on command line. Typically set in derived settings class, depending on needs of application
        Setting<ScreenCaptureFrequency> screenCaptureFrequency; //Not read on command line. Typically set in derived settings class, depending on needs of application
        Setting<bool> checkSystemMemoryFree;
        Setting<bool> vrRequired;
        Setting<bool> startInVR;
        Setting<bool> mirrorVR;
    };

} }
