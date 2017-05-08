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
#include "finjin/engine/PlatformCapabilities.hpp"
#include <Windows.h>

using namespace Finjin::Engine;
using namespace Windows::Graphics::Display;
using namespace Windows::Devices::Input;


//Macros------------------------------------------------------------------------
//Sizes from https://msdn.microsoft.com/en-us/windows/uwp/layout/screen-sizes-and-breakpoints-for-responsive-design
#define DISPLAY_SMALL_DIMENSION 320
#define DISPLAY_MEDIUM_DIMENSION 720
#define DISPLAY_LARGE_DIMENSION 1024


//Implementation----------------------------------------------------------------

//DisplayInfo
DisplayInfo::DisplayInfo() : frame(0, 0, 0, 0), clientFrame(0, 0, 0, 0)
{
    this->index = 0;
    this->isPrimary = false;
    this->density = 1;
    this->hasSmallScreen = false;
    this->hasTouchScreen = true;
    this->usesScreenEdgePadding = true;
    this->orientation = GenericScreenOrientation::PORTRAIT;
    this->monitorHandle = nullptr;
}

//DisplayInfos
void DisplayInfos::Enumerate(const HardwareGpuDescriptions& hardwareGpus)
{
    clear();

    auto touchCaps = ref new TouchCapabilities();
    auto hasTouchScreen = touchCaps->TouchPresent > 0; //Doesn't necessarily indicate a touchscreen. Indicates something that is touch sensitive

    auto currentDisplayInformation = DisplayInformation::GetForCurrentView();

    auto isPortrait = currentDisplayInformation->CurrentOrientation == DisplayOrientations::Portrait || currentDisplayInformation->CurrentOrientation == DisplayOrientations::PortraitFlipped;
    auto screenOrientation = isPortrait ? GenericScreenOrientation::PORTRAIT : GenericScreenOrientation::LANDSCAPE;

    auto& platformCaps = PlatformCapabilities::GetInstance();

    size_t displayIndex = 0;
    for (const auto& hardwareGpu : hardwareGpus)
    {
        for (const auto& output : hardwareGpu.outputs)
        {
            if (output.desc.AttachedToDesktop)
            {
                auto left = platformCaps.PixelsToDips(output.desc.DesktopCoordinates.left, currentDisplayInformation->LogicalDpi);
                auto top = platformCaps.PixelsToDips(output.desc.DesktopCoordinates.top, currentDisplayInformation->LogicalDpi);
                auto width = platformCaps.PixelsToDips(output.desc.DesktopCoordinates.right - output.desc.DesktopCoordinates.left, currentDisplayInformation->LogicalDpi);
                auto height = platformCaps.PixelsToDips(output.desc.DesktopCoordinates.bottom - output.desc.DesktopCoordinates.top, currentDisplayInformation->LogicalDpi);

                DisplayInfo display;
                display.index = displayIndex++;
                display.isPrimary = output.desc.DesktopCoordinates.left == 0 && output.desc.DesktopCoordinates.top == 0;

                display.density = currentDisplayInformation->LogicalDpi / 96.0f;
                display.hasSmallScreen = width <= DISPLAY_SMALL_DIMENSION || height <= DISPLAY_SMALL_DIMENSION;
                display.hasTouchScreen = hasTouchScreen;
                display.usesScreenEdgePadding = width >= DISPLAY_LARGE_DIMENSION;

                display.orientation = screenOrientation;

                display.frame = OSWindowRect(left, top, width, height);
                display.clientFrame = display.frame;
                display.name = output.desc.DeviceName;
                display.deviceName = display.name;

                display.monitorHandle = output.desc.Monitor;

                if (push_back(display).HasErrorOrValue(false))
                    break;

                //FINJIN_DEBUG_LOG_INFO("Output %1%: (%2%, %3%) @ (%4%, %5%)", deviceName.c_str(), output.desc.DesktopCoordinates.left, output.desc.DesktopCoordinates.top, width, height);
            }
        }
    }
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
