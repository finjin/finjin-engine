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
#include "finjin/common/StaticVector.hpp"
#include "finjin/common/VirtualFileSystemOperationQueue.hpp"
#include "finjin/engine/AssetPath.hpp"
#include "finjin/engine/AssetReference.hpp"


//Classes----------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;
    
    /**
     * Collects root directory names and file systems, providing the mechanism for loading asset files.
     * This class will often be used indirectly by way of AssetClassFileReader, which encapsulates asset-specific
     * directories to search.
     */
    class AssetFileReader
    {
        friend class AssetPathSelector;

    public:
        AssetFileReader();
        
        bool Create(Allocator* allocator);

        VirtualFileSystemOperationQueue* GetFileSystemOperationQueue();
        void SetFileSystemOperationQueue(VirtualFileSystemOperationQueue* fileSystemOperationQueue);

        bool AddFileSystem(VirtualFileSystem* fileSystem, Error& error);
        void RemoveFileSystem(VirtualFileSystem* fileSystem);
        size_t GetFileSystemCount() const;

        template <typename T>
        bool AddRootDirectoryName(const T& rootName, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            if (this->fileSystemRootDirectoryNames.find(rootName) == this->fileSystemRootDirectoryNames.end())
            {
                if (!this->fileSystemRootDirectoryNames.full())
                {
                    this->fileSystemRootDirectoryNames.push_back();
                    if (rootName.ToString(this->fileSystemRootDirectoryNames.back()).HasError())
                    {
                        this->fileSystemRootDirectoryNames.pop_back();
                        
                        FINJIN_SET_ERROR(error, "Failed to assign root name to string.");
                        return false;
                    }
                    return true;
                }
            }
            
            return false;
        }

        bool AddRootDirectoryName(const char* rootName, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            if (this->fileSystemRootDirectoryNames.find(rootName) == this->fileSystemRootDirectoryNames.end())
            {
                if (!this->fileSystemRootDirectoryNames.full())
                {
                    this->fileSystemRootDirectoryNames.push_back();
                    if (this->fileSystemRootDirectoryNames.back().assign(rootName).HasError())
                    {
                        this->fileSystemRootDirectoryNames.pop_back();

                        FINJIN_SET_ERROR(error, "Failed to assign root name to string.");
                        return false;
                    }
                    return true;
                }
            }

            return false;
        }

        template <typename T>
        void RemoveRootDirectoryName(const T& rootName)
        {
            auto foundAt = this->fileSystemRootDirectoryNames.find(rootName);
            if (foundAt != this->fileSystemRootDirectoryNames.end())
                this->fileSystemRootDirectoryNames.erase(foundAt);
        }

        size_t GetRootDirectoryNameCount() const;

        template <size_t directoriesSize>
        FileOperationResult ReadAsset
            (
            ByteBuffer& buffer,
            const AssetReference& assetRef,
            const AssetPathSelector& assetPathSelector,
            const StaticVector<AssetPath, directoriesSize>& directories
            )
        {
            BitArray<EngineConstants::MAX_MATCHING_ASSET_NAMES> invalidDirectories;

            //Try to find the file in the directories, from the best matching directory to the worst
            size_t invalidDirectoryCount = 0;
            while (invalidDirectoryCount < directories.size())
            {
                auto bestDirectory = assetPathSelector.SelectBest(directories.data(), directories.size(), invalidDirectories, invalidDirectoryCount);
                if (bestDirectory == nullptr)
                {
                    //No best matching directory
                    return FileOperationResult::NOT_FOUND;
                }

                //A directory was found. Compose a path to read
                if (this->workingSearchPath.assign(bestDirectory->GetFullName()).HasError())
                    return FileOperationResult::FAILURE;
                if ((this->workingSearchPath /= assetRef.filePath).HasError())
                    return FileOperationResult::FAILURE;

                //Attempt to read the asset
                for (auto fileSystem : this->fileSystems)
                {
                    auto readResult = fileSystem->Read(this->workingSearchPath, buffer);
                    if (readResult == FileOperationResult::SUCCESS)
                    {
                        //Succesfully read
                        return FileOperationResult::SUCCESS;
                    }
                    else if (readResult != FileOperationResult::NOT_FOUND)
                    {
                        //A critical error occurred
                        return readResult;
                    }
                }

                //Mark the current best as invalid so it won't be considered in the next iteration, forcing the next best directory to be selected
                auto invalidIndex = bestDirectory - directories.data(); 
                invalidDirectories.SetBit(invalidIndex);
                invalidDirectoryCount++;
            }

            return FileOperationResult::NOT_FOUND;
        }

        template <size_t directoriesSize>
        bool FindAssetPath
            (
            AssetReference& foundAssetPath,
            VirtualFileSystem*& assetFileSystem,
            const AssetReference& assetRef,
            const AssetPathSelector& assetPathSelector,
            const StaticVector<AssetPath, directoriesSize>& directories,
            Error& error
            )
        {
            FINJIN_ERROR_METHOD_START(error);

            BitArray<EngineConstants::MAX_MATCHING_ASSET_NAMES> invalidDirectories;

            //Try to find the file in the directories, from the best matching directory to the worst
            size_t invalidDirectoryCount = 0;
            while (invalidDirectoryCount < directories.size())
            {
                auto bestDirectory = assetPathSelector.SelectBest(directories.data(), directories.size(), invalidDirectories, invalidDirectoryCount);
                if (bestDirectory == nullptr)
                {
                    //No best matching directory
                    return false;
                }

                //A directory was found. Compose a path to read
                if (this->workingSearchPath.assign(bestDirectory->GetFullName()).HasError())
                {
                    FINJIN_SET_ERROR(error, "Failed to assign working search path.");
                    return false;
                }
                if ((this->workingSearchPath /= assetRef.filePath).HasError())
                {
                    FINJIN_SET_ERROR(error, "Failed to append asset reference file path to working search path.");
                    return false;
                }

                //Attempt to read the asset
                for (auto fileSystem : this->fileSystems)
                {
                    if (fileSystem->CanRead(this->workingSearchPath))
                    {
                        foundAssetPath.ForLocalFile(this->workingSearchPath);
                        assetFileSystem = fileSystem;
                        return true;
                    }
                }

                //Mark the current best as invalid so it won't be considered in the next iteration, forcing the next best directory to be selected
                auto invalidIndex = bestDirectory - directories.data();
                invalidDirectories.SetBit(invalidIndex);
                invalidDirectoryCount++;
            }

            return false;
        }
        
        //Finds the directories with names matching the specified selector
        template <size_t resultSize, typename RelativePath>
        void GetDirectories
            (
            StaticVector<AssetPath, resultSize>& directories,
            const AssetPathSelector& assetPathSelector,
            const RelativePath& rootPath, //Will be something like "app" or "engine"
            AssetClass assetClass, 
            Error& error
            )
        {
            FINJIN_ERROR_METHOD_START(error);

            if (this->workingSearchPath.assign(rootPath).HasError())
            {
                FINJIN_SET_ERROR(error, "Failed to assign root path.");
                return;
            }
            if ((this->workingSearchPath /= AssetClassUtilities::ToDirectoryName(assetClass)).HasError())
            {
                FINJIN_SET_ERROR(error, "Failed to append asset class directory name.");
                return;
            }

            for (auto fileSystem : this->fileSystems)
            {
                auto failedToAddDirectory = false;
                fileSystem->ForDirectoriesStartingWith(this->workingSearchPath, [&](const Path& foundRelativePath)
                {
                    auto fileNameLength = foundRelativePath.GetFileNameLength();

                    //If the selector accepts the directory, keep it. Otherwise reject it
                    this->workingAssetPath.Create(assetClass, foundRelativePath, foundRelativePath.length() - fileNameLength, error);
                    if (error)
                    {
                        FINJIN_SET_ERROR(error, "Failed to create working asset path.");
                        return false;
                    }

                    if (assetPathSelector.Accepts(this->workingAssetPath))
                    {
                        if (directories.full())
                            failedToAddDirectory = true;
                        else
                            directories.push_back(this->workingAssetPath);
                    }

                    //Enumeration can continue if there is space in 'directories' and there wasn't a failure
                    return !directories.full() && !failedToAddDirectory;
                });

                if (failedToAddDirectory)
                {
                    FINJIN_SET_ERROR(error, "Failed to get all directories. Directories collection is full.");
                    return;
                }
            }
        }

        template <size_t resultSize>
        void GetDirectories
            (
            StaticVector<AssetPath, resultSize>& directories,
            const AssetPathSelector& assetPathSelector,
            AssetClass assetClass,
            Error& error
            )
        {
            FINJIN_ERROR_METHOD_START(error);

            for (auto& rootPath : this->fileSystemRootDirectoryNames)
            {
                GetDirectories(directories, assetPathSelector, rootPath, assetClass, error);
                if (error)
                {
                    FINJIN_SET_ERROR(error, "Failed to get all directories.");
                    break;
                }
            }
        }
        
    private:
        VirtualFileSystemOperationQueue* fileSystemOperationQueue;
        StaticVector<VirtualFileSystem*, EngineConstants::MAX_FILE_SYSTEMS> fileSystems;
        StaticVector<Utf8String, EngineConstants::MAX_ASSET_ROOT_DIRECTORIES> fileSystemRootDirectoryNames;

        Path workingSearchPath;
        AssetPath workingAssetPath;
    };
    
} }
