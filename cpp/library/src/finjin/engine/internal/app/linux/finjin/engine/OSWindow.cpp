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
#include "finjin/engine/OSWindow.hpp"
#include "finjin/engine/PlatformCapabilities.hpp"
#include <xcb/xcb_icccm.h>
#include <xcb/xinerama.h>

using namespace Finjin::Engine;


//Macros------------------------------------------------------------------------
#define MOUSE_POINTER_ID 0

#define MAX_PROPERTY_SIZE 100000
#define DND_VERSION 4


//Local functions---------------------------------------------------------------
static OSWindowEventListener::Buttons XcbMouseButtonToOSWindowEventListenerButton(int xcbButton)
{
    switch (xcbButton)
    {
        case XCB_BUTTON_INDEX_1: return OSWindowEventListener::Buttons(1 << 0, 1 << 0); //Left mouse button
        case XCB_BUTTON_INDEX_2: return OSWindowEventListener::Buttons(1 << 2, 1 << 2); //Middle mouse button
        case XCB_BUTTON_INDEX_3: return OSWindowEventListener::Buttons(1 << 1, 1 << 1); //Right mouse button
        case 8: return OSWindowEventListener::Buttons(1 << 3, 1 << 3); //Left side button
        case 9: return OSWindowEventListener::Buttons(1 << 4, 1 << 4); //Right side button
        default: return 0;
    }
}

//Implementation----------------------------------------------------------------
OSWindow::OSWindow(Allocator* allocator, void* clientData) : AllocatedClass(allocator), internalName(allocator)
{
    this->window = 0;
    this->dndProxy = 0;
    this->blankCursorPixmap = 0;
    this->blankCursor = 0;

    this->clientData = clientData;

    this->isLiveResize = false;
}

OSWindow::~OSWindow()
{
}

void OSWindow::Create
    (
    const Utf8String& internalName,
    const Utf8String& titleOrSubtitle,
    const Utf8String& displayName,
    OSWindowRect rect,
    const WindowSize& windowSize,
    Error& error
    )
{
    FINJIN_ERROR_METHOD_START(error);

    if (this->internalName.assign(internalName).HasError())
    {
        FINJIN_SET_ERROR(error, "Failed to assign internal name.");
        return;
    }

    this->windowSize = windowSize;
    this->windowSize.SetWindow(this);

    //Connection-----------------
    this->connection = XcbConnection::GetOrCreate(displayName, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to connect to the display server.");
        return;
    }

    //Rectangle------------------
    if (rect.HasDefaultCoordinate())
    {
        DisplayInfos displays;
        displays.Enumerate();
        for (auto& display : displays)
        {
            if (display.isPrimary)
            {
                rect.CenterDefaultsInParent(display.clientFrame);
                break;
            }
        }
    }
    this->lastWindowRect = rect;

    //Create window--------------------
    this->screen = this->connection->GetDefaultScreen();
    this->window = this->connection->GenerateID();

    const uint32_t valueList[] =
    {
        this->screen->black_pixel,
        XCB_EVENT_MASK_EXPOSURE
            | XCB_EVENT_MASK_STRUCTURE_NOTIFY
            | XCB_EVENT_MASK_KEY_PRESS
            | XCB_EVENT_MASK_KEY_RELEASE
            | XCB_EVENT_MASK_BUTTON_PRESS
            | XCB_EVENT_MASK_BUTTON_RELEASE
            | XCB_EVENT_MASK_BUTTON_MOTION
            | XCB_EVENT_MASK_ENTER_WINDOW
            | XCB_EVENT_MASK_LEAVE_WINDOW
            | XCB_EVENT_MASK_POINTER_MOTION
            | XCB_EVENT_MASK_PROPERTY_CHANGE
            | XCB_EVENT_MASK_FOCUS_CHANGE
    };
    xcb_create_window
        (
        this->connection->c,
        XCB_COPY_FROM_PARENT,
        this->window,
        this->screen->root,
        rect.x,
        rect.y,
        rect.width,
        rect.height,
        0,
        XCB_WINDOW_CLASS_INPUT_OUTPUT,
        this->screen->root_visual,
        XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK,
        valueList
        );

    GetExtents();

    //Create cursors--------------------
    CreateBlankCursor(error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to create blank cursor.");
        return;
    }

    this->connection->AddWindow(this->window, this, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to add window to connection.");
        return;
    }

    //Set up various properties-------------------

    //Set title
    xcb_change_property(this->connection->c, XCB_PROP_MODE_REPLACE, this->window, this->connection->ewmh._NET_WM_NAME, this->connection->ewmh.UTF8_STRING, 8, titleOrSubtitle.length(), titleOrSubtitle.c_str());

    //Advertise various supported properties
    xcb_atom_t advertisedProperties[3];
    int advertisedPropertyCount = 0;
    advertisedProperties[advertisedPropertyCount++] = this->connection->WM_DELETE_WINDOW;
    xcb_change_property(this->connection->c, XCB_PROP_MODE_REPLACE, this->window, this->connection->ewmh.WM_PROTOCOLS, XCB_ATOM_ATOM, 32, advertisedPropertyCount, advertisedProperties);

    xcb_atom_t dndVersion = DND_VERSION;
    xcb_change_property(this->connection->c, XCB_PROP_MODE_REPLACE, this->window, this->connection->XdndAware, XCB_ATOM_ATOM, 32, 1, &dndVersion);

    Raise(); //Performs flush
}

