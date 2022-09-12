/*
 * INFO:
 * 
 *  C/C++ (single) header quaternion based 3D camera system
 *   for games and other 3D graphics applications.
 *
 * 
 * FEATURES:
 * 
 *  - Quaternion based
 *     This naturaly avoids gimbal lock and enables smooth interpolation (ex. for cinematic camera movement)
 *  - Precise manipulation
 *     A call of 'camera_rotate(&camera, {45 * DEG_TO_RAD, 0, 0});' will rotate exactly 45 degrees.
 *  - Engine agnostic
 *     No matter your input system, the camera is updated in angles
 *  - Different camera modes to control the cameras behaviour
 *    - Allows custom configuration
 *    - Can be changed at runtime
 *    - See CAMERA_MODE_* defines for more
 *    - Pre-defined modes: CAMERA_MODE_FREE, CAMERA_MODE_FIRST_PERSON, CAMERA_MODE_THIRD_PERSON, CAMERA_MODE_ORBITAL
 *  - Supports seamless camera mode transitions (ex. first person -> third person)
 *  - Full access to all camera state data - nothing is hidden
 *     You can access and manipulate the entire camera state at any time!
 *  - Supports angle clamping
 *     Restrict the angles your camera is allowed to work in
 * 
 * 
 * USAGE:
 * 
 *  Simply add 'camera.h' and one 'camera_math.h' to your project and '#include "camera.h"' it wherever.
 *
 *  ONE (and only ONE) source file must hold the implementation
 *   by using '#define CAMERA_IMPLEMENTATION' before including it with '#include "camera.h"'.
 *
 *  Camera code is math heavy. For all required math functions we provide a separate 'camera_math.h' file.
 *  To get started you can use the 'camera_math_default.h'.
 *  Simply add it to your project and rename it to 'camera_math.h'.
 * 
 *  But your engine probably brings its own math library.
 *  In order to reduce the duplication of math-code you can create your own
 *   'camera_math.h' implementing the required functions with the math library of your choice.
 *  This also allowes you to pass and receive arguments without the need to convert them.
 *  See 'camera_math_bx.h' as an example.
 * 
 * 
 * ANGLE CLAMPING:
 * 
 *  If angle clamping is activated, the corresponding limits must be set in the camera struct.
 *  All limits are expected in radians and min* must be smaller than max*.
 *  They are expected in the range [-pi; pi], with 0 representing no rotation.
 *  The camera rotations are restricting in WORLD space.
 *  This means if pitch AND yaw are clamped, this essentially creates a "window" the camera is not allowed to rotate out of.
 *  
 *  Example for clamping pitch:
 *   1. Activate pitch clamping: 'camera.mode |= CAMERA_MODE_CLAMP_PITCH;'
 *   2. Set limits: 'camera.maxPitch = -pi / 2.0f;  // aka restrict to 90 deg upwards'
 *                  'camera.minPitch = pi / 2.0f;   // aka restrict to 90 deg downwards'
 * 
 * 
 * GENERAL NOTES:
 * 
 *  ALL camera struct members can be safely manipulated at any time.
 * 
 *  To change to a right-handed coordinate system simply change CAMERA_WORLD_FORWARD to (0.0f, 0.0f, -1.0f)
 * 
 *  CameraQuaternions are well suited to represent 3D orientation, but they do not accumulate
 *   many rotations in differing axies well and thus require occasional re-normalization.
 *   To accomodate for this and for general performance reasons changes are accumulated and the
 *   orientation quaternion is only updated when the view matrix is requested (once per frame).
 * 
 *  Query functions only return the correct value AFTER pending changes have been applied. (i.e. calling camera_view_matrix(..))
 *   Example:
 *   1. camera_move(..)          // Changes NOT yet applied
 *   2. camera_eye_position(..)  // Returned state does NOT yet include previous move
 *   3. camera_view_matrix(..)   // Changes are now applied
 *   4. camera_eye_position(..)  // Returned state DOES now include previous move
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


#ifndef CAMERA_HEADER_GUARD
#define CAMERA_HEADER_GUARD

#include <stdint.h>
#include "camera_math.h"

/* World and mode defines */

#define CAMERA_WORLD_FORWARD                CameraVec3(0.0f, 0.0f, 1.0f)
#define CAMERA_WORLD_UP                     CameraVec3(0.0f, 1.0f, 0.0f)
#define CAMERA_WORLD_RIGHT                  CameraVec3(1.0f, 0.0f, 0.0f)

