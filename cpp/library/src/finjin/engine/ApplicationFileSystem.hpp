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
#include "finjin/common/Utf8String.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    enum class ApplicationFileSystem
    {
        READ_APPLICATION_ASSETS, //Application-specific read-only assets. Incorporates static assets bundled with executable and possibly non-user-specific application directories.
        READ_USER_DATA, //Non-application-specific "global" directories for the current user. For example: "C:\Users\Someone\Documents" (along with other user directories)

        READ_WRITE_USER_APPLICATION_CACHE_DATA, //Application-specific temporary data directory for the current user.
        READ_WRITE_APPLICATION_DATA, //Application-specific directory for the all users. For example: "C:\ProgramData\MyProduct"
        READ_WRITE_USER_APPLICATION_DATA, //User and application-specific directory for the current user. For example: "C:\Users\Someone\Documents\MyProduct"

        COUNT
    };

    static_assert(EngineConstants::MAX_FILE_SYSTEMS >= (size_t)ApplicationFileSystem::COUNT, "Invalid EngineConstants::MAX_FILE_SYSTEMS value.");

    struct ApplicationFileSystemUtilities
    {
        static const char* ToString(ApplicationFileSystem value)
        {
            switch (value)
            {
                case ApplicationFileSystem::READ_APPLICATION_ASSETS: return "application-assets";
                case ApplicationFileSystem::READ_USER_DATA: return "user-data";
                case ApplicationFileSystem::READ_WRITE_USER_APPLICATION_CACHE_DATA: return "user-application-cache-data";
                case ApplicationFileSystem::READ_WRITE_APPLICATION_DATA: return "application-data";
                case ApplicationFileSystem::READ_WRITE_USER_APPLICATION_DATA: return "user-application-data";
                default: return "<unknown>";
            }
        }

        template <typename T>
        static void Parse(ApplicationFileSystem& value, const T& s, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            value = Parse(s, ApplicationFileSystem::COUNT);
            if (value == ApplicationFileSystem::COUNT)
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to parse streaming file format. Invalid value in '%1%'.", s));
        }

        template <typename T>
        static ApplicationFileSystem Parse(const T& s, ApplicationFileSystem defaultValue)
        {
            if (s == "application-assets")
                return ApplicationFileSystem::READ_APPLICATION_ASSETS;
            else if (s == "user-data")
                return ApplicationFileSystem::READ_USER_DATA;
            else if (s == "user-application-cache-data")
                return ApplicationFileSystem::READ_WRITE_USER_APPLICATION_CACHE_DATA;
            else if (s == "application-data")
                return ApplicationFileSystem::READ_WRITE_APPLICATION_DATA;
            else if (s == "user-application-data")
                return ApplicationFileSystem::READ_WRITE_USER_APPLICATION_DATA;
            else
                return defaultValue;
        }
    };

} }
