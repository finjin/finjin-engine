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
#include "finjin/common/DebugLog.hpp"
#include "finjin/engine/DisplayInfo.hpp"
#include "finjin/engine/PlatformCapabilities.hpp"
#include <android/native_window.h>

using namespace Finjin::Engine;


//Implementation----------------------------------------------------------------
OSWindow::OSWindow(Allocator* allocator, void* clientData) : AllocatedClass(allocator)
{
    this->windowHandle = nullptr;
    this->clientData = clientData;
}

OSWindow::~OSWindow()
{
    Destroy();
}

void OSWindow::Create(const Utf8String& internalName, const Utf8String& titleOrSubtitle, const Utf8String& displayName, OSWindowRect rect, const WindowSize& windowSize, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    this->internalName = internalName;

    this->windowSize = windowSize;
    this->windowSize.SetWindow(this);
}

void OSWindow::Destroy()
{
    this->windowHandle = nullptr;
    this->clientData = nullptr;
    this->listeners.clear();
}

const Utf8String& OSWindow::GetInternalName() const
{
    return this->internalName;
}

void OSWindow::ShowMessage(const Utf8String& message, const Utf8String& title)
{
    //TODO: Implement this
    //std::cout << title << ": " << message << std::endl;
    FINJIN_DEBUG_LOG_INFO("%s: %s", title.c_str(), message.c_str());
}

void OSWindow::ShowErrorMessage(const Utf8String& message, const Utf8String& title)
{
    //TODO: Implement this
    FINJIN_DEBUG_LOG_ERROR("Error - %s: %s", title.c_str(), message.c_str());
}

void OSWindow::ApplyWindowSize()
{
    //Do nothing
}

WindowSize& OSWindow::GetWindowSize()
{
    return this->windowSize;
}

const WindowSize& OSWindow::GetWindowSize() const
{
    return this->windowSize;
}

void OSWindow::LimitBounds(WindowBounds& bounds) const
{
    auto displayRect = GetDisplayVisibleRect();//GetDisplayRect();
    auto newWidth = displayRect.GetWidth();
    auto newHeight = displayRect.GetHeight();
    bounds.AdjustSize(newWidth, newHeight);
}

bool OSWindow::IsMaximized() const
{
    return false;
}

bool OSWindow::IsMinimized() const
{
    return false;
}

bool OSWindow::IsVisible() const
{
    return true;
}

bool OSWindow::IsSizeLocked() const
{
    return true;
}

void OSWindow::LockSize(OSWindowSize size)
{
    //Do nothing
}

void OSWindow::UnlockSize(OSWindowSize minSize)
{
    //Do nothing
}

bool OSWindow::HasFocus() const
{
    //Since there can only be a single window, it always has focus.
    //Whether the application itself has focus is determined at the application level.
    return true;
}

void OSWindow::Raise()
{
    //Do nothing
}

void OSWindow::SetBorderedStyle()
{
    //Do nothing
}

void OSWindow::SetBorderlessStyle()
{
    //Do nothing
}

int OSWindow::GetDisplayID() const
{
    return 0;
}

float OSWindow::GetDisplayDensity() const
{
    DisplayInfos displays;
    if (PlatformCapabilities::GetInstance().GetDisplays(displays))
        return displays[0].density;
    else
        return 1;
}

OSWindowRect OSWindow::GetDisplayRect() const
{
    DisplayInfos displays;
    if (PlatformCapabilities::GetInstance().GetDisplays(displays))
        return displays[0].frame;
    else
        return OSWindowRect(0, 0, 0, 0);
}

OSWindowRect OSWindow::GetDisplayVisibleRect() const
{
    DisplayInfos displays;
    if (PlatformCapabilities::GetInstance().GetDisplays(displays))
    {
        FINJIN_DEBUG_LOG_INFO("Got displays: %1% x %2%", displays[0].clientFrame.width, displays[0].clientFrame.height);
        return displays[0].clientFrame;
    }
    else
    {
        FINJIN_DEBUG_LOG_INFO("Failed to get displays");
        return OSWindowRect(0, 0, 0, 0);
    }
}

OSWindowRect OSWindow::GetRect() const
{
    auto size = GetClientSize();
    return OSWindowRect(0, 0, size.GetWidth(), size.GetHeight());
}

OSWindowSize OSWindow::GetClientSize() const
{
    return OSWindowSize(ANativeWindow_getWidth((ANativeWindow*)this->windowHandle), ANativeWindow_getHeight((ANativeWindow*)this->windowHandle));
}

bool OSWindow::Move(OSWindowCoordinate x, OSWindowCoordinate y)
{
    return false;
}

bool OSWindow::HasWindowHandle() const
{
    return this->windowHandle != nullptr;
}

void OSWindow::ClearSystemWindowHandle()
{
    this->windowHandle = nullptr;
}

void* OSWindow::GetWindowHandle() const
{
    return this->windowHandle;
}

void OSWindow::SetWindowHandle(void* windowHandle)
{
    this->windowHandle = windowHandle;
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
    return false;
}
