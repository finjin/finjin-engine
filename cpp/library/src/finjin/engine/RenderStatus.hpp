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


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    class RenderStatus
    {
    public:
        RenderStatus() { this->continueRendering = false; this->modifyingScreenRenderTarget = false; }
        RenderStatus(bool c, bool m) { this->continueRendering = c; this->modifyingScreenRenderTarget = m; }

        bool IsRenderingRequired() const { return this->continueRendering && !this->modifyingScreenRenderTarget; }

        static const RenderStatus& GetRenderingRequired() { static const RenderStatus value(true, false); return value; }
        static const RenderStatus& GetFinishing() { static const RenderStatus value(false, false); return value; }
        static const RenderStatus& GetModifyingScreenRenderTarget() { static const RenderStatus value(true, true); return value; }

    private:
        bool continueRendering;
        bool modifyingScreenRenderTarget;
    };

    enum class ModifyScreenRenderTargetFlag
    {
        NONE = 0,

        /**
         * Indicates the window was resized (either by the user or as a result of the engine modifying the window).
         */
        WINDOW_RESIZED = 1 << 0,

        /**
         * Indicates the window was modified and will eventually generate a size change notification.
         */
        WAITING_FOR_WINDOW_RESIZED_NOTIFICATION = 1 << 1,

        /**
         * Indicates the engine should try to toggle full screen mode on/off.
         */
        TOGGLE_FULL_SCREEN = 1 << 2,

        /**
         * Indicates the engine should try to change the refresh rate.
         * This will only work if full screen exclusive mode is supported by the system and the engine is either in exclusive mode or changing to it.
         */
        CHANGE_REFRESH_RATE = 1 << 3
    };
    FINJIN_ENUM_BITWISE_OPERATIONS(ModifyScreenRenderTargetFlag)

    class ModifyScreenRenderTarget
    {
    public:
        ModifyScreenRenderTarget() { Reset(); }
        ModifyScreenRenderTarget(ModifyScreenRenderTargetFlag flags) { this->flags = flags; this->refreshRate[0] = this->refreshRate[1] = 0; }

        void Reset() { this->flags = ModifyScreenRenderTargetFlag::NONE; this->isWindowMinimized = false; this->isFullScreenExclusive = false; this->refreshRate[0] = this->refreshRate[1] = 0; }

        void AddModification(ModifyScreenRenderTargetFlag flags) { this->flags |= flags; }

        bool HasModification() const { return this->flags != ModifyScreenRenderTargetFlag::NONE; }
        bool HasModification(ModifyScreenRenderTargetFlag flag) const { return AnySet(this->flags & flag); }

        void ClearModification(ModifyScreenRenderTargetFlag flag) { this->flags &= Inverse(flag); }

        void WindowResized(bool isMinimized) { this->flags |= ModifyScreenRenderTargetFlag::WINDOW_RESIZED; this->isWindowMinimized = isMinimized; }
        bool IsWindowMinimized() const { return this->isWindowMinimized; }

        void ToggleFullScreen(bool exclusive) { this->flags |= ModifyScreenRenderTargetFlag::TOGGLE_FULL_SCREEN; this->isFullScreenExclusive = exclusive; }
        bool IsFullScreenExclusive() const { return this->isFullScreenExclusive; }

        std::array<uint32_t, 2> GetRefreshRate() const { return this->refreshRate; }
        void ChangeRefreshRate(uint32_t num, uint32_t denom = 1) { this->flags |= ModifyScreenRenderTargetFlag::CHANGE_REFRESH_RATE; this->refreshRate[0] = num; this->refreshRate[1] = denom; }

    private:
        ModifyScreenRenderTargetFlag flags;

        bool isWindowMinimized;

        bool isFullScreenExclusive;

        std::array<uint32_t, 2> refreshRate; //0 = numerator, 1 = denominator
    };

} }
