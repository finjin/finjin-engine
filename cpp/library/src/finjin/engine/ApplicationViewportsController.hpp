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
#include "finjin/common/StaticVector.hpp"
#include "finjin/engine/ApplicationViewport.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    class ApplicationViewportsClosing : public StaticVector<std::unique_ptr<ApplicationViewport>, EngineConstants::MAX_WINDOWS>
    {
    public:
        ApplicationViewportsClosing() {}

        ApplicationViewportsClosing(const ApplicationViewportsClosing& other) = delete;
        ApplicationViewportsClosing& operator = (const ApplicationViewportsClosing& other) = delete;

        explicit ApplicationViewportsClosing(ApplicationViewportsClosing&& other)
        {
            resize(other.size());

            for (size_t viewportIndex = 0; viewportIndex < this->count; viewportIndex++)
                this->items[viewportIndex] = std::move(other[viewportIndex]);

            other.count = 0;
        }

        ApplicationViewportsClosing& operator = (ApplicationViewportsClosing&& other)
        {
            resize(other.size());

            for (size_t viewportIndex = 0; viewportIndex < this->count; viewportIndex++)
                this->items[viewportIndex] = std::move(other[viewportIndex]);

            other.count = 0;

            return *this;
        }
    };

    class ApplicationViewportsFocusState
    {
    public:
        ApplicationViewportsFocusState()
        {
            this->anyHadFocus = this->anyHasFocus = this->anyLostFocus = false;
        }

    public:
        bool anyHadFocus;
        bool anyHasFocus;
        bool anyLostFocus;

        class HadFocus : public StaticVector<bool, EngineConstants::MAX_WINDOWS>
        {
        };
        HadFocus hadFocus;
    };

    class ApplicationViewportsController
    {
    public:
        ApplicationViewportsController();
        virtual ~ApplicationViewportsController() {}

        bool IsOnOwnDisplay(ApplicationViewport* appViewport) const;
        bool MoveNonFullscreenViewportsToOwnDisplay() const;

        bool AllHaveOutputHandle() const;
        bool AnyHasFocus() const;

        bool RequestFullScreenToggle(ApplicationViewport* appViewport);
        bool RequestFullScreenToggle();

        void GetAllViewports(ApplicationViewportsClosing& all);
        void GetViewportsClosing(ApplicationViewportsClosing& closing);

        bool FindAndDeleteAndRemove(ApplicationViewport* appViewport);

        ApplicationViewportsFocusState StartFocusUpdate();
        void FinishFocusUpdate(ApplicationViewportsFocusState& state);

        bool empty() const { return this->appViewports.empty(); }

        size_t size() const { return this->appViewports.size(); }

        std::unique_ptr<ApplicationViewport>& operator [] (size_t i) { return this->appViewports[i]; }

        void push_back(std::unique_ptr<ApplicationViewport>&& appViewport) { this->appViewports.push_back(std::move(appViewport)); }

        const std::unique_ptr<ApplicationViewport>& back() const { return this->appViewports.back(); }
        std::unique_ptr<ApplicationViewport>& back() { return this->appViewports.back(); }

        const std::unique_ptr<ApplicationViewport>* begin() const { return this->appViewports.begin(); }
        std::unique_ptr<ApplicationViewport>* begin() { return this->appViewports.begin(); }

        const std::unique_ptr<ApplicationViewport>* end() const { return this->appViewports.end(); }
        std::unique_ptr<ApplicationViewport>* end() { return this->appViewports.end(); }

    private:
        using ApplicationViewports = StaticVector<std::unique_ptr<ApplicationViewport>, EngineConstants::MAX_WINDOWS>;
        ApplicationViewports appViewports;
    };

} }
