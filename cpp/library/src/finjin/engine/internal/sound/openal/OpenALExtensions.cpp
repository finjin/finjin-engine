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
#include "OpenALExtensions.hpp"

using namespace Finjin::Engine;


//Implementation----------------------------------------------------------------

//OpenALEFXExtensions
OpenALEFXExtensions::OpenALEFXExtensions()
{
    //Effect objects
    this->alGenEffects = nullptr;
    this->alDeleteEffects = nullptr;
    this->alIsEffect = nullptr;
    this->alEffecti = nullptr;
    this->alEffectiv = nullptr;
    this->alEffectf = nullptr;
    this->alEffectfv = nullptr;
    this->alGetEffecti = nullptr;
    this->alGetEffectiv = nullptr;
    this->alGetEffectf = nullptr;
    this->alGetEffectfv = nullptr;

    //Filter objects
    this->alGenFilters = nullptr;
    this->alDeleteFilters = nullptr;
    this->alIsFilter = nullptr;
    this->alFilteri = nullptr;
    this->alFilteriv = nullptr;
    this->alFilterf = nullptr;
    this->alFilterfv = nullptr;
    this->alGetFilteri = nullptr;
    this->alGetFilteriv = nullptr;
    this->alGetFilterf = nullptr;
    this->alGetFilterfv = nullptr;

    //Auxiliary slot object
    this->alGenAuxiliaryEffectSlots = nullptr;
    this->alDeleteAuxiliaryEffectSlots = nullptr;
    this->alIsAuxiliaryEffectSlot = nullptr;
    this->alAuxiliaryEffectSloti = nullptr;
    this->alAuxiliaryEffectSlotiv = nullptr;
    this->alAuxiliaryEffectSlotf = nullptr;
    this->alAuxiliaryEffectSlotfv = nullptr;
    this->alGetAuxiliaryEffectSloti = nullptr;
    this->alGetAuxiliaryEffectSlotiv = nullptr;
    this->alGetAuxiliaryEffectSlotf = nullptr;
    this->alGetAuxiliaryEffectSlotfv = nullptr;
}

bool OpenALEFXExtensions::Initialize(ALCdevice* device)
{
    if (alcIsExtensionPresent(device, ALC_EXT_EFX_NAME))
    {
        //Get function pointers
        this->alGenEffects = (LPALGENEFFECTS)alGetProcAddress("alGenEffects");
        this->alDeleteEffects = (LPALDELETEEFFECTS )alGetProcAddress("alDeleteEffects");
        this->alIsEffect = (LPALISEFFECT )alGetProcAddress("alIsEffect");
        this->alEffecti = (LPALEFFECTI)alGetProcAddress("alEffecti");
        this->alEffectiv = (LPALEFFECTIV)alGetProcAddress("alEffectiv");
        this->alEffectf = (LPALEFFECTF)alGetProcAddress("alEffectf");
        this->alEffectfv = (LPALEFFECTFV)alGetProcAddress("alEffectfv");
        this->alGetEffecti = (LPALGETEFFECTI)alGetProcAddress("alGetEffecti");
        this->alGetEffectiv = (LPALGETEFFECTIV)alGetProcAddress("alGetEffectiv");
        this->alGetEffectf = (LPALGETEFFECTF)alGetProcAddress("alGetEffectf");
        this->alGetEffectfv = (LPALGETEFFECTFV)alGetProcAddress("alGetEffectfv");
        this->alGenFilters = (LPALGENFILTERS)alGetProcAddress("alGenFilters");
        this->alDeleteFilters = (LPALDELETEFILTERS)alGetProcAddress("alDeleteFilters");
        this->alIsFilter = (LPALISFILTER)alGetProcAddress("alIsFilter");
        this->alFilteri = (LPALFILTERI)alGetProcAddress("alFilteri");
        this->alFilteriv = (LPALFILTERIV)alGetProcAddress("alFilteriv");
        this->alFilterf = (LPALFILTERF)alGetProcAddress("alFilterf");
        this->alFilterfv = (LPALFILTERFV)alGetProcAddress("alFilterfv");
        this->alGetFilteri = (LPALGETFILTERI )alGetProcAddress("alGetFilteri");
        this->alGetFilteriv = (LPALGETFILTERIV )alGetProcAddress("alGetFilteriv");
        this->alGetFilterf = (LPALGETFILTERF )alGetProcAddress("alGetFilterf");
        this->alGetFilterfv = (LPALGETFILTERFV )alGetProcAddress("alGetFilterfv");
        this->alGenAuxiliaryEffectSlots = (LPALGENAUXILIARYEFFECTSLOTS)alGetProcAddress("alGenAuxiliaryEffectSlots");
        this->alDeleteAuxiliaryEffectSlots = (LPALDELETEAUXILIARYEFFECTSLOTS)alGetProcAddress("alDeleteAuxiliaryEffectSlots");
        this->alIsAuxiliaryEffectSlot = (LPALISAUXILIARYEFFECTSLOT)alGetProcAddress("alIsAuxiliaryEffectSlot");
        this->alAuxiliaryEffectSloti = (LPALAUXILIARYEFFECTSLOTI)alGetProcAddress("alAuxiliaryEffectSloti");
        this->alAuxiliaryEffectSlotiv = (LPALAUXILIARYEFFECTSLOTIV)alGetProcAddress("alAuxiliaryEffectSlotiv");
        this->alAuxiliaryEffectSlotf = (LPALAUXILIARYEFFECTSLOTF)alGetProcAddress("alAuxiliaryEffectSlotf");
        this->alAuxiliaryEffectSlotfv = (LPALAUXILIARYEFFECTSLOTFV)alGetProcAddress("alAuxiliaryEffectSlotfv");
        this->alGetAuxiliaryEffectSloti = (LPALGETAUXILIARYEFFECTSLOTI)alGetProcAddress("alGetAuxiliaryEffectSloti");
        this->alGetAuxiliaryEffectSlotiv = (LPALGETAUXILIARYEFFECTSLOTIV)alGetProcAddress("alGetAuxiliaryEffectSlotiv");
        this->alGetAuxiliaryEffectSlotf = (LPALGETAUXILIARYEFFECTSLOTF)alGetProcAddress("alGetAuxiliaryEffectSlotf");
        this->alGetAuxiliaryEffectSlotfv = (LPALGETAUXILIARYEFFECTSLOTFV)alGetProcAddress("alGetAuxiliaryEffectSlotfv");
    }

    return
        this->alGenEffects && this->alDeleteEffects && this->alIsEffect && this->alEffecti && this->alEffectiv && this->alEffectf &&
        this->alEffectfv && this->alGetEffecti && this->alGetEffectiv && this->alGetEffectf && this->alGetEffectfv && this->alGenFilters &&
        this->alDeleteFilters && this->alIsFilter && this->alFilteri && this->alFilteriv && this->alFilterf && this->alFilterfv &&
        this->alGetFilteri && this->alGetFilteriv && this->alGetFilterf && this->alGetFilterfv && this->alGenAuxiliaryEffectSlots &&
        this->alDeleteAuxiliaryEffectSlots && this->alIsAuxiliaryEffectSlot && this->alAuxiliaryEffectSloti &&
        this->alAuxiliaryEffectSlotiv && this->alAuxiliaryEffectSlotf && this->alAuxiliaryEffectSlotfv &&
        this->alGetAuxiliaryEffectSloti && this->alGetAuxiliaryEffectSlotiv && this->alGetAuxiliaryEffectSlotf &&
        this->alGetAuxiliaryEffectSlotfv;
}
