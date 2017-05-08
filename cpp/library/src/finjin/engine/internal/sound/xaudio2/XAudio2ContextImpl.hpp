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
#include "finjin/common/Error.hpp"
#include "finjin/common/Path.hpp"
#include "finjin/common/OperationStatus.hpp"
#include "finjin/common/UsableDynamicVector.hpp"
#include "XAudio2Context.hpp"
#include "XAudio2Includes.hpp"
#include "XAudio2Listener.hpp"
#include "XAudio2SoundBuffer.hpp"
#include "XAudio2SoundGroup.hpp"
#include "XAudio2SoundSourceVoice.hpp"
#include "XAudio2SoundSource.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    class XAudio2ContextImpl : public AllocatedClass
    {
    public:
        XAudio2ContextImpl(Allocator* allocator, XAudio2System* soundSystem);

        bool CreateSourceVoice(XAudio2SoundSourceVoice& sourceVoice, const SoundFormat& soundFormat);

        struct SoundSourceLookup
        {
            SoundFormat format;

            UsableDynamicVector<XAudio2SoundSourceVoice> sourceVoices;

            UsableDynamicVectorOfPointers<XAudio2SoundSource*> sources;

            DynamicVector<XAudio2SoundSource*> playingList;

            bool Virtualize(XAudio2SoundSource* source);
            bool Realize(XAudio2SoundSource* source);
        };

        SoundSourceLookup* GetSoundSourceLookup(const SoundFormat& soundFormat);
        SoundSourceLookup* AddSoundSourceLookup(const SoundFormat& soundFormat, size_t maxVoiceCount, Error& error);

        XAudio2SoundSource* CreateSource(const SoundFormat& soundFormat, Error& error);

        XAudio2SoundBuffer* CreateBuffer(const ByteBuffer& bytes, Error& error);

        XAudio2System* soundSystem;

        XAudio2Context::Settings settings;

        AssetClassFileReader settingsAssetFileReader;

        IXAudio2* xaudioInterface;
        IXAudio2MasteringVoice* masteringVoice;
        size_t masteringVoiceChannelCount;
        X3DAUDIO_HANDLE x3dAudioInterface;

        XAudio2Listener listener;

        XAudio2SoundGroup masterSoundGroup;

        DynamicVector<SoundSourceLookup> soundSourceLookups;
        DynamicVector<XAudio2SoundSource> sources;

        UsableDynamicVector<XAudio2SoundBuffer> buffers;

        SoundSorter<XAudio2SoundSource>* soundSorter;
        bool soundSortingEnabled;

        float metersPerUnit;
        float dopplerFactor;
        float referenceDistance;
        float maxDistance;
        float rolloffFactor;
        int createID;

        ByteBuffer readBuffer;
    };

} }
