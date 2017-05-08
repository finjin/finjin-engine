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
#include "finjin/common/Error.hpp"
#include "finjin/common/OperationStatus.hpp"
#include "finjin/common/Path.hpp"
#include "finjin/engine/SoundContextCommonSettings.hpp"
#include "finjin/engine/SoundFormat.hpp"
#include "finjin/engine/SoundSorter.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    class OpenSLESSystem;
    class OpenSLESSoundGroup;
    class OpenSLESSoundSource;
    class OpenSLESSoundBuffer;
    class OpenSLESListener;

    using namespace Finjin::Common;

    class OpenSLESContext : public AllocatedClass
    {
    public:
        OpenSLESContext(Allocator* allocator, OpenSLESSystem* soundSystem);
        ~OpenSLESContext();

        struct Settings : SoundContextCommonSettings
        {
            Settings(Allocator* allocator) : SoundContextCommonSettings(allocator)
            {
            }
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

        OpenSLESListener* GetListener();

        OpenSLESSoundGroup* GetMasterGroup();

        void DestroyBuffer(OpenSLESSoundBuffer* buffer, bool shuttingDown = false);

        OpenSLESSoundSource* CreateSource(OpenSLESSoundBuffer* soundBuffer, Error& error);
        OpenSLESSoundSource* CreateSource(const SoundFormat& soundFormat, Error& error);
        void DestroySource(OpenSLESSoundSource* source, bool shuttingDown = false);

        SoundSorter<OpenSLESSoundSource>* GetSoundSorter();
        void SetSoundSorter(SoundSorter<OpenSLESSoundSource>* soundSorter);

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

        //OpenSL-specific methods---------------
        int NewCreateTimeStamp();

    private:
        struct Impl;
        std::unique_ptr<Impl> impl;
    };

} }
