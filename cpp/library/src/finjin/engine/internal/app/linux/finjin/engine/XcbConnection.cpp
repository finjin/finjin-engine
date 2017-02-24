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
#include "XcbConnection.hpp"
#include <xcb/xcb_icccm.h>
#include <xcb/xinerama.h>
#include <xcb/randr.h>

using namespace Finjin::Common;
using namespace Finjin::Engine;

static XcbConnectionLookup connections;


//Implementation---------------------------------------------------------------
XcbConnection::XcbConnection()
{   
    this->c = nullptr;    
    FINJIN_ZERO_ITEM(this->ewmh);
    this->hasRandrExtension = false;
    this->randrFirstEvent = 0;
    this->defaultScreenNumber = 0;
}

XcbConnection::~XcbConnection()
{
    Destroy();
}

void XcbConnection::Create(const Utf8String& displayName, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);    
    
    this->displayName = displayName;
    
    //Connect--------------
    this->c = xcb_connect(displayName.c_str(), &this->defaultScreenNumber);    
    if (xcb_connection_has_error(this->c)) 
    {
        xcb_disconnect(this->c);
        this->c = nullptr;
        
        FINJIN_SET_ERROR(error, "Failed to connect to the display server.");
        return;
    }
     
    //Initialize utility data--------------
    auto ewmhCookie = xcb_ewmh_init_atoms(this->c, &this->ewmh);
    if (!xcb_ewmh_init_atoms_replies(&this->ewmh, ewmhCookie, nullptr))
    {
        xcb_disconnect(this->c);
        this->c = nullptr;
        
        FINJIN_SET_ERROR(error, "Failed to initialize XCB EWMH data.");
        return;
    }    
    
    //Set up extensions---------------------
    xcb_prefetch_extension_data(this->c, &xcb_randr_id);    
    auto extensionsReply = xcb_get_extension_data(this->c, &xcb_randr_id);
    if (extensionsReply != nullptr && extensionsReply->present)
    {
        this->randrFirstEvent = extensionsReply->first_event;
        this->hasRandrExtension = true;
        
        auto randrCookie = xcb_randr_query_version(this->c, XCB_RANDR_MAJOR_VERSION, XCB_RANDR_MINOR_VERSION);
        auto randrQuery = xcb_randr_query_version_reply(this->c, randrCookie, nullptr);
        if (randrQuery == nullptr || (randrQuery->major_version < 1 || (randrQuery->major_version == 1 && randrQuery->minor_version < 2))) 
            this->hasRandrExtension = false;
        free(randrQuery);
    }
    
    //Get some atoms---------------------------------------
    this->_NET_WM_STATE_FOCUSED = InternAtomReply(InternAtom("_NET_WM_STATE_FOCUSED"));
    this->WM_DELETE_WINDOW = InternAtomReply(InternAtom("WM_DELETE_WINDOW"));
    this->XdndAware = InternAtomReply(InternAtom("XdndAware"));
    this->XdndTypeList = InternAtomReply(InternAtom("XdndTypeList"));
    this->XdndSelection = InternAtomReply(InternAtom("XdndSelection"));
    this->XdndEnter = InternAtomReply(InternAtom("XdndEnter"));
    this->XdndPosition = InternAtomReply(InternAtom("XdndPosition"));
    this->XdndStatus = InternAtomReply(InternAtom("XdndStatus"));
    this->XdndLeave = InternAtomReply(InternAtom("XdndLeave"));
    this->XdndDrop = InternAtomReply(InternAtom("XdndDrop"));
    this->XdndFinished = InternAtomReply(InternAtom("XdndFinished"));
    this->XdndProxy = InternAtomReply(InternAtom("XdndProxy"));
    this->XdndActionCopy = InternAtomReply(InternAtom("XdndActionCopy"));
    this->XdndActionMove = InternAtomReply(InternAtom("XdndActionMove"));
    this->XdndActionLink = InternAtomReply(InternAtom("XdndActionLink"));
    this->XdndActionAsk = InternAtomReply(InternAtom("XdndActionAsk"));
    this->XdndActionPrivate = InternAtomReply(InternAtom("XdndActionPrivate"));
    this->CLIPBOARD_MANAGER = InternAtomReply(InternAtom("CLIPBOARD_MANAGER"));
    this->INCR = InternAtomReply(InternAtom("INCR"));
    this->TEXT = InternAtomReply(InternAtom("TEXT"));
    this->STRING = InternAtomReply(InternAtom("STRING"));
    this->TEXT_PLAIN_UTF8 = InternAtomReply(InternAtom("text/plain;charset=utf-8"));
    this->TEXT_PLAIN = InternAtomReply(InternAtom("text/plain"));
    this->TEXT_URI_LIST = InternAtomReply(InternAtom("text/uri-list"));    
}