// Camera mode configuration flags
//  Can be combined with bitwise OR
#define CAMERA_MODE_DISABLE_ROLL            UINT32_C(0x00000001) // Disables the roll axis
#define CAMERA_MODE_MOVE_IN_WORLDPLANE      UINT32_C(0x00000002) // Projects movement onto world plane
#define CAMERA_MODE_CLAMP_PITCH_ANGLE       UINT32_C(0x00000004) // Limits the pitch angle. Typically used in first/third person to prevent overrotation  (i.e. somersaults).
#define CAMERA_MODE_CLAMP_YAW_ANGLE         UINT32_C(0x00000008) // Limits the yaw angle.
#define CAMERA_MODE_CLAMP_ROLL_ANGLE        UINT32_C(0x00000010) // Limits the roll angle.

// Free float camera mode (no restrictions applied)
#define CAMERA_MODE_FREE (0)

// First person camera mode
//  Note: Set camera.minPitch = -pi/2 and camera.maxPitch = pi/2
#define CAMERA_MODE_FIRST_PERSON (0 \
      | CAMERA_MODE_DISABLE_ROLL \
      | CAMERA_MODE_MOVE_IN_WORLDPLANE \
      | CAMERA_MODE_CLAMP_PITCH_ANGLE \
      )

// Third person camera mode
//  Note: Set camera.minPitch = -pi/2 and camera.maxPitch = pi/2
//  Note: Use a target_distance > 0
#define CAMERA_MODE_THIRD_PERSON CAMERA_MODE_FIRST_PERSON

// Orbital camera mode (orbit around some target)
//  Useful for inspecting models.
//  Note: Set camera.minPitch = -pi/2 and camera.maxPitch = pi/2
#define CAMERA_MODE_ORBITAL (0 \
      | CAMERA_MODE_DISABLE_ROLL \
      | CAMERA_MODE_CLAMP_PITCH_ANGLE \
      )


/* Camera struct */

typedef struct camera {
    CameraVec3 target_position;         // The target point, the camera is looking at. Aka camera eye position if camera.target_distance == 0.
    float target_distance;              // Camera distance from eye to target. Note: negative values create zoom-like behaviour.
    CameraQuat orientation;             // Camera rotation in 3D.
    uint32_t mode;                      // Controlls camera behaviour. See CAMERA_MODE_* defines.

    // Temporary accumulator. Cleared on camera_view_matrix(..).
    CameraVec3 movement_accumulator;
    CameraVec3 rotation_accumulator;
    // Angle clamping limits. See "Angle Clamping" for further information.
    float minPitch;
    float maxPitch;
    float minYaw;
    float maxYaw;
    float minRoll;
    float maxRoll;
} Camera;


/* Function declarations */

// Initialize/Reset the camera struct.
extern Camera camera_init();

// Returns the cameras current forward direction (normalized)
extern CameraVec3 camera_forward(const Camera* _cam);

// Returns the cameras current up direction (normalized)
extern CameraVec3 camera_up(const Camera* _cam);

// Returns the cameras current right direction (normalized)
extern CameraVec3 camera_right(const Camera* _cam);

// Returns the cameras current eye position
extern CameraVec3 camera_eye(const Camera* _cam);

// Move the camera in its relative orientation
// _offset == (forward, up, right)
extern void camera_move(Camera* _cam, const CameraVec3 _offset);

// Rotate the camera view
//  _angles = (pitch, yaw, roll)
//  pitch == "Look Up/Down"   yaw == "Look Left/Right"   roll == "Turn head Left/Right"
// Note: angles are expected in radians
extern void camera_rotate(Camera* _cam, const CameraVec3 _angles);

// Rotate the camera to look into the direction _forward
//  This only changes the camera.orientation, it will still face its camera.target_position!
// Note: _forward and _up are expected to be normalized
extern void camera_look_at(Camera* _cam, CameraVec3 _forward, CameraVec3 _up);

// Update the camera and generate a view matrix
// Note: _out_matrix is expected to be a float[16]
extern void camera_view_matrix(Camera* _cam, float* _out_matrix);


#endif // !CAMERA_HEADER_GUARD


#ifdef CAMERA_IMPLEMENTATION

