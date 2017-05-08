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
#include "finjin/common/DynamicUnorderedMap.hpp"
#include "finjin/common/DynamicVector.hpp"
#include "finjin/common/Error.hpp"
#include "finjin/engine/GenericGpuNumericStructs.hpp"
#include "MetalIncludes.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    class MetalInputFormat
    {
    public:
        MetalInputFormat();

        void Create(const GpuInputFormatStruct& inputFormat, Error& error);

        void SetBufferIndex(size_t index);

        template <typename Dest, typename Source>
        static void CreateUnorderedMap(Dest& dest, Allocator* allocator, Source& src, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            if (!dest.Create(src.size(), src.size(), allocator))
            {
                FINJIN_SET_ERROR(error, "Failed to allocate outer Metal input format collection.");
                return;
            }

            for (auto& srcItem : src)
            {
                auto destItem = dest.GetOrAdd(srcItem.GetTypeName().GetHash());
                if (destItem.HasErrorOrValue(nullptr))
                {
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to add element '%1%'.", srcItem.GetTypeName()));
                    return;
                }
                destItem.value->Create(srcItem, error);
                if (error)
                {
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create the '%1%' input format'.", srcItem.GetTypeName()));
                    return;
                }
            }
        }

    public:
        size_t inputFormatHash;
        size_t inputFormatElementCount;

        MTLVertexDescriptor* vertexDescriptor;
    };

} }
