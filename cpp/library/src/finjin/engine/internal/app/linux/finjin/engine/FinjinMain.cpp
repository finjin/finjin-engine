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
#include "finjin/common/Error.hpp"
#include "finjin/engine/ApplicationDelegate.hpp"
#include "finjin/engine/Application.hpp"

using namespace Finjin::Engine;


//Implementation---------------------------------------------------------------
int FinjinMain(ApplicationDelegate* del, int argc, char* argv[])
{
    auto app = AllocatedClass::NewUnique<Application>(del->GetAllocator(), FINJIN_CALLER_ARGUMENTS, del);
    
    CommandLineArgsProcessor argsProcessor(del->GetAllocator());
    argsProcessor.Create(argc - 1);
    for (int i = 1; i < argc; i++)
        argsProcessor[i - 1] = argv[i];
    
    int result = 0;
    FINJIN_DECLARE_ERROR(error);
    app->Run(argsProcessor, error);
    if (error)
    {
        app->ReportError(error);        
        result = 1;
    }
    
    app->Destroy();
    app.reset();

    return result;
}
