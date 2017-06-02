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


#pragma once


//Macros------------------------------------------------------------------------
#if FINJIN_TARGET_PLATFORM == FINJIN_TARGET_PLATFORM_WINDOWS_WIN32 || \
    FINJIN_TARGET_PLATFORM == FINJIN_TARGET_PLATFORM_MACOS || \
    (FINJIN_TARGET_PLATFORM == FINJIN_TARGET_PLATFORM_LINUX && !FINJIN_TARGET_PLATFORM_IS_ANDROID)
    #define FINJIN_OS_WINDOW_COORDINATE_DEFAULT std::numeric_limits<Finjin::Engine::OSWindowCoordinate>::max()
#else
    #define FINJIN_OS_WINDOW_COORDINATE_DEFAULT 0
#endif


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

#if __APPLE__
    #if defined(__LP64__) && __LP64__
        using OSWindowCoordinate = double;
        using OSWindowDimension = double;
    #else
        using OSWindowCoordinate = float;
        using OSWindowDimension = float;
    #endif
#elif FINJIN_TARGET_PLATFORM_IS_WINDOWS_UWP
    using OSWindowCoordinate = float;
    using OSWindowDimension = float;
#else
    using OSWindowCoordinate = int32_t;
    using OSWindowDimension = int32_t;
#endif

    struct OSWindowPosition
    {
        OSWindowCoordinate x;
        OSWindowCoordinate y;

        OSWindowPosition()
        {
            this->x = this->y = 0;
        }

        OSWindowPosition(OSWindowCoordinate x, OSWindowCoordinate y)
        {
            this->x = x;
            this->y = y;
        }

        bool operator == (const OSWindowPosition& other) const
        {
            return this->x == other.x && this->y == other.y;
        }

        bool operator != (const OSWindowPosition& other) const
        {
            return !(*this == other);
        }
    };

    struct OSWindowSize
    {
        OSWindowDimension width;
        OSWindowDimension height;

        OSWindowSize()
        {
            this->width = this->height = 0;
        }

        OSWindowSize(OSWindowDimension w, OSWindowDimension h)
        {
            this->width = w;
            this->height = h;
        }

        OSWindowDimension GetWidth() const { return this->width; }
        OSWindowDimension GetHeight() const { return this->height; }

        bool IsEmpty() const { return this->width == 0 || this->height == 0; }

        bool operator == (const OSWindowSize& other) const
        {
            return this->width == other.width && this->height == other.height;
        }

        bool operator != (const OSWindowSize& other) const
        {
            return !(*this == other);
        }
    };

    struct OSWindowRect
    {
        OSWindowCoordinate x;
        OSWindowCoordinate y;
        OSWindowDimension width;
        OSWindowDimension height;

        OSWindowRect()
        {
            this->x = this->y = 0;
            this->width = this->height = 0;
        }

        OSWindowRect(OSWindowCoordinate x, OSWindowCoordinate y, OSWindowDimension w, OSWindowDimension h)
        {
            this->x = x;
            this->y = y;
            this->width = w;
            this->height = h;
        }

        OSWindowCoordinate GetX() const { return this->x; }
        OSWindowCoordinate GetY() const { return this->y; }
        OSWindowDimension GetWidth() const { return this->width; }
        OSWindowDimension GetHeight() const { return this->height; }

        OSWindowPosition GetPosition() const { return OSWindowPosition(this->x, this->y); }
        OSWindowSize GetSize() const { return OSWindowSize(this->width, this->height); }
        OSWindowPosition GetCenter() const { return OSWindowPosition(this->x + (int)this->width / 2, this->y + (int)this->height / 2); }

        OSWindowRect CenterChild(const OSWindowRect& child) const
        {
            auto childX = this->x + static_cast<OSWindowCoordinate>(this->width / 2) - static_cast<OSWindowCoordinate>(child.width / 2);
            auto childY = this->y + static_cast<OSWindowCoordinate>(this->height / 2) - static_cast<OSWindowCoordinate>(child.height / 2);
            return OSWindowRect(childX, childY, child.width, child.height);
        }

        bool IsEmpty() const
        {
            return this->width == 0 || this->height == 0;
        }

        bool Contains(OSWindowPosition pos) const
        {
            return Contains(pos.x, pos.y);
        }

        bool Contains(OSWindowCoordinate x, OSWindowCoordinate y) const
        {
            return
                x >= this->x &&
                x <= (this->x + this->width) &&
                y >= this->y &&
                y <= (this->y + this->height);
        }

        bool HasDefaultCoordinate() const
        {
            return this->x == FINJIN_OS_WINDOW_COORDINATE_DEFAULT || this->y == FINJIN_OS_WINDOW_COORDINATE_DEFAULT;
        }

        void PositionWindowRect(const OSWindowRect& screenRect)
        {
            auto defaultWindowX = screenRect.x + screenRect.width / 2 - this->width / 2;
            auto defaultWindowY = screenRect.y + screenRect.height / 2 - this->height / 2;
            if (this->x == FINJIN_OS_WINDOW_COORDINATE_DEFAULT)
                this->x = defaultWindowX;
            if (this->y == FINJIN_OS_WINDOW_COORDINATE_DEFAULT)
                this->y = defaultWindowY;
        }

        bool operator == (const OSWindowRect& other) const
        {
            return
                this->x == other.x &&
                this->y == other.y &&
                this->width == other.width &&
                this->height == other.height;
        }

        bool operator != (const OSWindowRect& other) const
        {
            return !(*this == other);
        }
    };

} }
