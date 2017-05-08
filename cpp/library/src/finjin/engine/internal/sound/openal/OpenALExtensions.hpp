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

    /** All EFX-related functions */
    class OpenALEFXExtensions
    {
    public:
        OpenALEFXExtensions();

        bool Initialize(ALCdevice* device);

        //EFX extension function pointer variables

        //Effect objects
        LPALGENEFFECTS alGenEffects;
        LPALDELETEEFFECTS alDeleteEffects;
        LPALISEFFECT alIsEffect;
        LPALEFFECTI alEffecti;
        LPALEFFECTIV alEffectiv;
        LPALEFFECTF alEffectf;
        LPALEFFECTFV alEffectfv;
        LPALGETEFFECTI alGetEffecti;
        LPALGETEFFECTIV alGetEffectiv;
        LPALGETEFFECTF alGetEffectf;
        LPALGETEFFECTFV alGetEffectfv;

        //Filter objects
        LPALGENFILTERS alGenFilters;
        LPALDELETEFILTERS alDeleteFilters;
        LPALISFILTER alIsFilter;
        LPALFILTERI alFilteri;
        LPALFILTERIV alFilteriv;
        LPALFILTERF alFilterf;
        LPALFILTERFV alFilterfv;
        LPALGETFILTERI alGetFilteri;
        LPALGETFILTERIV alGetFilteriv;
        LPALGETFILTERF alGetFilterf;
        LPALGETFILTERFV alGetFilterfv;

        //Auxiliary slot object
        LPALGENAUXILIARYEFFECTSLOTS alGenAuxiliaryEffectSlots;
        LPALDELETEAUXILIARYEFFECTSLOTS alDeleteAuxiliaryEffectSlots;
        LPALISAUXILIARYEFFECTSLOT alIsAuxiliaryEffectSlot;
        LPALAUXILIARYEFFECTSLOTI alAuxiliaryEffectSloti;
        LPALAUXILIARYEFFECTSLOTIV alAuxiliaryEffectSlotiv;
        LPALAUXILIARYEFFECTSLOTF alAuxiliaryEffectSlotf;
        LPALAUXILIARYEFFECTSLOTFV alAuxiliaryEffectSlotfv;
        LPALGETAUXILIARYEFFECTSLOTI alGetAuxiliaryEffectSloti;
        LPALGETAUXILIARYEFFECTSLOTIV alGetAuxiliaryEffectSlotiv;
        LPALGETAUXILIARYEFFECTSLOTF alGetAuxiliaryEffectSlotf;
        LPALGETAUXILIARYEFFECTSLOTFV alGetAuxiliaryEffectSlotfv;
    };

} }
