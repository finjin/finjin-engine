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
#include "OSWindow.hpp"
#include "finjin/common/DebugLog.hpp"
#include "finjin/common/Settings.hpp"
#include "finjin/engine/PlatformCapabilities.hpp"
#include <Windows.h>
#include <shellapi.h>
#include <vector>

using namespace Finjin::Engine;


//Macros------------------------------------------------------------------------
#define BORDERLESS_STYLE WS_POPUP | WS_VISIBLE
#define BORDERED_STYLE WS_OVERLAPPEDWINDOW | WS_VISIBLE

#define USE_MESSAGE_BOX_FOR_MESSAGES 1


//Local types-------------------------------------------------------------------
struct OSWindow::Impl : public AllocatedClass
{
    Impl(Allocator* allocator, OSWindow* osWindow, void* clientData);

    bool IsMaximized() const;

    OSWindow* osWindow; //Pointer to the OSWindow that owns this Impl

    Utf8String internalName;
    Setting<OSWindowSize> lockSize;
    OSWindowSize minSize;
    HWND windowHandle;
    void* clientData;

    WindowSize windowSize;
    StaticVector<OSWindowEventListener*, EngineConstants::MAX_WINDOW_LISTENERS> listeners;

    OSWindowRect sizeMoveStart;
    bool creating;
    bool inSizeMove;
    bool hasFocus;

    std::vector<wchar_t> droppedFileCharBuffer;
    std::vector<Path> droppedFiles;
};


