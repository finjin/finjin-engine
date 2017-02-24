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

using namespace Finjin::Common;
using namespace Finjin::Engine;


//Implementation---------------------------------------------------------------
AssetReadHandle::AssetReadHandle(Allocator* allocator) : assetRef(allocator)
{
    Clear();
}

bool AssetReadHandle::Create(Allocator* allocator)
{
    Destroy();

    return this->assetRef.Create(allocator);
}

void AssetReadHandle::Destroy()
{
    this->assetRef.Destroy();
    Clear();
}

void AssetReadHandle::Clear()
{
    this->assetReadQueue = nullptr;
    this->assetRef.Clear();
    this->operationIndex = (size_t)-1;
    this->sequenceID = 0;
}

bool AssetReadHandle::IsInUse() const
{
    return this->assetReadQueue != nullptr;
}

bool AssetReadHandle::IsQueuedAndFinished() const
{
    if (this->assetReadQueue == nullptr)
        return false;

    return this->assetReadQueue->IsQueuedAndFinished(*this);
}

void AssetReadHandle::FinishInProgress()
{
    if (this->assetReadQueue != nullptr && this->assetReadQueue->IsQueuedAndFinished(*this))
    {
        this->assetReadQueue->ClearAsset(*this);
        Clear();
    }
}

float AssetReadHandle::GetProgress() const
{
    if (this->assetReadQueue == nullptr)
        return 0;

    return this->assetReadQueue->GetProgress(*this);
}

void AssetReadHandle::Cancel()
{
    if (this->assetReadQueue != nullptr)
        this->assetReadQueue->Cancel(*this);
}

AssetReadQueue& AssetReadHandle::GetOperationQueue()
{
    return *this->assetReadQueue;
}

const AssetReference& AssetReadHandle::GetAssetReference() const
{
    return this->assetRef;
}
