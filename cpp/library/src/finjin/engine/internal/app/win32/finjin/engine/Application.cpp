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

using namespace Finjin::Common;
using namespace Finjin::Engine;


//Local functions---------------------------------------------------------------
static bool IsWindowsVersion(DWORD major, DWORD minor)
{
    OSVERSIONINFOEXW osVersion = {};
    osVersion.dwOSVersionInfoSize = sizeof(osVersion);
    osVersion.dwMajorVersion = major;
    osVersion.dwMinorVersion = minor;

    DWORD typeMask = VER_MAJORVERSION | VER_MINORVERSION;

    DWORDLONG condition = 0;
    VER_SET_CONDITION(condition, VER_MAJORVERSION, VER_EQUAL);
    VER_SET_CONDITION(condition, VER_MINORVERSION, VER_EQUAL);

    return VerifyVersionInfoW(&osVersion, typeMask, condition) ? true : false;
}

static bool IsWindowsVersion(DWORD major, DWORD minor, BYTE productType)
{
    OSVERSIONINFOEXW osVersion = {};
    osVersion.dwOSVersionInfoSize = sizeof(osVersion);
    osVersion.dwMajorVersion = major;
    osVersion.dwMinorVersion = minor;
    osVersion.wProductType = productType;

    DWORD typeMask = VER_MAJORVERSION | VER_MINORVERSION | VER_PRODUCT_TYPE;

    DWORDLONG condition = 0;
    VER_SET_CONDITION(condition, VER_MAJORVERSION, VER_EQUAL);
    VER_SET_CONDITION(condition, VER_MINORVERSION, VER_EQUAL);
    VER_SET_CONDITION(condition, VER_PRODUCT_TYPE, VER_EQUAL);

    return VerifyVersionInfoW(&osVersion, typeMask, condition) ? true : false;
}

static const char* GetWindowsName()
{
    //From https://msdn.microsoft.com/en-us/library/windows/desktop/ms724833(v=vs.85).aspx
    if (IsWindowsVersion(10, 0, VER_NT_WORKSTATION))
        return "Windows 10";
    else if (IsWindowsVersion(10, 0))
        return "Windows Server 2016 Technical Preview";
    else if (IsWindowsVersion(6, 3, VER_NT_WORKSTATION))
        return "Windows 8.1";
    else if (IsWindowsVersion(6, 3))
        return "Windows Server 2012 R2";
    else if (IsWindowsVersion(6, 2, VER_NT_WORKSTATION))
        return "Windows 8";
    else if (IsWindowsVersion(6, 2))
        return "Windows Server 2012";
    else if (IsWindowsVersion(6, 1, VER_NT_WORKSTATION))
        return "Windows 7";
    else if (IsWindowsVersion(6, 1))
        return "Windows Server 2008 R2";
    else
        return "Windows";
}

static const char* GetWindowsInternalVersion()
{
    if (IsWindowsVersion(10, 0, VER_NT_WORKSTATION))
        return "10.1";
    else if (IsWindowsVersion(10, 0))
        return "10";
    else if (IsWindowsVersion(6, 3, VER_NT_WORKSTATION))
        return "6.3.1";
    else if (IsWindowsVersion(6, 3))
        return "6.3";
    else if (IsWindowsVersion(6, 2, VER_NT_WORKSTATION))
        return "6.2.1";
    else if (IsWindowsVersion(6, 2))
        return "6.2";
    else if (IsWindowsVersion(6, 1, VER_NT_WORKSTATION))
        return "6.1.1";
    else if (IsWindowsVersion(6, 1))
        return "6.1";
    else
        return Utf8String::Empty().c_str();
}


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

    //Layout direction
    DWORD defaultLayout;
    if (GetProcessDefaultLayout(&defaultLayout) && defaultLayout == LAYOUT_RTL)
        this->layoutDirection = LayoutDirection::RIGHT_TO_LEFT;

    //Language/country
    const ULONG LANGUAGE_BUFFER_LENGTH = 100;
    wchar_t languageBufferW[LANGUAGE_BUFFER_LENGTH];
    ULONG languageBufferLength = LANGUAGE_BUFFER_LENGTH;
    ULONG languageCount = 0;
    Utf8String languageAndCountry;
    if (GetUserPreferredUILanguages(MUI_LANGUAGE_NAME, &languageCount, languageBufferW, &languageBufferLength) && languageCount > 0)
        languageAndCountry.assign(languageBufferW);
    SetLanguageAndCountry(languageAndCountry);

    //Operating system properties
    this->operatingSystemInternalName = "windows";
    this->operatingSystemInternalVersion = GetWindowsInternalVersion();
    this->operatingSystemName = GetWindowsName();
    this->applicationPlatformInternalName = "win32";

    //Console - for development
    if (FINJIN_DEBUG)
    {
        this->applicationGlobals.hasConsole = AllocConsole() == TRUE;
        if (this->applicationGlobals.hasConsole)
        {
            freopen("CONOUT$", "w", stdout);
            freopen("CONIN$", "r", stdin);
        }
    }
}

void Application::CreateSystems(Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    //Input
    this->inputSystem.Create(this->inputSystemSettings, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to initialize input system.");
        return;
    }

    //Sound
    this->soundSystem.Create(this->soundSystemSettings, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to initialize sound system.");
        return;
    }

    //GPU
    this->gpuSystem.Create(this->gpuSystemSettings, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to initialize GPU system.");
        return;
    }

#if FINJIN_TARGET_VR_SYSTEM != FINJIN_TARGET_VR_SYSTEM_NONE
    //VR
    this->vrSystem.Create(this->vrSystemSettings, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to initialize VR system.");
        return;
    }
#endif

    this->applicationDelegate->OnGpusEnumerated(this->gpuSystem.GetHardwareGpuDescriptions(), this->gpuSystem.GetSoftwareGpuDescriptions());
}

bool Application::MainLoop(Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto exitMainLoop = false;
    while (!exitMainLoop && !this->appViewportsController.empty())
    {
        //Get focus state of all windows---------
        auto focusState = this->appViewportsController.StartFocusUpdate();

        //Handle queued messages-----------------
        if (!focusState.anyHadFocus && !this->applicationDelegate->GetApplicationSettings().updateWhenNotFocused)
            WaitMessage();

        MSG msg;
        while (PeekMessageW(&msg, nullptr, 0U, 0U, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }

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
                    FINJIN_SET_ERROR(error, "Error processing tick.");
                    exitMainLoop = true;
                }
            }
        }
    }

    return true;
}

void Application::ReportError(const Error& error)
{
    auto errorString = error.ToString();

    if (this->applicationGlobals.hasConsole)
        FINJIN_DEBUG_LOG_ERROR("%1%", errorString);

    OSWindow::ShowErrorMessage(errorString, this->applicationDelegate->GetName(ApplicationNameFormat::DISPLAY));
}

void Application::ShowTheCursor()
{
    ShowCursor(TRUE);
}

void Application::HideTheCursor()
{
    ShowCursor(FALSE);
}
