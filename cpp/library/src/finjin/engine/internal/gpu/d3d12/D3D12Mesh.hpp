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
#include "finjin/common/Math.hpp"
#include "finjin/engine/FinjinSceneAssets.hpp"
#include "D3D12GpuBuffer.hpp"
#include "D3D12Includes.hpp"
#include "D3D12Utilities.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    class D3D12Mesh
    {
    public:
        struct Submesh
        {
            Submesh(D3D12Mesh* mesh = nullptr);

            D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView() const;
            D3D12_INDEX_BUFFER_VIEW GetIndexBufferView() const;

            GpuInputFormatStruct* GetInputFormat();

            D3D12Mesh* mesh;

            UINT vertexBufferIndex;
            UINT startIndexLocation;
            UINT indexCount;
            UINT baseVertexLocation;
            UINT vertexCount;

            D3D12GpuBuffer vertexBuffer;
            D3D12GpuBuffer indexBuffer;

            D3D12_PRIMITIVE_TOPOLOGY primitiveType;
        };

        D3D12Mesh(Allocator* allocator);

        void Destroy();

        void HandleCreationFailure();
        void ReleaseUploaders();

        D3D12_VERTEX_BUFFER_VIEW GetSharedVertexBufferView(UINT vertexBufferIndex) const;
        D3D12_INDEX_BUFFER_VIEW GetSharedIndexBufferView() const;

        bool IsResidentOnGpu() const;
        void UpdateResidentOnGpuStatus();

    public:
        FinjinMesh* finjinMesh;

        D3D12GpuBuffer sharedIndexBuffer;
        DynamicVector<D3D12GpuBuffer> sharedVertexBuffers;

        DynamicVector<Submesh> submeshes;

        size_t isResidentCountdown;
        D3D12Mesh* waitingToBeResidentNext;
    };

} }
