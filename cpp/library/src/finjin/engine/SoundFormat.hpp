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
#include "finjin/common/Utf8String.hpp"
#include "finjin/common/Chrono.hpp"


//Classes----------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    class SoundFormat
    {
    public:
        enum { MAX_CHANNELS = 8 }; //7.1 audio = 8 channels
        
        Utf8String typeName; //A name that can be used to identify or differentiate sound formats. None of the SoundFormat methods set this

        int channelCount;
        int bytesPerChannel;
        int samplesPerSecond;
        
        enum class SampleType
        {
            INTEGER, //unsigned 8-bit, signed 16-bit
            FLOATING_POINT
        };
        SampleType sampleType;

        SoundFormat();
        SoundFormat(int channelCount, int bytesPerChannel, int samplesPerSecond);
        SoundFormat(int channelCount, int bytesPerChannel, int samplesPerSecond, SampleType sampleType);

        bool operator == (const SoundFormat& other) const;
        bool operator != (const SoundFormat& other) const;

        Utf8String ToString() const;

        void Set(int channelCount, int bytesPerChannel, int samplesPerSecond, SampleType sampleType);

        bool IsValid() const;

        void Reset();

        int GetChannelCount() const;
        int GetBytesPerChannel() const;
        int GetBytesPerSample() const;
        
        int GetBytesPerSecond() const;        
        SimpleTimeCounter CalculateTimeLength(size_t size) const;
        int64_t GetSampleByteOffset(SimpleTimeCounter timeOffset) const;
        int64_t GetSampleFrameOffset(SimpleTimeCounter timeOffset) const;

        Utf8String ToTypeName() const;
        static SoundFormat FromTypeName(const Utf8String& name);

        template <typename T>
        static const SoundFormat* FindByTypeName(const T& formats, const Utf8String& typeName)
        {
            return FindByTypeName(formats.data(), formats.size(), typeName);
        }
        static const SoundFormat* FindByTypeName(const SoundFormat* formats, size_t count, const Utf8String& typeName);

        template <typename T>
        static const SoundFormat* FindByFormat(const T& formats, const SoundFormat& format)
        {
            return FindByFormat(formats.data(), formats.size(), format);
        }
        static const SoundFormat* FindByFormat(const SoundFormat* formats, size_t count, const SoundFormat& format);
    };

} }