void OSWindow::Destroy()
{
    if (this->connection != nullptr)
    {
        if (this->blankCursor != 0)
        {
            xcb_free_cursor(this->connection->c, this->blankCursor);
            this->blankCursor = 0;
        }

        if (this->blankCursorPixmap != 0)
        {
            xcb_free_pixmap(this->connection->c, this->blankCursorPixmap);
            this->blankCursorPixmap = 0;
        }

        if (this->window != 0)
        {
            this->connection->RemoveWindow(this->window);

            xcb_destroy_window(this->connection->c, this->window);
            this->connection->Flush();

            this->window = 0;
        }

        this->connection = nullptr;
    }
}

const Utf8String& OSWindow::GetInternalName() const
{
    return this->internalName;
}

void OSWindow::ShowMessage(const Utf8String& message, const Utf8String& title)
{
    std::cout << title << ": " << message << std::endl;
}

void OSWindow::ShowErrorMessage(const Utf8String& message, const Utf8String& title)
{
    std::cout << "Error - " << title << ": " << message << std::endl;
}

void OSWindow::ApplyWindowSize()
{
    this->windowSize.SetApplyingToWindow(true);

    auto bounds = this->windowSize.GetCurrentBounds();
    LimitBounds(bounds);

    /*if (bounds.IsLocked())
        LockSize(OSWindowSize(bounds.width, bounds.height));
    else
        UnlockSize();*/

    if (bounds.HasBorder())
    {
        SetBorderedStyle();

        if (bounds.IsMaximized())
            Maximize(this->windowSize.IsWindowedMaximized());
    }
    else
    {
        if (this->windowSize.IsFullScreen())
            SetFullScreenBorderlessStyle();
        else
            SetBorderlessStyle();
    }

    if (!this->windowSize.IsFullScreen() && !bounds.IsMaximized())
        Move(bounds.x, bounds.y, bounds.GetClientWidth(), bounds.GetClientHeight());

    this->windowSize.SetApplyingToWindow(false);
}

WindowSize& OSWindow::GetWindowSize()
{
    return this->windowSize;
}

const WindowSize& OSWindow::GetWindowSize() const
{
    return this->windowSize;
}

bool OSWindow::IsMaximized() const
{
    xcb_atom_t searchProperties[] = {this->connection->ewmh._NET_WM_STATE_MAXIMIZED_HORZ, this->connection->ewmh._NET_WM_STATE_MAXIMIZED_VERT};
    return HasWmProperties(searchProperties, 2);
}

bool OSWindow::IsMinimized() const
{
    return HasWmProperties(&this->connection->ewmh._NET_WM_STATE_HIDDEN, 1);
}

void OSWindow::LockSize(OSWindowSize size)
{
    xcb_size_hints_t hints;
    hints.flags = XCB_ICCCM_SIZE_HINT_P_MIN_SIZE | XCB_ICCCM_SIZE_HINT_P_MAX_SIZE;
    hints.min_width = hints.max_width = size.width;
    hints.min_height = hints.max_height = size.height;
    xcb_change_property(this->connection->c, XCB_PROP_MODE_REPLACE, this->window, XCB_ATOM_WM_NORMAL_HINTS, XCB_ATOM_WM_SIZE_HINTS, 32, sizeof(hints) / 4, &hints);
}

void OSWindow::UnlockSize(OSWindowSize minSize)
{
    xcb_size_hints_t hints;
    hints.flags = XCB_ICCCM_SIZE_HINT_P_MIN_SIZE | XCB_ICCCM_SIZE_HINT_P_MAX_SIZE;
    hints.min_width = minSize.width;
    hints.min_height = minSize.height;
    hints.max_width = hints.max_height = 32000;
    xcb_change_property(this->connection->c, XCB_PROP_MODE_REPLACE, this->window, XCB_ATOM_WM_NORMAL_HINTS, XCB_ATOM_WM_SIZE_HINTS, 32, sizeof(hints) / 4, &hints);
}

bool OSWindow::HasFocus() const
{
    return HasWmProperties(&this->connection->_NET_WM_STATE_FOCUSED, 1);
}

void OSWindow::Raise()
{
    xcb_map_window(this->connection->c, this->window);
    this->connection->Flush();
}

void OSWindow::SetBorderedStyle()
{
    ChangeNetWmState(false, this->connection->ewmh._NET_WM_STATE_FULLSCREEN);
    xcb_change_property(this->connection->c, XCB_PROP_MODE_REPLACE, this->window, this->connection->ewmh._NET_WM_WINDOW_TYPE, XCB_ATOM_ATOM, 32, 1, &this->connection->ewmh._NET_WM_WINDOW_TYPE_NORMAL);
    this->connection->Flush();
}

