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
#include "InputConstants.hpp"

using namespace Finjin::Engine;


//Local functions--------------------------------------------------------------
static void RemoveLeadingManufacturerDuplicate(Utf8String& productName)
{
    auto spaceFoundAt = productName.find(' ');
    if (spaceFoundAt != Utf8String::npos)
    {
        Utf8String maybeManufacturer;
        productName.substr(maybeManufacturer, 0, spaceFoundAt);
        
        auto maybeManufacturerDuplicate = maybeManufacturer;
        maybeManufacturerDuplicate += " ";
        maybeManufacturerDuplicate += maybeManufacturer;

        productName.ReplaceAll(maybeManufacturerDuplicate, maybeManufacturer);
    }
}


//Implementation---------------------------------------------------------------
Utf8String InputDeviceUtilities::MakeInputDeviceIdentifier(const Utf8String& identifierString)
{
    Utf8String safe;

    int consecutiveSpaces = 0;
    for (auto c : identifierString)
    {
        if (isascii(c))
        {
            if (c == ' ')
            {
                if (++consecutiveSpaces == 1)
                    safe.append(static_cast<char>(c));
            }
            else if (isalnum(c) || c == '_' || c == '.' || c == ')' || c == '(')
            {
                safe.append(static_cast<char>(tolower(c)));
                consecutiveSpaces = 0;
            }
            else
                consecutiveSpaces = 0;
        }
        else
            consecutiveSpaces = 0;
    }

    safe.ReplaceAll("(r)", "");
    safe.ReplaceAll('.', '-');
    safe.ReplaceAll(' ', '-');
    
    RemoveLeadingManufacturerDuplicate(safe);

    return safe;
}
