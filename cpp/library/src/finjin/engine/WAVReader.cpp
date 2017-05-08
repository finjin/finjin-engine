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
#include "WAVReader.hpp"
#include "finjin/common/ByteOrder.hpp"

using namespace Finjin::Engine;


//Local functions---------------------------------------------------------------
typedef WAVSoundData Codec(const void* data, size_t length, int numChannels, int bitsPerSample, int sampleFrequency, ByteBuffer& soundOutputBuffer, Error& error);

static WAVSoundData _soundCodecLinear(const void* data, size_t length, int numChannels, int bitsPerSample, int sampleFrequency, ByteBuffer& soundOutputBuffer, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    if (soundOutputBuffer.resize(length) < length)
        return WAVSoundData();

    auto dest = soundOutputBuffer.data();
    FINJIN_COPY_MEMORY(dest, data, length);
    return WAVSoundData(dest, length, numChannels, bitsPerSample / 8, sampleFrequency);
}

static WAVSoundData _soundCodecPCM8Signed(const void* data, size_t length, int numChannels, int bitsPerSample, int sampleFrequency, ByteBuffer& soundOutputBuffer, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto src = static_cast<const uint8_t*>(data);

    if (soundOutputBuffer.resize(length) < length)
        return WAVSoundData();

    auto dest = soundOutputBuffer.data();
    for (size_t i = 0; i < length; i++)
        dest[i] = src[i] + (int8_t)128;
    return WAVSoundData(dest, length, numChannels, bitsPerSample / 8, sampleFrequency);
}

static WAVSoundData _soundCodecPCM16(const void* data, size_t length, int numChannels, int bitsPerSample, int sampleFrequency, ByteBuffer& soundOutputBuffer, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto src = static_cast<const int16_t*>(data);

    if (soundOutputBuffer.resize(length) < length)
        return WAVSoundData();

    auto dest = reinterpret_cast<int16_t*>(soundOutputBuffer.data());
    for (size_t i = 0; i < length / 2; i++)
        dest[i] = ((src[i] << 8) & 0xFF00) | ((src[i] >> 8) & 0x00FF); //Byte swap
    return WAVSoundData(dest, length, numChannels, bitsPerSample / 8, sampleFrequency);
}

static int16_t mulaw2linear(uint8_t mulawbyte)
{
    //From: http://www.multimedia.cx/simpleaudio.html#tth_sEc6.1

    static const int16_t exp_lut[8] = {0, 132, 396, 924, 1980, 4092, 8316, 16764};

    mulawbyte = ~mulawbyte;
    int16_t sign = (mulawbyte & 0x80); //Bit 7
    int16_t exponent = (mulawbyte >> 4) & 0x07; //Bits 4-6
    int16_t mantissa = mulawbyte & 0x0F; //Bits 0-3
    int16_t sample = exp_lut[exponent] + (mantissa << (exponent + 3));
    if (sign != 0)
        sample = -sample;
    return sample;
}

static WAVSoundData _soundCodecULaw(const void* data, size_t length, int numChannels, int bitsPerSample, int sampleFrequency, ByteBuffer& soundOutputBuffer, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto desiredByteCount = length * 2;
    if (soundOutputBuffer.resize(desiredByteCount) < desiredByteCount)
        return WAVSoundData();

    auto src = static_cast<const uint8_t*>(data);
    auto dest = reinterpret_cast<int16_t*>(soundOutputBuffer.data());
    for (size_t i = 0; i < length; i++)
        dest[i] = mulaw2linear(src[i]);
    return WAVSoundData(dest, desiredByteCount, numChannels, bitsPerSample / 8, sampleFrequency);
}

//http://www.multimedia.cx/simpleaudio.html#tth_sEc6.1
#define SIGN_BIT 0x80 //Sign bit for a A-law byte.
#define QUANT_MASK 0xf //Quantization field mask.
#define SEG_SHIFT 4 //Left shift for segment number.
#define SEG_MASK 0x70 //Segment field mask.
static int16_t alaw2linear(uint8_t a_val)
{
    int16_t t, seg;
    a_val ^= 0x55;
    t = (a_val & QUANT_MASK) << 4;
    seg = ((int16_t) a_val & SEG_MASK) >> SEG_SHIFT;
    switch (seg)
    {
        case 0: t += 8; break;
        case 1: t += 0x108; break;
        default: t += 0x108; t <<= seg - 1; break;
    }
    return (a_val & SIGN_BIT) ? t : -t;
}

static WAVSoundData _soundCodecALaw(const void* data, size_t length, int numChannels, int bitsPerSample, int sampleFrequency, ByteBuffer& soundOutputBuffer, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto desiredByteCount = length * 2;
    if (soundOutputBuffer.resize(desiredByteCount) < desiredByteCount)
        return WAVSoundData();

    auto src = static_cast<const uint8_t*>(data);
    auto dest = reinterpret_cast<int16_t*>(soundOutputBuffer.data());
    for (size_t i = 0; i < length; i++)
        dest[i] = alaw2linear(src[i]);
    return WAVSoundData(dest, desiredByteCount, numChannels, bitsPerSample / 8, sampleFrequency);
}