//Local functions---------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    auto osWindow = reinterpret_cast<OSWindow*>(GetWindowLongPtrW(hWnd, GWLP_USERDATA));
    auto impl = osWindow ? osWindow->GetImpl() : nullptr;

    switch (message)
    {
        case WM_CREATE:
        {
            auto cs = reinterpret_cast<CREATESTRUCT*>(lParam);
            SetWindowLongPtrW(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(cs->lpCreateParams));

            DragAcceptFiles(hWnd, TRUE);

            break;
        }
        case WM_DESTROY:
        {
            break;
        }
        case WM_CLOSE:
        {
            osWindow->NotifyListenersClosing();
            break;
        }
        case WM_ENTERSIZEMOVE:
        {
            impl->sizeMoveStart = osWindow->GetRect();
            impl->inSizeMove = true;
            break;
        }
        case WM_EXITSIZEMOVE:
        {
            impl->inSizeMove = false;

            auto sizeMoveStop = osWindow->GetRect();

            if (impl->sizeMoveStart.GetPosition() != sizeMoveStop.GetPosition())
            {
                osWindow->GetWindowSize().WindowMoved(impl->IsMaximized());

                for (auto listener : impl->listeners)
                    listener->WindowMoved(osWindow);
            }

            if (impl->sizeMoveStart.GetSize() != sizeMoveStop.GetSize())
            {
                osWindow->GetWindowSize().WindowResized(impl->IsMaximized());

                for (auto listener : impl->listeners)
                    listener->WindowResized(osWindow);
            }

            break;
        }
        case WM_SIZE:
        {
            /*switch (wParam)
            {
                case SIZE_RESTORED:
                case SIZE_MINIMIZED:
                case SIZE_MAXIMIZED:
                case SIZE_MAXSHOW:
                case SIZE_MAXHIDE:
            }*/
            auto width = LOWORD(lParam);
            auto height = HIWORD(lParam);

            FINJIN_DEBUG_LOG_INFO("WM_SIZE: %d x %d", width, height);

            if (!impl->creating && !impl->inSizeMove)
            {
                if (wParam == SIZE_MAXIMIZED)
                    FINJIN_DEBUG_LOG_INFO("WM_SIZE: It is maximized");

                osWindow->GetWindowSize().WindowResized(impl->IsMaximized());

                for (auto listener : impl->listeners)
                    listener->WindowResized(osWindow);
            }

            break;
        }
        case WM_MOVE:
        {
            //auto x = LOWORD(lParam);
            //auto y = HIWORD(lParam);

            if (!impl->creating && !impl->inSizeMove && !impl->IsMaximized())
            {
                osWindow->GetWindowSize().WindowMoved(impl->IsMaximized());

                for (auto listener : impl->listeners)
                    listener->WindowMoved(osWindow);
            }

            break;
        }
        case WM_SETFOCUS:
        {
            impl->hasFocus = true;
            break;
        }
        case WM_KILLFOCUS:
        {
            impl->hasFocus = false;
            break;
        }
        case WM_GETMINMAXINFO:
        {
            if (osWindow != nullptr)
            {
                auto lpMMI = reinterpret_cast<LPMINMAXINFO>(lParam);
                if (impl->lockSize.IsSet())
                {
                    lpMMI->ptMinTrackSize.x = static_cast<LONG>(impl->lockSize.value.width);
                    lpMMI->ptMinTrackSize.y = static_cast<LONG>(impl->lockSize.value.height);
                    lpMMI->ptMaxTrackSize.x = static_cast<LONG>(impl->lockSize.value.width);
                    lpMMI->ptMaxTrackSize.y = static_cast<LONG>(impl->lockSize.value.height);
                }
                else
                {
                    lpMMI->ptMinTrackSize.x = static_cast<LONG>(impl->minSize.width);
                    lpMMI->ptMinTrackSize.y = static_cast<LONG>(impl->minSize.height);
                    lpMMI->ptMaxTrackSize.x = 32000;
                    lpMMI->ptMaxTrackSize.y = 32000;
                }
            }

            break;
        }
        case WM_DROPFILES:
        {
            if (!impl->listeners.empty())
            {
                auto hDrop = reinterpret_cast<HDROP>(wParam);

                //Get count
                auto fileCount = DragQueryFileW(hDrop, (UINT)-1, nullptr, 0);
                if (fileCount > impl->droppedFiles.size())
                    impl->droppedFiles.resize(fileCount);

                //Get the dropped file names
                Path droppedFile;
                for (UINT fileIndex = 0; fileIndex < fileCount; fileIndex++)
                {
                    auto bufferLength = DragQueryFileW(hDrop, fileIndex, nullptr, 0) + 1;
                    if (bufferLength > impl->droppedFileCharBuffer.size())
                        impl->droppedFileCharBuffer.resize(bufferLength);

                    auto fileNameLength = DragQueryFileW(hDrop, fileIndex, impl->droppedFileCharBuffer.data(), static_cast<UINT>(impl->droppedFileCharBuffer.size()));
                    droppedFile.assign(impl->droppedFileCharBuffer.data(), fileNameLength);
                    impl->droppedFiles[fileIndex] = droppedFile;
                }

                DragFinish(hDrop);

                //Notify window listener
                for (auto listener : impl->listeners)
                    listener->WindowOnDropFiles(osWindow, impl->droppedFiles.data(), fileCount);
            }

            break;
        }
        case WM_ERASEBKGND:
        {
            return 1; //Indicates background has been erased and nothing else should be done
        }
    }

    return DefWindowProcW(hWnd, message, wParam, lParam);
}

//Implementation----------------------------------------------------------------

//OSWindow::Impl
OSWindow::Impl::Impl(Allocator* allocator, OSWindow* osWindow, void* clientData) : AllocatedClass(allocator)
{
    this->osWindow = osWindow;
    this->windowHandle = nullptr;
    this->clientData = clientData;
    this->creating = true;
    this->inSizeMove = false;
    this->hasFocus = false;
    this->minSize = OSWindowSize(100, 100);
    this->droppedFileCharBuffer.resize(MAX_PATH + 1); //This will be resized if necessary
    this->droppedFiles.resize(10); //This will be resized if necessary
}

bool OSWindow::Impl::IsMaximized() const
{
    WINDOWPLACEMENT placement;
    placement.length = sizeof(placement);
    GetWindowPlacement(this->windowHandle, &placement);
    return placement.showCmd == SW_MAXIMIZE;
}

