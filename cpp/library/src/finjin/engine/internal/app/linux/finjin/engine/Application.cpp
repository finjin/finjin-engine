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
#include <sys/utsname.h>
#include <langinfo.h>
#include <stdio.h>

using namespace Finjin::Common;
using namespace Finjin::Engine;


//Local functions--------------------------------------------------------------
static Utf8String ParseLineValue(char* buff, const char* key)
{
    char* begin = strstr(buff, key);
    char* end = begin != nullptr ? strstr(begin, "\n") : nullptr;
    
    Utf8String value;
    if (begin != nullptr && end != nullptr)
    {
        begin += strlen(key);
        value.assign(begin, end);
    }
    return value;
}

static Utf8String GetLinuxName() 
{
    Utf8String operatingSystemName;
    
    FILE* f = fopen("/etc/lsb-release", "rt");
    if (f != nullptr)
    {
        const int buffLength = 1000;
        char buff[buffLength];
        int numRead = fread(buff, 1, buffLength - 1, f);
        fclose(f);
        
        if (numRead > 0)
        {
            buff[numRead] = 0;
            
            Utf8String distribID = ParseLineValue(buff, "DISTRIB_ID="); //Something like "Ubuntu"
            if (!distribID.empty())
            {
                operatingSystemName = distribID;
                
                Utf8String distribRelease = ParseLineValue(buff, "DISTRIB_RELEASE="); //Something like "15.10"
                if (!distribRelease.empty())
                {
                    operatingSystemName += " ";
                    operatingSystemName += distribRelease;
                }
            }
        }
    }
    
    utsname utsName = {};
    if (uname(&utsName) == 0)
    {
        auto hasDistributionName = !operatingSystemName.empty();
        
        if (hasDistributionName)
            operatingSystemName += " (";        
        operatingSystemName += utsName.sysname;
        operatingSystemName += " ";
        operatingSystemName += utsName.release;        
        if (hasDistributionName)
            operatingSystemName += ")";
    }
    
    return operatingSystemName;
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
    
    //Layout direction: How to determine this?
    //if (right to left)
        //this->layoutDirection = LayoutDirection::RIGHT_TO_LEFT;
    
    //Language
    char* languageAndCountry = getenv("LANG");
    if (languageAndCountry == nullptr)
        languageAndCountry = nl_langinfo(_NL_IDENTIFICATION_LANGUAGE);
    SetLanguageAndCountry(languageAndCountry);
    
    //Operating system
    this->operatingSystemName = GetLinuxName();
}

void Application::CreateSystems(Error& error)
{
    FINJIN_ERROR_METHOD_START(error);
    
    //Input------------------
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
    
    this->applicationDelegate->OnSoundDevicesEnumerated(this->soundSystem.GetAdapterDescriptions());
    
    //GPU---------------------
    this->gpuSystem.Create(this->gpuSystemSettings, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to initialize GPU system.");
        return;
    }

    this->applicationDelegate->OnGpusEnumerated(this->gpuSystem.GetHardwareGpuDescriptions(), this->gpuSystem.GetSoftwareGpuDescriptions());
}

bool Application::MainLoop(Error& error)
{
    FINJIN_ERROR_METHOD_START(error);
    
    bool exitMainLoop = false;
    while (!exitMainLoop && !this->appViewportsController.empty())
    {
        //Get focus state of all windows---------
        auto focusState = this->appViewportsController.StartFocusUpdate();
        
        //Handle queued events-----------------
        if (!focusState.anyHadFocus && !this->applicationDelegate->GetApplicationSettings().updateWhenNotFocused)
            usleep(100000);         
        XcbConnection::HandleEvents();
        
        //Perform update----------------------
        if (!this->appViewportsController.empty())
        {
            for (size_t appWindowIndex = 0; appWindowIndex < this->appViewportsController.size(); appWindowIndex++)
            {
                auto& appViewport = this->appViewportsController[appWindowIndex];

                const bool hasFocus = appViewport->GetOSWindow()->HasFocus();

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
    FINJIN_DEBUG_LOG_ERROR("%1%", error.ToString());

#if FINJIN_DEBUG
    std::cout << "Press any key to continue." << std::endl;
    std::getchar();
#endif
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