static WAVSoundData LoadWavFile(ByteBufferReader& reader, ByteBuffer& soundOutputBuffer, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WAVSoundData result;

    auto foundHeader = false;
    UInt32LittleEndian chunkLength;
    Int32BigEndian signature;
    UInt16LittleEndian audioFormat = 0;
    UInt16LittleEndian numChannels = 0;
    UInt32LittleEndian sampleFrequency = 0;
    UInt32LittleEndian byteRate = 0;
    UInt16LittleEndian blockAlign = 0;
    UInt16LittleEndian bitsPerSample = 0;
    Codec* codec = _soundCodecLinear;

    if (!reader.ReadLittleEndian32(chunkLength) || !reader.ReadBigEndian32(signature))
        return result;

    if (signature != FINJIN_FOURCC_BIG_ENDIAN('W', 'A', 'V', 'E')) //0x57415645
    {
        //"WAVE"
        FINJIN_SET_ERROR(error, "Unsupported sound file subtype.");
        return result;
    }

    while (true)
    {
        if (!reader.ReadBigEndian32(signature) || !reader.ReadLittleEndian32(chunkLength))
            return result;

        if (signature == FINJIN_FOURCC_BIG_ENDIAN('f', 'm', 't', ' ')) //0x666d7420
        {
            //"fmt "
            foundHeader = true;

            if (chunkLength < 16)
            {
                FINJIN_SET_ERROR(error, "The sound data was corrupt or truncated.");
                return result;
            }

            if (!reader.ReadLittleEndian16(audioFormat) ||
              !reader.ReadLittleEndian16(numChannels) ||
              !reader.ReadLittleEndian32(sampleFrequency) ||
              !reader.ReadLittleEndian32(byteRate) ||
              !reader.ReadLittleEndian16(blockAlign) ||
              !reader.ReadLittleEndian16(bitsPerSample))
            {
                FINJIN_SET_ERROR(error, "Failed to read sound header. The sound data was corrupt or truncated.");
                return result;
            }

            if (!reader.Skip(chunkLength - 16))
            {
                FINJIN_SET_ERROR(error, "Failed to skip past the remaining portion of the sound header. The sound data was corrupt or truncated.");
                return result;
            }

            switch (audioFormat)
            {
                case 1:
                {
                    //PCM
                    codec = (bitsPerSample == 8 || IsLittleEndian()) ? _soundCodecLinear : _soundCodecPCM16;
                    break;
                }
                case 7:
                {
                    //uLaw
                    bitsPerSample *= 2; //TODO: ???
                    codec = _soundCodecULaw;
                    break;
                }
                default:
                {
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Unsupported sound file audio format '%1%'.", audioFormat));
                    return result;
                }
            }
        }
        else if (signature == FINJIN_FOURCC_BIG_ENDIAN('d', 'a', 't', 'a')) //0x64617461
        {
            //"data"
            if (!foundHeader)
            {
                FINJIN_SET_ERROR(error, "Reached data chunk without encountering header.");
                return result;
            }

            auto ptr = reader.GetOffsetBytes(chunkLength);
            if (ptr == nullptr)
            {
                FINJIN_SET_ERROR(error, "Not enough data to decode.");
                return result;
            }

            result = codec(ptr, chunkLength, numChannels, bitsPerSample, sampleFrequency, soundOutputBuffer, error);
            if (error)
                FINJIN_SET_ERROR(error, "Failed to decode sound data.");

            return result;
        }
        else
        {
            if (!reader.Skip(chunkLength))
            {
                FINJIN_SET_ERROR(error, "Failed to skip chunk. The sound data was corrupt or truncated.");
                return result;
            }
        }

        if ((chunkLength & 1) && !reader.Skip(1))
        {
            FINJIN_SET_ERROR(error, "Failed to skip odd chunk byte. The sound data was corrupt or truncated.");
            return result;
        }
    }

    return result;
}

