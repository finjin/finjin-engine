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
#include "DisplayInfo.hpp"
#include <Windows.h>

using namespace Finjin::Engine;


//Local classes----------------------------------------------------------------
struct EnumHelper
{
    EnumHelper(DisplayInfos* displaysBeingEnumerated) : displays(displaysBeingEnumerated)
    {
        //Enumerates all the display names for subsequent monitor enumeration
        
        this->outputNameToMonitorName.resize(EngineConstants::MAX_DISPLAYS);
        DISPLAY_DEVICEW displayDevice;

        size_t okCount = 0;
        for (DWORD i = 0; okCount < this->outputNameToMonitorName.size(); i++)
        {
            displayDevice.cb = sizeof(displayDevice);

            //First call gets graphics card name
            if (!EnumDisplayDevicesW(nullptr, i, &displayDevice, 0))
                break;
            std::wstring graphicsCardNameW = displayDevice.DeviceName; //graphics card name (such as "GeForce GTX 980")

            if (displayDevice.StateFlags & DISPLAY_DEVICE_ACTIVE)
            {
                this->outputNameToMonitorName[okCount].first = displayDevice.DeviceName; //output name (such as "\\\\.\\DISPLAY1")

                //Second call gets monitor name
                if (!EnumDisplayDevicesW(graphicsCardNameW.c_str(), i, &displayDevice, 0))
                    break;
            
                this->outputNameToMonitorName[okCount].second = displayDevice.DeviceString; //monitor/device name (such as "Dell Ultrasharp")
                
                okCount++;
            }
        }
        this->outputNameToMonitorName.resize(okCount);
    }

    const Utf8String* FindMonitorName(const Utf8String& outputName) const
    {
        Utf8String result;

        for (size_t nameIndex = 0; nameIndex < this->outputNameToMonitorName.size(); nameIndex++)
        {
            if (this->outputNameToMonitorName[nameIndex].first == outputName)
                return &this->outputNameToMonitorName[nameIndex].second;
        }

        return nullptr;
    }

    DisplayInfos* displays;

    StaticVector<std::pair<Utf8String, Utf8String>, EngineConstants::MAX_DISPLAYS> outputNameToMonitorName;
};


//Local functions--------------------------------------------------------------
static BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hDC, LPRECT rect, LPARAM param)
{
    auto helper = reinterpret_cast<EnumHelper*>(param);

    //Get info for the monitor
    MONITORINFOEXW monitorInfo;
    monitorInfo.cbSize = sizeof(monitorInfo);
    if (!GetMonitorInfoW(hMonitor, &monitorInfo))
        return FALSE;

    DisplayInfo display;
    display.name = monitorInfo.szDevice; //output name (such as "\\\\.\\DISPLAY1")

    auto continueEnumeration = TRUE;
    
    //If the monitor's name can be found in the helper, then the monitor is connected and its info should be stored
    auto monitorName = helper->FindMonitorName(display.name);
    if (monitorName != nullptr)
    {
        display.index = helper->displays->size();

        display.isPrimary = (monitorInfo.dwFlags & MONITORINFOF_PRIMARY) != 0;

        display.frame.x = monitorInfo.rcMonitor.left;
        display.frame.y = monitorInfo.rcMonitor.top;
        display.frame.width = monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left;
        display.frame.height = monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top;

        display.clientFrame.x = monitorInfo.rcWork.left;
        display.clientFrame.y = monitorInfo.rcWork.top;
        display.clientFrame.width = monitorInfo.rcWork.right - monitorInfo.rcWork.left;
        display.clientFrame.height = monitorInfo.rcWork.bottom - monitorInfo.rcWork.top;

        display.deviceName = *monitorName; //monitor/device name (such as "Dell Ultrasharp")

        display.monitorHandle = hMonitor;

        if (helper->displays->push_back(display).HasErrorOrValue(false))
            continueEnumeration = FALSE;
    }
    
    return continueEnumeration;
}


//Implementation---------------------------------------------------------------
//DisplayInfo
DisplayInfo::DisplayInfo() : frame(0, 0, 0, 0), clientFrame(0, 0, 0, 0)
{
    this->index = 0;    
    this->isPrimary = false;
    this->monitorHandle = nullptr;
}

//DisplayInfos
void DisplayInfos::Enumerate()
{
    clear();
    
    EnumHelper helper(this);
    EnumDisplayMonitors(nullptr, nullptr, MonitorEnumProc, reinterpret_cast<LPARAM>(&helper));    
}

void DisplayInfos::SortLeftToRight()
{
    std::sort(begin(), end(), [](auto& a, auto& b) {return a.frame.x < b.frame.x;});
}

const DisplayInfo* DisplayInfos::GetByName(const Utf8String& name) const
{
    for (const auto& displayInfo : this->items)
    {
        if (displayInfo.name == name)
            return &displayInfo;
    }

    return nullptr;
}
