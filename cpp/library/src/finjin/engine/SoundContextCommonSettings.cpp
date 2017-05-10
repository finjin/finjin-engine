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
#include "finjin/engine/SoundContextCommonSettings.hpp"
#include "finjin/common/ConfigDocumentReader.hpp"
#include "finjin/common/Convert.hpp"

using namespace Finjin::Engine;


//Implementation----------------------------------------------------------------
SoundContextCommonSettings::SoundContextCommonSettings(Allocator* allocator) : initialAssetFileSelector(allocator), settingsFileNames(allocator)
{
    this->applicationHandle = nullptr;
    this->osWindow = nullptr;
    this->assetFileReader = nullptr;
    this->processorID = 0;

    this->tempReadBufferSize = EngineConstants::DEFAULT_SOUND_CONTEXT_READ_BUFFER_SIZE;

    this->maxCommandsPerUpdate = EngineConstants::DEFAULT_SUBSYSTEM_COMMANDS_PER_UPDATE;

    this->playbackSampleRate = 0; //Indicates default should be used
    this->speakerSetup = SoundSpeakerSetup::DEFAULT;
    this->sound3DQuality = Sound3DQuality::MEDIUM;
    this->speedOfSound.SetMeters(340); //340 meters/second

    this->maxSoundBufferCount = 100;
}

void SoundContextCommonSettings::SetDefaultSoundSourcePools()
{
    //A variety of sound formats suitable for early development
    //Normally a smaller number of formats will be chosen
    const int soundFormatCount = 7;
    static SoundFormat formats[soundFormatCount] =
    {
        SoundFormat(1, 1, 11025), //Mono, 8-bit, 11025hz
        SoundFormat(1, 2, 11025), //Mono, 16-bit, 11025hz
        SoundFormat(1, 1, 22050), //Mono, 8-bit, 22050hz
        SoundFormat(1, 2, 22050), //Mono, 16-bit, 22050hz
        SoundFormat(1, 1, 44100), //Mono, 8-bit, 44100hz
        SoundFormat(1, 2, 44100), //Mono, 16-bit, 44100hz
        SoundFormat(2, 2, 44100) //Stereo, 16-bit, 44100hz
    };
    static ResourcePoolDescription descriptors[soundFormatCount] =
    {
        ResourcePoolDescription(0, 1), //Mono, 8-bit, 11025hz
        ResourcePoolDescription(1, 1), //Mono, 16-bit, 11025hz
        ResourcePoolDescription(2, 1), //Mono, 8-bit, 22050hz
        ResourcePoolDescription(3, 1), //Mono, 16-bit, 22050hz
        ResourcePoolDescription(4, 1), //Mono, 8-bit, 44100hz
        ResourcePoolDescription(5, 1), //Mono, 16-bit, 44100hz
        ResourcePoolDescription(6, 1) //Stereo, 16-bit, 44100hz
    };

    this->soundFormats.assign(formats, soundFormatCount);

    this->soundSourcePools.clear();
    for (size_t i = 0; i < soundFormatCount * 100; i++)
        this->soundSourcePools.push_back(descriptors[i % soundFormatCount]);
}

size_t SoundContextCommonSettings::GetTotalSoundSourceCount() const
{
    size_t total = 0;

    for (size_t i = 0; i < this->soundSourcePools.size(); i++)
        total += this->soundSourcePools[i].count;

    return total;
}

size_t SoundContextCommonSettings::GetTotalSoundSourceCount(const SoundFormat& soundFormat) const
{
    size_t total = 0;

    for (size_t i = 0; i < this->soundSourcePools.size(); i++)
    {
        if (soundFormat == this->soundFormats[this->soundSourcePools[i].format])
            total += this->soundSourcePools[i].count;
    }

    return total;
}

void SoundContextCommonSettings::ReadSettings(AssetClassFileReader& settingsAssetReader, const AssetReference& settingsFileAssetRef, ByteBuffer& readBuffer, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto result = settingsAssetReader.ReadAsset(readBuffer, settingsFileAssetRef);
    if (result == FileOperationResult::SUCCESS)
    {
        ParseSettings(readBuffer, settingsFileAssetRef.filePath, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to process sound settings file.");
            return;
        }
    }
    else if (result != FileOperationResult::NOT_FOUND)
    {
        FINJIN_SET_ERROR(error, "Failed to read sound settings file.");
        return;
    }

    //Add sound descriptors if none were configured in the boot file
    if (this->soundSourcePools.empty())
        SetDefaultSoundSourcePools();
}

