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
#include "AssetReadHandle.hpp"
#include "AssetReadQueue.hpp"
#include "finjin/common/ConfigDocumentReader.hpp"
#include "finjin/common/Convert.hpp"
#include "finjin/common/DataChunkReader.hpp"
#include "finjin/common/DataChunkReaderInput.hpp"
#include "finjin/common/DataChunkReaderController.hpp"
#include "finjin/common/MemorySize.hpp"
#include "finjin/common/VirtualFileSystemOperationQueue.hpp"

using namespace Finjin::Engine;


//Implementation----------------------------------------------------------------

//AssetReadQueue::Settings
AssetReadQueue::Settings::Settings()
{
    this->allocator = nullptr;
    this->queueSize = 100;
    this->bufferSize = EngineConstants::DEFAULT_ASSET_BUFFER_SIZE;
    this->streamingMaxBytesPerLine = DataChunkReader::Settings::DEFAULT_MAX_BYTES_PER_LINE;
    this->streamingEstimatedLineCount = 1;
}

void AssetReadQueue::Settings::ParseSettings(const ByteBufferReader& configFileBuffer, const Path& assetFilePath, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    Utf8StringView section, key, value;

    ConfigDocumentReader reader;
    for (auto line = reader.Start(configFileBuffer); line != nullptr; line = reader.Next())
    {
        switch (line->GetType())
        {
            case ConfigDocumentLine::Type::SECTION:
            {
                line->GetSectionName(section);

                break;
            }
            case ConfigDocumentLine::Type::KEY_AND_VALUE:
            {
                line->GetKeyAndValue(key, value);

                if (section.empty())
                {
                    if (key == "queue-size")
                    {
                        this->queueSize = Convert::ToInteger(value, this->queueSize);
                        if (this->queueSize < 2)
                        {
                            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Invalid value '%1%' for setting 'queue-size'.", value));
                            return;
                        }
                    }
                    else if (key == "buffer-size")
                    {
                        MemorySize::Parse(this->bufferSize, value, error);
                        if (error)
                        {
                            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Invalid value '%1%' for setting 'buffer-size'.", value));
                            return;
                        }
                    }
                    else if (key == "streaming-buffer-line-size")
                    {
                        MemorySize::Parse(this->streamingMaxBytesPerLine, value, error);
                        if (error)
                        {
                            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Invalid value '%1%' for setting 'streaming-buffer-line-size'.", value));
                            return;
                        }
                    }
                    else if (key == "streaming-buffer-estimated-line-count")
                    {
                        this->streamingEstimatedLineCount = Convert::ToInteger(value, this->streamingEstimatedLineCount);
                        if (this->streamingEstimatedLineCount < 1)
                        {
                            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Invalid value '%1%' for setting 'streaming-buffer-estimated-line-count'.", value));
                            return;
                        }
                    }
                    else if (key == "streaming-formats")
                    {
                        auto parseFailed = false;
                        auto splitResult = Split(value, ' ', [this, &parseFailed, &error](Utf8StringView& stringFormat)
                        {
                            StreamingFileFormat format;
                            StreamingFileFormatUtilities::ParseFromExtension(format, stringFormat, error);
                            if (error)
                            {
                                parseFailed = true;

                                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Invalid value '%1%' for setting 'formats'.", stringFormat));
                                return ValueOrError<bool>::CreateError();
                            }

                            if (!this->streamingFormats.contains(format))
                                return this->streamingFormats.push_back(format);
                            else
                                return ValueOrError<bool>(true);
                        });
                        if (splitResult.HasErrorOrValue(false) && !parseFailed)
                        {
                            FINJIN_SET_ERROR(error, "Failed to add all specified 'streaming-formats' values.");
                            return;
                        }
                    }
                }

                break;
            }
            default: break;
        }
    }
}

//AssetReadQueue::InternalAssetReadRequest
AssetReadQueue::InternalAssetReadRequest::InternalAssetReadRequest(Allocator* allocator) : Super(allocator), assetReadHandle(allocator)
{
    this->isFinished = true;
}

bool AssetReadQueue::InternalAssetReadRequest::Create(Allocator* allocator)
{
    auto result = true;
    result &= Super::Create(allocator);
    result &= this->assetReadHandle.Create(allocator);
    return result;
}

void AssetReadQueue::InternalAssetReadRequest::Destroy()
{
    this->assetReadHandle.Destroy();

    Super::Destroy();
}

