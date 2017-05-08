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

#if FINJIN_TARGET_GPU_SYSTEM == FINJIN_TARGET_GPU_SYSTEM_D3D12

#include "D3D12FrameBuffer.hpp"

using namespace Finjin::Engine;


//Implementation----------------------------------------------------------------
D3D12FrameBuffer::D3D12FrameBuffer()
{
    this->index = 0;

    this->framePresentCompletedFenceValue = 0;
}

void D3D12FrameBuffer::SetIndex(size_t index)
{
    this->index = index;

    this->staticMeshRendererFrameState.index = index;
}

ID3D12GraphicsCommandList* D3D12FrameBuffer::NewGraphicsCommandList()
{
    if (!this->graphicsCommandLists.push_back())
        return nullptr;

    auto result = this->graphicsCommandLists.back().Get();
    this->commandListsToExecute.push_back(result);
    return result;
}

ID3D12GraphicsCommandList* D3D12FrameBuffer::GetCurrentGraphicsCommandList()
{
    return !this->graphicsCommandLists.empty() ? this->graphicsCommandLists.back().Get() : nullptr;
}

void D3D12FrameBuffer::ExecuteCommandLists(ID3D12CommandQueue* queue)
{
    if (!this->commandListsToExecute.empty())
    {
        queue->ExecuteCommandLists(static_cast<UINT>(this->commandListsToExecute.size()), this->commandListsToExecute.data());
        this->commandListsToExecute.clear();
    }
}

void D3D12FrameBuffer::ResetCommandLists()
{
    this->graphicsCommandLists.clear();
    this->commandListsToExecute.clear();
}

void D3D12FrameBuffer::ResetFences(uint64_t value)
{
    this->framePresentCompletedFenceValue = value;
}

#endif
