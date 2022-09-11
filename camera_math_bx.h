/*
 * INFO:
 * 
 *  This file provides an interface to math functions for camera.h
 * 
 *  Specifically interfacing with bx/math.h
 *  Ref.: https://github.com/bkaradzic/bgfx
 *  Ref.: https://github.com/bkaradzic/bx
 * 
 * 
 * LICENSE:
 * 
 *  MIT License
 * 
 *  Copyright (c) 2022 Crydsch Cube
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 * 
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 */

#include <bx/math.h>

#define CameraVec3 bx::Vec3
#define CameraQuat bx::Quaternion

static inline CameraVec3 cm_init_vec3(float _x, float _y, float _z) {
    return bx::Vec3(_x, _y, _z);
}

static inline CameraQuat cm_init_quat(float _x, float _y, float _z, float _w) {
    return bx::Quaternion(_x, _y, _z, _w);
}

static inline CameraVec3 cm_mul(CameraVec3 _a, CameraQuat _b) {
    return bx::mul(_a, _b);
}

static inline CameraQuat cm_invert(CameraQuat _a) {
    return bx::invert(_a);
}

static inline CameraVec3 cm_add(CameraVec3 _a, CameraVec3 _b) {
    return bx::add(_a, _b);
}

static inline CameraVec3 cm_scale(CameraVec3 _a, float _b) {
    return bx::mul(_a, _b);
}

static inline CameraVec3 cm_cross(CameraVec3 _a, CameraVec3 _b) {
    return bx::cross(_a, _b);
}

static inline float cm_min(float _a, float _b) {
    return bx::min(_a, _b);
}

static inline float cm_max(float _a, float _b) {
    return bx::max(_a, _b);
}

static inline float cm_sqrt(float _a) {
    return bx::sqrt(_a);
}

static inline CameraVec3 cm_toEuler(CameraQuat _a) {
    return bx::toEuler(_a);
}

static inline CameraQuat cm_fromAxisAngle(CameraVec3 _a, float _b) {
    return bx::fromAxisAngle(_a, _b);
}

static inline CameraQuat cm_mulQuat(CameraQuat _a, CameraQuat _b) {
    return bx::mul(_a, _b);
}

static inline CameraQuat cm_normalizeQuat(CameraQuat _a) {
    return bx::normalize(_a);
}

static inline CameraVec3 cm_normalizeVec3(CameraVec3 _a) {
    return bx::normalize(_a);
}

static inline CameraVec3 cm_negate(CameraVec3 _a) {
    return bx::neg(_a);
}

static inline void cm_matrixFromQuat(float* _a, CameraQuat _b) {
    return bx::mtxFromQuaternion(_a, _b);
}
