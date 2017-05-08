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
#include "finjin/engine/ApplicationDelegate.hpp"
#include "finjin/engine/Application.hpp"
#include <shellapi.h>

using namespace Finjin::Engine;


//Library references------------------------------------------------------------
#if FINJIN_TARGET_GPU_SYSTEM == FINJIN_TARGET_GPU_SYSTEM_D3D12
    #pragma comment(lib, "d3d12")
    #pragma comment(lib, "dxgi")
#endif
#pragma comment(lib, "Comctl32")
#pragma comment(lib, "Rpcrt4")
#pragma comment(lib, "XInput")
#pragma comment(lib, "dinput8")
#pragma comment(lib, "dxguid")
#pragma comment(lib, "xaudio2")
#if FINJIN_TARGET_VR_SYSTEM != FINJIN_TARGET_VR_SYSTEM_NONE
    #pragma comment(lib, "openvr_api")
#endif


//Local functions---------------------------------------------------------------
static void GetCommandLineArgs(CommandLineArgsProcessor& argsProcessor, LPSTR lpCmdLine)
{
    if (lpCmdLine != nullptr && lpCmdLine[0] != 0)
    {
        int argc;
        auto argv = CommandLineToArgvW(GetCommandLineW(), &argc);
        if (argv != nullptr)
        {
            if (argv[0] != 0)
            {
                argsProcessor.Create(argc - 1);
                for (int i = 1; i < argc; i++)
                    argsProcessor[i - 1] = argv[i];
            }

            LocalFree(argv);
        }
    }
}


//Implementation----------------------------------------------------------------
int FinjinMain(ApplicationDelegate* del, HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    auto app = AllocatedClass::NewUnique<Application>(del->GetAllocator(), FINJIN_CALLER_ARGUMENTS, del, hInstance);

    CommandLineArgsProcessor argsProcessor(del->GetAllocator());
    GetCommandLineArgs(argsProcessor, lpCmdLine);

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