//OSWindow
OSWindow::OSWindow(Allocator* allocator, void* clientData) :
    AllocatedClass(allocator),
    impl(AllocatedClass::New<Impl>(allocator, FINJIN_CALLER_ARGUMENTS, this, clientData))
{
}

OSWindow::~OSWindow()
{
    Destroy();
}

void OSWindow::Create(const Utf8String& internalName, const Utf8String& titleOrSubtitle, const Utf8String& displayName, OSWindowRect rect, const WindowSize& windowSize, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    const auto WINDOW_CLASS_NAME = L"FinjinAppWindow";

    auto hInstance = GetModuleHandleW(nullptr);

    static bool registeredClass = false;
    if (!registeredClass)
    {
        WNDCLASSW wndClass =
        {
            CS_HREDRAW | CS_VREDRAW,
            WndProc,
            0, 0,
            hInstance,
            LoadIconW(hInstance, MAKEINTRESOURCEW(IDI_APPLICATION)),
            LoadCursorW(nullptr, MAKEINTRESOURCEW(IDC_ARROW)),
            nullptr,
            nullptr,
            WINDOW_CLASS_NAME
        };
        RegisterClassW(&wndClass);
    }

    //Make copy of settings
    impl->internalName = internalName;

    impl->windowSize = windowSize;
    impl->windowSize.SetWindow(this);

    //Create the window
    Utf8StringToWideString internalNameW(internalName);
    impl->windowHandle = CreateWindowW(WINDOW_CLASS_NAME, internalNameW.c_str(), BORDERED_STYLE, rect.GetX(), rect.GetY(), static_cast<int>(rect.GetWidth()), static_cast<int>(rect.GetHeight()), 0L, 0L, hInstance, this);
    if (impl->windowHandle == nullptr)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create window '%1%'.", internalName.c_str()));
        return;
    }

    SetClientSize(rect.GetWidth(), rect.GetHeight());

    Utf8StringToWideString titleW(titleOrSubtitle);
    SetWindowTextW(impl->windowHandle, titleW.c_str());

    Raise();

    impl->hasFocus = true;

    impl->creating = false;
}

void OSWindow::Destroy()
{
    impl->listeners.clear();

    if (impl->windowHandle != nullptr)
    {
        DestroyWindow(impl->windowHandle);
        impl->windowHandle = nullptr;
    }
}

const Utf8String& OSWindow::GetInternalName() const
{
    return impl->internalName;
}

void OSWindow::ShowMessage(const Utf8String& message, const Utf8String& title)
{
#if USE_MESSAGE_BOX_FOR_MESSAGES
    Utf8StringToWideString messageW(message);
    Utf8StringToWideString titleW(title);
    MessageBoxExW(reinterpret_cast<HWND>(impl->windowHandle), messageW.c_str(), titleW.c_str(), MB_OK, 0);
#endif
}

void OSWindow::ShowErrorMessage(const Utf8String& message, const Utf8String& title)
{
#if USE_MESSAGE_BOX_FOR_MESSAGES
    Utf8StringToWideString messageW(message);
    Utf8StringToWideString titleW(title);
    MessageBoxExW(nullptr, messageW.c_str(), titleW.c_str(), MB_OK | MB_ICONERROR, 0);
#endif
}

void OSWindow::ApplyWindowSize()
{
    //No need to apply it to full screen exclusive window since the act of making it full screen exclusive changes its size
    if (impl->windowSize.IsFullScreenExclusive())
        return;

    impl->windowSize.SetApplyingToWindow(true);

    auto bounds = impl->windowSize.GetCurrentBounds();
    LimitBounds(bounds);

    /*if (bounds.IsLocked())
        LockSize(OSWindowSize(bounds.width, bounds.height));
    else
        UnlockSize();*/

    if (bounds.HasBorder())
        SetBorderedStyle();
    else
        SetBorderlessStyle();

    if (bounds.IsMaximized())
        Maximize();
    else
        Move(bounds.x, bounds.y, bounds.width, bounds.height);

    impl->windowSize.SetApplyingToWindow(false);
}

WindowSize& OSWindow::GetWindowSize()
{
    return impl->windowSize;
}

