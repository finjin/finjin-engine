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


//Includes----------------------------------------------------------------------
#include "finjin/common/EnumBitwise.hpp"
#include "finjin/engine/OSWindowDefs.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    enum class WindowBoundsFlags
    {
        NONE = 0,

        BORDER = 1 << 0,
        MAXIMIZED = 1 << 1,
        LOCKED = 1 << 2
    };
    FINJIN_ENUM_BITWISE_OPERATIONS(WindowBoundsFlags)

    struct WindowBounds
    {
        WindowBounds();
        WindowBounds(OSWindowCoordinate x, OSWindowCoordinate y, OSWindowDimension width, OSWindowDimension height, OSWindowDimension clientWidth = 0, OSWindowDimension clientHeight = 0);

        void Set(OSWindowCoordinate x, OSWindowCoordinate y, OSWindowDimension width, OSWindowDimension height, WindowBoundsFlags flags);

        bool IsEmpty() const;

        bool HasBorder() const;
        bool IsMaximized() const;
        bool IsLocked() const;

        OSWindowDimension GetClientWidth() const;
        OSWindowDimension GetClientHeight() const;

        void LimitClientSize(OSWindowDimension maxWidth, OSWindowDimension maxHeight);

        void AdjustSize(OSWindowDimension newWidth, OSWindowDimension newHeight);

        void Scale(float value);

        OSWindowCoordinate x, y;
        OSWindowDimension width, height;
        OSWindowDimension clientWidth, clientHeight;
        WindowBoundsFlags flags;
    };

} }
