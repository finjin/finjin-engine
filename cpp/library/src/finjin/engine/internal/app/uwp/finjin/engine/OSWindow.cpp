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
#include "finjin/common/Chrono.hpp"
#include "finjin/common/DebugLog.hpp"
#include "finjin/common/Math.hpp"
#include "finjin/engine/PlatformCapabilities.hpp"
#include "OSWindow.hpp"
#include <ppltasks.h>

using namespace Finjin::Engine;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::Devices::Input;
using namespace Windows::Foundation;
using namespace Windows::UI::Core;
using namespace Windows::UI::ViewManagement;


//Macros------------------------------------------------------------------------
#define USE_MESSAGE_BOX_FOR_MESSAGES 0 //Disabled since the below ShowMessageDialog() does not work


//Local functions---------------------------------------------------------------
static void CommandInvokedHandler(Windows::UI::Popups::IUICommand^ command)
{
    CoreApplication::Exit();
}

static void ShowMessageDialog(const Utf8String& message, const Utf8String& title)
{
    Utf8StringToWideString messageW(message);
    Utf8StringToWideString titleW(title);

    auto messageDialog = ref new Windows::UI::Popups::MessageDialog(ref new Platform::String(messageW.c_str()), ref new Platform::String(titleW.c_str()));
    messageDialog->Commands->Append(ref new Windows::UI::Popups::UICommand(ref new Platform::String(L"Close"), ref new Windows::UI::Popups::UICommandInvokedHandler(CommandInvokedHandler)));
    messageDialog->ShowAsync();

    CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessUntilQuit);
}


//Local types-------------------------------------------------------------------
struct PointerEventArgsUtility
{
    PointerEventArgsUtility(float density, PointerEventArgs^ args)
    {
        this->pointerType = args->CurrentPoint->PointerDevice->PointerDeviceType == PointerDeviceType::Mouse ? PointerType::MOUSE : PointerType::TOUCH_SCREEN;
        this->pointerID = args->CurrentPoint->PointerId;
        this->position = args->CurrentPoint->Position;
        this->x.Set(position.X, InputCoordinate::Type::PIXELS, density);
        this->y.Set(position.Y, InputCoordinate::Type::PIXELS, density);
        this->buttons = OSWindowEventListener::PackMouseButtons(args->CurrentPoint->Properties->IsLeftButtonPressed, args->CurrentPoint->Properties->IsRightButtonPressed, args->CurrentPoint->Properties->IsMiddleButtonPressed);

    }

    PointerType pointerType;
    int pointerID;
    Point position;
    InputCoordinate x;
    InputCoordinate y;
    int buttons;
};

ref class OSWindowHandlers sealed
{
public:
    OSWindowHandlers(uintptr_t impl);

    void Tick();

    void AddHandlers(CoreWindow^ window);
    void RemoveHandlers(CoreWindow^ window);

private:
    void OnWindowSizeChanged(CoreWindow^ sender, WindowSizeChangedEventArgs^ args);
    void OnWindowClosed(CoreWindow^ sender, CoreWindowEventArgs^ args);
    void OnActivated(CoreWindow ^sender, WindowActivatedEventArgs ^args);

    void OnPointerPressed(CoreWindow^ sender, PointerEventArgs^ args);
    void OnPointerMoved(CoreWindow^ sender, PointerEventArgs^ args);
    void OnPointerReleased(CoreWindow^ sender, PointerEventArgs^ args);
    void OnPointerWheelChanged(CoreWindow^ sender, PointerEventArgs^ args);
    void OnPointerCaptureLost(CoreWindow^ sender, PointerEventArgs^ args);

    void OnKeyDown(CoreWindow^ sender, KeyEventArgs^ args);
    void OnKeyUp(CoreWindow^ sender, KeyEventArgs^ args);

    Windows::Foundation::EventRegistrationToken OnWindowSizeChangedToken;
    Windows::Foundation::EventRegistrationToken OnWindowClosedToken;
    Windows::Foundation::EventRegistrationToken OnActivatedToken;

    Windows::Foundation::EventRegistrationToken OnPointerPressedToken;
    Windows::Foundation::EventRegistrationToken OnPointerMovedToken;
    Windows::Foundation::EventRegistrationToken OnPointerReleasedToken;
    Windows::Foundation::EventRegistrationToken OnPointerWheelChangedToken;
    Windows::Foundation::EventRegistrationToken OnPointerCaptureLostToken;

    Windows::Foundation::EventRegistrationToken OnKeyDownToken;
    Windows::Foundation::EventRegistrationToken OnKeyUpToken;

    uintptr_t impl; //Pointer to OSWindow::Impl that owns this OSWindowHandlers

    bool sizeChanged; //Used for throttling the rate at which size change events are sent to OSWindow listener
    Finjin::Common::DateTime sizeChangeTime;
};

