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
#include "finjin/common/Path.hpp"
#include "finjin/common/Utf8String.hpp"
#include "finjin/engine/PointerType.hpp"
#include "finjin/engine/InputCoordinate.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    class OSWindow;

    /** Event listener for the OSWindow class */
    class OSWindowEventListener
    {
    public:
        virtual ~OSWindowEventListener() {};

        /** Called when the window is resized */
        virtual void WindowMoved(OSWindow* osWindow) {}
        virtual void WindowResized(OSWindow* osWindow) {}

        virtual void WindowGainedFocus(OSWindow* osWindow) {}
        virtual void WindowLostFocus(OSWindow* osWindow) {}

        /** Called just before the window is closed */
        virtual void WindowClosing(OSWindow* osWindow)  {}

        virtual void WindowOnKeyDown(OSWindow* osWindow, int softwareKey, int hardwareCode, bool controlDown, bool shiftDown, bool altDown) {}
        virtual void WindowOnKeyUp(OSWindow* osWindow, int softwareKey, int hardwareCode, bool controlDown, bool shiftDown, bool altDown) {}

        struct Buttons
        {
            Buttons() { this->buttons = 0; this->used = 0; }
            Buttons(int buttons) { this->buttons = buttons; this->used = 0xffffffff; }
            Buttons(int buttons, int used) { this->buttons = buttons; this->used = used; }

            Buttons Inverse() const { return Buttons(~this->buttons, this->used); }

            bool IsUsed(size_t index) const { return (this->used & (1 << index)) != 0; }
            bool IsPressed(size_t index) const { return (this->buttons & (1 << index)) != 0; }

            int buttons;
            int used;
        };

        virtual void WindowOnMouseWheel(OSWindow* osWindow, PointerType pointerType, int pointerID, InputCoordinate x, InputCoordinate y, Buttons buttons, float wheelDelta) {}
        virtual void WindowOnPointerMove(OSWindow* osWindow, PointerType pointerType, int pointerID, InputCoordinate x, InputCoordinate y, Buttons buttons) {}
        virtual void WindowOnPointerDown(OSWindow* osWindow, PointerType pointerType, int pointerID, InputCoordinate x, InputCoordinate y, Buttons buttons) {}
        virtual void WindowOnPointerUp(OSWindow* osWindow, PointerType pointerType, int pointerID, InputCoordinate x, InputCoordinate y, Buttons buttons) {}

        virtual bool WindowOnDropFiles(OSWindow* osWindow, const Path* fileNames, size_t count) { return false; }

        inline int PackMouseButtons(bool left, bool right, bool middle, bool extended1 = false, bool extended2 = false)
        {
            int buttons = 0;
            if (left)
                buttons |= 1 << 0;
            if (right)
                buttons |= 1 << 1;
            if (middle)
                buttons |= 1 << 2;
            if (extended1)
                buttons |= 1 << 3;
            if (extended2)
                buttons |= 1 << 4;
            return buttons;
        }
    };

} }
