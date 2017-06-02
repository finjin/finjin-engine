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
#include "finjin/common/Utf8String.hpp"
#include <Windows.h>

using namespace Finjin::Engine;


//Local types-------------------------------------------------------------------
struct EnumHelper
{
    EnumHelper(DisplayInfos* displaysBeingEnumerated) : displays(displaysBeingEnumerated)
    {
        //Enumerates all the display names for subsequent monitor enumeration

        this->outputNameToMonitorName.clear();

        DISPLAY_DEVICEW displayDevice;

        for (DWORD monitorIndex = 0; !this->outputNameToMonitorName.full(); monitorIndex++)
        {
            FINJIN_ZERO_ITEM(displayDevice);
            displayDevice.cb = sizeof(displayDevice);
            if (!EnumDisplayDevicesW(nullptr, monitorIndex, &displayDevice, 0))
                break;

            if ((displayDevice.StateFlags & DISPLAY_DEVICE_ACTIVE) && this->outputNameToMonitorName.push_back())
            {
                auto& output = this->outputNameToMonitorName.back();
                output.first = displayDevice.DeviceName; //output name (such as "\\\\.\\DISPLAY1")
                output.second = displayDevice.DeviceString;
            }
        }
    }

    const Utf8String* FindMonitorName(const Utf8String& outputName) const
    {
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


//Local functions---------------------------------------------------------------
static BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hDC, LPRECT rect, LPARAM param)
{
    auto helper = reinterpret_cast<EnumHelper*>(param);

    //Get info for the monitor
    MONITORINFOEXW monitorInfo;
    monitorInfo.cbSize = sizeof(monitorInfo);
    if (!GetMonitorInfoW(hMonitor, &monitorInfo))
        return FALSE;

    Utf8String displayName(monitorInfo.szDevice); //output name (such as "\\\\.\\DISPLAY1")

    auto continueEnumeration = TRUE;

    //If the monitor's name can be found in the helper, then the monitor is connected and its info should be stored
    auto monitorName = helper->FindMonitorName(displayName);
    if (monitorName != nullptr)
    {
        if (!helper->displays->push_back())
            continueEnumeration = FALSE;
        else
        {
            auto index = helper->displays->size();

            auto& display = helper->displays->back();

            display.index = index;

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
        }
    }

    return continueEnumeration;
}


//Implementation----------------------------------------------------------------
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