struct OSWindow::Impl : public AllocatedClass
{
    Impl(Allocator* allocator, OSWindow* osWindow, void* clientData);

    void Create(const Utf8String& internalName, const Utf8String& titleOrSubtitle, OSWindowRect rect, const WindowSize& windowSize, CoreWindow^ window, bool isMainWindow, Error& error);
    void Destroy();

    bool IsMaximized() const;

    OSWindowRect GetRect() const;
    OSWindowRect GetClientRect() const;

    void NotifyListenersClosing();

    ApplicationView^ GetApplicationViewForWindow() const;

    bool CanResizeWindows() const;

    OSWindowHandlers^ handlers; //Provides support for things that require managed code
    OSWindow* osWindow; //Pointer to the OSWindow that owns this Impl
    Utf8String subtitle;
    Utf8String internalName;
    bool isMainWindow;
    bool hasFocus;
    IUnknown* windowHandle;
    void* clientData;
    WindowSize windowSize;
    StaticVector<OSWindowEventListener*, EngineConstants::MAX_WINDOW_LISTENERS> listeners;
};


//Local functions---------------------------------------------------------------
static const DisplayInfo* GetBestDisplay(OSWindowRect windowRect)
{
    static DisplayInfos displays;
    PlatformCapabilities::GetInstance().GetDisplays(displays);
    assert(!displays.empty());
    if (displays.empty())
        return nullptr;

    //NOTE: This isn't really the correct way to determine which display "owns" a particular window
    auto windowCenter = windowRect.GetCenter();
    for (auto& display : displays)
    {
        if (display.frame.Contains(windowCenter))
            return &display;
    }

    //Return left-most display
    displays.SortLeftToRight();
    return &displays[0];
}

static int GetVirtualKey(KeyEventArgs^ args)
{
    auto virtualKey = static_cast<int>(args->VirtualKey);
    switch (virtualKey)
    {
        case VK_SHIFT: virtualKey = (args->KeyStatus.ScanCode == 0x36) ? VK_RSHIFT : VK_LSHIFT; break;
        case VK_CONTROL: virtualKey = (args->KeyStatus.IsExtendedKey) ? VK_RCONTROL : VK_LCONTROL; break;
        case VK_MENU: virtualKey = (args->KeyStatus.IsExtendedKey) ? VK_RMENU : VK_LMENU; break;
    }

    return virtualKey;
}


//Implementation----------------------------------------------------------------

//OSWindowHandlers
OSWindowHandlers::OSWindowHandlers(uintptr_t impl)
{
    this->impl = impl;

    this->sizeChanged = false;
}

void OSWindowHandlers::Tick()
{
    auto impl = reinterpret_cast<OSWindow::Impl*>(this->impl);

    if (this->sizeChanged)
    {
        auto now = Finjin::Common::DateTime::NowUtc();

        auto inactivityTime = now - this->sizeChangeTime;
        if (inactivityTime > Finjin::Common::TimeDuration::Seconds(1))
        {
            this->sizeChanged = false;

            impl->osWindow->GetWindowSize().WindowResized(impl->IsMaximized());

            for (auto listener : impl->listeners)
                listener->WindowResized(impl->osWindow);
        }
    }
}

