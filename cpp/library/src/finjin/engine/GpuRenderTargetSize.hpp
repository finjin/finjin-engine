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
#include "finjin/engine/WindowBounds.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    class OSWindow;

    enum class GpuRenderTargetSizeType
    {
        EXPLICIT_SIZE,
        WINDOW_SIZE
    };

    class GpuRenderTargetSize
    {
    public:
        GpuRenderTargetSize();

        GpuRenderTargetSizeType GetType() const;
        void SetType(GpuRenderTargetSizeType value);

        void SetExplicit(double width, double height); //Sets type to GpuRenderTargetSizeType::EXPLICIT_SIZE

        double GetMinScale() const;
        double GetMaxScale() const;
        void SetScale(double minScale, double maxScale);

        std::array<uint32_t, 2> Evaluate(OSWindow* osWindow, const WindowBounds* renderBounds = nullptr, double scale = 1.0);
        std::array<uint32_t, 2> EvaluateMax(OSWindow* osWindow, const WindowBounds* renderBounds = nullptr);

    private:
        GpuRenderTargetSizeType sizeType;

        double explicitWidth;
        double explicitHeight;

        double minScale;
        double maxScale;
    };

} }
