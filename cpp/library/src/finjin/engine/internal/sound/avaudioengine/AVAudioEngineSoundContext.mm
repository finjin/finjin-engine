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
#include "AVAudioEngineSoundContext.hpp"
#include "finjin/common/DebugLog.hpp"
#include "AVAudioEngineSoundBufferImpl.hpp"
#include "AVAudioEngineSoundContextImpl.hpp"
#include "AVAudioEngineSoundSourceImpl.hpp"

using namespace Finjin::Engine;


//Implementation----------------------------------------------------------------
AVAudioEngineSoundContext::AVAudioEngineSoundContext(Allocator* allocator, AVAudioEngineSoundSystem* soundSystem) :
    AllocatedClass(allocator),
    impl(AllocatedClass::New<AVAudioEngineSoundContextImpl>(allocator, FINJIN_CALLER_ARGUMENTS, soundSystem))
{
}

AVAudioEngineSoundContext::~AVAudioEngineSoundContext()
{
}

void AVAudioEngineSoundContext::Create(const Settings& settings, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    FINJIN_ENGINE_CHECK_IMPL_NOT_NULL(impl, error);

    impl->Initialize(settings, error);
    if (error)
    {
        FINJIN_SET_ERROR_NO_MESSAGE(error);
        return;
    }
}

void AVAudioEngineSoundContext::GetSelectorComponents(AssetPathSelector& result)
{
}

const AVAudioEngineSoundContext::Settings& AVAudioEngineSoundContext::GetSettings() const
{
    return impl->nsimpl->settings;
}

size_t AVAudioEngineSoundContext::GetExternalAssetFileExtensions(StaticVector<Utf8String, EngineConstants::MAX_EXTERNAL_ASSET_FILE_EXTENSIONS>& extensions, AssetClass assetClass, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    switch (assetClass)
    {
        case AssetClass::SOUND:
        {
            size_t count = 0;
            for (auto ext : { "wav" })
            {
                if (extensions.push_back(ext).HasErrorOrValue(false))
                {
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to add '%1%' extension.", ext));
                    return count;
                }
                count++;
            }
            return count;
        }
        default: break;
    }

    return 0;
}

void AVAudioEngineSoundContext::Destroy()
{
    impl->Destroy();
}

void AVAudioEngineSoundContext::HandleApplicationViewportLostFocus()
{
}

void AVAudioEngineSoundContext::HandleApplicationViewportGainedFocus()
{
}

void AVAudioEngineSoundContext::Update(SimpleTimeDelta elapsedTime)
{
    impl->Update(elapsedTime);
}

void AVAudioEngineSoundContext::Execute(SoundEvents& events, SoundCommands& commands, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);
}

AVAudioEngineSoundListener* AVAudioEngineSoundContext::GetListener()
{
    return &impl->nsimpl->listener;
}

AVAudioEngineSoundGroup* AVAudioEngineSoundContext::GetMasterGroup()
{
    return &impl->nsimpl->masterSoundGroup;
}

/*AVAudioEngineSoundBuffer* AVAudioEngineSoundContext::CreateBuffer(const AssetReference& assetRef, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto item = impl->CreateBuffer(assetRef, error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);

    return item;
}*/

void AVAudioEngineSoundContext::DestroyBuffer(AVAudioEngineSoundBuffer* buffer)
{
    if (impl->nsimpl->buffers.IsUsed(buffer))
    {
        while (buffer->sourcesHead != nullptr)
            buffer->sourcesHead->SetBuffer(nullptr);

        impl->nsimpl->buffers.Unuse(buffer);
    }
}

AVAudioEngineSoundSource* AVAudioEngineSoundContext::CreateSource(AVAudioEngineSoundBuffer* soundBuffer, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto source = CreateSource(soundBuffer->GetSoundFormat(), error);
    if (error)
    {
        FINJIN_SET_ERROR_NO_MESSAGE(error);
        return nullptr;
    }

    source->SetBuffer(soundBuffer);
    return source;
}

AVAudioEngineSoundSource* AVAudioEngineSoundContext::CreateSource(const SoundFormat& soundFormat, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto source = impl->CreateSource(soundFormat, error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);

    return source;
}

void AVAudioEngineSoundContext::DestroySource(AVAudioEngineSoundSource* source)
{
    auto soundSourceLookup = [impl->nsimpl getSoundSourceLookup:source->GetSoundFormat()];
    if (soundSourceLookup != nullptr)
    {
        if (soundSourceLookup->sources.IsUsed(source))
        {
            soundSourceLookup->Virtualize(source);

            source->SetBuffer(nullptr);

            source->SetGroup(&impl->nsimpl->masterSoundGroup);

            soundSourceLookup->sources.Unuse(source);
        }
    }
}

