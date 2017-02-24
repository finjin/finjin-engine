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
#include "finjin/engine/SoundContextCommonSettings.hpp"
#include "finjin/common/AllocatedClass.hpp"
#include "finjin/common/Chrono.hpp"
#include "finjin/common/Error.hpp"
#include "finjin/common/OperationStatus.hpp"
#include "finjin/common/Path.hpp"
#include "finjin/engine/SoundFormat.hpp"
#include "finjin/engine/SoundSorter.hpp"
#include "finjin/engine/AssetClass.hpp"
#include "finjin/engine/AssetCreationCapability.hpp"


//Classes----------------------------------------------------------------------
namespace Finjin { namespace Engine {

    class XAudio2System;
    class XAudio2SoundGroup;
    class XAudio2SoundSource;
    class XAudio2SoundBuffer;
    class XAudio2Listener;
    class XAudio2ContextImpl;
        
    using namespace Finjin::Common;

    class XAudio2Context : public AllocatedClass
    {
    public:
        XAudio2Context(Allocator* allocator, XAudio2System* soundSystem);
        ~XAudio2Context();

        struct Settings : SoundContextCommonSettings
        {
            Settings(Allocator* allocator) : SoundContextCommonSettings(allocator)
            {
            }
        };

        void Create(const Settings& settings, Error& error);
        void Destroy();

        const OperationStatus& GetInitializationStatus() const;

        void GetSelectorComponents(AssetPathSelector& result);

        const Settings& GetSettings() const;

        template <typename T>
        size_t GetExternalAssetFileExtensions(T& extensions, AssetClass assetClass, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            switch (assetClass)
            {
                case AssetClass::SOUND:
                {
                    if (extensions.push_back("wav").HasErrorOrValue(false))
                    {
                        FINJIN_SET_ERROR(error, "Failed to add 'wav' extension.");
                        return 0;
                    }
                    return 1;
                }
            }

            return 0;
        }

        AssetCreationCapability GetAssetCreationCapabilities(AssetClass assetClass) const;

        void Update(SimpleTimeDelta elapsedTime);

        void Execute(SoundEvents& events, SoundCommands& commands, Error& error);

        void HandleApplicationViewportLostFocus();
        void HandleApplicationViewportGainedFocus();

        XAudio2Listener* GetListener();

        XAudio2SoundGroup* GetMasterGroup();

        XAudio2SoundBuffer* CreateBufferFromMainThread(const ByteBuffer& fileBytes, Error& error);
        void DestroyBuffer(XAudio2SoundBuffer* buffer);

        XAudio2SoundSource* CreateSource(XAudio2SoundBuffer* soundBuffer, Error& error);
        XAudio2SoundSource* CreateSource(const SoundFormat& soundFormat, Error& error);
        void DestroySource(XAudio2SoundSource* source);

        SoundSorter<XAudio2SoundSource>* GetSoundSorter();
        void SetSoundSorter(SoundSorter<XAudio2SoundSource>* soundSorter);

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

        //XAudio-specific methods---------------
        XAudio2ContextImpl* GetImpl();        
        
    private:
        std::unique_ptr<XAudio2ContextImpl> impl;
    };

} }
