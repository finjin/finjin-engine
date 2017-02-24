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


//Includes---------------------------------------------------------------------
#include "finjin/common/AllocatedClass.hpp"
#include "finjin/common/Error.hpp"
#include "finjin/engine/OSWindowDefs.hpp"
#include "finjin/engine/OSWindowEventListener.hpp"
#include "finjin/engine/DisplayInfo.hpp"
#include "finjin/engine/WindowSize.hpp"


//Classes----------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    struct OSWindowImpl;
    
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
            OSWindowRect windowRect,
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

        bool IsSizeLocked() const;
        void LockSize(OSWindowSize size);
        void UnlockSize(OSWindowSize minSize = OSWindowSize(800, 600));

        bool HasFocus() const;

        void Raise();

        int GetDisplayID() const;
        
        float GetDisplayDensity() const;

        OSWindowRect GetDisplayRect() const; //Gets the rectangle of the screen containing the window
        OSWindowRect GetDisplayVisibleRect() const; //Gets the rectangle of the screen containing the window

        OSWindowRect GetRect() const;
        
        OSWindowSize GetClientSize() const;
        
        bool Move(OSWindowCoordinate x, OSWindowCoordinate y, bool animate = false);
        
        bool HasWindowHandle() const;
        void ClearSystemWindowHandle();
        
        size_t GetWindowEventListenerCount() const;
        OSWindowEventListener* GetWindowEventListener(size_t index);
        void AddWindowEventListener(OSWindowEventListener* listener);
        void RemoveWindowEventListener(OSWindowEventListener* listener);
        void NotifyListenersClosing();

        void* GetClientData();
        void SetClientData(void* data);

        bool CenterCursor();
        
        void LimitBounds(WindowBounds& bounds) const;

        void Tick() {}
        
        //iOS-specific methods--------------------------
        float GetBackingScale() const;
        OSWindowSize GetBackingSize() const;

        bool Move(OSWindowCoordinate x, OSWindowCoordinate y, OSWindowDimension clientWidth, OSWindowDimension clientHeight, bool animate = false);
        
        void GetMetalLayerSize(OSWindowCoordinate& width, OSWindowCoordinate& height);
        
        OSWindowImpl* GetImpl();
        
    private:
        bool IsMaximized() const;
        
        std::unique_ptr<OSWindowImpl> impl;
    };
} }
