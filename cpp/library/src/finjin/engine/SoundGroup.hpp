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
#include "finjin/common/EnumBitwise.hpp"
#include "finjin/common/IntrusiveList.hpp"
#include "finjin/common/Math.hpp"
#include "finjin/engine/SoundContextCommonSettings.hpp"


//Classes----------------------------------------------------------------------
namespace Finjin { namespace Engine {
    
    using namespace Finjin::Common;

    enum class SoundGroupSetting
    {        
        VOLUME = 1 << 0,
        ALL = VOLUME
    };
    FINJIN_ENUM_BITWISE_OPERATIONS(SoundGroupSetting)
    
    /** 
     * A sound group provides an easy way of controlling many sounds at once
     * It is highly recommended that SetEnabledSettings() be called to define 
     * which settings the sound group handles
     */
    template <typename SoundSource, typename SoundContext>
    class SoundGroupTemplate
    {   
    public:
        SoundGroupTemplate()
        {
            this->context = nullptr;
            this->sourcesHead = nullptr;
            this->enabledSettings = SoundGroupSetting::ALL;
            this->volume = 1.0f;
        }
        
        ~SoundGroupTemplate()
        {            
        }
        
        void Create(SoundContext* context)
        {
            this->context = context;    
        }
        
        /** Moves all the sound sources in this group to the specified group */
        void MoveSourcesTo(SoundGroupTemplate* soundGroup)
        {
            if (soundGroup == this)
                return;

            auto sources = this->sourcesHead;    
            this->sourcesHead = nullptr; //Effectively clears list of sources
            FINJIN_INTRUSIVE_SINGLE_LIST_ITERATE(sources, soundGroupNext, source)
                source->SetGroup(soundGroup);
        }

        SoundGroupSetting GetEnabledSettings() const
        {
            return this->enabledSettings;
        }

        /** 
         * Sets the setting flags that control which sound settings are updated
         * This should be called before any sounds have been added to the group
         * @param settings [in] - The new enabled settings
         */
        void SetEnabledSettings(SoundGroupSetting settings)
        {
            this->enabledSettings = settings;
        }

        float GetVolume() const
        {
            return this->volume;
        }

        void SetVolume(float volume)
        {
            Limit(volume, 0.0f, 1.0f);
            this->volume = volume;

            if (AnySet(this->enabledSettings & SoundGroupSetting::VOLUME))
            {
                FINJIN_INTRUSIVE_SINGLE_LIST_ITERATE(this->sourcesHead, soundGroupNext, source)
                    source->SetVolume(this->volume);
            }
        }

        void Play()
        {
            FINJIN_INTRUSIVE_SINGLE_LIST_ITERATE(this->sourcesHead, soundGroupNext, source)
                source->Play();
        }

        void PlayPaused()
        {
            FINJIN_INTRUSIVE_SINGLE_LIST_ITERATE(this->sourcesHead, soundGroupNext, source)
            {
                if (AnySet(source->GetState() & SoundSourceState::PAUSED))
                    source->Play();
            }
        }

        void Pause()
        {
            FINJIN_INTRUSIVE_SINGLE_LIST_ITERATE(this->sourcesHead, soundGroupNext, source)
                source->Pause();
        }

        void Stop()
        {
            FINJIN_INTRUSIVE_SINGLE_LIST_ITERATE(this->sourcesHead, soundGroupNext, source)
                source->Stop();
        }

        void AddSource(SoundSource* source)
        {
            FINJIN_INTRUSIVE_SINGLE_LIST_ADD_ITEM(this->sourcesHead, soundGroupNext, source)
            
            if (AnySet(this->enabledSettings & SoundGroupSetting::VOLUME))
                source->SetVolume(this->volume);                
        }
        
        void RemoveSource(SoundSource* source)
        {
            FINJIN_INTRUSIVE_SINGLE_LIST_REMOVE_ITEM(this->sourcesHead, soundGroupNext, source)    
        }

    protected:
        SoundContext* context;
        SoundSource* sourcesHead; //Linked list of sources that are in this sound group. Use sourcesHead->soundGroupNext to iterate through list
        
        SoundGroupSetting enabledSettings;
        float volume;
    };
    
} }