void OSWindow::SetBorderlessStyle()
{
    ChangeNetWmState(false, this->connection->ewmh._NET_WM_STATE_FULLSCREEN);
    xcb_change_property(this->connection->c, XCB_PROP_MODE_REPLACE, this->window, this->connection->ewmh._NET_WM_WINDOW_TYPE, XCB_ATOM_ATOM, 32, 1, &this->connection->ewmh._NET_WM_WINDOW_TYPE_DND);
    this->connection->Flush();
}

void OSWindow::SetFullScreenBorderlessStyle()
{
    ChangeNetWmState(true, this->connection->ewmh._NET_WM_STATE_FULLSCREEN);
    xcb_change_property(this->connection->c, XCB_PROP_MODE_REPLACE, this->window, this->connection->ewmh._NET_WM_WINDOW_TYPE, XCB_ATOM_ATOM, 32, 1, &this->connection->ewmh._NET_WM_WINDOW_TYPE_NORMAL);
    this->connection->Flush();
}

int OSWindow::GetDisplayID() const
{
    DisplayInfos displays;
    PlatformCapabilities::GetInstance().GetDisplays(displays);
    if (displays.isXineramaActive)
    {
        //Return value is an index
        for (auto& display : displays)
        {
            if (display.isPrimary)
                return display.index;
        }

        return 0;
    }
    else
    {
        //Return value is a window identifier
        for (auto& display : displays)
        {
            if (display.root == this->screen->root)
                return display.root;
        }

        return 0;
    }
}

float OSWindow::GetDisplayDensity() const
{
    return 1.0f;
}

void OSWindow::GetDisplayInfo(DisplayInfo& displayInfo) const
{
    DisplayInfos displays;
    PlatformCapabilities::GetInstance().GetDisplays(displays);
    if (displays.isXineramaActive)
    {
        auto windowRect = GetRect();
        auto windowCenter = windowRect.GetCenter();

        //NOTE: This isn't really the correct way to determine which display "owns" a particular window
        for (auto& display : displays)
        {
            if (display.frame.Contains(windowCenter))
            {
                displayInfo = display;
                break;
            }
        }
    }
    else
    {
        for (auto& display : displays)
        {
            if (display.root == this->screen->root)
            {
                displayInfo = display;
                break;
            }
        }
    }

    if ((displayInfo.clientFrame.GetWidth() == 0 || displayInfo.clientFrame.GetHeight() == 0) && !displays.empty())
    {
        displayInfo = displays[0];
    }
}

OSWindowRect OSWindow::GetDisplayRect() const
{
    DisplayInfo displayInfo;
    GetDisplayInfo(displayInfo);
    return displayInfo.frame;
}

OSWindowRect OSWindow::GetDisplayVisibleRect() const
{
    DisplayInfo displayInfo;
    GetDisplayInfo(displayInfo);
    return displayInfo.clientFrame;
}

OSWindowRect OSWindow::GetRect() const
{
    OSWindowRect rect;

    //Get position
    {
        //Get the top-most window
        xcb_window_t topLevelWindow = 0;
        auto nextWindow = this->window;
        while (nextWindow != this->screen->root)
        {
            topLevelWindow = nextWindow;

            auto treeCookie = xcb_query_tree(this->connection->c, topLevelWindow);
            auto treeReply = xcb_query_tree_reply(this->connection->c, treeCookie, nullptr);
            if (treeReply != nullptr)
            {
                nextWindow = treeReply->parent;

                free(treeReply);
            }
            else
                break;
        }

        auto geomCookie = xcb_get_geometry(this->connection->c, topLevelWindow);
        auto geomReply = xcb_get_geometry_reply(this->connection->c, geomCookie, nullptr);
        if (geomReply != nullptr)
        {
            rect.x = geomReply->x;
            rect.y = geomReply->y;

            free(geomReply);
        }
    }

    //Get size
    {
        auto geomCookie = xcb_get_geometry(this->connection->c, this->window);
        auto geomReply = xcb_get_geometry_reply(this->connection->c, geomCookie, nullptr);
        if (geomReply != nullptr)
        {
            rect.width = geomReply->width;
            rect.height = geomReply->height;

            free(geomReply);
        }
    }

    return rect;
}

OSWindowSize OSWindow::GetClientSize() const
{
    auto clientRect = GetRect();

    if (this->windowSize.GetCurrentBounds().HasBorder())
    {
        //clientRect.y += this->extents.top;
        clientRect.height -= this->extents.top;
        clientRect.height -= this->extents.bottom;

        //clientRect.x += this->extents.left;
        clientRect.width -= this->extents.left;
        clientRect.width -= this->extents.right;
    }

    return OSWindowSize(clientRect.width, clientRect.height);
}

