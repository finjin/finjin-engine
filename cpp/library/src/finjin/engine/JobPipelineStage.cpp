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
#include "JobPipelineStage.hpp"

using namespace Finjin::Engine;


//Implementation----------------------------------------------------------------
JobPipelineStage::JobPipelineStage()
{
    this->index = 0;
}

JobPipelineStage::~JobPipelineStage()
{
}

size_t JobPipelineStage::GetIndex() const
{
    return this->index;
}

bool JobPipelineStage::IsStarted() const
{
    return this->simulateAndRenderFuture.valid();
}

void JobPipelineStage::Start(future<void>&& f)
{
    this->simulateAndRenderFuture = std::move(f);
}

void JobPipelineStage::Finish(Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    try
    {
        this->simulateAndRenderFuture.get();
    }
    catch (...)
    {
        FINJIN_SET_ERROR(error, "An exception was thrown while getting simulation/render future.");
    }
}
