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
}

void D3D12FrameBuffer::SetIndex(size_t index)
{
    this->index = index;
    this->commandListWaitIndex = 0;
    this->commandListCommitIndex = 0;
}

void D3D12FrameBuffer::Destroy()
{
}

void D3D12FrameBuffer::CreateCommandLists(ID3D12Device* device, size_t maxGpuCommandListsPerStage, Allocator* allocator, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    if (FINJIN_CHECK_HRESULT_FAILED(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&this->commandAllocator))))
    {
        FINJIN_SET_ERROR(error, "Failed to create frame buffer command allocator.");
        return;
    }
    FINJIN_D3D12_SET_RESOURCE_NAME(this->commandAllocator, Utf8StringFormatter::Format("D3D12 frame buffer %1% graphics command allocator", this->index));

    if (!this->freeGraphicsCommandLists.Create(maxGpuCommandListsPerStage, allocator))
    {
        FINJIN_SET_ERROR(error, "Failed to create frame buffer free command lists collection.");
        return;
    }
    for (size_t commandListIndex = 0; commandListIndex < this->freeGraphicsCommandLists.size(); commandListIndex++)
    {
        auto& commandList = this->freeGraphicsCommandLists[commandListIndex];
        if (FINJIN_CHECK_HRESULT_FAILED(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, this->commandAllocator.Get(), nullptr, IID_PPV_ARGS(&commandList.commandList))))
        {
            FINJIN_SET_ERROR(error, "Failed to create frame buffer command list.");
            return;
        }
        FINJIN_D3D12_SET_RESOURCE_NAME(this->commandAllocator, Utf8StringFormatter::Format("D3D12 frame buffer %1% graphics command list %2%", this->index, commandListIndex));

        commandList.commandList->Close();
    }
    ResetCommandLists();

    if (!this->graphicsCommandListsToExecute.CreateEmpty(maxGpuCommandListsPerStage, allocator, nullptr))
    {
        FINJIN_SET_ERROR(error, "Failed to create frame buffer 'graphics command lists to execute' collection.");
        return;
    }

    if (!this->plainGraphicsCommandLists.CreateEmpty(maxGpuCommandListsPerStage, allocator, nullptr))
    {
        FINJIN_SET_ERROR(error, "Failed to create frame buffer plain graphics command lists collection.");
        return;
    }
}

D3D12FrameBuffer::GraphicsCommandList* D3D12FrameBuffer::NewGraphicsCommandList()
{
    //Clear command allocator if it's the first command list
    if (this->freeGraphicsCommandLists.empty())
    {
        if (FINJIN_CHECK_HRESULT_FAILED(this->commandAllocator->Reset()))
            return nullptr;
    }

    if (!this->freeGraphicsCommandLists.push_back())
        return nullptr;

    auto& commandList = this->freeGraphicsCommandLists.back();
    this->graphicsCommandListsToExecute.push_back(&commandList);

    if (FINJIN_CHECK_HRESULT_FAILED(commandList.commandList->Reset(this->commandAllocator.Get(), nullptr)))
        return nullptr;

    return &commandList;
}

D3D12FrameBuffer::GraphicsCommandList* D3D12FrameBuffer::GetCurrentGraphicsCommandList()
{
    return !this->graphicsCommandListsToExecute.empty() ? this->graphicsCommandListsToExecute.back() : nullptr;
}

void D3D12FrameBuffer::ExecuteCommandLists(ID3D12CommandQueue* queue, uint64_t fenceValue)
{
    if (this->commandListCommitIndex < this->graphicsCommandListsToExecute.size())
    {
        this->plainGraphicsCommandLists.clear();
        for (; this->commandListCommitIndex < this->graphicsCommandListsToExecute.size(); this->commandListCommitIndex++)
        {
            auto commandList = this->graphicsCommandListsToExecute[this->commandListCommitIndex];

            this->plainGraphicsCommandLists.push_back(commandList->commandList.Get());
            commandList->fenceValue = fenceValue;
        }

        auto commandLists = reinterpret_cast<ID3D12CommandList**>(this->plainGraphicsCommandLists.data());
        queue->ExecuteCommandLists(static_cast<UINT>(this->plainGraphicsCommandLists.size()), commandLists);
    }
}

void D3D12FrameBuffer::WaitForCommandLists(ID3D12Fence* fence)
{
    for (; this->commandListWaitIndex < this->commandListCommitIndex; this->commandListWaitIndex++)
    {
        auto commandList = this->graphicsCommandListsToExecute[this->commandListWaitIndex];
        while (fence->GetCompletedValue() < commandList->fenceValue)
        {
            //Busy wait
        }
        commandList->fenceValue = 0;
    }
}

void D3D12FrameBuffer::ResetCommandLists()
{
    this->freeGraphicsCommandLists.clear();
    this->graphicsCommandListsToExecute.clear();

    this->commandListWaitIndex = 0;
    this->commandListCommitIndex = 0;
}

void D3D12FrameBuffer::WaitForNotInUse()
{
    while (!this->graphicsCommandListsToExecute.empty())
    {
    }
}

#endif
