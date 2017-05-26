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
#import <AppKit/NSScreen.h>
#import <CoreGraphics/CoreGraphics.h>
#import <IOKit/graphics/IOGraphicsLib.h>
#import <Foundation/NSString.h>

using namespace Finjin::Engine;


//Local functions---------------------------------------------------------------
static void GetNameForDisplayID(Utf8String& name, CGDirectDisplayID displayID)
{
    NSDictionary* deviceInfo = (__bridge NSDictionary*)IODisplayCreateInfoDictionary(CGDisplayIOServicePort(displayID), kIODisplayOnlyPreferredName);
    NSDictionary* localizedNames = [deviceInfo objectForKey:[NSString stringWithUTF8String:kDisplayProductName]];
    if ([localizedNames count] > 0)
    {
        NSString* screenName = [localizedNames objectForKey:[[localizedNames allKeys] objectAtIndex:0]];
        name = screenName.UTF8String;
    }
    else
        name.clear();
}


//Implementation----------------------------------------------------------------

//DisplayInfo
DisplayInfo::DisplayInfo() : frame(0, 0, 0, 0)
{
    this->index = 0;
    this->isPrimary = false;
    this->rotation = 0;
    this->density = 1;
}

//DisplayInfos
void DisplayInfos::Enumerate()
{
    CGDirectDisplayID displayIDs[MAX_ITEMS];
    uint32_t displayIDCount = 0;
    CGGetActiveDisplayList(MAX_ITEMS, displayIDs, &displayIDCount);

    //Resize optimistically, assuming all displays are active
    resize(static_cast<size_t>(displayIDCount));

    //Iterate over displays
    //First display will be main display
    size_t okCount = 0;
    for (size_t displayIndex = 0; displayIndex < displayIDCount && okCount < size(); displayIndex++)
    {
        if (CGDisplayIsActive(displayIDs[displayIndex]))
        {
            DisplayInfo& displayInfo = (*this)[okCount];

            displayInfo.index = okCount;

            displayInfo.isPrimary = CGDisplayIsMain(displayIDs[displayIndex]);

            GetNameForDisplayID(displayInfo.name, displayIDs[displayIndex]);

            displayInfo.rotation = CGDisplayRotation(displayIDs[displayIndex]);

            auto rect = CGDisplayBounds(displayIDs[displayIndex]);
            displayInfo.frame.x = rect.origin.x;
            displayInfo.frame.y = rect.origin.y;
            displayInfo.frame.width = rect.size.width;
            displayInfo.frame.height = rect.size.height;

            auto displayRef = CGDisplayCopyDisplayMode(displayIDs[displayIndex]);
            double pixelsWidth = CGDisplayModeGetPixelWidth(displayRef);
            displayInfo.density = round(pixelsWidth / rect.size.width);

            okCount++;
        }
    }

    //Call resize again in case some of the displays weren't active
    resize(okCount);
}

void DisplayInfos::SortLeftToRight()
{
    std::sort(begin(), end(), [](const DisplayInfo& a, const DisplayInfo& b) {return a.frame.x < b.frame.x;});
}
