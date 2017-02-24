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
#include "finjin/common/Settings.hpp"
#include "finjin/engine/OSWindowDefs.hpp"
#include "finjin/engine/OSWindowEventListener.hpp"
#include "finjin/engine/DisplayInfo.hpp"
#include "finjin/engine/WindowSize.hpp"
#include "finjin/engine/XcbConnection.hpp"


//Classes----------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    class OSWindow : public AllocatedClass, public XcbWindow
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

        bool HasFocus() const;

        void Raise();

        int GetDisplayID() const;

        OSWindowRect GetDisplayRect() const; //Gets the rectangle of the screen containing the window
        OSWindowRect GetDisplayVisibleRect() const;

        OSWindowRect GetRect() const;
        
        OSWindowSize GetClientSize() const;
        void SetClientSize(OSWindowDimension width, OSWindowDimension height);
        
        bool Move(OSWindowCoordinate x, OSWindowCoordinate y);
        bool Move(OSWindowCoordinate x, OSWindowCoordinate y, OSWindowDimension clientWidth, OSWindowDimension clientHeight);
        
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
        
        //Linux-specific methods------------------------------
        xcb_window_t GetWindowHandle() const;
        xcb_connection_t* GetConnection();
        
        void Maximize(bool maximize = true);
        
        void ShowTheCursor();
        void HideTheCursor();

        bool HandleEvent(const xcb_generic_event_t* ev) override;
        
    private:
        bool IsMaximized() const;

        void LockSize(OSWindowSize size);
        void UnlockSize(OSWindowSize minSize = OSWindowSize(800, 600));
        
        void SetBorderedStyle();
        
        void SetBorderlessStyle();
        void SetFullScreenBorderlessStyle();
        
        void GetDisplayInfo(DisplayInfo& displayInfo) const;
        
        bool HasWmProperties(const xcb_atom_t* searchProperties, size_t searchPropertyCount) const;
        void ChangeNetWmState(bool set, xcb_atom_t one, xcb_atom_t two = 0);
        
        bool GetExtents();        
        void CreateBlankCursor(Error& error);
        
        void HandleGrab();
        void HandleUngrab();
        
        bool IsAcceptedDndFormat() const;
        
        std::shared_ptr<XcbConnection> connection;
        xcb_screen_t* screen;
        xcb_window_t window;
        xcb_window_t dndProxy;
        xcb_pixmap_t blankCursorPixmap;
        xcb_cursor_t blankCursor;
        struct Extents
        {
            Extents()
            {
                this->left = this->right = this->top = this->bottom = 0;
            }
            int left, right, top, bottom;
        };
        Extents extents;

        StaticVector<OSWindowEventListener*, EngineConstants::MAX_WINDOW_LISTENERS> listeners;
        void* clientData;
        Utf8String internalName;
        WindowSize windowSize;
        
        OSWindowRect lastWindowRect;
        OSWindowRect liveResizeGrabRect;
        bool isLiveResize;
        
        struct DndData
        {
            DndData()
            {
                this->dragSource = 0;
                this->targetTime = XCB_CURRENT_TIME;
                this->expecting = false;
            }
            
            xcb_window_t dragSource;
            uint32_t targetTime;
            bool expecting;
            std::vector<xcb_atom_t> types;
        };
        DndData dndData;
    };
    
} }
