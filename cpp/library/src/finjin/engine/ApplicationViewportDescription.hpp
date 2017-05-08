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
#include "finjin/common/Setting.hpp"
#include "finjin/common/Utf8String.hpp"
#include "finjin/engine/OSWindowDefs.hpp"
#include "finjin/engine/GpuDescription.hpp"
#include "finjin/engine/GpuID.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    class ApplicationViewportDescription
    {
    public:
        ApplicationViewportDescription();

        GpuID desiredGpuID;
        HardwareGpuDescriptions::value_type desiredGpuDescription;

        Utf8String internalName; //Internal window name. If not set, a default name will be created
        Utf8String title; //Depending on platform, may or may not be used
        Utf8String subtitle; //Depending on platform, may or may not be used
        Utf8String displayName; //Screen/display name. Only used on Linux
        Setting<OSWindowRect> windowFrame; //Initial window frame
        GpuID gpuID;

        //Number of swap chain frames. The value is 'requested' since the the 3D APIs may give more or less
        //This will ultimately affect the maximum amount render buffering
        //2 = double buffer
        //3 = triple buffer
        //...and so on
        //Will be limited to [EngineConstants::MIN_FRAME_BUFFERS, EngineConstants::MAX_FRAME_BUFFERS]
        size_t requestedFrameBufferCount;

        //Number of frames to update before rendering. Increasing this number increases parallelism at the expense of more processing lag
        //Notice that when the application starts, there may be a bit of updating warmup before the rendering starts:
        //  1 (no warmup) = update frame N, render frame N
        //  2 (one frame of warmup) = update frame N, update frame N + 1, render frame N, update frame N + 2, render frame N + 1, update frame N + 3, render frame N + 2...etc
        //  3 (two frames of warmup) = update frame N, update frame N + 1, update frame N + 2, render frame N, update frame N + 3, render frame N + 1...etc
        //  ...and so on
        //Will be limited to [1, actual frame count - 1]
        size_t jobProcessingPipelineSize;
    };

} }
