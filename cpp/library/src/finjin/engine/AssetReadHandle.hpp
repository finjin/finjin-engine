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
#include "finjin/common/Path.hpp"
#include "finjin/engine/Asset.hpp"
#include "finjin/engine/AssetReference.hpp"


//Classes----------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    class AssetReadQueue;

    class AssetReadHandle
    {
        friend class AssetReadQueue;

    public:
        AssetReadHandle(Allocator* allocator = nullptr);

        bool Create(Allocator* allocator);
        void Destroy();

        void Clear();

        bool IsInUse() const;

        bool IsQueuedAndFinished() const;

        void FinishInProgress();

        float GetProgress() const;

        void Cancel();

        AssetReadQueue& GetOperationQueue();

        const AssetReference& GetAssetReference() const;

    private:
        AssetReadQueue* assetReadQueue;
        AssetReference assetRef;
        size_t operationIndex;
        size_t sequenceID;
    };

} }