void OSWindow::SetClientSize(OSWindowDimension width, OSWindowDimension height)
{
    if (this->windowSize.GetCurrentBounds().HasBorder())
    {
        width += this->extents.left + this->extents.right;
        height += this->extents.top + this->extents.bottom;
    }

    const uint32_t valueList[] = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
    xcb_configure_window(this->connection->c, this->window, XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT, valueList);
    this->connection->Flush();

    this->lastWindowRect = GetRect();
}

bool OSWindow::Move(OSWindowCoordinate x, OSWindowCoordinate y)
{
    const uint32_t valueList[] = {static_cast<uint32_t>(x), static_cast<uint32_t>(y)};
    xcb_configure_window(this->connection->c, this->window, XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y, valueList);
    this->connection->Flush();

    this->lastWindowRect = GetRect();

    return true;
}

bool OSWindow::Move(OSWindowCoordinate x, OSWindowCoordinate y, OSWindowDimension clientWidth, OSWindowDimension clientHeight)
{
    OSWindowDimension width = clientWidth;
    OSWindowDimension height = clientHeight;

    if (this->windowSize.GetCurrentBounds().HasBorder())
    {
        width += this->extents.left + this->extents.right;
        height += this->extents.top + this->extents.bottom;
    }

    const uint32_t valueList[] =
    {
        static_cast<uint32_t>(x),
        static_cast<uint32_t>(y),
        static_cast<uint32_t>(width),
        static_cast<uint32_t>(height)
    };
    xcb_configure_window(this->connection->c, this->window, XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y | XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT, valueList);
    this->connection->Flush();

    this->lastWindowRect = GetRect();

    return true;
}

bool OSWindow::HasWindowHandle() const
{
    return this->window != 0;
}

void OSWindow::ClearSystemWindowHandle()
{
    //Do nothing
}

size_t OSWindow::GetWindowEventListenerCount() const
{
    return this->listeners.size();
}

OSWindowEventListener* OSWindow::GetWindowEventListener(size_t index)
{
    return this->listeners[index];
}

void OSWindow::AddWindowEventListener(OSWindowEventListener* listener)
{
    this->listeners.push_back(listener);
}

void OSWindow::RemoveWindowEventListener(OSWindowEventListener* listener)
{
    auto foundAt = this->listeners.find(listener);
    if (foundAt != this->listeners.end())
        this->listeners.erase(foundAt);
}

void OSWindow::NotifyListenersClosing()
{
    auto listeners = this->listeners;
    for (auto listener : listeners)
        listener->WindowClosing(this);
}

void* OSWindow::GetClientData()
{
    return this->clientData;
}

void OSWindow::SetClientData(void* data)
{
    this->clientData = data;
}

bool OSWindow::CenterCursor()
{
    auto geomCookie = xcb_get_geometry(this->connection->c, this->window);
    auto geomReply = xcb_get_geometry_reply(this->connection->c, geomCookie, nullptr);
    if (geomReply != nullptr)
    {
        auto width = geomReply->width;
        auto height = geomReply->height;
        xcb_warp_pointer(this->connection->c, this->window, this->window, 0, 0, width, height, width/2, height/2);
        this->connection->Flush();

        free(geomReply);
    }

    return true;
}

void OSWindow::LimitBounds(WindowBounds& bounds) const
{
    switch (this->windowSize.GetState())
    {
        case WindowSizeState::WINDOWED_NORMAL:
        {
            auto displayRect = GetDisplayVisibleRect();

            if (bounds.x == FINJIN_OS_WINDOW_COORDINATE_DEFAULT || bounds.y == FINJIN_OS_WINDOW_COORDINATE_DEFAULT)
            {
                OSWindowRect boundsRect(bounds.x, bounds.y, bounds.width, bounds.height);
                boundsRect.CenterDefaultsInParent(displayRect);

                bounds.x = boundsRect.x;
                bounds.y = boundsRect.y;
            }

            auto newWidth = std::min(bounds.width, displayRect.GetWidth());
            auto newHeight = std::min(bounds.height, displayRect.GetHeight());
            bounds.AdjustSize(newWidth, newHeight);

            if (!displayRect.Contains(bounds.x, bounds.y))
            {
                bounds.x = displayRect.GetX();
                bounds.y = displayRect.GetY();
            }

            break;
        }
        case WindowSizeState::WINDOWED_MAXIMIZED:
        {
            auto displayRect = GetDisplayVisibleRect();

            auto newWidth = displayRect.GetWidth();
            auto newHeight = displayRect.GetHeight();
            bounds.AdjustSize(newWidth, newHeight);

            if (!displayRect.Contains(bounds.x, bounds.y))
            {
                bounds.x = displayRect.GetX();
                bounds.y = displayRect.GetY();
            }

            break;
        }
        case WindowSizeState::BORDERLESS_FULLSCREEN:
        case WindowSizeState::EXCLUSIVE_FULLSCREEN:
        {
            auto displayRect = GetDisplayRect();

            if (!displayRect.Contains(bounds.x, bounds.y))
            {
                bounds.x = displayRect.GetX();
                bounds.y = displayRect.GetY();
            }

            bounds.AdjustSize(displayRect.GetWidth(), displayRect.GetHeight());

            break;
        }
        default: break;
    }
}

