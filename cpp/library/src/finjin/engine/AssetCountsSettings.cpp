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
#include "finjin/engine/AssetCountsSettings.hpp"
#include "finjin/common/ConfigDocumentReader.hpp"
#include "finjin/common/Convert.hpp"

using namespace Finjin::Engine;


//Implementation---------------------------------------------------------------
AssetCountsSettings::AssetCountsSettings()
{
    for (auto& item : this->countsByClass)
        item = 0;
}

void AssetCountsSettings::ParseSettings(const ByteBufferReader& configFileBuffer, const Path& assetFilePath, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    Utf8StringView section, key, value;

    ConfigDocumentReader reader;
    for (auto line = reader.Start(configFileBuffer); line != nullptr; line = reader.Next())
    {
        switch (line->GetType())
        {
            case ConfigDocumentLine::Type::KEY_AND_VALUE:
            {
                line->GetKeyAndValue(key, value);

                auto assetClass = AssetClassUtilities::Parse(key);
                if (assetClass == AssetClass::COUNT)
                {
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Invalid asset class '%1%'.", key.ToString()));
                    return;
                }
                
                this->countsByClass[assetClass] = Convert::ToInteger(value.ToString(), this->countsByClass[assetClass]);
                
                break;
            }
            default: break;
        }
    }
}