static WAVSoundData LoadAUFile(ByteBufferReader& reader, ByteBuffer& soundOutputBuffer, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WAVSoundData result;

    const int AU_HEADER_SIZE = 24;

    //see: http://en.wikipedia.org/wiki/Au_file_format, G.72x are missing
    enum AUEncoding
    {
        AU_ULAW_8 = 1, //8-bit ISDN u-law
        AU_PCM_8 = 2, //8-bit linear PCM (signed)
        AU_PCM_16 = 3, //16-bit linear PCM (signed, big-endian)
        AU_PCM_24 = 4, //24-bit linear PCM
        AU_PCM_32 = 5, //32-bit linear PCM
        AU_FLOAT_32 = 6, //32-bit IEEE floating point
        AU_FLOAT_64 = 7, //64-bit IEEE floating point
        AU_ALAW_8 = 27 //8-bit ISDN a-law
    };

    Int32BigEndian dataOffset; //Byte offset to data part, minimum 24
    Int32BigEndian len; //Number of bytes in the data part, -1 = not known
    Int32BigEndian encoding; //Encoding of the data part, see AUEncoding
    Int32BigEndian sampleFrequency; //Number of samples per second
    Int32BigEndian numChannels; //Number of interleaved channels
    size_t length;
    Codec* codec;
    int bitsPerSample;

    if (!reader.ReadBigEndian32(dataOffset) ||
      !reader.ReadBigEndian32(len) ||
      !reader.ReadBigEndian32(encoding) ||
      !reader.ReadBigEndian32(sampleFrequency) ||
      !reader.ReadBigEndian32(numChannels))
    {
        return result;
    }

    length = (len == -1) ? (reader.size_left() - AU_HEADER_SIZE - dataOffset) : (size_t) len;

    if (!(dataOffset >= AU_HEADER_SIZE && length > 0 && sampleFrequency >= 1 && numChannels >= 1))
    {
        FINJIN_SET_ERROR(error, "Failed to read sound header. The sound data was corrupt or truncated.");
        return result;
    }

    if (!reader.Skip(dataOffset - AU_HEADER_SIZE))
    {
        FINJIN_SET_ERROR(error, "Failed to skip past the remaining portion of the sound header. The sound data was corrupt or truncated.");
        return result;
    }

    switch (encoding)
    {
        case AU_ULAW_8:
        {
            bitsPerSample = 16;
            codec = _soundCodecULaw;
            break;
        }
        case AU_PCM_8:
        {
            bitsPerSample = 8;
            codec = _soundCodecPCM8Signed;
            break;
        }
        case AU_PCM_16:
        {
            bitsPerSample = 16;
            codec = IsLittleEndian() ? _soundCodecPCM16 : _soundCodecLinear;
            break;
        }
        case AU_ALAW_8:
        {
            bitsPerSample = 16;
            codec = _soundCodecALaw;
            break;
        }
        default:
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Unsupported sound file subtype '%1%'.", encoding));
            return result;
        }
    }

    auto ptr = reader.GetOffsetBytes(length);
    if (ptr == nullptr)
    {
        FINJIN_SET_ERROR(error, "Not enough data to decode.");
        return result;
    }

    result = codec(ptr, length, numChannels, bitsPerSample, sampleFrequency, soundOutputBuffer, error);
    if (error)
        FINJIN_SET_ERROR(error, "Failed to decode sound data.");

    return result;
}

static WAVSoundData ReadWav(ByteBufferReader& reader, ByteBuffer& soundOutputBuffer, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    soundOutputBuffer.clear();

    WAVSoundData wavSoundData;

    //Read the four character code
    Int32BigEndian containerSignature;
    if (reader.ReadBigEndian32(containerSignature))
    {
        switch (containerSignature)
        {
            case FINJIN_FOURCC_BIG_ENDIAN('R', 'I', 'F', 'F'): //0x52494646
            {
                //Microsoft 'wav' format
                wavSoundData = LoadWavFile(reader, soundOutputBuffer, error);
                if (error)
                {
                    FINJIN_SET_ERROR(error, "An error was encountered with the RIFF format WAV data.");
                    return wavSoundData;
                }
                break;
            }
            case FINJIN_FOURCC_BIG_ENDIAN('.', 's', 'n', 'd'): //0x2E736E64
            {
                //Sun & Next's 'au' format
                wavSoundData = LoadAUFile(reader, soundOutputBuffer, error);
                if (error)
                {
                    FINJIN_SET_ERROR(error, "An error was encountered with the SND/AU format WAV data.");
                    return wavSoundData;
                }
                break;
            }
        }
    }

    if (wavSoundData.data == nullptr)
        FINJIN_SET_ERROR(error, "No WAV data.");

    return wavSoundData;
}

//Implementation----------------------------------------------------------------

//WAVSoundData
WAVSoundData::WAVSoundData()
{
    this->data = nullptr;
    this->length = 0;
}

WAVSoundData::WAVSoundData(const void* data, size_t length, int channelCount, int bytesPerChannel, int samplesPerSecond)
{
    this->data = data;
    this->length = length;
    this->soundFormat.channelCount = channelCount;
    this->soundFormat.bytesPerChannel = bytesPerChannel;
    this->soundFormat.samplesPerSecond = samplesPerSecond;
}

size_t WAVSoundData::GetSampleCount() const
{
    auto sampleSize = this->soundFormat.GetBytesPerSample();
    if (sampleSize > 0)
        return this->length / sampleSize;
    else
        return 0;
}

//WAVReader
WAVReader::WAVReader()
{
}

WAVSoundData WAVReader::Read(ByteBufferReader& reader, ByteBuffer& soundOutputBuffer, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto wavSoundData = ReadWav(reader, soundOutputBuffer, error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);

    return wavSoundData;
}
