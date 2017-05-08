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
#include "OpenALFilter.hpp"
#include "finjin/common/IntrusiveList.hpp"
#include "finjin/common/Math.hpp"
#include "OpenALContext.hpp"
#include "OpenALExtensions.hpp"
#include "OpenALSoundSource.hpp"

using namespace Finjin::Engine;


//Implementation----------------------------------------------------------------

//OpenALFilter
OpenALFilter::OpenALFilter()
{
    this->context = context;
    this->enabled = false;
    this->filterID = AL_FILTER_NULL;
    this->invalidated = false;
    this->sourcesHead = nullptr;
}

OpenALFilter::~OpenALFilter()
{
}

void OpenALFilter::Create(OpenALContext* context)
{
    this->context = context;
    this->enabled = true;

    auto& efx = this->context->GetEfxExtensions();
    if (efx.alGenFilters != nullptr && GetFilterType() != AL_FILTER_NULL)
    {
        efx.alGenFilters(1, &this->filterID);
        if (alGetError() == AL_NO_ERROR)
        {
            //Make sure filter can be set to specified type
            efx.alFilteri(this->filterID, AL_FILTER_TYPE, GetFilterType());
            if (alGetError() != AL_NO_ERROR)
            {
                //Failed to set type. Destroy filter
                efx.alDeleteFilters(1, &this->filterID);
                this->filterID = AL_FILTER_NULL;
            }
        }
    }

    this->invalidated = this->filterID != AL_FILTER_NULL;

    this->sourcesHead = nullptr;
}

ALuint OpenALFilter::_GetFilterID() const
{
    return this->filterID;
}

bool OpenALFilter::IsValid() const
{
    return this->filterID != AL_FILTER_NULL;
}

ALenum OpenALFilter::GetFilterType() const
{
    return AL_FILTER_NULL;
}

void OpenALFilter::Commit()
{
    auto& efx = this->context->GetEfxExtensions();

    if (this->invalidated)
    {
        //The filter state has been changed
        //std::cout << "Filter state has been changed" << std::endl;

        //Disable filter if it is now disabled
        if (!this->enabled)
            efx.alFilteri(this->filterID, AL_FILTER_TYPE, AL_FILTER_NULL);

        this->invalidated = false;
    }
}

void OpenALFilter::Enable(bool enable)
{
    this->enabled = enable;
    this->invalidated = this->filterID != AL_FILTER_NULL;
}

bool OpenALFilter::IsEnabled() const
{
    return this->enabled;
}

void OpenALFilter::AddSource(OpenALSoundSource* source)
{
    FINJIN_INTRUSIVE_SINGLE_LIST_ADD_UNIQUE_ITEM(this->sourcesHead, filterNext, source)
}

void OpenALFilter::RemoveSource(OpenALSoundSource* source)
{
    FINJIN_INTRUSIVE_SINGLE_LIST_REMOVE_ITEM(this->sourcesHead, filterNext, source)
}

//OpenALLowPassFilter
OpenALLowPassFilter::OpenALLowPassFilter()
{
    this->gain = 1.0f;
    this->gainHighFrequency = 1.0f;
}

void OpenALLowPassFilter::Create(OpenALContext* context)
{
    OpenALFilter::Create(context);
}

ALenum OpenALLowPassFilter::GetFilterType() const
{
    return AL_FILTER_LOWPASS;
}

void OpenALLowPassFilter::Commit()
{
    if (this->invalidated && this->enabled)
    {
        auto& efx = this->context->GetEfxExtensions();
        efx.alFilteri(this->filterID, AL_FILTER_TYPE, AL_FILTER_LOWPASS);
        efx.alFilterf(this->filterID, AL_LOWPASS_GAIN, this->gain);
        efx.alFilterf(this->filterID, AL_LOWPASS_GAINHF, this->gainHighFrequency);
    }

    OpenALFilter::Commit();
}

float OpenALLowPassFilter::GetGain() const
{
    return this->gain;
}

void OpenALLowPassFilter::SetGain(float gain)
{
    Limit(gain, 0.0f, 1.0f);
    this->gain = gain;
    this->invalidated = this->filterID != AL_FILTER_NULL;
}

