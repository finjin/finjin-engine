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
#include "finjin/engine/WindowBounds.hpp"


//Classes----------------------------------------------------------------------
namespace Finjin { namespace Engine {

    class OSWindow;
    
    /**
     * Default state: WINDOWED_NORMAL
     * Default full screen state: BORDERLESS_FULLSCREEN
     * Default non full screen state: WINDOWED_NORMAL
     */
    class WindowSize
    {
    public:
        enum State
        {
            //Not full screen--------------------------------------------------
            /** Windowed, not full screen. */
            WINDOWED_NORMAL,

            /** Maximized window, and takes up the full screen but not in the conventional sense since the window chrome still exists. */
            WINDOWED_MAXIMIZED,

            //Full screen-------------------------------------------------------
            /* Full screen, not exclusive. */
            BORDERLESS_FULLSCREEN,

            /** Full screen, exclusive access. */
            EXCLUSIVE_FULLSCREEN,

            STATE_COUNT
        };
        
        WindowSize();

        bool IsFullScreen() const;
        bool IsFullScreenExclusive() const;
        bool IsFullScreenBorderless() const;
        
        bool IsWindowed() const;
        bool IsWindowedNormal() const;
        bool IsWindowedMaximized() const;

        State GetState() const;
        void SetState(WindowSize::State state);

        State GetFullScreenState() const;
        bool SetFullScreenState(State value); //Valid: BORDERLESS_FULLSCREEN, EXCLUSIVE_FULLSCREEN

        State GetWindowedState() const;
        bool SetWindowedState(State value); //Valid: WINDOWED_NORMAL, WINDOWED_MAXIMIZED

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

        const WindowBounds& GetBounds(State state) const;
        void SetBounds(State state, const WindowBounds& value);
        
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
        State state;
        State fullScreenState;
        State windowedState;        
        WindowBounds bounds[STATE_COUNT];

        //---------------
        OSWindow* window;
        bool applyingToWindow;
    };

} }