void AssetReadQueue::InternalAssetReadRequest::Start(const AssetReadRequest& assetReadRequest, const AssetReadHandle& assetReadHandle)
{
    static_cast<AssetReadRequest&>(*this) = assetReadRequest;
    this->isFinished = false;
    this->cancel = false;
    this->assetReadHandle = assetReadHandle;
}

void AssetReadQueue::InternalAssetReadRequest::Finish()
{
    this->isFinished = true;
    this->cancel = false;

    this->fileOperationHandle.FinishInProgress();
}

void AssetReadQueue::ContinueStreaming(InternalAssetReadRequest& request, size_t maxReadByteCountHint, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    DataHeader dataHeader;

    auto totalReadBefore = this->dataChunkReaders[request.streamingFileFormat]->GetReaderInput()->GetTotalBytesRead();

    auto reading = true;
    while (reading && (this->dataChunkReaders[request.streamingFileFormat]->GetReaderInput()->GetTotalBytesRead() - totalReadBefore) < maxReadByteCountHint)
    {
        this->dataChunkReaders[request.streamingFileFormat]->ReadDataHeader(dataHeader, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to read data header.");
            reading = false;
        }
        else
        {
            request.streamingReadCallback(request.assetReadHandle, *this->dataChunkReaders[request.streamingFileFormat], dataHeader, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Error processing streaming document.");
                reading = false;
            }
            else if (dataHeader.type == DataHeaderType::END)
            {
                reading = false;
            }
        }
    }
}

//AssetReadQueue
AssetReadQueue::AssetReadQueue()
{
    this->sequenceID = 0;

    for (auto& reader : this->dataChunkReaders)
        reader = nullptr;
}

void AssetReadQueue::Create(const Settings& settings, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    if (settings.queueSize < 2)
    {
        FINJIN_SET_ERROR(error, "The operation queue size must be 2 or more.");
        return;
    }

    if (settings.bufferSize == 0)
    {
        FINJIN_SET_ERROR(error, "The asset buffer size must be greater than zero.");
        return;
    }

    this->settings = settings;
    if (this->settings.streamingFormats.empty())
    {
        this->settings.streamingFormats.push_back(StreamingFileFormat::STREAMING_BINARY);
        this->settings.streamingFormats.push_back(StreamingFileFormat::STREAMING_TEXT);
    #if FINJIN_DEBUG
        this->settings.streamingFormats.push_back(StreamingFileFormat::STREAMING_CONFIG);
        this->settings.streamingFormats.push_back(StreamingFileFormat::STREAMING_JSON);
    #endif
    }

    if (!this->workingFileOperationRequest.Create(this->settings.allocator))
    {
        FINJIN_SET_ERROR(error, "Failed to create internal file operation request.");
        return;
    }

    if (!this->internalAssetReadRequestQueue.Create(this->settings.queueSize, this->settings.allocator, this->settings.allocator))
    {
        FINJIN_SET_ERROR(error, "Failed to create internal asset read queue.");
        return;
    }

    if (!this->assetBuffer.CreateEmpty(this->settings.bufferSize, this->settings.allocator))
    {
        FINJIN_SET_ERROR(error, "Failed to create asset buffer.");
        return;
    }

    DataChunkReader::Settings readerSettings;
    readerSettings.Create(this->dataChunkReaderFileInput, DefaultDataChunkReaderController::GetInstance());
    readerSettings.maxBytesPerLine = this->settings.streamingMaxBytesPerLine;
    readerSettings.estimatedLineCount = this->settings.streamingEstimatedLineCount;
    if (this->settings.streamingFormats.contains(StreamingFileFormat::STREAMING_BINARY))
    {
        this->fsbdBinaryDataChunkReader.Create(readerSettings, nullptr, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to create streaming binary reader.");
            return;
        }
        this->dataChunkReaders[StreamingFileFormat::STREAMING_BINARY] = &this->fsbdBinaryDataChunkReader;
    }
    if (this->settings.streamingFormats.contains(StreamingFileFormat::STREAMING_TEXT))
    {
        this->fstdTextDataChunkReader.Create(readerSettings, nullptr, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to create streaming text reader.");
            return;
        }
        this->dataChunkReaders[StreamingFileFormat::STREAMING_TEXT] = &this->fstdTextDataChunkReader;
    }
    if (this->settings.streamingFormats.contains(StreamingFileFormat::STREAMING_CONFIG))
    {
        this->configDataChunkReader.Create(readerSettings, nullptr, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to create streaming config reader.");
            return;
        }
        this->dataChunkReaders[StreamingFileFormat::STREAMING_CONFIG] = &this->configDataChunkReader;
    }
    if (this->settings.streamingFormats.contains(StreamingFileFormat::STREAMING_JSON))
    {
        this->jsonDataChunkReader.Create(readerSettings, nullptr, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to create streaming JSON reader.");
            return;
        }
        this->dataChunkReaders[StreamingFileFormat::STREAMING_JSON] = &this->jsonDataChunkReader;
    }
}

