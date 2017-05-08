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


//Includes----------------------------------------------------------------------
#include "finjin/common/Error.hpp"
#include "finjin/engine/FinjinSceneAssets.hpp"
#include "MetalBufferView.hpp"
#include "MetalGpuBuffer.hpp"
#include "MetalUtilities.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    class MetalMesh
    {
    public:
        struct Submesh
        {
            Submesh(MetalMesh* mesh = nullptr);

            MetalVertexBufferView GetVertexBufferView() const;
            MetalIndexBufferView GetIndexBufferView() const;

            GpuInputFormatStruct* GetInputFormat();

            MetalMesh* mesh;

            NSUInteger vertexBufferIndex;
            NSUInteger startIndexLocation;
            NSUInteger indexCount;
            NSUInteger baseVertexLocation;
            NSUInteger vertexCount;

            MetalGpuBuffer vertexBuffer;
            MetalGpuBuffer indexBuffer;

            MTLPrimitiveType primitiveType;
        };

        MetalMesh(Allocator* allocator);

        void Destroy();

        void HandleCreationFailure();

        MetalVertexBufferView GetSharedVertexBufferView(size_t vertexBufferIndex) const;
        MetalIndexBufferView GetSharedIndexBufferView() const;

    public:
        FinjinMesh* finjinMesh;

        MetalGpuBuffer sharedIndexBuffer;
        DynamicVector<MetalGpuBuffer> sharedVertexBuffers;

        DynamicVector<Submesh> submeshes;
    };

} }
