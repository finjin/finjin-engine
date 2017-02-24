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
#import <UIKit/UIScreen.h>
#import <CoreGraphics/CoreGraphics.h>

using namespace Finjin::Engine;


//Implementation---------------------------------------------------------------
//DisplayInfo
DisplayInfo::DisplayInfo() : frame(0, 0, 0, 0)
{
    this->index = 0;
    this->isPrimary = false;
    this->density = 1;
    this->hasSmallScreen = false;
    this->hasTouchScreen = true;
    this->usesScreenEdgePadding = true;
}

//DisplayInfos
void DisplayInfos::Enumerate()
{
    auto screens = [UIScreen screens];
    auto mainScreen = [UIScreen mainScreen];
    
    resize(static_cast<size_t>(screens.count));
    
    //First display will be main display
    for (size_t i = 0; i < size(); i++)
    {
        DisplayInfo& displayInfo = (*this)[i];
        
        displayInfo.index = i;
        
        displayInfo.isPrimary = screens[i] == mainScreen;
        
        displayInfo.density = screens[i].nativeScale;
        
        displayInfo.hasSmallScreen = [UIDevice currentDevice].userInterfaceIdiom == UIUserInterfaceIdiomPhone;
        displayInfo.hasTouchScreen = [UIDevice currentDevice].userInterfaceIdiom == UIUserInterfaceIdiomPhone || [UIDevice currentDevice].userInterfaceIdiom == UIUserInterfaceIdiomPad;
        displayInfo.usesScreenEdgePadding = !displayInfo.hasSmallScreen;
        
        auto rect = screens[i].bounds;
        displayInfo.frame.x = rect.origin.x;
        displayInfo.frame.y = rect.origin.y;
        displayInfo.frame.width = rect.size.width;
        displayInfo.frame.height = rect.size.height;
    }
}

void DisplayInfos::SortLeftToRight()
{
    std::sort(begin(), end(), [](const DisplayInfo& a, const DisplayInfo& b) {return a.frame.x < b.frame.x;});
}
