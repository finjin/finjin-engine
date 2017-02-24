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
#include "D3D12Includes.hpp"
#include "D3D12Utilities.hpp"
#include "D3D12StaticMeshRendererFrameState.hpp"
#include "D3D12RenderTarget.hpp"


//Classes----------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    class D3D12FrameBuffer
    {
    public:
        D3D12FrameBuffer();

        void SetIndex(size_t index);

        ID3D12GraphicsCommandList* NewGraphicsCommandList();
        ID3D12GraphicsCommandList* CurrentGraphicsCommandList();
        void ExecuteCommandLists(ID3D12CommandQueue* queue);
        void ResetCommandLists();

        void ResetFences(uint64_t value);

    public:
        size_t index;
        D3D12RenderTarget renderTarget;
        
        uint64_t framePresentCompletedFenceValue;
        
        Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator;
        AllocatedVector<Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> > graphicsCommandLists;
        AllocatedVector<ID3D12CommandList*> commandListsToExecute;

        D3D12StaticMeshRendererFrameState staticMeshRendererFrameState;
    };

} }