void OSWindowHandlers::AddHandlers(CoreWindow^ window)
{
    this->OnWindowSizeChangedToken = (window->SizeChanged += ref new TypedEventHandler<CoreWindow^, WindowSizeChangedEventArgs^>(this, &OSWindowHandlers::OnWindowSizeChanged));
    this->OnWindowClosedToken = (window->Closed += ref new TypedEventHandler<CoreWindow^, CoreWindowEventArgs^>(this, &OSWindowHandlers::OnWindowClosed));
    this->OnActivatedToken = (window->Activated += ref new TypedEventHandler<CoreWindow^, WindowActivatedEventArgs^>(this, &OSWindowHandlers::OnActivated));

    this->OnPointerPressedToken = (window->PointerPressed += ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this, &OSWindowHandlers::OnPointerPressed));
    this->OnPointerMovedToken = (window->PointerMoved += ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this, &OSWindowHandlers::OnPointerMoved));
    this->OnPointerReleasedToken = (window->PointerReleased += ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this, &OSWindowHandlers::OnPointerReleased));
    this->OnPointerWheelChangedToken = (window->PointerWheelChanged += ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this, &OSWindowHandlers::OnPointerWheelChanged));
    this->OnPointerCaptureLostToken = (window->PointerCaptureLost += ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this, &OSWindowHandlers::OnPointerCaptureLost));

    this->OnKeyDownToken = (window->KeyDown += ref new TypedEventHandler<CoreWindow^, KeyEventArgs^>(this, &OSWindowHandlers::OnKeyDown));
    this->OnKeyUpToken = (window->KeyUp += ref new TypedEventHandler<CoreWindow^, KeyEventArgs^>(this, &OSWindowHandlers::OnKeyUp));
}

void OSWindowHandlers::RemoveHandlers(CoreWindow^ window)
{
    window->SizeChanged -= this->OnWindowSizeChangedToken;
    window->Closed -= this->OnWindowClosedToken;
    window->Activated -= this->OnActivatedToken;

    window->PointerPressed -= this->OnPointerPressedToken;
    window->PointerMoved -= this->OnPointerMovedToken;
    window->PointerReleased -= this->OnPointerReleasedToken;
    window->PointerWheelChanged -= this->OnPointerWheelChangedToken;
    window->PointerCaptureLost -= this->OnPointerCaptureLostToken;

    window->KeyDown -= this->OnKeyDownToken;
    window->KeyUp -= this->OnKeyUpToken;
}

void OSWindowHandlers::OnWindowSizeChanged(CoreWindow^ sender, WindowSizeChangedEventArgs^ args)
{
    FINJIN_DEBUG_LOG_INFO("Size changed to: %1% x %2%", args->Size.Width, args->Size.Height);
    auto impl = reinterpret_cast<OSWindow::Impl*>(this->impl);
    if (!impl->listeners.empty())
    {
        this->sizeChangeTime = Finjin::Common::DateTime::NowUtc();
        this->sizeChanged = true;
    }

    args->Handled = true;
}

void OSWindowHandlers::OnWindowClosed(CoreWindow^ sender, CoreWindowEventArgs^ args)
{
    FINJIN_DEBUG_LOG_INFO("Window closed");

    auto impl = reinterpret_cast<OSWindow::Impl*>(this->impl);
    impl->NotifyListenersClosing();

    args->Handled = true;
}

void OSWindowHandlers::OnActivated(CoreWindow^ sender, WindowActivatedEventArgs^ args)
{
    auto impl = reinterpret_cast<OSWindow::Impl*>(this->impl);

    impl->hasFocus = args->WindowActivationState != CoreWindowActivationState::Deactivated;
    //No need to call impl->listeners->WindowGainedFocus or impl->listeners->WindowLostFocus since the main loop handles focus changes

    args->Handled = true;
}