float OpenALLowPassFilter::GetGainHighFrequency() const
{
    return this->gainHighFrequency;
}

void OpenALLowPassFilter::SetGainHighFrequency(float gainHighFrequency)
{
    Limit(gainHighFrequency, 0.0f, 1.0f);
    this->gainHighFrequency = gainHighFrequency;
    this->invalidated = this->filterID != AL_FILTER_NULL;
}

//OpenALHighPassFilter
OpenALHighPassFilter::OpenALHighPassFilter()
{
    this->gain = 1.0f;
    this->gainLowFrequency = 1.0f;
}

void OpenALHighPassFilter::Create(OpenALContext* context)
{
    OpenALFilter::Create(context);
}

ALenum OpenALHighPassFilter::GetFilterType() const
{
    return AL_FILTER_HIGHPASS;
}

void OpenALHighPassFilter::Commit()
{
    if (this->invalidated && this->enabled)
    {
        auto& efx = this->context->GetEfxExtensions();
        efx.alFilteri(this->filterID, AL_FILTER_TYPE, AL_FILTER_HIGHPASS);
        efx.alFilterf(this->filterID, AL_HIGHPASS_GAIN, this->gain);
        efx.alFilterf(this->filterID, AL_HIGHPASS_GAINLF, this->gainLowFrequency);
    }

    OpenALFilter::Commit();
}

float OpenALHighPassFilter::GetGain() const
{
    return this->gain;
}

void OpenALHighPassFilter::SetGain(float gain)
{
    Limit(gain, 0.0f, 1.0f);
    this->gain = gain;
    this->invalidated = this->filterID != AL_FILTER_NULL;
}

float OpenALHighPassFilter::GetGainLowFrequency() const
{
    return this->gainLowFrequency;
}

void OpenALHighPassFilter::SetGainLowFrequency(float gainLowFrequency)
{
    Limit(gainLowFrequency, 0.0f, 1.0f);
    this->gainLowFrequency = gainLowFrequency;
    this->invalidated = this->filterID != AL_FILTER_NULL;
}

//OpenALBandPassFilter
OpenALBandPassFilter::OpenALBandPassFilter()
{
    this->gain = 1.0f;
    this->gainHighFrequency = 1.0f;
    this->gainLowFrequency = 1.0f;
}

void OpenALBandPassFilter::Create(OpenALContext* context)
{
     OpenALFilter::Create(context);
}

ALenum OpenALBandPassFilter::GetFilterType() const
{
    return AL_FILTER_BANDPASS;
}

void OpenALBandPassFilter::Commit()
{
    if (this->invalidated && this->enabled)
    {
        auto& efx = this->context->GetEfxExtensions();
        efx.alFilteri(this->filterID, AL_FILTER_TYPE, AL_FILTER_BANDPASS);
        efx.alFilterf(this->filterID, AL_BANDPASS_GAIN, this->gain);
        efx.alFilterf(this->filterID, AL_BANDPASS_GAINHF, this->gainHighFrequency);
        efx.alFilterf(this->filterID, AL_BANDPASS_GAINLF, this->gainLowFrequency);
    }

    OpenALFilter::Commit();
}

float OpenALBandPassFilter::GetGain() const
{
    return this->gain;
}

void OpenALBandPassFilter::SetGain(float gain)
{
    Limit(gain, 0.0f, 1.0f);
    this->gain = gain;
    this->invalidated = this->filterID != AL_FILTER_NULL;
}

float OpenALBandPassFilter::GetGainHighFrequency() const
{
    return this->gainHighFrequency;
}

void OpenALBandPassFilter::SetGainHighFrequency(float gainHighFrequency)
{
    Limit(gainHighFrequency, 0.0f, 1.0f);
    this->gainHighFrequency = gainHighFrequency;
    this->invalidated = this->filterID != AL_FILTER_NULL;
}

float OpenALBandPassFilter::GetGainLowFrequency() const
{
    return this->gainLowFrequency;
}

void OpenALBandPassFilter::SetGainLowFrequency(float gainLowFrequency)
{
    Limit(gainLowFrequency, 0.0f, 1.0f);
    this->gainLowFrequency = gainLowFrequency;
    this->invalidated = this->filterID != AL_FILTER_NULL;
}
