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
#include "finjin/engine/FinjinEngineLibrary.hpp"
#include "Application.hpp"
#include "ApplicationDelegate.hpp"
#include "finjin/common/DebugLog.hpp"
#import <AppKit/AppKit.h>

using namespace Finjin::Engine;


//Implementation----------------------------------------------------------------
void Application::InitializeGlobals(Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    //Set up root file system
    GetFileSystem(ApplicationFileSystem::READ_APPLICATION_ASSETS).AddDirectory(this->standardPaths.applicationBundleDirectory.path, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to add application assets to file system.");
        return;
    }

    //Language/country
    auto language = [[NSLocale preferredLanguages] objectAtIndex:0];
    if (language != nullptr)
        SetLanguageAndCountry(language.UTF8String);

    //Layout direction
    if ([NSApplication sharedApplication].userInterfaceLayoutDirection == NSUserInterfaceLayoutDirectionRightToLeft)
        this->layoutDirection = LayoutDirection::RIGHT_TO_LEFT;

    //Device model/manufacturer
    this->deviceManufacturer = "Apple";
    this->deviceModel = "Macbook";

    //Operating system
    this->operatingSystemInternalName = "macos";
    auto osVersion = [[NSProcessInfo processInfo] operatingSystemVersion];
    this->operatingSystemInternalVersion = Convert::ToString(osVersion.majorVersion);
    this->operatingSystemInternalVersion += ".";
    this->operatingSystemInternalVersion += Convert::ToString(osVersion.minorVersion);
    this->operatingSystemName = "macOS";
    this->applicationPlatformInternalName = "cocoa";
}

void Application::CreateSystems(Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    //Input--------------------
    this->inputSystem.Create(this->inputSystemSettings, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to initialize input system.");
        return;
    }

    //Sound--------------------
    this->soundSystem.Create(this->soundSystemSettings, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to initialize sound system.");
        return;
    }

    //GPU------------------
    this->gpuSystem.Create(this->gpuSystemSettings, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to initialize GPU system.");
        return;
    }

    //Notify delegate
    this->applicationDelegate->OnGpusEnumerated(this->gpuSystem.GetHardwareGpuDescriptions(), this->gpuSystem.GetSoftwareGpuDescriptions());
}

bool Application::MainLoop(Error& error)
{
    return false; //Not handled
}

void Application::ReportError(const Error& error)
{
    auto errorString = error.ToString();

    FINJIN_DEBUG_LOG_ERROR("%1%", errorString);

    OSWindow::ShowErrorMessage(errorString, this->applicationDelegate->GetName(ApplicationNameFormat::DISPLAY));
}

void Application::ShowTheCursor()
{
    [NSCursor unhide];
}

void Application::HideTheCursor()
{
    [NSCursor hide];
}