xcb_window_t OSWindow::GetWindowHandle() const
{
    return this->window;
}

xcb_connection_t* OSWindow::GetConnection()
{
    return this->connection->c;
}

void OSWindow::Maximize(bool maximize)
{
    ChangeNetWmState(maximize, this->connection->ewmh._NET_WM_STATE_MAXIMIZED_HORZ, this->connection->ewmh._NET_WM_STATE_MAXIMIZED_VERT);
    this->connection->Flush();

    this->lastWindowRect = GetRect();
}

void OSWindow::ShowTheCursor()
{
    xcb_cursor_t defaultCursor = 0;
    xcb_change_window_attributes(this->connection->c, this->window, XCB_CW_CURSOR, &defaultCursor);
    this->connection->Flush();
}

void OSWindow::HideTheCursor()
{
    xcb_change_window_attributes(this->connection->c, this->window, XCB_CW_CURSOR, &this->blankCursor);
    this->connection->Flush();
}

bool OSWindow::HandleEvent(const xcb_generic_event_t* ev)
{
    bool continueHandling = true;

    switch (ev->response_type & 0x7f)
    {
        case XCB_CONFIGURE_NOTIFY:
        {
            auto notifyEvent = reinterpret_cast<const xcb_configure_notify_event_t*>(ev);

            if (!this->isLiveResize)
            {
                if (notifyEvent->x != this->lastWindowRect.x || notifyEvent->y != this->lastWindowRect.y)
                {
                    this->lastWindowRect.x = notifyEvent->x;
                    this->lastWindowRect.y = notifyEvent->y;

                    this->windowSize.WindowMoved(IsMaximized());

                    for (auto listener : this->listeners)
                        listener->WindowMoved(this);
                }
                if (notifyEvent->width != this->lastWindowRect.width || notifyEvent->height != this->lastWindowRect.height)
                {
                    this->lastWindowRect.width = notifyEvent->width;
                    this->lastWindowRect.height = notifyEvent->height;

                    this->windowSize.WindowResized(IsMaximized());

                    for (auto listener : this->listeners)
                        listener->WindowResized(this);
                }
            }

            //std::cout << "notifyEvent received: " << (int)ev->response_type << ", " << ev->full_sequence << std::endl;

            break;
        }
        case XCB_KEY_PRESS:
        {
            auto keyEvent = reinterpret_cast<const xcb_key_press_event_t*>(ev);

            for (auto listener : this->listeners)
                listener->WindowOnKeyDown(this, 0, keyEvent->detail, (keyEvent->state & XCB_KEY_BUT_MASK_CONTROL) != 0, (keyEvent->state & XCB_KEY_BUT_MASK_SHIFT) != 0, (keyEvent->state & XCB_KEY_BUT_MASK_LOCK) != 0);

            break;
        }
        case XCB_KEY_RELEASE:
        {
            auto keyEvent = reinterpret_cast<const xcb_key_release_event_t*>(ev);

            for (auto listener : this->listeners)
                listener->WindowOnKeyUp(this, 0, keyEvent->detail, (keyEvent->state & XCB_KEY_BUT_MASK_CONTROL) != 0, (keyEvent->state & XCB_KEY_BUT_MASK_SHIFT) != 0, (keyEvent->state & XCB_KEY_BUT_MASK_LOCK) != 0);

            break;
        }
        case XCB_BUTTON_PRESS:
        {
            auto buttonEvent = reinterpret_cast<const xcb_button_press_event_t*>(ev);
            InputCoordinate x(buttonEvent->event_x, InputCoordinate::Type::PIXELS, 1);
            InputCoordinate y(buttonEvent->event_y, InputCoordinate::Type::PIXELS, 1);
            auto button = buttonEvent->detail;

            switch (button)
            {
                case XCB_BUTTON_INDEX_4:
                {
                    //Wheel up
                    for (auto listener : this->listeners)
                        listener->WindowOnMouseWheel(this, PointerType::MOUSE, MOUSE_POINTER_ID, x, y, 0, 1);
                    break;
                }
                case XCB_BUTTON_INDEX_5:
                {
                    //Wheel down
                    for (auto listener : this->listeners)
                        listener->WindowOnMouseWheel(this, PointerType::MOUSE, MOUSE_POINTER_ID, x, y, 0, -1);
                    break;
                }
                default:
                {
                    //Regular button
                    auto buttons = XcbMouseButtonToOSWindowEventListenerButton(button);
                    for (auto listener : this->listeners)
                        listener->WindowOnPointerDown(this, PointerType::MOUSE, MOUSE_POINTER_ID, x, y, buttons);
                    break;
                }
            }

            break;
        }
        case XCB_BUTTON_RELEASE:
        {
            auto buttonEvent = reinterpret_cast<const xcb_button_release_event_t*>(ev);
            InputCoordinate x(buttonEvent->event_x, InputCoordinate::Type::PIXELS, 1);
            InputCoordinate y(buttonEvent->event_y, InputCoordinate::Type::PIXELS, 1);
            auto button = buttonEvent->detail;

            switch (button)
            {
                case XCB_BUTTON_INDEX_4:
                case XCB_BUTTON_INDEX_5: break; //Ignore wheel buttons
                default:
                {
                    //Regular button
                    auto buttons = XcbMouseButtonToOSWindowEventListenerButton(button).Inverse();
                    for (auto listener : this->listeners)
                        listener->WindowOnPointerUp(this, PointerType::MOUSE, MOUSE_POINTER_ID, x, y, buttons);

                    break;
                }
            }

            break;
        }
        case XCB_MOTION_NOTIFY:
        {
            auto motionEvent = reinterpret_cast<const xcb_motion_notify_event_t*>(ev);

            InputCoordinate x(motionEvent->event_x, InputCoordinate::Type::PIXELS, 1);
            InputCoordinate y(motionEvent->event_y, InputCoordinate::Type::PIXELS, 1);
            auto buttons = motionEvent->detail;

            for (auto listener : this->listeners)
                listener->WindowOnPointerMove(this, PointerType::MOUSE, MOUSE_POINTER_ID, x, y, buttons);

            break;
        }
        case XCB_FOCUS_IN:
        {
            auto focusEvent = reinterpret_cast<const xcb_focus_in_event_t*>(ev);

            if (focusEvent->mode == XCB_NOTIFY_MODE_NORMAL)
            {
                for (auto listener : this->listeners)
                    listener->WindowGainedFocus(this);
            }
            else if (focusEvent->mode == XCB_NOTIFY_MODE_GRAB)
                HandleGrab();
            else if (focusEvent->mode == XCB_NOTIFY_MODE_UNGRAB)
                HandleUngrab();

            break;
        }
        case XCB_FOCUS_OUT:
        {
            auto focusEvent = reinterpret_cast<const xcb_focus_out_event_t*>(ev);

            if (focusEvent->mode == XCB_NOTIFY_MODE_NORMAL)
            {
                for (auto listener : this->listeners)
                    listener->WindowLostFocus(this);
            }
            else if (focusEvent->mode == XCB_NOTIFY_MODE_GRAB)
                HandleGrab();
            else if (focusEvent->mode == XCB_NOTIFY_MODE_UNGRAB)
                HandleUngrab();

            break;
        }
        case XCB_CLIENT_MESSAGE:
        {
            auto clientMessageEvent = reinterpret_cast<const xcb_client_message_event_t*>(ev);
            if (clientMessageEvent->type == this->connection->ewmh.WM_PROTOCOLS)
            {
                if (clientMessageEvent->data.data32[0] == this->connection->WM_DELETE_WINDOW)
                {
                    for (auto listener : this->listeners)
                        listener->WindowClosing(this);

                    continueHandling = false;
                }
            }
            else if (clientMessageEvent->type == this->connection->XdndEnter)
            {
                this->dndData.types.clear();

                auto version = clientMessageEvent->data.data32[1] >> 24;
                if (!this->listeners.empty() && version <= DND_VERSION)
                {
                    this->dndData.dragSource = clientMessageEvent->data.data32[0];

                    if (clientMessageEvent->data.data32[1] & 1)
                    {
                        //Get the types from XdndTypeList
                        const int DND_MAX_TYPE_BYTES = 400; //100 atoms = 400 bytes
                        auto cookie = xcb_get_property(this->connection->c, false, this->dndData.dragSource, this->connection->XdndTypeList, XCB_ATOM_ATOM, 0, DND_MAX_TYPE_BYTES);
                        auto reply = xcb_get_property_reply(this->connection->c, cookie, 0);
                        if (reply != nullptr && reply->type != XCB_NONE && reply->format == 32)
                        {
                            int length = xcb_get_property_value_length(reply) / 4;
                            if (length > DND_MAX_TYPE_BYTES)
                                length = DND_MAX_TYPE_BYTES;

                            auto atoms = (xcb_atom_t *)xcb_get_property_value(reply);
                            for (int i = 0; i < length; ++i)
                                this->dndData.types.push_back(atoms[i]);
                        }

                        free(reply);
                    }
                    else
                    {
                        //Get the types from the message
                        for (int i = 2; i < 5; i++)
                        {
                            if (clientMessageEvent->data.data32[i])
                                this->dndData.types.push_back(clientMessageEvent->data.data32[i]);
                        }
                    }

                    //for (size_t i = 0; i < this->dndData.types.size(); ++i)
                        //std::cout << "DND type " << i << ": " << this->connection->GetAtomName(this->dndData.types[i]) << std::endl;
                }
            }
            else if (clientMessageEvent->type == this->connection->XdndPosition)
            {
                this->dndData.dragSource = clientMessageEvent->data.data32[0];

                if (clientMessageEvent->data.data32[3] != XCB_NONE)
                    this->dndData.targetTime = clientMessageEvent->data.data32[3];

                xcb_client_message_event_t reply;
                reply.response_type = XCB_CLIENT_MESSAGE;
                reply.window = this->dndData.dragSource;
                reply.format = 32;
                reply.sequence = 0;
                reply.type = this->connection->XdndStatus;
                reply.data.data32[0] = this->window;
                reply.data.data32[1] = (!this->listeners.empty() && IsAcceptedDndFormat()) ? 1 : 0;
                reply.data.data32[2] = 0;
                reply.data.data32[3] = 0;
                reply.data.data32[4] = this->connection->XdndActionLink;
                xcb_send_event(this->connection->c, 0, this->dndData.dragSource, XCB_EVENT_MASK_NO_EVENT, reinterpret_cast<char*>(&reply));
            }
            else if (clientMessageEvent->type == this->connection->XdndDrop)
            {
                if (clientMessageEvent->data.data32[0] == this->dndData.dragSource)
                {
                    if (clientMessageEvent->data.data32[2] != XCB_NONE)
                        this->dndData.targetTime = clientMessageEvent->data.data32[2];

                    xcb_delete_property(this->connection->c, this->window, this->connection->XdndSelection);
                    xcb_convert_selection(this->connection->c, this->window, this->connection->XdndSelection, this->connection->ewmh.UTF8_STRING, this->connection->XdndSelection, this->dndData.targetTime);
                    this->connection->Flush();
                }
            }

            break;
        }
        case XCB_PROPERTY_NOTIFY:
        {
            auto propertyEvent = reinterpret_cast<const xcb_property_notify_event_t*>(ev);
            if (propertyEvent->atom == this->connection->XdndSelection &&
                propertyEvent->state == XCB_PROPERTY_NEW_VALUE &&
                !this->listeners.empty())
            {
                this->dndData.expecting = true;
            }

            break;
        }
        case XCB_SELECTION_NOTIFY:
        {
            //auto selectionEvent = reinterpret_cast<const xcb_selection_notify_event_t*>(ev);
            if (!this->dndData.expecting || this->listeners.empty())
            {
                this->dndData.expecting = false;
                break;
            }

            //Get the size of the property data
            auto cookie = xcb_get_property(this->connection->c, false, this->window, this->connection->XdndSelection, XCB_GET_PROPERTY_TYPE_ANY, 0, 0);
            auto reply = xcb_get_property_reply(this->connection->c, cookie, 0);
            if (reply == nullptr || reply->type == XCB_NONE || reply->bytes_after == 0 || !this->connection->IsStringType(reply->type))
            {
                //Failed
                if (reply != nullptr)
                    free(reply);
            }
            else
            {
                //Hold onto some basic information
                auto replyBytesLeft = static_cast<size_t>(reply->bytes_after);
                auto startType = reply->type;
                free(reply);

                //The buffer will be used to cache the dropped data
                std::vector<char> buffer;
                buffer.resize(replyBytesLeft + 4);
                size_t bufferOffset = 0;

                //Get all the property data in a loop
                size_t propertyBufferOffset = 0;
                while (replyBytesLeft > 0)
                {
                    cookie = xcb_get_property(this->connection->c, false, this->window, this->connection->XdndSelection, XCB_GET_PROPERTY_TYPE_ANY, (int)propertyBufferOffset, buffer.size()/4);
                    reply = xcb_get_property_reply(this->connection->c, cookie, 0);
                    if (reply == nullptr || reply->type == XCB_NONE || reply->type != startType)
                    {
                        //Unexpected error

                        bufferOffset = 0;

                        if (reply != nullptr)
                            free(reply);

                        break;
                    }

                    replyBytesLeft = static_cast<size_t>(reply->bytes_after);

                    auto replyData = static_cast<const char*>(xcb_get_property_value(reply));
                    auto replyLength = static_cast<size_t>(xcb_get_property_value_length(reply));

                    //Avoid overflow
                    if (bufferOffset + replyLength > buffer.size())
                    {
                        replyLength = buffer.size() - bufferOffset;
                        replyBytesLeft = 0;
                    }

                    //Copy data
                    FINJIN_COPY_MEMORY(buffer.data() + bufferOffset, replyData, replyLength);
                    bufferOffset += replyLength;

                    //Increment offset into property buffer
                    if (replyBytesLeft > 0)
                        propertyBufferOffset += replyLength / 4; //Offset is specified in 32-bit multiples

                    free(reply);
                }

                if (bufferOffset > 0)
                {
                    Utf8String rawString(buffer.data(), bufferOffset);

                    std::vector<Path> fileNames;
                    fileNames.reserve(10);

                    Split(rawString, '\n', [&fileNames](Utf8StringView& s)
                    {
                        if (s.StartsWith("file://"))
                        {
                            fileNames.push_back(Path::GetEmpty());
                            auto& path = fileNames.back();
                            path.assign(s.begin(), s.size());
                            path.ReplaceFirst("file://", Utf8String::GetEmpty());
                        }

                        return ValueOrError<bool>();
                    });

                    if (!fileNames.empty())
                    {
                        for (auto listener : this->listeners)
                            listener->WindowOnDropFiles(this, fileNames.data(), fileNames.size());
                    }
                }
            }

            break;
        }
        default: break;
    }

    return continueHandling;
}

