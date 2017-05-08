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
#include "OpenALIncludes.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    class OpenALSoundSource;
    class OpenALContext;

    /** Base filter class */
    class OpenALFilter
    {
        friend class OpenALContext;
        friend class OpenALContextImpl;

    public:
        OpenALFilter();
        virtual ~OpenALFilter();

        void Create(OpenALContext* context);

        ALuint _GetFilterID() const;

        bool IsValid() const;

        virtual ALenum GetFilterType() const;

        virtual void Commit();

        void Enable(bool enable = true);
        bool IsEnabled() const;

        void AddSource(OpenALSoundSource* source);
        void RemoveSource(OpenALSoundSource* source);

    protected:
        OpenALContext* context;
        ALuint filterID;
        bool invalidated;
        bool enabled;
        OpenALSoundSource* sourcesHead; //Linked list of sources that use this sound buffer. Use sourcesHead->filterNext to iterate through list
    };

    /** Removes high frequency content from a signal */
    class OpenALLowPassFilter : public OpenALFilter
    {
    public:
        OpenALLowPassFilter();

        void Create(OpenALContext* context);

        ALenum GetFilterType() const override;

        void Commit() override;

        float GetGain() const;
        void SetGain(float gain);

        float GetGainHighFrequency() const;
        void SetGainHighFrequency(float gainHighFrequency);

    protected:
        float gain;
        float gainHighFrequency;
    };

    /** Removes low frequency content from a signal */
    class OpenALHighPassFilter : public OpenALFilter
    {
    public:
        OpenALHighPassFilter();

        void Create(OpenALContext* context);

        ALenum GetFilterType() const override;

        void Commit() override;

        float GetGain() const;
        void SetGain(float gain);

        float GetGainLowFrequency() const;
        void SetGainLowFrequency(float gainHighFrequency);

    protected:
        float gain;
        float gainLowFrequency;
    };

    /** Removes low and high frequency content from a signal */
    class OpenALBandPassFilter : public OpenALFilter
    {
    public:
        OpenALBandPassFilter();

        void Create(OpenALContext* context);

        ALenum GetFilterType() const override;

        void Commit() override;

        float GetGain() const;
        void SetGain(float gain);

        float GetGainHighFrequency() const;
        void SetGainHighFrequency(float gainHighFrequency);

        float GetGainLowFrequency() const;
        void SetGainLowFrequency(float gainLowFrequency);

    protected:
        float gain;
        float gainHighFrequency;
        float gainLowFrequency;
    };

} }
