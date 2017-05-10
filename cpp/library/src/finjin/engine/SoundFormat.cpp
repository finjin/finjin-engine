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
#include "SoundFormat.hpp"
#include "finjin/common/Convert.hpp"
#include "finjin/common/Math.hpp"
#include "finjin/common/Utf8StringFormatter.hpp"

using namespace Finjin::Engine;


//Local functions---------------------------------------------------------------
template <typename StringType>
SoundFormat FromTypeName(const StringType& name)
{
    SoundFormat format;

    if (name.StartsWith("low-"))
        format.samplesPerSecond = 11025;
    else if (name.StartsWith("medium-"))
        format.samplesPerSecond = 22050;
    else if (name.StartsWith("high-"))
        format.samplesPerSecond = 44100;
    else
    {
        auto foundAt = name.find('-');
        if (foundAt != Utf8String::npos)
        {
            Utf8String samplesPerSecondString;
            name.substr(samplesPerSecondString, 0, foundAt);
            format.samplesPerSecond = Convert::ToInteger(samplesPerSecondString, format.samplesPerSecond);
        }
    }

    if (name.find("mono-") != Utf8String::npos)
        format.channelCount = 1;
    else if (name.find("stereo-") != Utf8String::npos)
        format.channelCount = 2;
    else if (name.find("21-") != Utf8String::npos)
        format.channelCount = 3;
    else if (name.find("quad-") != Utf8String::npos)
        format.channelCount = 4;
    else if (name.find("41-") != Utf8String::npos)
        format.channelCount = 5;
    else if (name.find("51-") != Utf8String::npos)
        format.channelCount = 6;
    else if (name.find("71-") != Utf8String::npos)
        format.channelCount = 8;

    if (name.EndsWith("-8"))
        format.bytesPerChannel = 1;
    else if (name.EndsWith("-16"))
        format.bytesPerChannel = 2;
    else if (name.EndsWith("-24"))
        format.bytesPerChannel = 3;
    else if (name.EndsWith("-32"))
        format.bytesPerChannel = 4;
    else if (name.EndsWith("-32f"))
    {
        format.bytesPerChannel = 4;
        format.sampleType = SoundFormat::SampleType::FLOATING_POINT;
    }

    return format;
}


//Implementation----------------------------------------------------------------

//SoundFormat
SoundFormat::SoundFormat()
{
    this->channelCount = 0;
    this->bytesPerChannel = 0;
    this->samplesPerSecond = 0;
    this->sampleType = SampleType::INTEGER;
}

SoundFormat::SoundFormat(int channelCount, int bytesPerChannel, int samplesPerSecond)
{
    Set(channelCount, bytesPerChannel, samplesPerSecond, SampleType::INTEGER);
}

SoundFormat::SoundFormat(int channelCount, int bytesPerChannel, int samplesPerSecond, SampleType sampleType)
{
    Set(channelCount, bytesPerChannel, samplesPerSecond, sampleType);
}

bool SoundFormat::operator == (const SoundFormat& other) const
{
    return
        this->typeName == other.typeName &&
        this->channelCount == other.channelCount &&
        this->bytesPerChannel == other.bytesPerChannel &&
        this->samplesPerSecond == other.samplesPerSecond &&
        this->sampleType == other.sampleType;
}

bool SoundFormat::operator != (const SoundFormat& other) const
{
    return !(*this == other);
}

Utf8String SoundFormat::ToString() const
{
    auto formatString = this->sampleType == SampleType::FLOATING_POINT ? "float" : "int";
    return Utf8StringFormatter::Format("channels=%1%, bytes per channel=%2%, samples per second=%3%, sample type=%4%", this->channelCount, this->bytesPerChannel, this->samplesPerSecond, formatString);
}

void SoundFormat::Set(int channelCount, int bytesPerChannel, int samplesPerSecond, SampleType sampleType)
{
    this->channelCount = channelCount;
    this->bytesPerChannel = bytesPerChannel;
    this->samplesPerSecond = samplesPerSecond;
    this->sampleType = sampleType;
}

bool SoundFormat::IsValid() const
{
    return this->channelCount > 0 && this->bytesPerChannel > 0 && this->samplesPerSecond > 0;
}

void SoundFormat::Reset()
{
    this->typeName.clear();

    Set(0, 0, 0, SampleType::INTEGER);
}

int SoundFormat::GetChannelCount() const
{
    return this->channelCount;
}

int SoundFormat::GetBytesPerChannel() const
{
    return this->bytesPerChannel;
}

int SoundFormat::GetBytesPerSample() const
{
    return this->channelCount * this->bytesPerChannel;
}

int SoundFormat::GetBytesPerSecond() const
{
    return GetBytesPerSample() * this->samplesPerSecond;
}

SimpleTimeCounter SoundFormat::CalculateTimeLength(size_t size) const
{
    auto bytesPerSample = GetBytesPerSample();
    if (bytesPerSample > 0 && this->samplesPerSecond > 0)
    {
        auto sampleCount = RoundToDouble(size) / RoundToDouble(bytesPerSample);
        return sampleCount / RoundToDouble(this->samplesPerSecond);
    }
    else
        return 0;
}

int64_t SoundFormat::GetSampleByteOffset(SimpleTimeCounter timeOffset) const
{
    auto bytesPerSample = GetBytesPerSample();
    auto offset = RoundToInt64(timeOffset * this->samplesPerSecond * bytesPerSample);
    offset -= offset % bytesPerSample; //Ensure offset starts on a sample boundary
    return offset;
}

int64_t SoundFormat::GetSampleFrameOffset(SimpleTimeCounter timeOffset) const
{
    return RoundToInt64(timeOffset * this->samplesPerSecond);
}

Utf8String SoundFormat::ToTypeName() const
{
    Utf8String name;

    switch (this->samplesPerSecond)
    {
        case 11025: name += "low-"; break;
        case 22050: name += "medium-"; break;
        case 44100: name += "high-"; break;
        default: name += Convert::ToString(this->samplesPerSecond); name += "-"; break;
    }

    switch (this->channelCount)
    {
        case 1: name += "mono-"; break;
        case 2: name += "stereo-"; break;
        case 3: name += "21-"; break;
        case 4: name += "quad-"; break;
        case 5: name += "41-"; break;
        case 6: name += "51-"; break;
        case 8: name += "71-"; break;
    }

    switch (this->bytesPerChannel)
    {
        case 1: name += "-8"; break;
        case 2: name += "-16"; break;
        case 3: name += "-24"; break;
        case 4:
        {
            name += "-32";
            if (this->sampleType == SampleType::FLOATING_POINT)
                name += "f";
            break;
        }
    }

    if (name.EndsWith("-"))
        name.pop_back();

    return name;
}

SoundFormat SoundFormat::FromTypeName(const Utf8String& name)
{
    return ::FromTypeName(name);
}

SoundFormat SoundFormat::FromTypeName(const Utf8StringView& name)
{
    return ::FromTypeName(name);
}