void OSWindowHandlers::OnPointerPressed(CoreWindow^ sender, PointerEventArgs^ args)
{
    auto impl = reinterpret_cast<OSWindow::Impl*>(this->impl);
    if (!impl->listeners.empty())
    {
        auto display = GetBestDisplay(impl->GetRect());

        PointerEventArgsUtility pointerDownArgs(display->density, args);

        for (auto listener : impl->listeners)
            listener->WindowOnPointerDown(impl->osWindow, pointerDownArgs.pointerType, pointerDownArgs.pointerID, pointerDownArgs.x, pointerDownArgs.y, pointerDownArgs.buttons);
    }

    args->Handled = true;
}

void OSWindowHandlers::OnPointerMoved(CoreWindow^ sender, PointerEventArgs^ args)
{
    auto impl = reinterpret_cast<OSWindow::Impl*>(this->impl);
    if (!impl->listeners.empty())
    {
        auto display = GetBestDisplay(impl->GetRect());

        PointerEventArgsUtility pointerMovedArgs(display->density, args);

        for (auto listener : impl->listeners)
            listener->WindowOnPointerMove(impl->osWindow, pointerMovedArgs.pointerType, pointerMovedArgs.pointerID, pointerMovedArgs.x, pointerMovedArgs.y, pointerMovedArgs.buttons);
    }

    args->Handled = true;
}

void OSWindowHandlers::OnPointerReleased(CoreWindow^ sender, PointerEventArgs^ args)
{
    auto impl = reinterpret_cast<OSWindow::Impl*>(this->impl);
    if (!impl->listeners.empty())
    {
        auto display = GetBestDisplay(impl->GetRect());

        PointerEventArgsUtility pointerUpArgs(display->density, args);

        for (auto listener : impl->listeners)
            listener->WindowOnPointerUp(impl->osWindow, pointerUpArgs.pointerType, pointerUpArgs.pointerID, pointerUpArgs.x, pointerUpArgs.y, pointerUpArgs.buttons);
    }

    args->Handled = true;
}

void OSWindowHandlers::OnPointerWheelChanged(CoreWindow^ sender, PointerEventArgs^ args)
{
    auto impl = reinterpret_cast<OSWindow::Impl*>(this->impl);
    if (!impl->listeners.empty())
    {
        auto display = GetBestDisplay(impl->GetRect());
        auto density = display->density;

        auto currentPoint = args->CurrentPoint;
        auto pointerType = currentPoint->PointerDevice->PointerDeviceType == PointerDeviceType::Mouse ? PointerType::MOUSE : PointerType::TOUCH_SCREEN;
        auto pointerID = currentPoint->PointerId;
        auto position = currentPoint->Position;
        InputCoordinate x(position.X, InputCoordinate::Type::PIXELS, density);
        InputCoordinate y(position.Y, InputCoordinate::Type::PIXELS, density);
        auto buttons = OSWindowEventListener::PackMouseButtons(currentPoint->Properties->IsLeftButtonPressed, currentPoint->Properties->IsRightButtonPressed, currentPoint->Properties->IsMiddleButtonPressed, currentPoint->Properties->IsXButton1Pressed, currentPoint->Properties->IsXButton2Pressed);
        auto wheelDelta = RoundToFloat(args->CurrentPoint->Properties->MouseWheelDelta) / 30.0f;

        for (auto listener : impl->listeners)
            listener->WindowOnMouseWheel(impl->osWindow, pointerType, pointerID, x, y, buttons, wheelDelta);
    }

    args->Handled = true;
}

