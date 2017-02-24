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


//Includes---------------------------------------------------------------------
#include "finjin/common/ForwardAllocator.hpp"
#include "finjin/common/IntrusiveList.hpp"


//Classes----------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    template <typename ObjectType, typename MutexType>
    class PerFrameObjectAllocator : public ForwardAllocator<MutexType>
    {
    public:
        using Super = ForwardAllocator<MutexType>;
        
        using ListType = IntrusiveSingleList<ObjectType, NextAccessor<ObjectType> >;
        using iterator = typename ListType::iterator;
        using const_iterator = typename ListType::const_iterator;

        const_iterator begin() const { return this->commands.begin(); }
        iterator begin() { return this->commands.begin(); }

        const_iterator end() const { return this->commands.end(); }
        iterator end() { return this->commands.end(); }

        void clear()
        {
            this->commands.clear();
            Super::DeallocateAll();
        }

        template <typename T>
        size_t GetObjectSize() const
        {
            return AlignSizeUp(sizeof(T), Super::GetAlignment());
        }

        template <typename T>
        size_t ByteCountToObjectCount(size_t byteCount) const
        {
            return byteCount / GetObjectSize<T>();
        }
        
        template <typename T>
        T* NewObject(FINJIN_CALLER_PARAMETERS_DECLARATION)
        {
            auto commandMem = Super::Allocate(sizeof(T), FINJIN_CALLER_PARAMETERS);
            if (commandMem == nullptr)
                return nullptr;

            return new (commandMem) T();
        }

    protected:
        ListType commands;
    };

} }
