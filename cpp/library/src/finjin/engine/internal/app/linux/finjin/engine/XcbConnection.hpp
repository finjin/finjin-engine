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
#include "finjin/common/Error.hpp"
#include "XcbWindow.hpp"
#include <unordered_map>
#include <xcb/xcb.h>
#include <xcb/xcb_ewmh.h>


//Classes----------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;
    
    class XcbConnection
    {
    public: 
        XcbConnection();
        ~XcbConnection();
        
        void Create(const Utf8String& displayName, Error& error);
        void Destroy();
        
        void AddWindow(xcb_window_t window, XcbWindow* windowPtr);
        void RemoveWindow(xcb_window_t window);
        
        static void GetDefaultDisplayName(Utf8String& result, Error& error);
        
        xcb_screen_t* GetDefaultScreen();
        
        void GetAtomName(Utf8String& result, xcb_atom_t atom, Error& error) const;
        
        uint32_t GenerateID();
        
        void Flush();
        
        bool IsStringType(xcb_atom_t type) const;
        
        static void HandleEvents(bool queuedOnly = true);        
        static std::shared_ptr<XcbConnection> GetOrCreate(const Utf8String& displayName, Error& error);
        
    private:
        xcb_intern_atom_cookie_t InternAtom(const Utf8String& s);
        xcb_atom_t InternAtomReply(xcb_intern_atom_cookie_t cookie);
        
        void HandleEvent(const xcb_generic_event_t* ev, xcb_window_t window);
        void HandleEvent(const xcb_generic_event_t* ev);        
        
    public:        
        xcb_connection_t* c;
        mutable xcb_ewmh_connection_t ewmh;
        bool hasRandrExtension;
        uint8_t randrFirstEvent;
        int defaultScreenNumber;
        
        Utf8String displayName; //Xorg display name
        
        xcb_atom_t _NET_WM_STATE_FOCUSED;
        xcb_atom_t WM_DELETE_WINDOW;
        xcb_atom_t XdndAware;
        xcb_atom_t XdndTypeList;
        xcb_atom_t XdndSelection;
        xcb_atom_t XdndEnter;
        xcb_atom_t XdndPosition;
        xcb_atom_t XdndStatus;
        xcb_atom_t XdndLeave;
        xcb_atom_t XdndDrop;
        xcb_atom_t XdndFinished;
        xcb_atom_t XdndProxy;
        xcb_atom_t XdndActionCopy;
        xcb_atom_t XdndActionMove;
        xcb_atom_t XdndActionLink;
        xcb_atom_t XdndActionAsk;
        xcb_atom_t XdndActionPrivate;
        xcb_atom_t CLIPBOARD_MANAGER;
        xcb_atom_t INCR;
        xcb_atom_t TEXT;
        xcb_atom_t STRING;
        xcb_atom_t TEXT_PLAIN_UTF8;
        xcb_atom_t TEXT_PLAIN;
        xcb_atom_t TEXT_URI_LIST;
        
        using WindowLookup = std::unordered_map<xcb_window_t, XcbWindow*>;
        WindowLookup windowLookup;
    };
    
    using XcbConnectionLookup = std::unordered_map<Utf8String, std::weak_ptr<XcbConnection> >;
    
} }