void OSWindowHandlers::OnPointerCaptureLost(CoreWindow^ sender, PointerEventArgs^ args)
{
    /*auto impl = reinterpret_cast<OSWindow::Impl*>(this->impl);
    if (!impl->listeners.empty())
    {
        auto pointerType = args->CurrentPoint->PointerDevice->PointerDeviceType == PointerDeviceType::Mouse ? PointerType::MOUSE : PointerType::TOUCH_SCREEN;
        auto pointerID = args->CurrentPoint->PointerId;

        for (auto listener : impl->listeners)
            listener->WindowOnPointerCaptureLost(impl->osWindow, pointerType, pointerID);
    }*/

    args->Handled = true;
}

void OSWindowHandlers::OnKeyDown(CoreWindow^ sender, KeyEventArgs^ args)
{
    auto softwareKey = GetVirtualKey(args);
    auto hardwareCode = static_cast<int>(args->KeyStatus.ScanCode);

    auto impl = reinterpret_cast<OSWindow::Impl*>(this->impl);
    for (auto listener : impl->listeners)
        listener->WindowOnKeyDown(impl->osWindow, softwareKey, hardwareCode, false, false, false);

    args->Handled = true;
}

void OSWindowHandlers::OnKeyUp(CoreWindow^ sender, KeyEventArgs^ args)
{
    auto softwareKey = GetVirtualKey(args);
    auto hardwareCode = static_cast<int>(args->KeyStatus.ScanCode);

    auto impl = reinterpret_cast<OSWindow::Impl*>(this->impl);
    for (auto listener : impl->listeners)
        listener->WindowOnKeyUp(impl->osWindow, softwareKey, hardwareCode, false, false, false);

    args->Handled = true;
}

//OSWindow::Impl
OSWindow::Impl::Impl(Allocator* allocator, OSWindow* osWindow, void* clientData) :
    AllocatedClass(allocator),
    handlers(ref new OSWindowHandlers(reinterpret_cast<uintptr_t>(this))),
    subtitle(allocator),
    internalName(allocator)
{
    this->osWindow = osWindow;
    this->windowHandle = nullptr;
    this->isMainWindow = true;
    this->hasFocus = false;
    this->clientData = clientData;
}

void OSWindow::Impl::Create(const Utf8String& internalName, const Utf8String& titleOrSubtitle, OSWindowRect rect, const WindowSize& windowSize, CoreWindow^ window, bool isMainWindow, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    if (this->subtitle.assign(titleOrSubtitle).HasError())
    {
        FINJIN_SET_ERROR(error, "Failed to assign subtitle.");
        return;
    }

    if (this->internalName.assign(internalName).HasError())
    {
        FINJIN_SET_ERROR(error, "Failed to assign internal name.");
        return;
    }

    this->windowSize = windowSize;
    this->windowSize.SetWindow(this->osWindow);

    window->IsInputEnabled = true;

    this->windowHandle = reinterpret_cast<IUnknown*>(window);
    this->isMainWindow = isMainWindow;

    this->handlers->AddHandlers(window);
}

void OSWindow::Impl::Destroy()
{
    if (this->windowHandle != nullptr)
    {
        if (!this->isMainWindow)
        {
            auto window = reinterpret_cast<CoreWindow^>(this->windowHandle);
            this->handlers->RemoveHandlers(window);
            window->Close();
        }

        this->windowHandle = nullptr;
    }
}

bool OSWindow::Impl::IsMaximized() const
{
    if (CanResizeWindows())
    {
        auto applicationView = GetApplicationViewForWindow();
        if (applicationView->AdjacentToLeftDisplayEdge && applicationView->AdjacentToRightDisplayEdge)
            FINJIN_DEBUG_LOG_INFO("OSWindow::Impl::IsMaximized(): It is adjacent to an edge")
        else if (applicationView->IsFullScreenMode)
            FINJIN_DEBUG_LOG_INFO("OSWindow::Impl::IsMaximized(): It is in full screen mode")

            auto isFullScreen = (applicationView->AdjacentToLeftDisplayEdge && applicationView->AdjacentToRightDisplayEdge) || applicationView->IsFullScreenMode;
        return isFullScreen;
    }
    else
        return false;
}