SoundSorter<AVAudioEngineSoundSource>* AVAudioEngineSoundContext::GetSoundSorter()
{
    return impl->nsimpl->soundSorter;
}

void AVAudioEngineSoundContext::SetSoundSorter(SoundSorter<AVAudioEngineSoundSource>* soundSorter)
{
    impl->nsimpl->soundSorter = soundSorter;
}

bool AVAudioEngineSoundContext::IsSoundSortingEnabled() const
{
    return impl->nsimpl->soundSortingEnabled;
}

void AVAudioEngineSoundContext::EnableSoundSorting(bool enable)
{
    impl->nsimpl->soundSortingEnabled = enable;
}

float AVAudioEngineSoundContext::GetDopplerFactor() const
{
    return impl->nsimpl->dopplerFactor;
}

void AVAudioEngineSoundContext::SetDopplerFactor(float factor)
{
    impl->nsimpl->dopplerFactor = factor;
}

float AVAudioEngineSoundContext::GetVolume() const
{
    return impl->nsimpl->avenv.outputVolume;
}

void AVAudioEngineSoundContext::SetVolume(float volume)
{
    impl->nsimpl->avenv.outputVolume = volume;
}

float AVAudioEngineSoundContext::GetMetersPerUnit() const
{
    return impl->nsimpl->metersPerUnit;
}

void AVAudioEngineSoundContext::SetMetersPerUnit(float metersPerUnit)
{
    impl->nsimpl->metersPerUnit = metersPerUnit;
}

float AVAudioEngineSoundContext::GetReferenceDistance() const
{
    return impl->nsimpl->avenv.distanceAttenuationParameters.referenceDistance;
}

void AVAudioEngineSoundContext::SetReferenceDistance(float distance)
{
    if (distance < 0)
        distance = 0;

    impl->nsimpl->avenv.distanceAttenuationParameters.referenceDistance = distance;
}

float AVAudioEngineSoundContext::GetMaxDistance() const
{
    return impl->nsimpl->avenv.distanceAttenuationParameters.maximumDistance;
}

void AVAudioEngineSoundContext::SetMaxDistance(float distance)
{
    if (distance < 0)
        distance = 0;

    impl->nsimpl->avenv.distanceAttenuationParameters.maximumDistance = distance;
}

float AVAudioEngineSoundContext::GetRolloffFactor() const
{
    return impl->nsimpl->avenv.distanceAttenuationParameters.rolloffFactor;
}

void AVAudioEngineSoundContext::SetRolloffFactor(float factor)
{
    impl->nsimpl->avenv.distanceAttenuationParameters.rolloffFactor = factor;
}

SoundDistanceModel AVAudioEngineSoundContext::GetDistanceModel() const
{
    switch (impl->nsimpl->avenv.distanceAttenuationParameters.distanceAttenuationModel)
    {
        case AVAudioEnvironmentDistanceAttenuationModelExponential: return SoundDistanceModel::EXPONENT_DISTANCE_CLAMPED;
        case AVAudioEnvironmentDistanceAttenuationModelInverse: return SoundDistanceModel::INVERSE_DISTANCE_CLAMPED;
        case AVAudioEnvironmentDistanceAttenuationModelLinear: return SoundDistanceModel::LINEAR_DISTANCE_CLAMPED;
        default: return SoundDistanceModel::LINEAR_DISTANCE_CLAMPED;
    }
}

void AVAudioEngineSoundContext::SetDistanceModel(SoundDistanceModel model)
{
    switch (model)
    {
        case SoundDistanceModel::INVERSE_DISTANCE_CLAMPED:
        {
            impl->nsimpl->avenv.distanceAttenuationParameters.distanceAttenuationModel = AVAudioEnvironmentDistanceAttenuationModelInverse;
            break;
        }
        case SoundDistanceModel::LINEAR_DISTANCE_CLAMPED:
        {
            impl->nsimpl->avenv.distanceAttenuationParameters.distanceAttenuationModel = AVAudioEnvironmentDistanceAttenuationModelLinear;
            break;
        }
        case SoundDistanceModel::EXPONENT_DISTANCE_CLAMPED:
        {
            impl->nsimpl->avenv.distanceAttenuationParameters.distanceAttenuationModel = AVAudioEnvironmentDistanceAttenuationModelExponential;
            break;
        }
        default: break;
    }
}