void AssetReadQueue::Destroy()
{
    CancelAndFinishPendingOperations();

    for (auto& reader : this->dataChunkReaders)
        reader = nullptr;
}

const AssetReadQueue::Settings& AssetReadQueue::GetSettings() const
{
    return this->settings;
}

void AssetReadQueue::Update(Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    if (HasPendingOperations())
    {
        auto& currentAssetReadRequest = this->internalAssetReadRequestQueue.current();

        if (currentAssetReadRequest.cancel)
        {
            if (currentAssetReadRequest.cancelCallback != nullptr)
                currentAssetReadRequest.cancelCallback(currentAssetReadRequest.assetReadHandle);

            currentAssetReadRequest.Finish();
        }
        else
        {
            if (currentAssetReadRequest.fileOperationHandle.IsQueuedAndFinished())
            {
                //Call post-read callback
                if (currentAssetReadRequest.fullPostReadCallback != nullptr)
                {
                    //currentAssetReadRequest.fullPostReadCallback(currentAssetReadRequest.assetReadHandle, );
                }

                currentAssetReadRequest.Finish();
            }
            else
            {
                //currentAssetReadRequest.fullPostReadCallback(currentAssetReadRequest.assetReadHandle, false);

                //It's possible the operation became cancelled during the update
                if (currentAssetReadRequest.cancel)
                {
                    if (currentAssetReadRequest.cancelCallback != nullptr)
                        currentAssetReadRequest.cancelCallback(currentAssetReadRequest.assetReadHandle);

                    currentAssetReadRequest.Finish();
                }
            }
        }

        //Move to next operation
        if (currentAssetReadRequest.isFinished)
            this->internalAssetReadRequestQueue.pop();
    }
}

bool AssetReadQueue::CanAddRequest() const
{
    return !this->internalAssetReadRequestQueue.full();
}

void AssetReadQueue::AddRequest(AssetReadHandle& assetReadHandle, const AssetReadRequest& assetReadRequest, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    assetReadHandle.Clear();

    //Validate request
    if (!assetReadRequest.assetRef.IsValid())
    {
        FINJIN_SET_ERROR(error, "No asset reference was specified.");
        return;
    }

    if (assetReadRequest.fullPostReadCallback == nullptr && assetReadRequest.streamingReadCallback == nullptr)
    {
        FINJIN_SET_ERROR(error, "No post-read callback was specified.");
        return;
    }

    if (assetReadRequest.streamingReadCallback != nullptr)
    {
        if (!StreamingFileFormatUtilities::IsValid(assetReadRequest.streamingFileFormat))
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Invalid streaming file format '%1%'.", StreamingFileFormatUtilities::ToString(assetReadRequest.streamingFileFormat)));
            return;
        }
        if (this->dataChunkReaders[assetReadRequest.streamingFileFormat] == nullptr)
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Invalid streaming file format '%1%'. No corresponding reader available.", StreamingFileFormatUtilities::ToString(assetReadRequest.streamingFileFormat)));
            return;
        }
    }

    if (this->internalAssetReadRequestQueue.full())
    {
        FINJIN_SET_ERROR(error, "The asset read request queue is full.");
        return;
    }

    //Set up queue entry
    this->internalAssetReadRequestQueue.push();
    auto& internalAssetReadRequest = this->internalAssetReadRequestQueue.back();

    //Add to file operation queue
    auto requestValid = false;

    if (assetReadRequest.streamingReadCallback != nullptr)
    {
        this->workingFileOperationRequest.ReadRequest([this, &internalAssetReadRequest](VirtualFileOperationHandle& assetReadHandle, VirtualFileHandle& fileHandle, uint8_t* readBuffer, size_t maxReadByteCount, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            uint64_t totalBytesReadBefore = 0;

            if (assetReadHandle.GetTotalBytesProcessed() == 0)
            {
                //Starting a read
                this->dataChunkReaderFileInput.SetHandle(&fileHandle);

                DataHeader dataHeader;
                this->dataChunkReaders[internalAssetReadRequest.streamingFileFormat]->ReadReaderHeader(dataHeader, error);
                if (error)
                    FINJIN_SET_ERROR(error, "Failed to read reader header.");
            }
            else
            {
                //Continuing a read
                totalBytesReadBefore = this->dataChunkReaders[internalAssetReadRequest.streamingFileFormat]->GetReaderInput()->GetTotalBytesRead();
            }

            if (!error)
            {
                ContinueStreaming(internalAssetReadRequest, maxReadByteCount, error);
                if (error)
                    FINJIN_SET_ERROR(error, "Error continuing the streaming of asset streaming text.");
            }

            return this->dataChunkReaders[internalAssetReadRequest.streamingFileFormat]->GetReaderInput()->GetTotalBytesRead() - totalBytesReadBefore;
        });
        requestValid = true;
    }
    else
    {
        this->workingFileOperationRequest.ReadRequest([this, &internalAssetReadRequest](VirtualFileOperationHandle& assetReadHandle, const uint8_t* bytes, size_t byteCount, bool isFinished, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            if (assetReadHandle.GetTotalBytesProcessed() == 0 && isFinished)
            {
                //The entire file was read in a single iteration
                internalAssetReadRequest.fullPostReadCallback(internalAssetReadRequest.assetReadHandle, bytes, byteCount, error);
            }
            else
            {
                //The file needs more than one iteration to be read
                if (assetReadHandle.GetTotalBytesProcessed() == 0)
                    this->assetBuffer.clear();

                this->assetBuffer.Write(bytes, byteCount);

                if (isFinished)
                    internalAssetReadRequest.fullPostReadCallback(internalAssetReadRequest.assetReadHandle, this->assetBuffer.data(), this->assetBuffer.size(), error);
            }

            if (error)
                FINJIN_SET_ERROR(error, "Error calling asset full post read callback.");
        });
        requestValid = true;
    }

    if (requestValid)
    {
        assetReadRequest.assetClassFileReader->RequestReadAsset(internalAssetReadRequest.fileOperationHandle, this->workingFileOperationRequest, assetReadRequest.assetRef, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to request read asset.");
            return;
        }
    }

    //Fill in result
    assetReadHandle.assetReadQueue = this;
    assetReadHandle.assetRef = assetReadRequest.assetRef;
    assetReadHandle.operationIndex = this->internalAssetReadRequestQueue.GetEntryIndex(internalAssetReadRequest);
    assetReadHandle.sequenceID = this->sequenceID;

    internalAssetReadRequest.Start(assetReadRequest, assetReadHandle);

    //Advance
    this->sequenceID++;
}

