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
#include "finjin/engine/Application.hpp"
#include "finjin/engine/ApplicationDelegate.hpp"
#include "finjin/engine/OSWindow.hpp"
#include "finjin/common/DebugLog.hpp"
#include "finjin/common/Version.hpp"

using namespace Finjin::Common;
using namespace Finjin::Engine;
using namespace Windows::UI::Core;
using namespace Windows::ApplicationModel::Core;


//Local functions--------------------------------------------------------------
static Utf8String GetWindowsInternalVersion()
{
    auto deviceFamilyVersion = Windows::System::Profile::AnalyticsInfo::VersionInfo->DeviceFamilyVersion;
    Utf8String versionString(deviceFamilyVersion->Data());
    uint64_t version = Convert::ToInteger(versionString, 0ull);
    auto major = (version & 0xFFFF000000000000ull) >> 48;
    auto minor = (version & 0x0000FFFF00000000ull) >> 32;
    auto build = (version & 0x00000000FFFF0000ull) >> 16;
    auto revision = (version & 0x000000000000FFFFull);
    
    Utf8String internalVersion;
    internalVersion += Convert::ToString(major);
    internalVersion += ".";
    internalVersion += Convert::ToString(minor);    
    return internalVersion;
}


//Implementation---------------------------------------------------------------
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

    //One application window
    this->maxWindows = 1; 

    //Window properties
    this->canCloseWindowsInternally = false;
    this->canExitInternally = false;
    this->hasDelayedWindowSizeChangeNotification = true;
    this->titleBarUsesSubtitle = true;

    //Layout direction
    if (CoreWindow::GetForCurrentThread()->FlowDirection == Windows::UI::Core::CoreWindowFlowDirection::RightToLeft)
        this->layoutDirection = LayoutDirection::RIGHT_TO_LEFT;

    //Language/country
    Utf8String languageAndCountry = Windows::Globalization::ApplicationLanguages::Languages->GetAt(0)->Data();
    SetLanguageAndCountry(languageAndCountry);
    
    //Operating system properties
    this->operatingSystemInternalName = "windows";
    this->operatingSystemInternalVersion = GetWindowsInternalVersion();
    this->operatingSystemName = "Windows (Universal Platform)";
    this->applicationPlatformInternalName = "uwp";
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

    //GPU--------------------
    this->gpuSystem.Create(this->gpuSystemSettings, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to initialize GPU system.");
        return;
    }

    this->applicationDelegate->OnGpusEnumerated(this->gpuSystem.GetHardwareGpuDescriptions(), this->gpuSystem.GetSoftwareGpuDescriptions());

    //Update displays cache
    DisplayInfos displays;
    displays.Enumerate(this->gpuSystem.GetHardwareGpuDescriptions());
    PlatformCapabilities::GetInstance().CacheDisplays(displays);
}

bool Application::MainLoop(Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto processOption = CoreProcessEventsOption::ProcessAllIfPresent;
    while (!this->appViewportsController.empty())
    {
        //Get focus state of all windows---------
        auto focusState = this->appViewportsController.StartFocusUpdate();

        //Handled queued messages---------------
        CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(processOption);
        
        //Perform update----------------------
        if (!this->appViewportsController.empty())
        {   
            for (size_t appWindowIndex = 0; appWindowIndex < this->appViewportsController.size(); appWindowIndex++)
            {
                auto& appViewport = this->appViewportsController[appWindowIndex];

                auto hasFocus = appViewport->GetOSWindow()->HasFocus();

                if (focusState.hadFocus[appWindowIndex] && !hasFocus)
                    HandleApplicationViewportLostFocus(appViewport.get());
                else if (!focusState.hadFocus[appWindowIndex] && hasFocus)
                    HandleApplicationViewportGainedFocus(appViewport.get());
            }

            this->appViewportsController.FinishFocusUpdate(focusState);
            if (focusState.anyHasFocus || this->applicationDelegate->GetApplicationSettings().updateWhenNotFocused)
            {
                Tick(error);
                if (error)
                {
                    Utf8StringToWideString errorString(error.ToString());
                    OutputDebugStringW(errorString.c_str());

                    processOption = CoreProcessEventsOption::ProcessUntilQuit;
                    CoreApplication::Exit();

                    break;
                }
                else
                    processOption = CoreProcessEventsOption::ProcessAllIfPresent;
            }
            else
            {
                //Go into a sleep/wait state waiting for the next event
                processOption = CoreProcessEventsOption::ProcessOneIfPresent;
            }
        }
        
        if (this->appViewportsController.empty())
        {
            processOption = CoreProcessEventsOption::ProcessUntilQuit;
            CoreApplication::Exit();
        }
    }
    
    return true;
}

void Application::ReportError(const Error& error)
{
    FINJIN_DEBUG_LOG_ERROR("%1%", error.ToString());
}

void Application::ShowTheCursor()
{
    for (auto& appViewport : this->appViewportsController)
        appViewport->GetOSWindow()->ShowTheCursor();    
}

void Application::HideTheCursor()
{
    for (auto& appViewport : this->appViewportsController)
        appViewport->GetOSWindow()->HideTheCursor();
}
