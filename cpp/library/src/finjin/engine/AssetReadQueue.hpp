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
#include "finjin/common/AllocatedClass.hpp"
#include "finjin/common/AllocatedQueue.hpp"
#include "finjin/common/BinaryDataChunkReader.hpp"
#include "finjin/common/ByteBuffer.hpp"
#include "finjin/common/ConfigDataChunkReader.hpp"
#include "finjin/common/EnumValues.hpp"
#include "finjin/common/Error.hpp"
#include "finjin/common/JsonDataChunkReader.hpp"
#include "finjin/common/Path.hpp"
#include "finjin/common/StreamingFileFormat.hpp"
#include "finjin/common/TextDataChunkReader.hpp"
#include "finjin/common/VirtualFileHandleDataChunkReaderInput.hpp"
#include "finjin/common/VirtualFileSystemOperationQueue.hpp"
#include "finjin/engine/AssetClass.hpp"
#include "finjin/engine/AssetClassFileReader.hpp"
#include "finjin/engine/AssetReadHandle.hpp"
#include "finjin/engine/AssetReference.hpp"


//Classes----------------------------------------------------------------------
namespace Finjin { namespace Engine {

    class AssetReadRequest
    {
    public:
        AssetReadRequest(Allocator* allocator) : assetRef(allocator)
        {
            this->assetClassFileReader = nullptr;
            this->streamingFileFormat = StreamingFileFormat::COUNT;
        }

        bool Create(Allocator* allocator)
        {
            return this->assetRef.Create(allocator);
        }

        void Destroy()
        {
            this->assetRef.Destroy();

            Reset();
        }

        using FullPostReadCallback = std::function<void(AssetReadHandle& assetReadHandle, const uint8_t* bytes, size_t byteCount, Error& error)>;
        using StreamingReadCallback = std::function<void(AssetReadHandle& assetReadHandle, DataChunkReader& reader, DataHeader& dataHeader, Error& error)>;
        using CancelCallback = std::function<void(AssetReadHandle& assetReadHandle)>;

        AssetReadRequest& FullReadRequest(FullPostReadCallback postReadCallback)
        {
            Reset();
            this->fullPostReadCallback = postReadCallback;
            return *this;
        }

        AssetReadRequest& StreamingReadRequest(StreamingReadCallback callback, StreamingFileFormat format)
        {
            Reset();
            this->streamingReadCallback = callback;
            this->streamingFileFormat = format;
            return *this;
        }

        AssetReadRequest& SetAssetReference(const AssetReference& assetRef, AssetClassFileReader& assetClassFileReader)
        {
            this->assetRef = assetRef;
            this->assetClassFileReader = &assetClassFileReader;
            return *this;
        }

    private:
        void Reset()
        {
            this->assetRef.Clear();
            this->assetClassFileReader = nullptr;

            this->fullPostReadCallback = nullptr;
            this->streamingReadCallback = nullptr;
            this->streamingFileFormat = StreamingFileFormat::COUNT;
            this->cancelCallback = nullptr;
        }

    public:
        AssetReference assetRef;
        AssetClassFileReader* assetClassFileReader;
        
        FullPostReadCallback fullPostReadCallback;
        StreamingReadCallback streamingReadCallback;
        StreamingFileFormat streamingFileFormat;
        CancelCallback cancelCallback;
    };

    class AssetReadQueue
    {
    public:
        struct Settings
        {
            Settings();

            void ParseSettings(const ByteBufferReader& configFileBuffer, const Path& assetFilePath, Error& error);

            Allocator* allocator;
            size_t queueSize;
            size_t bufferSize;
            size_t streamingMaxBytesPerLine;
            size_t streamingEstimatedLineCount;
            StaticVector<StreamingFileFormat, (size_t)StreamingFileFormat::COUNT> streamingFormats;
        };

        AssetReadQueue();

        void Create(const Settings& settings, Error& error);
        void Destroy();

        const Settings& GetSettings() const;

        void Update(Error& error);

        bool CanAddRequest() const;
        void AddRequest(AssetReadHandle& assetReadHandle, const AssetReadRequest& assetReadRequest, Error& error);
        
        float GetProgress(const AssetReadHandle& assetReadHandle) const;
        bool IsQueuedAndFinished(const AssetReadHandle& assetReadHandle) const;

        bool ClearAsset(const AssetReadHandle& assetReadHandle);
        
        void Cancel(const AssetReadHandle& assetReadHandle);

        bool HasPendingOperations() const;

        void CancelAndFinishPendingOperations();

    private:
        struct InternalAssetReadRequest : AssetReadRequest
        {
            using Super = AssetReadRequest;

            InternalAssetReadRequest(Allocator* allocator);

            bool Create(Allocator* allocator);
            void Destroy();

            void Start(const AssetReadRequest& assetReadRequest, const AssetReadHandle& assetReadHandle);

            void Finish();

            bool isFinished;
            bool cancel;
            VirtualFileOperationHandle fileOperationHandle;
            AssetReadHandle assetReadHandle;            
        };

        void ContinueStreaming(InternalAssetReadRequest& request, size_t maxReadByteCountHint, Error& error);

    private:
        Settings settings;

        VirtualFileOperationRequest workingFileOperationRequest;
        AllocatedQueue<InternalAssetReadRequest> internalAssetReadRequestQueue;
        
        size_t sequenceID;

        ByteBuffer assetBuffer;

        VirtualFileHandleDataChunkReaderInput dataChunkReaderFileInput;
        EnumValues<StreamingFileFormat, StreamingFileFormat::COUNT, DataChunkReader*> dataChunkReaders;
        BinaryDataChunkReader fsbdBinaryDataChunkReader;
        TextDataChunkReader fstdTextDataChunkReader;
        ConfigDataChunkReader configDataChunkReader;
        JsonDataChunkReader jsonDataChunkReader;
    };

} }
