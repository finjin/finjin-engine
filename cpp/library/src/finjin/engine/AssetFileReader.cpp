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
#include "AssetFileReader.hpp"

using namespace Finjin::Engine;


//Implementation----------------------------------------------------------------
AssetFileReader::AssetFileReader()
{
    this->fileSystemOperationQueue = nullptr;

}

bool AssetFileReader::Create(Allocator* allocator)
{
    auto result = true;

    result &= this->workingSearchPath.Create(allocator);

    this->fileSystemRootDirectoryNames.maximize();
    for (auto& directoryName : this->fileSystemRootDirectoryNames)
        result &= directoryName.Create(allocator);
    this->fileSystemRootDirectoryNames.clear();

    result &= this->workingAssetPath.Create(allocator);

    return result;
}

VirtualFileSystemOperationQueue* AssetFileReader::GetFileSystemOperationQueue()
{
    return this->fileSystemOperationQueue;
}

void AssetFileReader::SetFileSystemOperationQueue(VirtualFileSystemOperationQueue* fileSystemOperationQueue)
{
    this->fileSystemOperationQueue = fileSystemOperationQueue;
}

bool AssetFileReader::AddFileSystem(VirtualFileSystem* fileSystem, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    if (fileSystem->GetRootCount() == 0)
    {
        FINJIN_SET_ERROR(error, "The specified file system doesn't have any roots.");
        return false;
    }

    if (this->fileSystems.full())
    {
        FINJIN_SET_ERROR(error, "The file systems collection is full.");
        return false;
    }

    if (this->fileSystems.find(fileSystem) == this->fileSystems.end())
    {
        auto addResult = this->fileSystems.push_back(fileSystem);
        if (addResult.HasError())
        {
            FINJIN_SET_ERROR(error, "There was an error while adding the file system to the collection.");
            return false;
        }
        else
            return addResult.value;
    }
    else
        return false;
}

void AssetFileReader::RemoveFileSystem(VirtualFileSystem* fileSystem)
{
    auto foundAt = this->fileSystems.find(fileSystem);
    if (foundAt != this->fileSystems.end())
        this->fileSystems.erase(foundAt);
}

size_t AssetFileReader::GetFileSystemCount() const
{
    return this->fileSystems.size();
}

size_t AssetFileReader::GetRootDirectoryNameCount() const
{
    return this->fileSystemRootDirectoryNames.size();
}
