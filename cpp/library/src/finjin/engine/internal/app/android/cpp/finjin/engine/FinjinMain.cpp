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
#include "finjin/common/NvAndroidNativeAppGlue.h"

using namespace Finjin::Engine;


//Implementation---------------------------------------------------------------
void FinjinMain(ApplicationDelegate* del, android_app* androidApp)
{
    FINJIN_DECLARE_ERROR(error);

    app_dummy();

    auto app = AllocatedClass::NewUnique<Application>(del->GetAllocator(), FINJIN_CALLER_ARGUMENTS, del, androidApp);

    app->Run(error);
    if (error)
    {
        app->ReportError(error);
    }

    app->Destroy();
    app.reset();
}
