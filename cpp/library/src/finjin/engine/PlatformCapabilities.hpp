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


//Includes---------------------------------------------------------------------
#include "finjin/common/Chrono.hpp"
#include "finjin/common/LayoutDirection.hpp"
#include "finjin/common/MemoryArchitecture.hpp"
#include "finjin/common/Utf8String.hpp"
#include "finjin/common/Version.hpp"
#include "finjin/engine/ApplicationNameFormat.hpp"
#include "finjin/engine/DisplayInfo.hpp"


//Classes----------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    /**
     * Intrinsic information about the platform's hardware/software capabilities.
     */
    class PlatformCapabilities
    {
    public:
        PlatformCapabilities();
        virtual ~PlatformCapabilities();

        static bool HasInstance();
        static PlatformCapabilities& GetInstance();

        void SetLanguageAndCountry(const char* lang, const char* country);
        void SetLanguageAndCountry(const Utf8String& culture);
        void SetLanguageAndCountry(const char* culture); //Example: "en", "en-US", "en-US.UTF8", etc.

        enum class ClipboardName
        {
            CLIPBOARD,
            PASTEBOARD
        };
        ClipboardName GetClipboardName();

        virtual SimpleTimeDelta GetDoubleClickTimeSeconds();

        float DipsToPixels(float dips, float dpi); //Platform-specific
        float PixelsToDips(float pixels, float dpi); //Platform-specific

        bool GetDisplays(DisplayInfos& displays); //Platform-specific
        void CacheDisplays(const DisplayInfos& displays);

    public:
        size_t maxWindows; //The maximum number of application windows. On regular desktops (Win32, MacOS, Linux) this is unlimited, on mobile this is 1
        
        ApplicationNameFormat directoryApplicationNameFormat; //Camel case by default. The most suitable application name to use when creating directories

        bool hasDelayedWindowSizeChangeNotification; //false by default. Indicates whether window size change notifications are delayed (either due to the platform itself or the implementation)
        bool canCloseWindowsInternally;  //true by default. Indicates whether windows can be closed programmatically
        bool canExitInternally; //true by default. Indicates whether the application can be exited programmatically
        bool canMoveWindows; //true by default. Indicates whether windows can be moved around. On regular desktops this is true, on other platforms false
        bool canResizeWindows; //true by default. Indicates whether windows can be resized around. On regular desktops this is true, on other platforms false
        bool titleBarUsesSubtitle; //false by default. Indicates whether the application window's title bar displays a subtitle. A subtitle is part of a non-changing title that is set during development time
        bool hasClipboard; //true by default. Indicates whether copy/paste is possible
        bool isGameControllerCapable; //true by default. Indicates whether game controllers are able to be used
        bool isKeyboardCapable; //true by default. Indicates whether keyboards are able to be used
        bool isMouseCapable; //true by default. Indicates whether mice are able to be used
        bool isNetworkCapable; //true by default. Indicates whether network access can be used
        bool isSoundCapable; //true by default. Indicates whether sound can be used
        LayoutDirection layoutDirection;
        MemoryArchitecture memoryArchitecture;
                
        Utf8String lang;
        Utf8String country;
        Utf8String operatingSystemInternalName;
        Utf8String operatingSystemInternalVersion;
        Utf8String operatingSystemName;
        Utf8String applicationPlatformInternalName; //Used in cases where the operating system may have multiple platforms. For example "uwp" or "win32" on Windows
        Utf8String deviceManufacturer;
        Utf8String deviceModel;
    
    private:
        static PlatformCapabilities* instance;

        DisplayInfos cachedDisplays;
    };

} }
