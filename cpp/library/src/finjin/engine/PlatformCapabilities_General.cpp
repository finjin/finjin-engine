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
#include "PlatformCapabilities.hpp"

using namespace Finjin::Engine;


//Static initialization---------------------------------------------------------
PlatformCapabilities* PlatformCapabilities::instance;


//Implementation----------------------------------------------------------------
PlatformCapabilities::PlatformCapabilities()
{
    if (instance == nullptr)
        instance = this;

    this->maxWindows = std::numeric_limits<size_t>::max();

    this->directoryApplicationNameFormat = ApplicationNameFormat::UPPER_CAMEL_CASE;

    this->hasDelayedWindowSizeChangeNotification = false;
    this->canCloseWindowsInternally = true;
    this->canExitInternally = true;
    this->canMoveWindows = true;
    this->canResizeWindows = true;
    this->titleBarUsesSubtitle = false;
    this->hasClipboard = true;
    this->isGameControllerCapable = true;
    this->isKeyboardCapable = true;
    this->isMouseCapable = true;
    this->isNetworkCapable = true;
    this->isSoundCapable = true;
    this->layoutDirection = LayoutDirection::LEFT_TO_RIGHT;
    this->memoryArchitecture = MemoryArchitecture::SEGMENTED;
}

PlatformCapabilities::~PlatformCapabilities()
{
    if (instance == this)
        instance = nullptr;
}

bool PlatformCapabilities::HasInstance()
{
    return instance != nullptr;
}

PlatformCapabilities& PlatformCapabilities::GetInstance()
{
    assert(instance != nullptr);
    return *instance;
}

void PlatformCapabilities::SetLanguageAndCountry(const char* lang, const char* country)
{
    this->lang = lang;
    this->lang.ToLowerAscii();

    this->country = country;
    this->country.ToLowerAscii();
}

void PlatformCapabilities::SetLanguageAndCountry(const Utf8String& culture)
{
    SetLanguageAndCountry(culture.c_str());
}

void PlatformCapabilities::SetLanguageAndCountry(const char* culture)
{
    this->lang.clear();
    this->country.clear();

    if (culture != nullptr && culture[0] != 0)
    {
        size_t cultureLength = strlen(culture);
        if (cultureLength > 2 && (culture[2] == '-' || culture[2] == '_'))
        {
            this->lang.assign(culture, 2);

            //Figure out country
            //Shorten the country a little if there is a '.' (usually indicating some type of character encoding)
            Utf8String country = &culture[3];
            auto dotIndex = country.find('.');
            if (dotIndex != Utf8String::npos)
                this->country.assign(country.begin(), dotIndex);
            else
                this->country = country;
        }
        else
            this->lang = culture;
    }
}

PlatformCapabilities::ClipboardName PlatformCapabilities::GetClipboardName()
{
#if FINJIN_TARGET_PLATFORM_IS_APPLE
    return ClipboardName::PASTEBOARD;
#else
    return ClipboardName::CLIPBOARD;
#endif
}

SimpleTimeDelta PlatformCapabilities::GetDoubleClickTimeSeconds()
{
    return (SimpleTimeDelta).24;
}

void PlatformCapabilities::CacheDisplays(const DisplayInfos& displays)
{
    this->cachedDisplays = displays;
}
