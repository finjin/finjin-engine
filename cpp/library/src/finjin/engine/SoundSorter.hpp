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


//Classes----------------------------------------------------------------------
namespace Finjin { namespace Engine {
    
    /** Base sound sorting class */
    template <typename SoundSource>
    class SoundSorter
    {
    public:
        SoundSorter() {}
        virtual ~SoundSorter() {}

        /** Updates the sorter. This is only called when the sorter is selected in the sound system */
        virtual void SetListener(float listenerX, float listenerY, float listenerZ) {}

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
            this->listenerPosition[0] = this->listenerPosition[1] = this->listenerPosition[2] = 0;
        }

        void SetListener(float listenerX, float listenerY, float listenerZ) override
        {
            this->listenerPosition[0] = listenerX;
            this->listenerPosition[1] = listenerY;
            this->listenerPosition[2] = listenerZ;
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
            float x1, y1, z1;
            GetRelativePosition(s1, x1, y1, z1);

            float x2, y2, z2;
            GetRelativePosition(s2, x2, y2, z2);

            //Use squared distance to determine who's closer
            return (x1*x1 + y1*y1 + z1*z1) < (x2*x2 + y2*y2 + z2*z2);
        }

        void GetRelativePosition(const SoundSource* s, float& x, float& y, float& z) const
        {
            s->GetPosition(x, y, z);

            //Make position relative to listener
            x -= this->listenerPosition[0];
            y -= this->listenerPosition[1];
            z -= this->listenerPosition[2];
        }
        
    private:
        float listenerPosition[3];
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
