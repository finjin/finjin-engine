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
#include "finjin/common/Math.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    /** Base sound sorting class */
    template <typename SoundSource>
    class SoundSorter
    {
    public:
        SoundSorter() {}
        virtual ~SoundSorter() {}

        /** Updates the sorter. This is only called when the sorter is selected in the sound system */
        virtual void SetListener(const MathVector3& listener) {}

        /**
         * The sorting function
         * @param s1 [in] - Sound source 1
         * @param s2 [in] - Sound source 2
         * @return Returns true if s1 is less than s2, otherwise false
         */
        virtual bool operator () (const SoundSource* s1, const SoundSource* s2) const = 0;
    };

    /**
     * Functor class that sorts two sound sources based on their priority, 2D-ness,
     * and distance from listener
     */
    template <typename SoundSource>
    class PriorityListenerDistanceSoundSorter : public SoundSorter<SoundSource>
    {
    public:
        PriorityListenerDistanceSoundSorter()
        {
            this->listenerPosition = MathVector3::Zero();
        }

        void SetListener(const MathVector3& position) override
        {
            this->listenerPosition = position;
        }

        bool operator () (const SoundSource* s1, const SoundSource* s2) const override
        {
            //Note: 0 = higher priority than 1

            if (s1->GetPriority() < s2->GetPriority())
                return true;
            else if (s1->GetPriority() > s2->GetPriority())
                return false;
            else
            {
                //Same priority
                if (s1->IsAmbient() && s2->IsAmbient())
                {
                    //Both sounds are 2D
                    return s1->GetBufferCreateTimeStamp() < s2->GetBufferCreateTimeStamp();
                }
                else if (s1->IsAmbient())
                {
                    //s1 is 2D, s2 is 3D
                    return true;
                }
                else if (s2->IsAmbient())
                {
                    //s1 is 3D, s2 is 2D
                    return false;
                }
                else
                {
                    //Both sounds are 3D
                    return CloserToListener(s1, s2);
                }
            }
        }

    private:
        /** Determines if s1 is closer to the listener than s2, false otherwise */
        bool CloserToListener(const SoundSource* s1, const SoundSource* s2) const
        {
            MathVector3 pos1, pos2;

            GetRelativePosition(s1, pos1);
            GetRelativePosition(s2, pos2);

            //Use squared distance to determine who's closer
            return pos1.dot(pos1) < pos2.dot(pos2);
        }

        void GetRelativePosition(const SoundSource* s, MathVector3& pos) const
        {
            s->GetPosition(pos);

            //Make position relative to listener
            pos -= this->listenerPosition;
        }

    private:
        MathVector3 listenerPosition;
    };

    template <typename SoundSource>
    class SoundSorterProxy
    {
    public:
        SoundSorterProxy(SoundSorter<SoundSource>* soundSorter) {this->soundSorter = soundSorter;}
        bool operator () (const SoundSource* s1, const SoundSource* s2) const {return (*this->soundSorter)(s1, s2);}

    private:
        SoundSorter<SoundSource>* soundSorter;
    };

} }
