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
#include "AssetClassFileReader.hpp"

using namespace Finjin::Common;
using namespace Finjin::Engine;


//Implementation---------------------------------------------------------------
AssetClassFileReader::AssetClassFileReader()
{
    this->reader = nullptr;
}

void AssetClassFileReader::Create(AssetFileReader& reader, const AssetPathSelector& initialSelector, AssetClass assetClass, Allocator* allocator, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    this->reader = &reader;

    if (!this->selector.Create(allocator))
    {
        FINJIN_SET_ERROR(error, "Failed to create selector.");
        return;
    }
    
    this->directories.maximize();
    for (auto& directory : this->directories)
    {
        if (!directory.Create(allocator))
        {
            FINJIN_SET_ERROR(error, "Failed to create all of the lookup directories.");
            return;
        }
    }
    this->directories.clear();
    
    //Set up selector by copying from another selector and replacing the prefix
    this->selector = initialSelector;
    this->selector.SetAssetClassDirectoryName(assetClass);

    //Use the selector to get the matching asset directories, using the asset class name as the subdirectory name to search for
    this->reader->GetDirectories(this->directories, this->selector, assetClass, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to get directories for asset reader.");
        return;
    }
}

AssetPathSelector& AssetClassFileReader::GetSelector()
{
    return this->selector;
}
