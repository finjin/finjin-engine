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
#include "finjin/common/Convert.hpp"
#include "XcbConnection.hpp"
#include <xcb/xcb.h>
#include <xcb/xcb_ewmh.h>
#include <xcb/xinerama.h>
#include <xcb/randr.h>

using namespace Finjin::Engine;


//Implementation----------------------------------------------------------------
//DisplayInfo
DisplayInfo::DisplayInfo() : frame(0, 0, 0, 0), clientFrame(0, 0, 0, 0)
{
    this->index = 0;
    this->isPrimary = false;
    this->root = 0;
}

//DisplayInfos
DisplayInfos::DisplayInfos()
{
    this->isXineramaActive = false;
}

void DisplayInfos::Enumerate()
{
    Utf8String defaultDisplayName;
    if (!XcbConnection::GetDefaultDisplayName(defaultDisplayName))
        return;

    size_t okCount = 0;
    resize(EngineConstants::MAX_DISPLAYS);

    Utf8String name;
    for (size_t displayIndex = 0; displayIndex < EngineConstants::MAX_DISPLAYS && okCount < size(); displayIndex++)
    {
        name = ":";
        name += Convert::ToString(displayIndex);

        std::shared_ptr<XcbConnection> connection;
        XcbConnection::GetOrCreate(connection, name);
        if (connection != nullptr)
        {
            auto defaultScreen = connection->GetDefaultScreen();

            bool isRandrPrimaryScreen = false;
            if (connection->hasRandrExtension)
            {
                auto primaryCookie = xcb_randr_get_output_primary(connection->c, defaultScreen->root);
                auto resourcesCookie = xcb_randr_get_screen_resources(connection->c, defaultScreen->root);
                auto primaryReply = xcb_randr_get_output_primary_reply(connection->c, primaryCookie, nullptr);
                if (primaryReply != nullptr)
                {
                    auto resourcesReply = xcb_randr_get_screen_resources_reply(connection->c, resourcesCookie, nullptr);
                    if (resourcesReply != nullptr)
                    {
                        auto timestamp = resourcesReply->config_timestamp;

                        //Check outputs--------------
                        auto outputCount = xcb_randr_get_screen_resources_outputs_length(resourcesReply);
                        auto outputs = xcb_randr_get_screen_resources_outputs(resourcesReply);
                        for (int i = 0; i < outputCount; i++)
                        {
                            auto outputReply = xcb_randr_get_output_info_reply(connection->c, xcb_randr_get_output_info_unchecked(connection->c, outputs[i], timestamp), nullptr);
                            if (outputReply != nullptr)
                            {
                                //Utf8String outputName((const char*)xcb_randr_get_output_info_name(outputReply), xcb_randr_get_output_info_name_length(outputReply));
                                if (outputReply->crtc != XCB_NONE)
                                {
                                    //It's connected
                                    if (outputs[i] == primaryReply->output)
                                    {
                                        isRandrPrimaryScreen = true;
                                        break;
                                    }
                                }

                                free(outputReply);
                            }
                        }
                        free(resourcesReply);
                    }
                    free(primaryReply);
                }
            }

            //Get screen frame
            OSWindowRect screenFrame;
            auto geomReply = xcb_get_geometry_reply(connection->c, xcb_get_geometry(connection->c, defaultScreen->root), nullptr);
            if (geomReply != nullptr)
            {
                screenFrame.x = static_cast<OSWindowCoordinate>(geomReply->x);
                screenFrame.y = static_cast<OSWindowCoordinate>(geomReply->y);
                screenFrame.width = static_cast<OSWindowDimension>(geomReply->width);
                screenFrame.height = static_cast<OSWindowDimension>(geomReply->height);

                free(geomReply);
            }
            else
            {
                screenFrame.x = static_cast<OSWindowCoordinate>(0);
                screenFrame.y = static_cast<OSWindowCoordinate>(0);
                screenFrame.width = static_cast<OSWindowDimension>(defaultScreen->width_in_pixels);
                screenFrame.height = static_cast<OSWindowDimension>(defaultScreen->height_in_pixels);
            }

            //Get work area frame
            OSWindowRect workAreaFrame;
            auto workAreaCookie = xcb_get_property(connection->c, 0, defaultScreen->root, connection->ewmh._NET_WORKAREA, XCB_ATOM_CARDINAL, 0, 1000);
            auto workAreaReply = xcb_get_property_reply(connection->c, workAreaCookie, nullptr);
            if (workAreaReply != nullptr)
            {
                if (workAreaReply->value_len >= 4)
                {
                    auto values = static_cast<const uint32_t*>(xcb_get_property_value(workAreaReply));
                    workAreaFrame.x = values[0];
                    workAreaFrame.y = values[1];
                    workAreaFrame.width = values[2];
                    workAreaFrame.height = values[3];
                }

                free(workAreaReply);
            }

            auto xineramaActiveCookie = xcb_xinerama_is_active(connection->c);
            auto xineramaActiveReply = xcb_xinerama_is_active_reply(connection->c, xineramaActiveCookie, nullptr);
            if (xineramaActiveReply != nullptr)
            {
                this->isXineramaActive = xineramaActiveReply->state == 1;
                free(xineramaActiveReply);
            }

            if (this->isXineramaActive)
            {
                auto queryScreensCookie = xcb_xinerama_query_screens(connection->c);
                auto queryScreensReply = xcb_xinerama_query_screens_reply(connection->c, queryScreensCookie, nullptr);
                if (queryScreensReply != nullptr)
                {
                    auto xineramaScreenIterator = xcb_xinerama_query_screens_screen_info_iterator(queryScreensReply);
                    while (xineramaScreenIterator.rem > 0 && okCount < size())
                    {
                        auto& displayInfo = (*this)[okCount];

                        displayInfo.index = okCount;

                        if (connection->hasRandrExtension)
                            displayInfo.isPrimary = isRandrPrimaryScreen;
                        else
                            displayInfo.isPrimary = xineramaScreenIterator.data->x_org == 0 && xineramaScreenIterator.data->y_org == 0;

                        displayInfo.frame.x = static_cast<OSWindowCoordinate>(xineramaScreenIterator.data->x_org);
                        displayInfo.frame.y = static_cast<OSWindowCoordinate>(xineramaScreenIterator.data->y_org);
                        displayInfo.frame.width = static_cast<OSWindowDimension>(xineramaScreenIterator.data->width);
                        displayInfo.frame.height = static_cast<OSWindowDimension>(xineramaScreenIterator.data->height);

                        if (!workAreaFrame.IsEmpty())
                        {
                            displayInfo.clientFrame.x = displayInfo.frame.x + workAreaFrame.x;
                            displayInfo.clientFrame.y = displayInfo.frame.y + workAreaFrame.y;
                            displayInfo.clientFrame.width = displayInfo.frame.width - (screenFrame.width - workAreaFrame.width);
                            displayInfo.clientFrame.height = displayInfo.frame.height - (screenFrame.height - workAreaFrame.height);
                        }
                        else
                            displayInfo.clientFrame = displayInfo.frame;

                        displayInfo.name = name;

                        displayInfo.root = defaultScreen->root;

                        okCount++;

                        xcb_xinerama_screen_info_next(&xineramaScreenIterator);
                    }

                    free(queryScreensReply);
                }
            }
            else
            {
                auto& displayInfo = (*this)[okCount];

                displayInfo.index = okCount;

                if (connection->hasRandrExtension)
                    displayInfo.isPrimary = isRandrPrimaryScreen;
                else
                    displayInfo.isPrimary = name == defaultDisplayName;

                displayInfo.frame = screenFrame;

                if (!workAreaFrame.IsEmpty())
                    displayInfo.clientFrame = workAreaFrame;
                else
                    displayInfo.clientFrame = displayInfo.frame;

                displayInfo.name = name;

                displayInfo.root = defaultScreen->root;

                okCount++;
            }
        }
    }

    resize(okCount);
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