OSWindowRect OSWindow::Impl::GetRect() const
{
    auto window = reinterpret_cast<CoreWindow^>(this->windowHandle);
    auto bounds = window->Bounds;
    return OSWindowRect(bounds.Left, bounds.Top, bounds.Width, bounds.Height);
}

OSWindowRect OSWindow::Impl::GetClientRect() const
{
    return GetRect();
}

void OSWindow::Impl::NotifyListenersClosing()
{
    auto listeners = this->listeners;
    for (auto listener : listeners)
        listener->WindowClosing(this->osWindow);
}

ApplicationView^ OSWindow::Impl::GetApplicationViewForWindow() const
{
    //Note: This just gets the current application view, not the one specific to the window
    //The way the code in this file (and application) is written, using anything else would not work correctly since
    //it would potentially allow for methods to be called on another view (that isn't the current one), which is a bad idea
    return ApplicationView::GetForCurrentView();
}

bool OSWindow::Impl::CanResizeWindows() const
{
    auto currentView = UIViewSettings::GetForCurrentView();
    assert(currentView != nullptr);
    return currentView->UserInteractionMode == Windows::UI::ViewManagement::UserInteractionMode::Mouse;
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

    auto applicationView = ApplicationView::GetForCurrentView();
    auto window = CoreWindow::GetForCurrentThread();
    auto isMainWindow = CoreApplication::GetCurrentView()->CoreWindow == window && CoreApplication::GetCurrentView()->IsMain;

    impl->Create(internalName, titleOrSubtitle, rect, windowSize, window, isMainWindow, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to create window implementation.");
        return;
    }

    //applicationView->SetDesiredBoundsMode(ApplicationViewBoundsMode::UseCoreWindow);

    if (impl->CanResizeWindows())
    {
        applicationView->PreferredLaunchViewSize = Size(rect.width, rect.height);
        if (impl->windowSize.IsWindowedMaximized())
            applicationView->PreferredLaunchWindowingMode = ApplicationViewWindowingMode::FullScreen;
        else
            applicationView->PreferredLaunchWindowingMode = ApplicationViewWindowingMode::PreferredLaunchViewSize;
    }
}

void OSWindow::Destroy()
{
    impl->listeners.clear();

    impl->Destroy();
}

const Utf8String& OSWindow::GetInternalName() const
{
    return impl->internalName;
}

void OSWindow::ShowMessage(const Utf8String& message, const Utf8String& title)
{
#if USE_MESSAGE_BOX_FOR_MESSAGES
    ShowMessageDialog(message, title);
#endif
}

void OSWindow::ShowErrorMessage(const Utf8String& message, const Utf8String& title)
{
#if USE_MESSAGE_BOX_FOR_MESSAGES
    ShowMessageDialog(message, title);
#endif
}

