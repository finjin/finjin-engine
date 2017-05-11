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
#include "ApplicationViewportsController.hpp"
#include "finjin/engine/DisplayInfo.hpp"
#include "finjin/engine/OSWindow.hpp"
#include "PlatformCapabilities.hpp"

using namespace Finjin::Engine;


//Implementation----------------------------------------------------------------
ApplicationViewportsController::ApplicationViewportsController()
{
}

bool ApplicationViewportsController::IsOnOwnDisplay(ApplicationViewport* testAppWindow) const
{
    //Determine whether there is a window on the same display as the window passed in
    auto testAppWindowDisplayID = testAppWindow->GetOSWindow()->GetDisplayID();
    for (auto& appViewport : this->appViewports)
    {
        if (testAppWindow != appViewport.get() && appViewport->GetOSWindow()->GetDisplayID() == testAppWindowDisplayID)
            return false;
    }

    return true;
}

bool ApplicationViewportsController::MoveNonFullscreenViewportsToOwnDisplay() const
{
    //Get all displays
    DisplayInfos displays;
    PlatformCapabilities::GetInstance().GetDisplays(displays);
    if (this->appViewports.size() > displays.size())
        return false;

    //Put the windows onto each display left to right
    displays.SortLeftToRight();
    size_t windowIndex = 0;
    for (auto& displayInfo : displays)
    {
        //Break out if no more windows
        if (windowIndex >= this->appViewports.size())
            break;

        const auto& appViewport = this->appViewports[windowIndex++];
        if (!appViewport->GetOSWindow()->GetWindowSize().IsFullScreen() && !IsOnOwnDisplay(appViewport.get()))
        {
            //Move window into center of display
            auto centeredRect = displayInfo.frame.CenterChild(appViewport->GetOSWindow()->GetRect());
            if (!appViewport->GetOSWindow()->Move(centeredRect.x, centeredRect.y))
                return false;

            appViewport->GetOSWindow()->Raise();
        }
    }

    return true;
}

bool ApplicationViewportsController::AllHaveOutputHandle() const
{
    for (auto& appViewport : this->appViewports)
    {
        if (!appViewport->GetOSWindow()->HasWindowHandle())
            return false;
    }

    return true;
}

bool ApplicationViewportsController::AnyHasFocus() const
{
    for (auto& appViewport : this->appViewports)
    {
        if (appViewport->GetOSWindow()->HasFocus())
            return true;
    }

    return false;
}

bool ApplicationViewportsController::RequestFullScreenToggle(ApplicationViewport* appViewport)
{
    if (appViewport->GetOSWindow()->GetWindowSize().IsFullScreen() || //Window is full screen already
        IsOnOwnDisplay(appViewport) ||  //Not full screen, but is on own display
        MoveNonFullscreenViewportsToOwnDisplay()) //Not full screen and not on own display, but was moved to own display
    {
        appViewport->RequestFullScreenToggle();
        return true;
    }
    else
    {
        //Window can't have its full screen state toggled
        return false;
    }
}

bool ApplicationViewportsController::RequestFullScreenToggle()
{
    if (MoveNonFullscreenViewportsToOwnDisplay())
    {
        for (auto& appViewport : this->appViewports)
            appViewport->RequestFullScreenToggle();

        return true;
    }
    else
        return false;
}

ApplicationViewportsClosing ApplicationViewportsController::GetAllViewports()
{
    ApplicationViewportsClosing all;

    for (size_t viewportIndex = 0; viewportIndex < this->appViewports.size(); viewportIndex++)
        all.push_back(std::move(this->appViewports[viewportIndex]));
    this->appViewports.clear();

    return all;
}

ApplicationViewportsClosing ApplicationViewportsController::GetViewportsClosing()
{
    ApplicationViewportsClosing closing;

    //See if any window has requested the application exit
    bool exitApplication = false;
    for (size_t viewportIndex = 0; viewportIndex < this->appViewports.size(); viewportIndex++)
    {
        if (this->appViewports[viewportIndex]->ExitApplicationRequested())
        {
            exitApplication = true;
            break;
        }
    }

    //If any window requested the application exit, get all windows. Otherwise just get the windows that want to close
    for (size_t viewportIndex = 0; viewportIndex < this->appViewports.size();)
    {
        if (exitApplication || this->appViewports[viewportIndex]->CloseRequested())
        {
            closing.push_back(std::move(this->appViewports[viewportIndex]));
            this->appViewports.erase(&this->appViewports[viewportIndex]);
        }
        else
            viewportIndex++;
    }

    return closing;
}

bool ApplicationViewportsController::FindAndDeleteAndRemove(ApplicationViewport* appViewport)
{
    auto it = std::find_if(begin(), end(), [appViewport](std::unique_ptr<ApplicationViewport>& aw) { return aw.get() == appViewport; });
    if (it != end())
    {
        //User manually closed the window
        it->reset(); //Delete/destroy it
        this->appViewports.erase(it); //Remove it
        return true;
    }
    else
    {
        //Exiting programmatically
        return false;
    }
}

ApplicationViewportsFocusState ApplicationViewportsController::StartFocusUpdate()
{
    ApplicationViewportsFocusState state;

    for (auto& appViewport : *this)
    {
        bool hadFocus = appViewport->GetOSWindow()->HasFocus();

        state.hadFocus.push_back(hadFocus);

        state.anyHadFocus |= hadFocus;
    }

    return state;
}

void ApplicationViewportsController::FinishFocusUpdate(ApplicationViewportsFocusState& state)
{
    size_t appWindowIndex = 0;
    for (auto& appViewport : *this)
    {
        auto hasFocus = appViewport->GetOSWindow()->HasFocus();
        state.anyHasFocus |= hasFocus;

        auto lostFocus = state.hadFocus[appWindowIndex] && !hasFocus;
        state.anyLostFocus |= lostFocus;

        appWindowIndex++;
    }
}
