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
#include "finjin/common/EnumArray.hpp"
#include "finjin/engine/WindowBounds.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    class OSWindow;

    enum class WindowSizeState
    {
        WINDOWED_NORMAL,
        WINDOWED_MAXIMIZED,
        BORDERLESS_FULLSCREEN,
        EXCLUSIVE_FULLSCREEN,

        COUNT
    };

    /**
     * Default state: WindowSizeState::WINDOWED_NORMAL
     * Default full screen state: WindowSizeState::BORDERLESS_FULLSCREEN
     * Default non full screen state: WindowSizeState::WINDOWED_NORMAL
     */
    class WindowSize
    {
    public:
        WindowSize();

        bool IsFullScreen() const;
        bool IsFullScreenExclusive() const;
        bool IsFullScreenBorderless() const;

        bool IsWindowed() const;
        bool IsWindowedNormal() const;
        bool IsWindowedMaximized() const;

        WindowSizeState GetState() const;
        void SetState(WindowSizeState state);

        WindowSizeState GetFullScreenState() const;
        bool SetFullScreenState(WindowSizeState value); //Valid: WindowSizeState::BORDERLESS_FULLSCREEN, WindowSizeState::EXCLUSIVE_FULLSCREEN

        WindowSizeState GetWindowedState() const;
        bool SetWindowedState(WindowSizeState value); //Valid: WindowSizeState::WINDOWED_NORMAL, WindowSizeState::WINDOWED_MAXIMIZED

        bool IsInFullScreenState() const;
        bool IsInWindowedState() const;

        void SetFullScreen(bool value);

        void Next(bool applyToWindow = true);

        OSWindowDimension GetCurrentWidth() const;
        OSWindowDimension GetCurrentHeight() const;
        OSWindowDimension GetCurrentClientWidth() const;
        OSWindowDimension GetCurrentClientHeight() const;
        float GetCurrentAspectRatio() const;

        const WindowBounds& GetCurrentBounds() const;
        void SetCurrentBounds(const WindowBounds& value);

        const WindowBounds& GetBounds(WindowSizeState state) const;
        void SetBounds(WindowSizeState state, const WindowBounds& value);

        const WindowBounds& GetFullScreenBounds() const;

        const WindowBounds& GetWindowedBounds() const;

        //-------------------------------------
        void SetWindow(OSWindow* window);

        void WindowMoved(bool isWindowMaximized);
        void WindowResized(bool isWindowMaximized);

        void SetApplyingToWindow(bool value);
        bool IsApplyingToWindow() const;

        WindowBounds GetSafeCurrentBounds() const;

    private:
        WindowSizeState state;
        WindowSizeState fullScreenState;
        WindowSizeState windowedState;
        EnumArray<WindowSizeState, WindowSizeState::COUNT, WindowBounds> bounds;

        OSWindow* window;
        bool applyingToWindow;
    };

} }
