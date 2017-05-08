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
#include "finjin/common/AllocatedClass.hpp"
#include "finjin/common/DynamicVector.hpp"
#include "finjin/common/Error.hpp"
#include "finjin/common/OperationStatus.hpp"
#include "finjin/common/UsableDynamicVector.hpp"
#include "finjin/engine/AssetFileReader.hpp"
#include "AVAudioEngineSoundBuffer.hpp"
#include "AVAudioEngineSoundContext.hpp"
#include "AVAudioEngineSoundGroup.hpp"
#include "AVAudioEngineSoundListener.hpp"
#include "AVAudioEngineSoundSource.hpp"
#include "AVAudioEngineSoundSourceImpl.hpp"
#include "SoundFormat.hpp"
#include "SoundSorter.hpp"
#import <AVFoundation/AVFoundation.h>


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    class AVAudioEngineSoundSystem;

    class AVAudioEngineSoundSourceLookup
    {
    public:
        SoundFormat format;
        AVAudioFormat* avformat;

        Finjin::Common::UsableDynamicVector<AVAudioEngineSoundSourceVoice> sourceVoices;

        Finjin::Common::UsableDynamicVectorOfPointers<AVAudioEngineSoundSource*> sources;

        Finjin::Common::DynamicVector<AVAudioEngineSoundSource*> playingList;

        AVAudioEngineSoundSourceLookup();

        bool Virtualize(AVAudioEngineSoundSource* source);
        bool Realize(AVAudioEngineSoundSource* source);
    };

} }

@interface AVAudioEngineSoundContextImplNSObject : NSObject
{
@public
    Finjin::Engine::AVAudioEngineSoundContext::Settings settings;

    Finjin::Engine::AssetClassFileReader settingsAssetFileReader;

    AVAudioEngine* avengine;
    AVAudioEnvironmentNode* avenv;
    AVAudioFormat* avenvOutputFormat;

    Finjin::Common::Allocator* allocator;

    Finjin::Engine::AVAudioEngineSoundSystem* soundSystem;

    Finjin::Engine::AVAudioEngineSoundListener listener;

    Finjin::Engine::AVAudioEngineSoundGroup masterSoundGroup;

    Finjin::Common::DynamicVector<Finjin::Engine::AVAudioEngineSoundSourceLookup> soundSourceLookups;
    Finjin::Common::DynamicVector<Finjin::Engine::AVAudioEngineSoundSource> sources;

    Finjin::Common::UsableDynamicVector<Finjin::Engine::AVAudioEngineSoundBuffer> buffers;

    Finjin::Engine::SoundSorter<Finjin::Engine::AVAudioEngineSoundSource>* soundSorter;
    bool soundSortingEnabled;

    float metersPerUnit;
    float dopplerFactor;
    int createID;

    Finjin::Common::ByteBuffer readBuffer;

    bool _isSessionInterrupted;
    bool _isConfigChangePending;
    id<NSObject> configChangeNotificationID;

    bool _addedSessionObservers;
}

- (Finjin::Engine::AVAudioEngineSoundSourceLookup*)getSoundSourceLookup:(const Finjin::Engine::SoundFormat&)soundFormat;

@end

namespace Finjin { namespace Engine {

    class AVAudioEngineSoundSystem;

    class AVAudioEngineSoundContextImpl : public AllocatedClass
    {
    public:
        AVAudioEngineSoundContextImpl(Allocator* allocator, AVAudioEngineSoundSystem* soundSystem);

        void Initialize(const AVAudioEngineSoundContext::Settings& settings, Error& error);
        void Destroy();

        void Update(SimpleTimeDelta elapsedTime);

        void CreateBuffer(AVAudioEngineSoundBuffer* item, const AssetReference& assetRef, ByteBufferReader& reader, Error& error);
        AVAudioEngineSoundBuffer* CreateBuffer(const AssetReference& assetRef, const ByteBuffer& bytes, Error& error);

        AVAudioEngineSoundSource* CreateSource(const SoundFormat& soundFormat, Error& error);

    public:
        AVAudioEngineSoundContextImplNSObject* nsimpl;
    };

} }
