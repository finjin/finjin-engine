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
#include "finjin/common/AllocatedClass.hpp"
#include "finjin/common/Error.hpp"
#include "finjin/engine/OSWindowDefs.hpp"
#include "finjin/engine/OSWindowEventListener.hpp"
#include "finjin/engine/DisplayInfo.hpp"
#include "finjin/engine/WindowSize.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    class OSWindow : public AllocatedClass
    {
    public:
        OSWindow(Allocator* allocator, void* clientData = nullptr);
        ~OSWindow();

        void Create
            (
            const Utf8String& internalName,
            const Utf8String& titleOrSubtitle,
            const Utf8String& displayName,
            OSWindowRect rect,
            const WindowSize& windowSize,
            Error& error
            );
        void Destroy();

        const Utf8String& GetInternalName() const;

        /** Displays a message box that has the window as its parent. */
        void ShowMessage(const Utf8String& message, const Utf8String& title);

        /** Displays an error message that has the desktop as its parent. */
        static void ShowErrorMessage(const Utf8String& message, const Utf8String& title);

        void ApplyWindowSize();

        WindowSize& GetWindowSize();
        const WindowSize& GetWindowSize() const;

        bool IsMinimized() const;

        bool IsVisible() const;

        bool HasFocus() const;

        void Raise();

        int GetDisplayID() const;

        float GetDisplayDensity() const;

        OSWindowRect GetDisplayRect() const; //Gets the rectangle of the screen containing the window
        OSWindowRect GetDisplayVisibleRect() const;

        OSWindowRect GetRect() const;

        OSWindowSize GetClientSize() const;
        void SetClientSize(OSWindowDimension width, OSWindowDimension height);

        bool Move(OSWindowCoordinate x, OSWindowCoordinate y);

        bool HasWindowHandle() const;
        void ClearSystemWindowHandle();

        void* GetWindowHandle() const;

        size_t GetWindowEventListenerCount() const;
        OSWindowEventListener* GetWindowEventListener(size_t index);
        void AddWindowEventListener(OSWindowEventListener* listener);
        void RemoveWindowEventListener(OSWindowEventListener* listener);
        void NotifyListenersClosing();

        void* GetClientData();
        void SetClientData(void* data);

        bool CenterCursor();

        void LimitBounds(WindowBounds& bounds) const;

        void Tick();

        //Windows-specific methods------------------------------
        bool IsSizeLocked() const;

        void* GetMonitorHandle() const;

        void ShowTheCursor();
        void HideTheCursor();

        void SetTitle() const;

        struct Impl;

    private:
        bool IsMaximized() const;

        void LockSize(OSWindowSize size);
        void UnlockSize(OSWindowSize minSize = OSWindowSize(800, 600));

        void Maximize(bool maximize = true);

        std::unique_ptr<Impl> impl;
    };
} }
