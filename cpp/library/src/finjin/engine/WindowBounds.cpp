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
#include "WindowBounds.hpp"

using namespace Finjin::Common;
using namespace Finjin::Engine;


//Implementation---------------------------------------------------------------
WindowBounds::WindowBounds()
{
    this->x = this->y = 0;
    this->width = this->height = 0;
    this->clientWidth = this->clientHeight = 0;
    this->flags = 0;
}

WindowBounds::WindowBounds(OSWindowCoordinate x, OSWindowCoordinate y, OSWindowDimension width, OSWindowDimension height, OSWindowDimension clientWidth, OSWindowDimension clientHeight)
{
    this->x = x;
    this->y = y;
    this->width = width;
    this->height = height;
    this->clientWidth = clientWidth;
    this->clientHeight = clientHeight;
    this->flags = 0;
}

void WindowBounds::Set(OSWindowCoordinate x, OSWindowCoordinate y, OSWindowDimension width, OSWindowDimension height, int flags)
{
    this->x = x;
    this->y = y;
    this->width = width;
    this->height = height;
    this->flags = flags;
}

bool WindowBounds::IsEmpty() const
{
    return this->width == 0 || this->height == 0;
}

bool WindowBounds::HasBorder() const
{
    return (this->flags & BORDER) != 0;
}

bool WindowBounds::IsMaximized() const
{
    return (this->flags & MAXIMIZED) != 0;
}

bool WindowBounds::IsLocked() const
{
    return (this->flags & LOCKED) != 0;
}

OSWindowDimension WindowBounds::GetClientWidth() const
{
    return this->clientWidth > 0 ? this->clientWidth : this->width;
}

OSWindowDimension WindowBounds::GetClientHeight() const
{
    return this->clientHeight > 0 ? this->clientHeight : this->height;
}

void WindowBounds::AdjustSize(OSWindowDimension newWidth, OSWindowDimension newHeight)
{
    //Hold onto existing values
    auto oldWidth = this->width;
    auto oldHeight = this->height;

    //Set new values
    this->width = newWidth;
    this->height = newHeight;

    //Adjust client value if necessary
    if (this->clientWidth != 0)
    {
        if (this->width < oldWidth)
            this->clientWidth -= oldWidth - this->width;
        else if (this->width > oldWidth)
            this->clientWidth += this->width - oldWidth;
    }

    if (this->clientHeight != 0)
    {
        if (this->height < oldHeight)
            this->clientHeight -= oldHeight - this->height;
        else if (this->height > oldHeight)
            this->clientHeight += this->height - oldHeight;
    }
}

void WindowBounds::Scale(float value)
{
    this->x = static_cast<OSWindowCoordinate>(RoundToInt(this->x * value));
    this->y = static_cast<OSWindowCoordinate>(RoundToInt(this->y * value));
    this->width = static_cast<OSWindowDimension>(RoundToInt(this->width * value));
    this->height = static_cast<OSWindowDimension>(RoundToInt(this->height * value));
    this->clientWidth = static_cast<OSWindowDimension>(RoundToInt(this->clientWidth * value));
    this->clientHeight = static_cast<OSWindowDimension>(RoundToInt(this->clientHeight * value));
}
