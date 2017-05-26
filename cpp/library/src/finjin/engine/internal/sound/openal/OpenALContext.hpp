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
#include "finjin/common/Chrono.hpp"
#include "finjin/common/EnumBitwise.hpp"
#include "finjin/common/Error.hpp"
#include "finjin/common/OperationStatus.hpp"
#include "finjin/common/Path.hpp"
#include "finjin/engine/SoundContextCommonSettings.hpp"
#include "finjin/engine/SoundFormat.hpp"
#include "finjin/engine/SoundSorter.hpp"
#include "OpenALExtensions.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    class OpenALSystem;
    class OpenALSoundGroup;
    class OpenALSoundSource;
    class OpenALSoundBuffer;
    class OpenALListener;
    class OpenALEffectSlot;
    class OpenALEffect;
    class OpenALFilter;
    class OpenALContextImpl;

    using namespace Finjin::Common;

    /** Extensions that are known and handled by this sound library */
    enum class OpenALKnownExtension
    {
        NONE = 0,

        EFX = 1 << 0,
        EXTENDED_CHANNEL_FORMATS = 1 << 1,

        ALL = EFX | EXTENDED_CHANNEL_FORMATS
    };
    FINJIN_ENUM_BITWISE_OPERATIONS(OpenALKnownExtension)

    class OpenALContext : public AllocatedClass
    {
    public:
        OpenALContext(Allocator* allocator, OpenALSystem* soundSystem);
        ~OpenALContext();

        struct Settings : SoundContextCommonSettings
        {
            Settings(Allocator* initialAllocator) : SoundContextCommonSettings(initialAllocator)
            {
                this->requestedExtensions = OpenALKnownExtension::ALL;
            }

            OpenALKnownExtension requestedExtensions;
        };

        void Create(const Settings& settings, Error& error);
        void Destroy();

        void GetSelectorComponents(AssetPathSelector& result);

        const Settings& GetSettings() const;

        size_t GetExternalAssetFileExtensions(StaticVector<Utf8String, EngineConstants::MAX_EXTERNAL_ASSET_FILE_EXTENSIONS>& extensions, AssetClass assetClass, Error& error);

        void HandleApplicationViewportLostFocus();
        void HandleApplicationViewportGainedFocus();

        void Update(SimpleTimeDelta elapsedTime);

        void Execute(SoundEvents& events, SoundCommands& commands, Error& error);

        OpenALListener* GetListener();

        OpenALSoundGroup* GetMasterGroup();

        void DestroyBuffer(OpenALSoundBuffer* buffer, bool shuttingDown = false);

        OpenALSoundSource* CreateSource(OpenALSoundBuffer* soundBuffer, Error& error);
        OpenALSoundSource* CreateSource(const SoundFormat& soundFormat, Error& error);
        void DestroySource(OpenALSoundSource* source, bool shuttingDown = false);

        SoundSorter<OpenALSoundSource>* GetSoundSorter();
        void SetSoundSorter(SoundSorter<OpenALSoundSource>* soundSorter);

        bool IsSoundSortingEnabled() const;
        void EnableSoundSorting(bool enable = true);

        float GetDopplerFactor() const;
        void SetDopplerFactor(float factor);

        float GetVolume() const;
        void SetVolume(float volume);

        float GetMetersPerUnit() const;
        void SetMetersPerUnit(float metersPerUnit);

        float GetReferenceDistance() const;
        void SetReferenceDistance(float distance);

        float GetMaxDistance() const;
        void SetMaxDistance(float distance);

        float GetRolloffFactor() const;
        void SetRolloffFactor(float factor);

        SoundDistanceModel GetDistanceModel() const;
        void SetDistanceModel(SoundDistanceModel model);

        //OpenAL-related methods---------------
        OpenALContextImpl* GetImpl();

        void AddSourceLowPassFilter(OpenALSoundSource* source);
        void AddSourceHighPassFilter(OpenALSoundSource* source);
        void AddSourceBandPassFilter(OpenALSoundSource* source);

        size_t GetMaxSourceChannelCount() const;

        size_t GetEffectSlotCount() const;
        OpenALEffectSlot* GetEffectSlot(size_t index);

        bool HasExtension(OpenALKnownExtension extension) const;

        size_t GetMaxAuxSendsPerSource() const;

        ALenum SoundFormatToEnum(const SoundFormat& soundFormat);

        const OpenALEFXExtensions& GetEfxExtensions() const;

    private:
        std::unique_ptr<OpenALContextImpl> impl;
    };

} }
