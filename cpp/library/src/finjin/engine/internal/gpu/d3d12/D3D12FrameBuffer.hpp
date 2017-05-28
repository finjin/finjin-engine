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
#include "D3D12Includes.hpp"
#include "D3D12Utilities.hpp"
#include "D3D12RenderTarget.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    class D3D12FrameBuffer
    {
    public:
        struct GraphicsCommandList
        {
            GraphicsCommandList()
            {
                this->fenceValue = 0;
            }

            uint64_t fenceValue;
            Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList;
        };

    public:
        D3D12FrameBuffer();

        void SetIndex(size_t index);

        void CreateCommandLists(ID3D12Device* device, size_t maxGpuCommandListsPerStage, Allocator* allocator, Error& error);
        void CreateScreenCaptureBuffer(ID3D12Device* device, size_t byteCount, bool isScreenSizeDependent, Error& error);

        void Destroy();
        void DestroyScreenSizeDependentResources();

        GraphicsCommandList* NewGraphicsCommandList();
        GraphicsCommandList* GetCurrentGraphicsCommandList();
        void ExecuteCommandLists(ID3D12CommandQueue* queue, uint64_t fenceValue);
        void WaitForCommandLists(ID3D12Fence* fence);
        void ResetCommandLists();

        void WaitForNotInUse();

    public:
        size_t index;
        std::atomic_size_t commandListWaitIndex;
        std::atomic_size_t commandListCommitIndex;

        D3D12RenderTarget renderTarget;

        Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator;
        DynamicVector<GraphicsCommandList> freeGraphicsCommandLists;
        DynamicVector<GraphicsCommandList*> graphicsCommandListsToExecute;
        DynamicVector<ID3D12GraphicsCommandList*> plainGraphicsCommandLists;

        Microsoft::WRL::ComPtr<ID3D12Resource> screenCaptureBuffer;
        void* screenCaptureBufferBytes;
        std::array<uint32_t, 2> screenCaptureSize;
        bool screenCaptureRequested;
        bool isScreenCaptureScreenSizeDependent;
    };

} }