void SoundContextCommonSettings::ParseSettings(const ByteBufferReader& configFileBuffer, const Path& assetFilePath, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    Utf8StringView section, key, value;

    ConfigDocumentReader reader;
    reader.Start(configFileBuffer);

    auto soundFormatCount = reader.GetSectionCount("sound-format");
    auto soundPoolCount = reader.GetSectionCount("sound-pool");

    this->soundFormats.resize(soundFormatCount + soundPoolCount); //Add soundPoolCount since the sound pool defintions can contain a format
    this->soundSourcePools.resize(soundPoolCount);

    soundFormatCount = 0;
    soundPoolCount = 0;

    for (auto line = reader.Start(configFileBuffer); line != nullptr; line = reader.Next())
    {
        switch (line->GetType())
        {
            case ConfigDocumentLine::Type::SECTION:
            {
                line->GetSectionName(section);

                if (section.empty() || section == "sound-format" || section == "sound-pool")
                {
                }
                else
                {
                    reader.SkipScope();
                }

                break;
            }
            case ConfigDocumentLine::Type::KEY_AND_VALUE:
            {
                line->GetKeyAndValue(key, value);

                if (section.empty())
                {
                    HandleTopLevelSetting(key, value, error);
                    if (error)
                    {
                        FINJIN_SET_ERROR_NO_MESSAGE(error);
                        return;
                    }
                }
                else if (section == "sound-format")
                {
                    if (soundFormatCount == this->soundFormats.max_size())
                    {
                        FINJIN_SET_ERROR(error, "The maximum number of sound formats was reached.");
                        return;
                    }

                    auto& soundFormat = this->soundFormats[soundFormatCount];

                    if (key == "type")
                    {
                        soundFormat = SoundFormat::FromTypeName(value);
                        if (!soundFormat.IsValid())
                        {
                            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to parse sound format '%1%'.", value));
                            return;
                        }

                        soundFormat.typeName = value;
                    }
                    else if (key == "name")
                        soundFormat.typeName = value;
                    else if (key == "channels")
                        soundFormat.channelCount = Convert::ToInteger(value, soundFormat.channelCount);
                    else if (key == "bytes-per-channel")
                        soundFormat.bytesPerChannel = Convert::ToInteger(value, soundFormat.bytesPerChannel);
                    else if (key == "sample-rate")
                        soundFormat.samplesPerSecond = Convert::ToInteger(value, soundFormat.samplesPerSecond);
                    else if (key == "sample-type")
                    {
                        if (value == "int")
                            soundFormat.sampleType = SoundFormat::SampleType::INTEGER;
                        else if (value == "float")
                            soundFormat.sampleType = SoundFormat::SampleType::FLOATING_POINT;
                        else
                        {
                            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to parse sound format sample type '%1%'.", value));
                            return;
                        }
                    }

                    if (!soundFormat.typeName.empty() && soundFormat.IsValid())
                    {
                        auto foundByName = SoundFormat::FindByTypeName(this->soundFormats, soundFormat.typeName);
                        if (foundByName != nullptr)
                        {
                            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("The sound format '%1%' was defined more than once.", value));
                            return;
                        }

                        soundFormatCount++;
                    }
                }
                else if (section == "sound-pool")
                {
                    if (soundPoolCount == this->soundSourcePools.max_size())
                    {
                        FINJIN_SET_ERROR(error, "The maximum number of sound pools was reached.");
                        return;
                    }

                    auto& pool = this->soundSourcePools[soundPoolCount];

                    if (key == "type")
                    {
                        auto foundFormat = SoundFormat::FindByTypeName(this->soundFormats, value);
                        if (foundFormat != nullptr)
                        {
                            //Existing format was found
                            pool.format = foundFormat - this->soundFormats.data();
                        }
                        else
                        {
                            //Existing format was not found

                            //See if the format can be parsed
                            auto soundFormat = SoundFormat::FromTypeName(value);
                            if (!soundFormat.IsValid())
                            {
                                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to parse sound format '%1%' or find an existing format with that name.", value));
                                return;
                            }

                            soundFormat.typeName = value;

                            //Figure out the format index
                            auto foundByFormat = SoundFormat::FindByFormat(this->soundFormats, soundFormat);
                            if (foundByFormat != nullptr)
                            {
                                //Format exists - ignore the fact that the name is different
                                pool.format = foundByFormat - this->soundFormats.data();
                            }
                            else
                            {
                                //Add the format
                                if (soundFormatCount == this->soundFormats.max_size())
                                {
                                    FINJIN_SET_ERROR(error, "The maximum number of sound formats was reached.");
                                    return;
                                }

                                pool.format = soundFormatCount;

                                this->soundFormats[soundFormatCount++] = soundFormat;
                            }
                        }
                    }
                    else if (key == "count")
                        pool.count = Convert::ToInteger(value, pool.count);

                    if (pool.IsValid())
                        soundPoolCount++;
                }

                break;
            }
            default: break;
        }
    }

    //Trim the size of the sound formats and clear the sound format names
    this->soundFormats.resize(soundFormatCount);
    for (auto& format : this->soundFormats)
        format.typeName.clear();
}

void SoundContextCommonSettings::HandleTopLevelSetting(const Utf8StringView& key, const Utf8StringView& value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    if (key == "processor-id")
        this->processorID = Convert::ToInteger(value, this->processorID);
    else if (key == "temp-read-buffer-size")
        this->tempReadBufferSize = Convert::ToInteger(value, this->tempReadBufferSize);
    else if (key == "max-commands-per-update")
        this->maxCommandsPerUpdate = Convert::ToInteger(value, this->maxCommandsPerUpdate);
}