float AssetReadQueue::GetProgress(const AssetReadHandle& assetReadHandle) const
{
    if (assetReadHandle.assetReadQueue == this && assetReadHandle.operationIndex != (size_t)-1)
    {
        auto& operation = this->internalAssetReadRequestQueue[assetReadHandle.operationIndex];
        if (operation.assetReadHandle.sequenceID == assetReadHandle.sequenceID)
            return operation.fileOperationHandle.GetProgress();
    }

    return 0;
}

bool AssetReadQueue::IsQueuedAndFinished(const AssetReadHandle& assetReadHandle) const
{
    if (assetReadHandle.assetReadQueue == this && assetReadHandle.operationIndex != (size_t)-1)
    {
        auto& operation = this->internalAssetReadRequestQueue[assetReadHandle.operationIndex];
        if (operation.assetReadHandle.sequenceID == assetReadHandle.sequenceID)
            return operation.isFinished;
    }

    return false;
}

bool AssetReadQueue::ClearAsset(const AssetReadHandle& assetReadHandle)
{
    auto result = false;

    if (assetReadHandle.assetReadQueue == this && assetReadHandle.operationIndex != (size_t)-1)
    {
        auto& operation = this->internalAssetReadRequestQueue[assetReadHandle.operationIndex];
        if (operation.assetReadHandle.sequenceID == assetReadHandle.sequenceID)
            result = true;
    }

    return result;
}

void AssetReadQueue::Cancel(const AssetReadHandle& assetReadHandle)
{
    if (assetReadHandle.assetReadQueue == this && assetReadHandle.operationIndex != (size_t)-1)
    {
        auto& operation = this->internalAssetReadRequestQueue[assetReadHandle.operationIndex];
        if (operation.assetReadHandle.sequenceID == assetReadHandle.sequenceID)
            operation.cancel = true;
    }
}

bool AssetReadQueue::HasPendingOperations() const
{
    return !this->internalAssetReadRequestQueue.empty();
}

void AssetReadQueue::CancelAndFinishPendingOperations()
{
    for (auto& operation : this->internalAssetReadRequestQueue)
        operation.cancel = true;

    FINJIN_DECLARE_ERROR(error);
    Update(error);
}