const WindowSize& OSWindow::GetWindowSize() const
{
    return impl->windowSize;
}

void OSWindow::LimitBounds(WindowBounds& bounds) const
{
    auto displayRect = GetDisplayVisibleRect();//GetDisplayRect();
    if (!displayRect.IsEmpty())
    {
        switch (impl->windowSize.GetState())
        {
            case WindowSizeState::WINDOWED_NORMAL:
            {
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
            {
                if (!displayRect.Contains(bounds.x, bounds.y))
                {
                    bounds.x = displayRect.GetX();
                    bounds.y = displayRect.GetY();
                }

                bounds.AdjustSize(displayRect.GetWidth(), displayRect.GetHeight());

                break;
            }
            case WindowSizeState::EXCLUSIVE_FULLSCREEN:
            {
                break;
            }
            default: break;
        }
    }
}

bool OSWindow::IsMaximized() const
{
    return impl->IsMaximized();
}

void OSWindow::Maximize(bool maximize)
{
    ShowWindow(impl->windowHandle, maximize ? SW_MAXIMIZE : SW_SHOWNORMAL);
}

bool OSWindow::IsMinimized() const
{
    return IsIconic(impl->windowHandle) ? true : false;
}

bool OSWindow::IsVisible() const
{
    return IsWindowVisible(impl->windowHandle) ? true : false;
}

bool OSWindow::IsSizeLocked() const
{
    return impl->lockSize.IsSet();
}

void OSWindow::LockSize(OSWindowSize size)
{
    impl->lockSize = size;
}

void OSWindow::UnlockSize(OSWindowSize minSize)
{
    impl->minSize = minSize;
    impl->lockSize.Reset();
}

bool OSWindow::HasFocus() const
{
    return impl->hasFocus;
}

void OSWindow::Raise()
{
    ShowWindow(impl->windowHandle, SW_SHOW);
    BringWindowToTop(impl->windowHandle);
}

void OSWindow::SetBorderedStyle()
{
    //auto rect = GetRect();
    SetWindowLongPtr(impl->windowHandle, GWL_STYLE, BORDERED_STYLE);
    //MoveWindow(impl->windowHandle, rect.x, rect.y, rect.width, rect.height, TRUE);
}

void OSWindow::SetBorderlessStyle()
{
    //auto rect = GetRect();
    SetWindowLongPtr(impl->windowHandle, GWL_STYLE, BORDERLESS_STYLE);
    //MoveWindow(impl->windowHandle, rect.x, rect.y, rect.width, rect.height, TRUE);
}

int OSWindow::GetDisplayID() const
{
    auto monitorHandle = GetMonitorHandle();
    DisplayInfos displays;
    PlatformCapabilities::GetInstance().GetDisplays(displays);
    for (auto& display : displays)
    {
        if (display.monitorHandle == monitorHandle)
            return static_cast<int>(display.index);
    }

    return 0;
}

float OSWindow::GetDisplayDensity() const
{
    return 1;
}

OSWindowRect OSWindow::GetDisplayRect() const
{
    OSWindowRect result(0, 0, 1920, 1080);

    auto monitorHandle = reinterpret_cast<HMONITOR>(GetMonitorHandle());
    if (monitorHandle != nullptr)
    {
        MONITORINFOEXW monitorInfo;
        monitorInfo.cbSize = sizeof(monitorInfo);
        if (GetMonitorInfoW(monitorHandle, &monitorInfo))
        {
            result.x = monitorInfo.rcMonitor.left;
            result.y = monitorInfo.rcMonitor.top;
            result.width = monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left;
            result.height = monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top;
        }
    }

    return result;
}

OSWindowRect OSWindow::GetDisplayVisibleRect() const
{
    OSWindowRect result(0, 0, 1920, 1080);

    auto monitorHandle = reinterpret_cast<HMONITOR>(GetMonitorHandle());
    if (monitorHandle != nullptr)
    {
        MONITORINFOEXW monitorInfo;
        monitorInfo.cbSize = sizeof(monitorInfo);
        if (GetMonitorInfoW(monitorHandle, &monitorInfo))
        {
            result.x = monitorInfo.rcWork.left;
            result.y = monitorInfo.rcWork.top;
            result.width = monitorInfo.rcWork.right - monitorInfo.rcWork.left;
            result.height = monitorInfo.rcWork.bottom - monitorInfo.rcWork.top;
        }
    }

    return result;
}

OSWindowRect OSWindow::GetRect() const
{
    RECT rect;
    GetWindowRect(impl->windowHandle, &rect);
    return OSWindowRect(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
}

OSWindowSize OSWindow::GetClientSize() const
{
    RECT rect;
    ::GetClientRect(impl->windowHandle, &rect);
    return OSWindowSize(rect.right - rect.left, rect.bottom - rect.top);
}

OSWindowRect OSWindow::GetClientRect() const
{
    RECT rect;
    ::GetClientRect(impl->windowHandle, &rect);
    return OSWindowRect(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
}

void OSWindow::SetClientSize(OSWindowDimension width, OSWindowDimension height)
{
    auto windowRect = GetRect();
    auto clientRect = GetClientSize();

    if (width > clientRect.width)
        windowRect.width += width - clientRect.width;
    else
        windowRect.width -= clientRect.width - width;

    if (height > clientRect.height)
        windowRect.height += height - clientRect.height;
    else
        windowRect.height -= clientRect.height - height;

    Move(windowRect.x, windowRect.y, windowRect.width, windowRect.height);
}

bool OSWindow::Move(OSWindowCoordinate x, OSWindowCoordinate y)
{
    auto rect = GetRect();
    Move(x, y, rect.GetWidth(), rect.GetHeight());
    return true;
}

void OSWindow::Move(OSWindowCoordinate x, OSWindowCoordinate y, OSWindowDimension width, OSWindowDimension height)
{
    FINJIN_DEBUG_LOG_INFO("OSWindow::Move: (%1%, %2%) @ (%3% x %4%)", x, y, width, height);
    MoveWindow(impl->windowHandle, x, y, static_cast<int>(width), static_cast<int>(height), TRUE);
}

bool OSWindow::HasWindowHandle() const
{
    return impl->windowHandle != nullptr;
}

void OSWindow::ClearSystemWindowHandle()
{
    //Do nothing
}

void* OSWindow::GetWindowHandle() const
{
    return impl->windowHandle;
}

void* OSWindow::GetMonitorHandle() const
{
    return ::MonitorFromWindow(impl->windowHandle, MONITOR_DEFAULTTONEAREST);
}

size_t OSWindow::GetWindowEventListenerCount() const
{
    return impl->listeners.size();
}

OSWindowEventListener* OSWindow::GetWindowEventListener(size_t index)
{
    return impl->listeners[index];
}

void OSWindow::AddWindowEventListener(OSWindowEventListener* listener)
{
    impl->listeners.push_back(listener);
}

void OSWindow::RemoveWindowEventListener(OSWindowEventListener* listener)
{
    auto foundAt = impl->listeners.find(listener);
    if (foundAt != impl->listeners.end())
        impl->listeners.erase(foundAt);
}

void OSWindow::NotifyListenersClosing()
{
    auto listeners = impl->listeners;
    for (auto listener : listeners)
        listener->WindowClosing(this);
}

void* OSWindow::GetClientData()
{
    return impl->clientData;
}

void OSWindow::SetClientData(void* data)
{
    impl->clientData = data;
}

bool OSWindow::CenterCursor()
{
    auto windowRect = GetRect();
    auto clientRect = GetClientRect();
    auto x = windowRect.GetX() + clientRect.GetX();
    auto y = windowRect.GetY() + clientRect.GetY();

    auto displayRect = GetDisplayRect();
    x += displayRect.x;
    y += displayRect.y;

    SetCursorPos(static_cast<int>(x + clientRect.GetWidth() / 2), static_cast<int>(y + clientRect.GetHeight()/2));

    return true;
}

OSWindow::Impl* OSWindow::GetImpl()
{
    return impl.get();
}