void XcbConnection::Destroy()
{
    if (this->c != nullptr)
    {
        this->windowLookup.clear();
        
        this->defaultScreenNumber = 0;
        
        xcb_ewmh_connection_wipe(&this->ewmh);
        memset(&this->ewmh, 0, sizeof(this->ewmh));
        
        xcb_disconnect(this->c);
        this->c = nullptr;        
    }
}

void XcbConnection::AddWindow(xcb_window_t window, XcbWindow* windowPtr)
{
    this->windowLookup[window] = windowPtr;
}

void XcbConnection::RemoveWindow(xcb_window_t window)
{
    auto foundAt = this->windowLookup.find(window);
    if (foundAt != this->windowLookup.end())
        this->windowLookup.erase(foundAt);
}

void XcbConnection::GetDefaultDisplayName(Utf8String& result, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);
    
    result.clear();
    
    if (result.assign(getenv("DISPLAY")).HasError())
    {
        FINJIN_SET_ERROR(error, "Failed to assign result.");
        return;
    }
    
    auto foundAt = result.find('.');
    if (foundAt != Utf8String::npos)
        result.pop_back(result.length() - foundAt); //Remove the '.' and everything after it
}

xcb_screen_t* XcbConnection::GetDefaultScreen()
{
    return this->ewmh.screens[this->defaultScreenNumber];
}

void XcbConnection::GetAtomName(Utf8String& result, xcb_atom_t atom, Error& error) const
{
    FINJIN_ERROR_METHOD_START(error);
    
    result.clear();
    
    if (atom != 0)
    {
        auto cookie = xcb_get_atom_name(this->c, atom);
        auto reply = xcb_get_atom_name_reply(this->c, cookie, nullptr);
        if (reply != nullptr) 
        {
            if (result.assign(xcb_get_atom_name_name(reply), xcb_get_atom_name_name_length(reply)).HasError())
                FINJIN_SET_ERROR(error, "Failed to assign result.");
            free(reply);
        }        
    }
}

uint32_t XcbConnection::GenerateID()
{
    return xcb_generate_id(this->c);
}

void XcbConnection::Flush()
{
    xcb_flush(this->c);
}

bool XcbConnection::IsStringType(xcb_atom_t type) const
{    
    return 
        type == this->ewmh.UTF8_STRING ||
        type == this->STRING ||
        type == this->TEXT ||
        type == this->TEXT_PLAIN ||
        type == this->TEXT_PLAIN_UTF8 ||
        type == this->TEXT_URI_LIST;
}

void XcbConnection::HandleEvents(bool queuedOnly)
{
    xcb_generic_event_t* ev;
    for (auto connectionItem : connections)
    {
        auto connection = connectionItem.second.lock();
        if (connection != nullptr)
        {
            do
            {
                if (queuedOnly)
                    ev = xcb_poll_for_queued_event(connection->c);    
                else
                    ev = xcb_poll_for_event(connection->c);

                if (ev != nullptr)
                {
                    connection->HandleEvent(ev);
                    free(ev);
                }
            } while (ev != nullptr);            
        }
    }
}

