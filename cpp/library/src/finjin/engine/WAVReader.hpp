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
#include "finjin/common/ByteBuffer.hpp"
#include "finjin/common/Error.hpp"
#include "finjin/engine/SoundFormat.hpp"


//Classes----------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    class WAVSoundData
    {
    public:
        WAVSoundData();
        WAVSoundData(const void* data, size_t length, int channelCount, int bytesPerChannel, int samplesPerSecond);

        size_t GetSampleCount() const;

    public:
        const void* data;
        size_t length;
        SoundFormat soundFormat;
    };

    /** Decompresses a complete WAV audio stream */
    class WAVReader
    {
    public:
        WAVReader();

        /** 
         * Decompresses a file from memory 
         * @param fileImage [in] - The file image
         * @param decompressedBuffer [in] - A modifiable buffer that can be used during the decode process.
         * @param error [out] - Error object.
         * @return The read audio bytes. The returned data pointer will point to decompressedBuffer.
         */
        WAVSoundData DecompressFileImage(ByteBufferReader& reader, ByteBuffer& decompressedBuffer, Error& error);
    };

} }