bool OSWindow::HasWmProperties(const xcb_atom_t* searchProperties, size_t searchPropertyCount) const
{
    size_t foundCount = 0;

    auto searchPropertiesEnd = searchProperties + searchPropertyCount;

    auto cookie = xcb_get_property(this->connection->c, 0, this->window, this->connection->ewmh._NET_WM_STATE, XCB_GET_PROPERTY_TYPE_ANY, 0, MAX_PROPERTY_SIZE);
    auto reply = xcb_get_property_reply(this->connection->c, cookie, nullptr);
    if (reply != nullptr)
    {
        if (reply->value_len > 0)
        {
            auto values = static_cast<const xcb_atom_t*>(xcb_get_property_value(reply));
            for (int i = 0; i < reply->value_len; i++)
            {
                if (std::find(searchProperties, searchPropertiesEnd, values[i]) != searchPropertiesEnd)
                    foundCount++;
            }
        }

        free(reply);
    }

    return foundCount == searchPropertyCount;
}

void OSWindow::ChangeNetWmState(bool set, xcb_atom_t one, xcb_atom_t two)
{
    const uint32_t _NET_WM_STATE_REMOVE = 0; //Remove/unset property
    const uint32_t _NET_WM_STATE_ADD = 1; //Add/set property
    //const uint32_t _NET_WM_STATE_TOGGLE = 2; //Toggle property

    xcb_client_message_event_t ev;
    ev.response_type = XCB_CLIENT_MESSAGE;
    ev.format = 32;
    ev.sequence = 0;
    ev.window = this->window;
    ev.type = this->connection->ewmh._NET_WM_STATE;
    ev.data.data32[0] = set ? _NET_WM_STATE_ADD : _NET_WM_STATE_REMOVE;
    ev.data.data32[1] = one;
    ev.data.data32[2] = two;
    ev.data.data32[3] = 0;
    ev.data.data32[4] = 0;
    xcb_send_event(this->connection->c, 0, this->screen->root, XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY | XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT, reinterpret_cast<char*>(&ev));
}