extern Camera camera_init()
{
    static Camera cam = {
        .target_position = cm_init_vec3(0.0f, 0.0f, 0.0f),
        .target_distance = 0.0f,
        .orientation = cm_init_quat(0.0f, 0.0f, 0.0, 0.0f),
        .mode = CAMERA_MODE_FREE,

        .movement_accumulator = cm_init_vec3(0.0f, 0.0f, 0.0f),
        .rotation_accumulator = cm_init_vec3(0.0f, 0.0f, 0.0f),

        .minPitch = 0.0f,
        .maxPitch = 0.0f,
        .minYaw = 0.0f,
        .maxYaw = 0.0f,
        .minRoll = 0.0f,
        .maxRoll = 0.0f,
    };

    return cam;
};

extern CameraVec3 camera_forward(const Camera* _cam)
{
    return cm_mul(CAMERA_WORLD_FORWARD, cm_invert(_cam->orientation));
};

extern CameraVec3 camera_up(const Camera* _cam)
{
    return cm_mul(CAMERA_WORLD_UP, cm_invert(_cam->orientation));
};

extern CameraVec3 camera_right(const Camera* _cam)
{
    return cm_mul(CAMERA_WORLD_RIGHT, cm_invert(_cam->orientation));
};

extern CameraVec3 camera_eye(const Camera* _cam)
{
    return cm_add(_cam->target_position, cm_scale(camera_forward(_cam), -_cam->target_distance));
};

extern void camera_move(Camera* _cam, const CameraVec3 _offset)
{
    _cam->movement_accumulator = cm_add(_cam->movement_accumulator, _offset);
}

extern void camera_rotate(Camera* _cam, const CameraVec3 _angles)
{
    _cam->rotation_accumulator = cm_add(_cam->rotation_accumulator, _angles);
}

extern void camera_look_at(Camera* _cam, CameraVec3 _forward, CameraVec3 _up)
{
    // Based on typical vector to matrix to quaternion approach

    // Get orthogonal basis vectors
    const CameraVec3 right = cm_normalizeVec3(cm_cross(_up, _forward));
    _up = cm_cross(_forward, right);

    // Convert to Quaternion
    // Ref.: https://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/

    const float m0 = right.x;
    const float m1 = right.y;
    const float m2 = right.z;

    const float m4 = _up.x;
    const float m5 = _up.y;
    const float m6 = _up.z;

    const float m8 = _forward.x;
    const float m9 = _forward.y;
    const float m10 = _forward.z;

    float trace = m0 + m5 + m10;
    if (trace > 0)
    {
        float s = 0.5f / cm_sqrt(trace + 1.0f);
        _cam->orientation.w = 0.25f / s;
        _cam->orientation.x = (m6 - m9) * s;
        _cam->orientation.y = (m8 - m2) * s;
        _cam->orientation.z = (m1 - m4) * s;
    }
    else {
        if (m0 > m5 && m0 > m10)
        {
            float s = 2.0f * cm_sqrt(1.0f + m0 - m5 - m10);
            _cam->orientation.w = (m6 - m9) / s;
            _cam->orientation.x = 0.25f * s;
            _cam->orientation.y = (m4 + m1) / s;
            _cam->orientation.z = (m8 + m2) / s;
        }
        else if (m5 > m10)
        {
            float s = 2.0f * cm_sqrt(1.0f + m5 - m0 - m10);
            _cam->orientation.w = (m8 - m2) / s;
            _cam->orientation.x = (m4 + m1) / s;
            _cam->orientation.y = 0.25f * s;
            _cam->orientation.z = (m9 + m6) / s;
        }
        else
        {
            float s = 2.0f * cm_sqrt(1.0f + m10 - m0 - m5);
            _cam->orientation.w = (m1 - m4) / s;
            _cam->orientation.x = (m8 + m2) / s;
            _cam->orientation.y = (m9 + m6) / s;
            _cam->orientation.z = 0.25f * s;
        }
    }
}

