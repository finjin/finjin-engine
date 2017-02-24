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
#include "finjin/common/Angle.hpp"
#include "finjin/common/Math.hpp"
#include "finjin/engine/CameraState.hpp"


//Classes----------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    class Camera
    {
    public:
        Camera();
        ~Camera();

        const MathVector3& GetPosition() const;
        void SetPosition(float x, float y, float z);
        
        const MathVector3& GetRight() const;
        const MathVector3& GetUp() const;
        const MathVector3& GetLook() const;

        void Set(const CameraState& cameraState);

        void SetBases(const float* right, const float* up, const float* forward);

        float GetNearZ() const;
        void SetNearZ(float value);
        
        float GetFarZ() const;
        void SetFarZ(float value);
        
        float GetAspect() const;
        void SetAspect(float value);
        
        Angle GetFovX() const;
        
        Angle GetFovY() const;
        void SetFovY(Angle angle);

        float GetNearWindowWidth() const;
        float GetNearWindowHeight() const;
        float GetFarWindowWidth() const;
        float GetFarWindowHeight() const;

        void SetLens(Angle fovY, float aspect, float nearZ, float farZ);

        void LookAt(const MathVector3& pos, const MathVector3& target, const MathVector3& up);

        const MathMatrix44& GetViewMatrix() const;
        const MathMatrix44& GetProjectionMatrix() const;

        void Strafe(float d);
        void Pan(float dx, float dy);
        void Walk(float d);

        void Pitch(Angle angle);
        void RotateY(Angle angle);

        //After modifying, call to rebuild the view matrix
        void Update();

    private:
        //Coordinate system relative to world space
        MathVector3 position;
        MathVector3 right;
        MathVector3 up;
        MathVector3 look;

        //Cached frustum properties
        float nearZ;
        float farZ;
        float aspect;
        float fovY;
        float nearWindowHeight;
        float farWindowHeight;

        bool isViewDirty;

        MathMatrix44 viewMatrix;
        MathMatrix44 projectionMatrix;
    };

} }
