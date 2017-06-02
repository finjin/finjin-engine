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
#include "WindowSize.hpp"
#include "finjin/engine/OSWindow.hpp"
#include "finjin/common/DebugLog.hpp"

using namespace Finjin::Engine;


//Local functions---------------------------------------------------------------
static bool IsFullScreen(WindowSizeState state)
{
    return state == WindowSizeState::BORDERLESS_FULLSCREEN || state == WindowSizeState::EXCLUSIVE_FULLSCREEN;
}

static bool IsWindowed(WindowSizeState state)
{
    return state == WindowSizeState::WINDOWED_NORMAL || state == WindowSizeState::WINDOWED_MAXIMIZED;
}


//Implementation----------------------------------------------------------------
WindowSize::WindowSize()
{
    this->fullScreenState = WindowSizeState::BORDERLESS_FULLSCREEN; //EXCLUSIVE_FULLSCREEN;// BORDERLESS_FULLSCREEN;
    this->windowedState = WindowSizeState::WINDOWED_NORMAL; //WINDOWED_MAXIMIZED; //WINDOWED_NORMAL;
    this->state = this->windowedState;//this->windowedState; //this->fullScreenState;

    this->bounds[WindowSizeState::WINDOWED_NORMAL].Set(FINJIN_OS_WINDOW_COORDINATE_DEFAULT, FINJIN_OS_WINDOW_COORDINATE_DEFAULT, 800, 600, WindowBoundsFlags::BORDER | WindowBoundsFlags::LOCKED);
    this->bounds[WindowSizeState::WINDOWED_MAXIMIZED].Set(0, 0, 0, 0, WindowBoundsFlags::BORDER | WindowBoundsFlags::MAXIMIZED);
    this->bounds[WindowSizeState::BORDERLESS_FULLSCREEN].Set(0, 0, 0, 0, WindowBoundsFlags::NONE);
    this->bounds[WindowSizeState::EXCLUSIVE_FULLSCREEN].Set(0, 0, 1920, 1080, WindowBoundsFlags::NONE);

    this->window = nullptr;
    this->applyingToWindow = false;
}

bool WindowSize::IsFullScreen() const
{
    return ::IsFullScreen(this->state);
}

bool WindowSize::IsFullScreenExclusive() const
{
    return this->state == WindowSizeState::EXCLUSIVE_FULLSCREEN;
}

bool WindowSize::IsFullScreenBorderless() const
{
    return this->state == WindowSizeState::BORDERLESS_FULLSCREEN;
}

bool WindowSize::IsWindowed() const
{
    return ::IsWindowed(this->state);
}

bool WindowSize::IsWindowedNormal() const
{
    return this->state == WindowSizeState::WINDOWED_NORMAL;
}

bool WindowSize::IsWindowedMaximized() const
{
    return this->state == WindowSizeState::WINDOWED_MAXIMIZED;
}

WindowSizeState WindowSize::GetState() const
{
    return this->state;
}

void WindowSize::SetState(WindowSizeState state)
{
    this->state = state;
}

WindowSizeState WindowSize::GetFullScreenState() const
{
    return this->fullScreenState;
}

bool WindowSize::SetFullScreenState(WindowSizeState value)
{
    if (::IsFullScreen(value))
    {
        if (this->state == this->fullScreenState)
            this->state = value;

        this->fullScreenState = value;

        return true;
    }
    else
        return false;
}

WindowSizeState WindowSize::GetWindowedState() const
{
    return this->windowedState;
}

bool WindowSize::SetWindowedState(WindowSizeState value)
{
    if (::IsWindowed(value))
    {
        if (this->state == this->windowedState)
            this->state = value;

        this->windowedState = value;

        return true;
    }
    else
        return false;
}

bool WindowSize::IsInFullScreenState() const
{
    return this->state == this->fullScreenState;
}

bool WindowSize::IsInWindowedState() const
{
    return this->state == this->windowedState;
}

void WindowSize::SetFullScreen(bool value)
{
    if (value)
        this->state = this->fullScreenState;
    else
        this->state = this->windowedState;
}

void WindowSize::Next(bool applyToWindow)
{
    FINJIN_DEBUG_LOG_INFO("WindowSize::Next(%1%)", (int)applyToWindow);

    if (IsFullScreen())
    {
        FINJIN_DEBUG_LOG_INFO("  Changing to windowed state");
        this->state = this->windowedState;
    }
    else
    {
        FINJIN_DEBUG_LOG_INFO("  Changing to full screen state");
        this->state = this->fullScreenState;
    }

    if (applyToWindow)
        window->ApplyWindowSize();
}

OSWindowDimension WindowSize::GetCurrentWidth() const
{
    auto bounds = GetCurrentBounds();
    return bounds.width;
}

