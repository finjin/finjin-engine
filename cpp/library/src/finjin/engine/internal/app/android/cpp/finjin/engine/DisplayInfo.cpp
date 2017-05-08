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
#include "finjin/engine/DisplayInfo.hpp"
#include "finjin/common/AndroidJniUtilities.hpp"
#include "finjin/common/AndroidUtilities.hpp"

using namespace Finjin::Engine;


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
}

//DisplayInfos
void DisplayInfos::SortLeftToRight()
{
    std::sort(begin(), end(), [](const DisplayInfo& a, const DisplayInfo& b) {return a.frame.x < b.frame.x;});
}

void DisplayInfos::Enumerate(void* applicationHandle)
{
    auto androidApp = reinterpret_cast<android_app*>(applicationHandle);
    assert(androidApp != nullptr);

    resize(1);
    auto& displayInfo = (*this)[0];

    displayInfo.index = 0;

    displayInfo.isPrimary = true;

    auto androidScreenSize = AConfiguration_getScreenSize(androidApp->config);
    displayInfo.density = AndroidUtilities::GetScreenDensity(androidApp);

    displayInfo.hasSmallScreen = androidScreenSize <= ACONFIGURATION_SCREENSIZE_NORMAL;
    displayInfo.hasTouchScreen = AndroidUtilities::HasTouchScreen(androidApp);
    displayInfo.usesScreenEdgePadding = androidScreenSize >= ACONFIGURATION_SCREENSIZE_LARGE;

    displayInfo.orientation = AndroidUtilities::GetGenericScreenOrientation(androidApp);

    AndroidJniUtilities jniUtils(androidApp);
    jniUtils.CallVoidMethod("updateDisplaySize");
    int androidDisplayWidthPixels, androidDisplayHeightPixels;
    jniUtils.GetIntField(androidDisplayWidthPixels, "displayWidthPixels");
    jniUtils.GetIntField(androidDisplayHeightPixels, "displayHeightPixels");
    displayInfo.frame.x = 0;
    displayInfo.frame.y = 0;
    displayInfo.frame.width = androidDisplayWidthPixels;
    displayInfo.frame.height = androidDisplayHeightPixels;

    displayInfo.clientFrame.x = androidApp->contentRect.left;
    displayInfo.clientFrame.y = androidApp->contentRect.top;
    displayInfo.clientFrame.width = androidApp->contentRect.right - androidApp->contentRect.left;
    displayInfo.clientFrame.height = androidApp->contentRect.bottom - androidApp->contentRect.top;
    if (displayInfo.clientFrame.width == 0 || displayInfo.clientFrame.height == 0)
        displayInfo.clientFrame = displayInfo.frame;

    displayInfo.name = "Main display";
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
