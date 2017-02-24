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
#include "finjin/common/Error.hpp"
#include "finjin/engine/AssetClass.hpp"
#include "finjin/engine/AssetFileReader.hpp"
#include "finjin/engine/AssetReference.hpp"


//Classes----------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;
    
    /**
     * Provides access to AssetFileReader (and its underlying queue) for a specific asset class.
     */
    class AssetClassFileReader
    {
    public:
        AssetClassFileReader();
        
        void Create(AssetFileReader& reader, const AssetPathSelector& initialSelector, AssetClass assetClass, Allocator* allocator, Error& error);
        
        AssetPathSelector& GetSelector();

        FileOperationResult ReadAsset(ByteBuffer& buffer, const AssetReference& assetRef)
        {
            assert(this->reader != nullptr);
            return this->reader->ReadAsset(buffer, assetRef, this->selector, this->directories);
        }

        bool FindAssetPath(AssetReference& foundAssetRef, VirtualFileSystem*& assetFileSystem, const AssetReference& fileName, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            assert(this->reader != nullptr);
            auto result = this->reader->FindAssetPath(foundAssetRef, assetFileSystem, fileName, this->selector, this->directories, error);
            if (error)
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to find asset path for '%1%'.", fileName.ToUriString()));
            return result;
        }

        void RequestReadAsset(VirtualFileOperationHandle& operationHandle, VirtualFileOperationRequest& request, const AssetReference& fileName, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            if (this->reader->GetFileSystemOperationQueue() == nullptr)
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to request asset path for '%1%'.", fileName.ToUriString()));
                return;
            }

            AssetReference foundAssetRef;
            VirtualFileSystem* foundFileSystem;
            auto foundAssetPath = FindAssetPath(foundAssetRef, foundFileSystem, fileName, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("There was an error while finding asset path for '%1%'.", fileName.ToUriString()));
                return;
            }

            if (foundAssetPath)
            {
                request.SetReadUri(foundAssetRef.ToSimpleUri(), *foundFileSystem);
                this->reader->GetFileSystemOperationQueue()->AddRequest(operationHandle, request, error);
                if (error)
                {
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to register asset request for '%1%' with file system operation queue.", fileName.ToUriString()));
                    return;
                }
            }
            else
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to find asset path for '%1%'.", fileName.ToUriString()));
                return;
            }
        }

        template <typename Settings>
        FileOperationResult ReadAndParseSettingsFile(Settings& settings, ByteBuffer& readBuffer, const AssetReference& settingsAssetRef, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            auto result = ReadAsset(readBuffer, settingsAssetRef);
            if (result == FileOperationResult::SUCCESS)
            {
                settings.ParseSettings(readBuffer, settingsAssetRef.filePath, error);
                if (error)
                {
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to parse the '%1%' settings file.", settingsAssetRef.ToUriString()));
                    return FileOperationResult::FAILURE;
                }
                
                return FileOperationResult::SUCCESS;
            }
            else if (result != FileOperationResult::NOT_FOUND)
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read the '%1%' settings file.", settingsAssetRef.ToUriString()));
                return FileOperationResult::FAILURE;
            }
            
            return FileOperationResult::NOT_FOUND;
        }

        template <typename Settings, typename FileNames>
        void ReadAndParseSettingsFiles(Settings& settings, ByteBuffer& readBuffer, FileNames& fileNames, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            for (auto& fileName : fileNames)
            {
                ReadAndParseSettingsFile(settings, readBuffer, fileName, error);
                if (error)
                {
                    FINJIN_SET_ERROR_NO_MESSAGE(error);
                    return;
                }
            }
        }

    private:
        AssetFileReader* reader;
        AssetPathSelector selector;
        StaticVector<AssetPath, EngineConstants::MAX_ASSET_CLASS_DIRECTORIES> directories;
    };
   
} }