OSWindowDimension WindowSize::GetCurrentHeight() const
{
    auto bounds = GetCurrentBounds();
    return bounds.height;
}

OSWindowDimension WindowSize::GetCurrentClientWidth() const
{
    auto bounds = GetCurrentBounds();
    return bounds.GetClientWidth();
}

OSWindowDimension WindowSize::GetCurrentClientHeight() const
{
    auto bounds = GetCurrentBounds();
    return bounds.GetClientHeight();
}

float WindowSize::GetCurrentAspectRatio() const
{
    auto bounds = GetCurrentBounds();
    if (bounds.height > 0)
        return static_cast<float>(bounds.GetClientWidth()) / bounds.GetClientHeight();
    else
        return 0;
}

const WindowBounds& WindowSize::GetCurrentBounds() const
{
    return this->bounds[this->state];
}

void WindowSize::SetCurrentBounds(const WindowBounds& bounds)
{
    FINJIN_DEBUG_LOG_INFO("WindowSize::SetCurrentBounds: %1%: %2% x %3%", (int)this->state, bounds.width, bounds.height);
    this->bounds[this->state] = bounds;
}

const WindowBounds& WindowSize::GetBounds(WindowSizeState state) const
{
    return this->bounds[state];
}

void WindowSize::SetBounds(WindowSizeState state, const WindowBounds& bounds)
{
    this->bounds[state].x = bounds.x;
    this->bounds[state].y = bounds.y;
    this->bounds[state].width = bounds.width;
    this->bounds[state].height = bounds.height;
    this->bounds[state].clientWidth = bounds.clientWidth;
    this->bounds[state].clientHeight = bounds.clientHeight;
}

const WindowBounds& WindowSize::GetFullScreenBounds() const
{
    return this->bounds[this->fullScreenState];
}

const WindowBounds& WindowSize::GetWindowedBounds() const
{
    return this->bounds[this->windowedState];
}

void WindowSize::SetWindow(OSWindow* window)
{
    this->window = window;
}

void WindowSize::WindowMoved(bool isWindowMaximized)
{
    FINJIN_DEBUG_LOG_INFO("WindowSize::WindowMoved()");

    if (!IsApplyingToWindow())
    {
        if (IsWindowed() && isWindowMaximized)
            FINJIN_DEBUG_LOG_INFO("WindowSize::WindowMoved(): It is maximized");

        if (IsWindowed())
        {
            if (IsWindowedMaximized() != isWindowMaximized)
            {
                //Possibly break out of maximized state
                FINJIN_DEBUG_LOG_INFO("Maximized mode changed");
                SetWindowedState(IsWindowedMaximized() ? WindowSizeState::WINDOWED_NORMAL : WindowSizeState::WINDOWED_MAXIMIZED);
            }

            if (!IsWindowedMaximized())
            {
                auto windowRect = this->window->GetRect();

                auto bounds = GetCurrentBounds();
                bounds.x = windowRect.GetX();
                bounds.y = windowRect.GetY();
                SetCurrentBounds(bounds);
            }
        }
    }
}

void WindowSize::WindowResized(bool isWindowMaximized)
{
    FINJIN_DEBUG_LOG_INFO("WindowSize::WindowResized()");

    if (!IsApplyingToWindow())
    {
        if (IsWindowed() && isWindowMaximized)
            FINJIN_DEBUG_LOG_INFO("WindowSize::WindowResized(): It is maximized");

        if (IsWindowed())
        {
            if (IsWindowedMaximized() != isWindowMaximized)
            {
                //Possibly break out of maximized state
                FINJIN_DEBUG_LOG_INFO("Maximized mode changed");
                SetWindowedState(IsWindowedMaximized() ? WindowSizeState::WINDOWED_NORMAL : WindowSizeState::WINDOWED_MAXIMIZED);
            }

            if (!IsWindowedMaximized())
            {
                auto windowRect = this->window->GetRect();
                auto clientSize = this->window->GetClientSize();

                auto bounds = GetCurrentBounds();
                bounds.x = windowRect.GetX();
                bounds.y = windowRect.GetY();
                bounds.width = windowRect.GetWidth();
                bounds.height = windowRect.GetHeight();
                bounds.clientWidth = clientSize.GetWidth();
                bounds.clientHeight = clientSize.GetHeight();
                SetCurrentBounds(bounds);
            }
        }
    }
}

void WindowSize::SetApplyingToWindow(bool value)
{
    this->applyingToWindow = value;
}

bool WindowSize::IsApplyingToWindow() const
{
    return this->applyingToWindow;
}

WindowBounds WindowSize::GetSafeCurrentBounds() const
{
    auto safeBounds = GetCurrentBounds();
    this->window->LimitBounds(safeBounds);
    return safeBounds;
}
