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
#include "finjin/common/Error.hpp"
#include "finjin/common/StaticUnorderedMap.hpp"
#include "XcbWindow.hpp"
#include <xcb/xcb.h>
#include <xcb/xcb_ewmh.h>


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    class XcbConnection
    {
    public:
        XcbConnection();
        ~XcbConnection();

        enum class CreateResult
        {
            SUCCESS,
            FAILED_TO_CONNECT_TO_DISPLAY_SERVER,
            FAILED_TO_INITIALIZE_XCB_DATA
        };
        CreateResult Create(const Utf8String& displayName);
        void Create(const Utf8String& displayName, Error& error);

        void Destroy();

        void AddWindow(xcb_window_t window, XcbWindow* windowPointer, Error& error);
        void RemoveWindow(xcb_window_t window);

        xcb_screen_t* GetDefaultScreen();

        void GetAtomName(Utf8String& result, xcb_atom_t atom, Error& error) const;

        uint32_t GenerateID();

        void Flush();

        bool IsStringType(xcb_atom_t type) const;

        static void HandleEvents(bool queuedOnly = true);

        static bool GetDefaultDisplayName(Utf8String& result);
        static void GetDefaultDisplayName(Utf8String& result, Error& error);

        enum class GetOrCreateResult
        {
            SUCCESS,
            FAILED_TO_GET_DEFAULT_DISPLAY_NAME,
            FAILED_TO_CREATE_CONNECTION
        };
        static GetOrCreateResult GetOrCreate(std::shared_ptr<XcbConnection>& connection, const Utf8String& displayName);
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

        using WindowPointerLookup = StaticUnorderedMap<xcb_window_t, XcbWindow*, EngineConstants::MAX_WINDOWS, EngineConstants::MAX_WINDOWS * 2 + 1>;
        WindowPointerLookup windowLookup; //Pointers to created windows
    };

    using XcbConnectionLookup = StaticUnorderedMap<Utf8String, std::weak_ptr<XcbConnection>, 8, 13>;

} }
