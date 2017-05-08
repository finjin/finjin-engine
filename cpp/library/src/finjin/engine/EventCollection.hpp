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
#include "finjin/common/BitArray.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    /**
     * An abstract collection of events, with additional (and optional) time-based
     * event throttling, which is useful if you wish to space out event generation.
     * Note that this throttling is not enforced in any way - it is up to derived classes
     * to enforce a throttling policy
     *
     * In addition to overriding the required methods, derived classes may wish
     * to implement a ResetState() method that allows all events and their associated
     * state to be reset. This is typically useful during development, when not all
     * events are checked/removed during an update - in this case ResetState() would
     * be called after the limited event checking code
     */
    class EventCollection
    {
    public:
        using EventID = int;

        EventCollection()
        {
            this->canAddActions = true;
        }

        virtual ~EventCollection()
        {
        }

        /**
         * Adds an event to the collection.
         * The implementation should take care to record the event only once.
         */
        virtual void Add(EventID eventID) = 0;

        /** Removes an event from the collection */
        virtual void Remove(EventID eventID) = 0;

        /** Determines if the collection has any events */
        virtual bool ContainsAny() const = 0;

        /** Determines if the collection has the specified event */
        virtual bool Contains(EventID eventID) const = 0;

        /**
         * Removes the event if present, and returns whether the event was removed
         * Most of the time this will be called to simultaneously check and clear
         * an event.
         */
        virtual bool Consume(EventID eventID)
        {
            auto contains = Contains(eventID);
            if (contains)
                Remove(eventID);
            return contains;
        }

    protected:
        /** Indicates whether events can be added. Use of this during Add() is optional */
        bool canAddActions;
    };

    template <uint32_t bitCount = 64>
    class EventCollectionFlags : public EventCollection
    {
    public:
        EventCollectionFlags()
        {
        }

        void ClearFlags()
        {
            this->bits.clear();
        }

        void Add(EventID eventID) override
        {
            if (this->canAddActions)
                this->bits.SetBit(eventID);
        }

        void Remove(EventID eventID) override
        {
            this->bits.ClearBit(eventID);
        }

        bool ContainsAny() const override
        {
            return this->bits.IsAnyBitSet();
        }

        bool Contains(EventID eventID) const override
        {
            return this->bits.IsBitSet(eventID);
        }

    private:
        Finjin::Common::BitArray<bitCount> bits;
    };

} }