extern void camera_view_matrix(Camera* _cam, float* _out_matrix)
{
    /* Clamp angles */

    if (_cam->mode & (CAMERA_MODE_CLAMP_PITCH_ANGLE | CAMERA_MODE_CLAMP_YAW_ANGLE | CAMERA_MODE_CLAMP_ROLL_ANGLE))
    {
        const CameraVec3 angles = cm_toEuler(_cam->orientation);

        if (_cam->mode & CAMERA_MODE_CLAMP_PITCH_ANGLE)
        {
            _cam->rotation_accumulator.x = cm_max(_cam->minPitch - angles.x, _cam->rotation_accumulator.x);
            _cam->rotation_accumulator.x = cm_min(_cam->maxPitch - angles.x, _cam->rotation_accumulator.x);
        }

        if (_cam->mode & CAMERA_MODE_CLAMP_YAW_ANGLE)
        {
            _cam->rotation_accumulator.y = cm_max(_cam->minYaw - angles.y, _cam->rotation_accumulator.y);
            _cam->rotation_accumulator.y = cm_min(_cam->maxYaw - angles.y, _cam->rotation_accumulator.y);
        }

        if (_cam->mode & CAMERA_MODE_CLAMP_ROLL_ANGLE)
        {
            _cam->rotation_accumulator.z = cm_max(_cam->minRoll - angles.z, _cam->rotation_accumulator.z);
            _cam->rotation_accumulator.z = cm_min(_cam->maxRoll - angles.z, _cam->rotation_accumulator.z);
        }
    }


    /* Update orientation */

    const CameraQuat pitch = cm_fromAxisAngle(CAMERA_WORLD_RIGHT, _cam->rotation_accumulator.x);
    const CameraQuat yaw = cm_fromAxisAngle(CAMERA_WORLD_UP, _cam->rotation_accumulator.y);

    if (_cam->mode & CAMERA_MODE_DISABLE_ROLL)
    {
        // Note: The multiplication order is important, not to induce roll from pitch+yaw
        _cam->orientation = cm_mulQuat(_cam->orientation, pitch);
        _cam->orientation = cm_mulQuat(yaw, _cam->orientation);
    }
    else
    {
        const CameraQuat roll = cm_fromAxisAngle(CAMERA_WORLD_FORWARD, _cam->rotation_accumulator.z);

        _cam->orientation = cm_mulQuat(_cam->orientation, pitch);
        _cam->orientation = cm_mulQuat(_cam->orientation, yaw);
        _cam->orientation = cm_mulQuat(_cam->orientation, roll);
    }

    _cam->orientation = cm_normalizeQuat(_cam->orientation); // Re-Normalize orientation quaternion

    // Reset accumulator
    _cam->rotation_accumulator.x = 0.0f;
    _cam->rotation_accumulator.y = 0.0f;
    _cam->rotation_accumulator.z = 0.0f;


    /* Update target_position */

    CameraVec3 forward = camera_forward(_cam);
    CameraVec3 up = camera_up(_cam);
    CameraVec3 right = camera_right(_cam);

    if (_cam->mode & CAMERA_MODE_MOVE_IN_WORLDPLANE)
    {
        const float epsilon = 0.0001f; // Avoid floating point errors

        if (forward.y > 1.0f - epsilon) // Note: forward is normalized, so checking .y is sufficent
        { // Special case: Looking straight up
            forward = cm_negate(up);
        }
        else if (forward.y < -1.0f + epsilon)
        { // Special case: Looking straight down
            forward = up;
        }
        else if (right.y > 1.0f - epsilon)
        {
            right = up;
        }
        else if (right.y < -1.0f + epsilon)
        {
            right = cm_negate(up);
        }

        // Project the forward and right into the world plane
        forward.y = 0;
        forward = cm_normalizeVec3(forward);

        right.y = 0;
        right = cm_normalizeVec3(right);

        up = CAMERA_WORLD_UP;
    }

    // Scale by desired distance
    forward = cm_scale(forward, _cam->movement_accumulator.x);
    up = cm_scale(up, _cam->movement_accumulator.y);
    right = cm_scale(right, _cam->movement_accumulator.z);

    // Apply changes to target_position
    _cam->target_position = cm_add(_cam->target_position, forward);
    _cam->target_position = cm_add(_cam->target_position, up);
    _cam->target_position = cm_add(_cam->target_position, right);

    // Reset accumulator
    _cam->movement_accumulator.x = 0.0f;
    _cam->movement_accumulator.y = 0.0f;
    _cam->movement_accumulator.z = 0.0f;


    /* Generate view matrix */

    // Get rotation matrix
    cm_matrixFromQuat(_out_matrix, _cam->orientation);

    // Add translation
    CameraVec3 translation = cm_negate(camera_eye(_cam));

    translation = cm_mul(translation, _cam->orientation);

    _out_matrix[12] = translation.x;
    _out_matrix[13] = translation.y;
    _out_matrix[14] = translation.z;

}

#endif // CAMERA_IMPLEMENTATION