bool OSWindow::GetExtents()
{
    bool result = false;

    auto cookie = xcb_get_property(this->connection->c, 0, this->window, this->connection->ewmh._NET_FRAME_EXTENTS, XCB_ATOM_CARDINAL, 0, 32);
    auto reply = xcb_get_property_reply(this->connection->c, cookie, nullptr);
    if (reply != nullptr)
    {
        if (reply->value_len == 4)
        {
            auto values = static_cast<const uint32_t*>(xcb_get_property_value(reply));
            this->extents.left = values[0];
            this->extents.right = values[1];
            this->extents.top = values[2];
            this->extents.bottom = values[3];

            result = true;
        }

        free(reply);
    }

    return result;
}

void OSWindow::CreateBlankCursor(Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    this->blankCursorPixmap = this->connection->GenerateID();
    auto pixmapCookie = xcb_create_pixmap_checked(this->connection->c, 1, this->blankCursorPixmap, this->screen->root, 1, 1);
    auto pixmapError = xcb_request_check(this->connection->c, pixmapCookie);
    if (pixmapError != nullptr)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create pixmap: %1%", pixmapError->error_code));
        free(pixmapError);
        return;
    }

    this->blankCursor = this->connection->GenerateID();
    auto cursorCookie = xcb_create_cursor_checked(this->connection->c, this->blankCursor, this->blankCursorPixmap, this->blankCursorPixmap, 0, 0, 0, 0, 0, 0, 0, 0);
    auto cursorError = xcb_request_check(this->connection->c, cursorCookie);
    if (cursorError != nullptr)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create cursor: %1%", cursorError->error_code));
        free(cursorError);
        return;
    }
}

void OSWindow::HandleGrab()
{
    this->isLiveResize = true;
    this->liveResizeGrabRect = GetRect();
}

void OSWindow::HandleUngrab()
{
    if (this->isLiveResize)
    {
        this->isLiveResize = false;

        auto liveResizeUngrabRect = GetRect();

        if (liveResizeUngrabRect.x != this->liveResizeGrabRect.x || liveResizeUngrabRect.y != this->liveResizeGrabRect.y)
        {
            this->windowSize.WindowMoved(IsMaximized());

            for (auto listener : this->listeners)
                listener->WindowMoved(this);
        }

        if (liveResizeUngrabRect.width != this->liveResizeGrabRect.width || liveResizeUngrabRect.height != this->liveResizeGrabRect.height)
        {
            this->windowSize.WindowResized(IsMaximized());

            for (auto listener : this->listeners)
                listener->WindowResized(this);
        }
    }
}

bool OSWindow::IsAcceptedDndFormat() const
{
    for (auto type : this->dndData.types)
    {
        if (this->connection->IsStringType(type))
            return true;
    }

    return false;
}