void OSWindow::ApplyWindowSize()
{
    FINJIN_DEBUG_LOG_INFO("OSWindow::ApplyWindowSize()");

    if (impl->CanResizeWindows())
    {
        FINJIN_DEBUG_LOG_INFO("  Can resize windows");

        impl->windowSize.SetApplyingToWindow(true);

        auto bounds = impl->windowSize.GetCurrentBounds();
        LimitBounds(bounds);

        auto applicationView = impl->GetApplicationViewForWindow();

        if (impl->windowSize.IsWindowed())
        {
            FINJIN_DEBUG_LOG_INFO("  Exiting full screen mode");

            applicationView->ExitFullScreenMode();
            SetClientSize(bounds.GetClientWidth(), bounds.GetClientHeight());
        }
        else
        {
            FINJIN_DEBUG_LOG_INFO("  Maximizing");

            Maximize();
        }

        impl->windowSize.SetApplyingToWindow(false);
    }
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
    if (impl->CanResizeWindows())
    {
        switch (impl->windowSize.GetState())
        {
            case WindowSizeState::WINDOWED_NORMAL:
            {
                auto displayRect = GetDisplayVisibleRect();

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
    else
    {
        auto displayRect = GetDisplayVisibleRect();//GetDisplayRect();
        auto newWidth = displayRect.GetWidth();
        auto newHeight = displayRect.GetHeight();
        bounds.AdjustSize(newWidth, newHeight);
    }
}

void OSWindow::Tick()
{
    impl->handlers->Tick();
}

bool OSWindow::IsMaximized() const
{
    return impl->IsMaximized();
}

void OSWindow::Maximize(bool maximize)
{
    if (impl->CanResizeWindows())
    {
        auto applicationView = impl->GetApplicationViewForWindow();
        applicationView->TryEnterFullScreenMode();
    }
}

bool OSWindow::IsMinimized() const
{
    return !IsVisible();
}

bool OSWindow::IsVisible() const
{
    auto window = reinterpret_cast<CoreWindow^>(impl->windowHandle);
    return window->Visible;
}

bool OSWindow::IsSizeLocked() const
{
    return false;
}

void OSWindow::LockSize(OSWindowSize size)
{
}

void OSWindow::UnlockSize(OSWindowSize minSize)
{
}

bool OSWindow::HasFocus() const
{
    return impl->hasFocus;
}

void OSWindow::Raise()
{
    auto window = reinterpret_cast<CoreWindow^>(impl->windowHandle);
    window->Activate();
}

int OSWindow::GetDisplayID() const
{
    return static_cast<int>(GetBestDisplay(GetRect())->index);
}

float OSWindow::GetDisplayDensity() const
{
    return GetBestDisplay(GetRect())->density;
}

OSWindowRect OSWindow::GetDisplayRect() const
{
    return GetBestDisplay(GetRect())->frame;
}

OSWindowRect OSWindow::GetDisplayVisibleRect() const
{
    return GetBestDisplay(GetRect())->clientFrame;
}

OSWindowRect OSWindow::GetRect() const
{
    return impl->GetRect();
}

OSWindowSize OSWindow::GetClientSize() const
{
    return impl->GetClientRect().GetSize();
}

void OSWindow::SetClientSize(OSWindowDimension width, OSWindowDimension height)
{
    if (impl->CanResizeWindows())
    {
        FINJIN_DEBUG_LOG_INFO("OSWindow::SetClientSize: (%1% x %2%)", width, height);
        auto applicationView = impl->GetApplicationViewForWindow();
        applicationView->TryResizeView(Size(width, height));
    }
}

bool OSWindow::Move(OSWindowCoordinate x, OSWindowCoordinate y)
{
    return false;
}

bool OSWindow::HasWindowHandle() const
{
    return impl->windowHandle != nullptr;
}

void OSWindow::ClearSystemWindowHandle()
{
    impl->windowHandle = nullptr;
}

void* OSWindow::GetWindowHandle() const
{
    return impl->windowHandle;
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
    impl->NotifyListenersClosing();
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
    auto window = reinterpret_cast<CoreWindow^>(impl->windowHandle);

    auto center = GetRect().GetCenter();
    window->PointerPosition = Point(center.x, center.y);
    return true;
}

void* OSWindow::GetMonitorHandle() const
{
    return GetBestDisplay(GetRect())->monitorHandle;
}

void OSWindow::ShowTheCursor()
{
    auto window = reinterpret_cast<CoreWindow^>(impl->windowHandle);
    window->PointerCursor = nullptr;
}

void OSWindow::HideTheCursor()
{
    auto window = reinterpret_cast<CoreWindow^>(impl->windowHandle);
    window->PointerCursor = ref new CoreCursor(CoreCursorType::Arrow, 1);
}

void OSWindow::SetTitle() const
{
    auto applicationView = impl->GetApplicationViewForWindow();

    Utf8StringToWideString title(impl->subtitle);
    applicationView->Title = ref new Platform::String(title.c_str());
}
