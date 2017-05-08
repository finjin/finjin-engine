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
#include "OpenALContext.hpp"
#include "finjin/common/AllocatedClass.hpp"
#include "finjin/common/DynamicVector.hpp"
#include "finjin/common/Error.hpp"
#include "finjin/common/OperationStatus.hpp"
#include "finjin/common/Path.hpp"
#include "finjin/common/StaticVector.hpp"
#include "finjin/common/UsableDynamicVector.hpp"
#include "finjin/engine/SoundSorter.hpp"
#include "OpenALSoundBuffer.hpp"
#include "OpenALSoundGroup.hpp"
#include "OpenALSoundSource.hpp"
#include "OpenALEffect.hpp"
#include "OpenALEffectSlot.hpp"
#include "OpenALFilter.hpp"
#include "OpenALExtensions.hpp"
#include "OpenALListener.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    /** Buffer format identifiers for various extended channel formats */
    class OpenALExtendedChannelFormats
    {
    public:
        OpenALExtendedChannelFormats()
        {
            this->AL_FORMAT_QUAD8 = 0;
            this->AL_FORMAT_51CHN8 = 0;
            this->AL_FORMAT_61CHN8 = 0;
            this->AL_FORMAT_71CHN8 = 0;
            this->AL_FORMAT_QUAD16 = 0;
            this->AL_FORMAT_51CHN16 = 0;
            this->AL_FORMAT_61CHN16 = 0;
            this->AL_FORMAT_71CHN16 = 0;
        }

        bool Initialize(ALCdevice* device)
        {
            this->AL_FORMAT_QUAD8 = alGetEnumValue("AL_FORMAT_QUAD8");
            this->AL_FORMAT_51CHN8 = alGetEnumValue("AL_FORMAT_51CHN8");
            this->AL_FORMAT_61CHN8 = alGetEnumValue("AL_FORMAT_61CHN8");
            this->AL_FORMAT_71CHN8 = alGetEnumValue("AL_FORMAT_71CHN8");
            this->AL_FORMAT_QUAD16 = alGetEnumValue("AL_FORMAT_QUAD16");
            this->AL_FORMAT_51CHN16 = alGetEnumValue("AL_FORMAT_51CHN16");
            this->AL_FORMAT_61CHN16 = alGetEnumValue("AL_FORMAT_61CHN16");
            this->AL_FORMAT_71CHN16 = alGetEnumValue("AL_FORMAT_71CHN16");

            //If any format is non-zero, consider the initialization a success
            return
                this->AL_FORMAT_QUAD8 != 0 ||
                this->AL_FORMAT_51CHN8 != 0 ||
                this->AL_FORMAT_61CHN8 != 0 ||
                this->AL_FORMAT_71CHN8 != 0 ||
                this->AL_FORMAT_QUAD16 != 0 ||
                this->AL_FORMAT_51CHN16 != 0 ||
                this->AL_FORMAT_61CHN16 != 0 ||
                this->AL_FORMAT_71CHN16 != 0;
        }

        size_t GetMaxChannelCount() const
        {
            if (this->AL_FORMAT_71CHN16 != 0 || this->AL_FORMAT_71CHN8 != 0)
                return 8;
            if (this->AL_FORMAT_61CHN16 != 0 || this->AL_FORMAT_61CHN8 != 0)
                return 7;
            if (this->AL_FORMAT_51CHN16 != 0 || this->AL_FORMAT_51CHN8 != 0)
                return 6;
            if (this->AL_FORMAT_QUAD16 != 0 || this->AL_FORMAT_QUAD8 != 0)
                return 4;

            return 0;
        }

        //If any of these are 0, that means the format is not supported
        ALenum AL_FORMAT_QUAD8; //8-bit 4-channel
        ALenum AL_FORMAT_51CHN8; //8-bit 5.1 channels
        ALenum AL_FORMAT_61CHN8; //8-bit 6.1 channels
        ALenum AL_FORMAT_71CHN8; //8-bit 7.1 channels
        ALenum AL_FORMAT_QUAD16; //16-bit 4 channels
        ALenum AL_FORMAT_51CHN16; //16-bit 5.1 channels
        ALenum AL_FORMAT_61CHN16; //16-bit 6.1 channels
        ALenum AL_FORMAT_71CHN16; //16-bit 7.1 channels
    };

    enum class SoundContextChange
    {
        NONE = 0,
        ROLLOFF_FACTOR = 1 << 0,
        REFERENCE_DISTANCE = 1 << 1,
        MAX_DISTANCE = 1 << 2,
        ALL = 0xffff
    };
    FINJIN_ENUM_BITWISE_OPERATIONS(SoundContextChange)

    class OpenALContextImpl : public AllocatedClass
    {
    public:
        OpenALContextImpl(Allocator* allocator, OpenALSystem* soundSystem);

        void DestroyFilter(OpenALFilter* filter, bool shuttingDown);

        struct SoundSourceLookup
        {
            SoundFormat format;

            UsableDynamicVector<OpenALSourceVoice> sourceVoices;

            UsableDynamicVectorOfPointers<OpenALSoundSource*> sources;

            DynamicVector<OpenALSoundSource*> playingList;

            bool Virtualize(OpenALSoundSource* source);
            bool Realize(OpenALSoundSource* source);
        };

        SoundSourceLookup* GetSoundSourceLookup(const SoundFormat& soundFormat);
        SoundSourceLookup* AddSoundSourceLookup(size_t& count, const SoundFormat& soundFormat, size_t maxVoiceCount, Error& error);

        OpenALSoundBuffer* CreateBuffer(const ByteBuffer& bytes, Error& error);

        OpenALSoundSource* CreateSource(const SoundFormat& soundFormat, Error& error);

        OpenALSystem* soundSystem;

        OpenALContext::Settings settings;

        AssetClassFileReader settingsAssetFileReader;

        ALCdevice* aldevice;
        ALCcontext* alcontext;
        size_t maxSimultaneousChannels;
        int maxAuxSendsPerSource;
        OpenALKnownExtension extensions;

        OpenALListener listener;

        OpenALSoundGroup masterSoundGroup;

        DynamicVector<SoundSourceLookup> soundSourceLookups;
        DynamicVector<OpenALSoundSource> sources;
        StaticVector<ALuint, FINJIN_OPENAL_MAX_SOURCE_IDS> sourceIDs;

        UsableDynamicVector<OpenALSoundBuffer> buffers;

        SoundContextChange invalidated;

        SoundSorter<OpenALSoundSource>* soundSorter;
        bool soundSortingEnabled;

        float metersPerUnit;
        float referenceDistance;
        float maxDistance;
        float rolloffFactor;
        int createID;

        OpenALLowPassFilter lowPassFilter;
        OpenALHighPassFilter highPassFilter;
        OpenALBandPassFilter bandPassFilter;

        StaticVector<OpenALEffectSlot, 4> effectSlots; //Creative Labs X-Fi has 4, so this seems a reasonable upper limit

        OpenALExtendedChannelFormats extendedChannelFormats;

        OpenALEFXExtensions efxExtensions;

        ByteBuffer readBuffer;
    };

} }
