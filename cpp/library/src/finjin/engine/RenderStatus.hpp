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

    struct RenderStatus
    {
        RenderStatus()
        {
            this->continueRendering = false;
            this->modifyingRenderTarget = false;
        }

        RenderStatus(bool continueRendering, bool modifyingRenderTarget)
        {
            this->continueRendering = continueRendering;
            this->modifyingRenderTarget = modifyingRenderTarget;
        }

        bool IsRenderingRequired() const { return this->continueRendering && !this->modifyingRenderTarget; }

        static const RenderStatus& GetRenderingRequired() { static const RenderStatus value(true, false); return value; }
        static const RenderStatus& GetFinishing() { static const RenderStatus value(false, false); return value; }
        static const RenderStatus& GetModifyingRenderTarget() { static const RenderStatus value(true, true); return value; }

        bool continueRendering;
        bool modifyingRenderTarget;
    };

} }