std::shared_ptr<XcbConnection> XcbConnection::GetOrCreate(const Utf8String& requestedDisplayName, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);
    
    //Determine display name
    const Utf8String* displayName = nullptr;
    Utf8String defaultDisplayName;
    if (!requestedDisplayName.empty())
        displayName = &requestedDisplayName;        
    else
    {
        GetDefaultDisplayName(defaultDisplayName, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to get default display name.");
            return nullptr;
        }
        displayName = &defaultDisplayName;
    }

    //Look up connection
    std::shared_ptr<XcbConnection> connection;
    
    auto foundAt = connections.find(*displayName);
    if (foundAt != connections.end())
        connection = foundAt->second.lock();        
    
    //Create connection if it doesn't exist
    if (connection == nullptr)
    {    
        connection.reset(new XcbConnection());
        connection->Create(*displayName, error);
        if (error)
        {
            connection.reset();

            FINJIN_SET_ERROR(error, "Failed to create XCB connection.");
        }
        else
            connections[*displayName] = connection;
    }
    
    return connection;
}
    
xcb_intern_atom_cookie_t XcbConnection::InternAtom(const Utf8String& s)
{
    return xcb_intern_atom(this->c, false, s.length(), s.c_str());
}

xcb_atom_t XcbConnection::InternAtomReply(xcb_intern_atom_cookie_t cookie)
{
    xcb_atom_t atom = XCB_ATOM_NONE;
    
    auto reply = xcb_intern_atom_reply(this->c, cookie, nullptr);
    if (reply != nullptr) 
    {
        atom = reply->atom;
        free(reply);
    }

    return atom;
}

void XcbConnection::HandleEvent(const xcb_generic_event_t* ev, xcb_window_t window)
{    
    auto foundAt = this->windowLookup.find(window);
    if (foundAt != this->windowLookup.end())
    {
        if (!foundAt->second->HandleEvent(ev))
            this->windowLookup.erase(foundAt);
    }
}

void XcbConnection::HandleEvent(const xcb_generic_event_t* ev)
{
    switch (ev->response_type & 0x7f) 
    {
        case XCB_CONFIGURE_NOTIFY:
        {
            auto notifyEvent = reinterpret_cast<const xcb_configure_notify_event_t*>(ev);
            HandleEvent(ev, notifyEvent->window);            
            break;
        }        
        case XCB_KEY_PRESS:
        {
            auto keyEvent = reinterpret_cast<const xcb_key_press_event_t*>(ev);
            HandleEvent(ev, keyEvent->event);            
            break;
        }
        case XCB_KEY_RELEASE:
        {
            auto keyEvent = reinterpret_cast<const xcb_key_release_event_t*>(ev);            
            HandleEvent(ev, keyEvent->event);            
            break;
        }
        case XCB_BUTTON_PRESS:
        {
            auto buttonEvent = reinterpret_cast<const xcb_button_press_event_t*>(ev);
            HandleEvent(ev, buttonEvent->event);            
            break;
        }
        case XCB_BUTTON_RELEASE:
        {
            auto buttonEvent = reinterpret_cast<const xcb_button_release_event_t*>(ev);
            HandleEvent(ev, buttonEvent->event);            
            break;
        }
        case XCB_MOTION_NOTIFY:
        {
            auto motionEvent = reinterpret_cast<const xcb_motion_notify_event_t*>(ev);
            HandleEvent(ev, motionEvent->event);            
            break;
        }
        case XCB_FOCUS_IN:
        {
            auto focusEvent = reinterpret_cast<const xcb_focus_in_event_t*>(ev);
            HandleEvent(ev, focusEvent->event);            
            break;
        }
        case XCB_FOCUS_OUT:
        {
            auto focusEvent = reinterpret_cast<const xcb_focus_out_event_t*>(ev);
            HandleEvent(ev, focusEvent->event);            
            break;
        }
        case XCB_CLIENT_MESSAGE:
        {
            auto clientMessageEvent = reinterpret_cast<const xcb_client_message_event_t*>(ev);            
            HandleEvent(ev, clientMessageEvent->window);
            break;
        }        
        case XCB_PROPERTY_NOTIFY:
        {
            auto propertyEvent = reinterpret_cast<const xcb_property_notify_event_t*>(ev);            
            HandleEvent(ev, propertyEvent->window);
            break;
        }
        case XCB_SELECTION_NOTIFY:
        {
            auto selectionEvent = reinterpret_cast<const xcb_selection_notify_event_t*>(ev);
            HandleEvent(ev, selectionEvent->requestor);
            break;
        }
        default: break;
    }
}
