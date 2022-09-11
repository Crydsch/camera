## 3D camera system
C/C++ (single) header, quaternion based, 3D camera system for games and other graphics applications


## Features

 - Quaternion based  
    This naturaly avoids gimbal lock and enables smooth interpolation (ex. for cinematic camera movement)
 - Precise manipulation  
    A call of `camera_rotate(&camera, {45 * DEG_TO_RAD, 0, 0});` will rotate exactly 45 degrees.
 - Engine agnostic  
    No matter your input system, the camera is updated in angles
 - Different camera modes to control the cameras behaviour  
   - Allows custom configuration
   - Can be changed at runtime
   - See CAMERA_MODE_* defines for more
   - Pre-defined modes:  
     `CAMERA_MODE_FREE`, `CAMERA_MODE_FIRST_PERSON`, `CAMERA_MODE_THIRD_PERSON`, `CAMERA_MODE_ORBITAL`
 - Supports seamless camera mode transitions (ex. first person -> third person)
 - Full access to all camera state data - nothing is hidden  
    You can access and manipulate the entire camera state at any time!
 - Supports angle clamping  
    Restrict the angles your camera is allowed to work in


## Usage
Simply add `camera.h` and one `camera_math.h` to your project and `#include "camera.h"` it wherever.

ONE (and only ONE) source file must hold the implementation  
  by using `#define CAMERA_IMPLEMENTATION` before including it with `#include "camera.h"`.

Camera code is math heavy. For all required math functions we provide a separate `camera_math.h` file.  
To get started you can use the `camera_math_default.h`.  
Simply add it to your project and rename it to `camera_math.h`.

But your engine probably brings its own math library.  
In order to reduce the duplication of math-code you can create your own  
 `camera_math.h` implementing the required functions with the math library of your choice.  
This also allowes you to pass and receive arguments without the need to convert them.  
See `camera_math_bx.h` as an example.  


## Angle Clamping

If angle clamping is activated, the corresponding limits must be set in the camera struct.  
All limits are expected in radians and min* should be smaller than max*.  
They are expected in the range `[-pi; pi]`, with `0` representing the angle at rest.  
The camera rotations are restricting in WORLD space.  
This means if pitch AND yaw are clamped, this essentially creates a "window" the camera is not allowed to rotate out of.  

Example for clamping pitch:  
 1. Activate pitch clamping: `camera.mode |= CAMERA_MODE_CLAMP_PITCH;`  
 2. Set limits:  
 `camera.maxPitch = -pi / 2.0f;  // aka restrict to 90 deg upwards`  
 `camera.minPitch = pi / 2.0f;   // aka restrict to 90 deg downwards`  


## General Notes

- ALL camera struct members can be safely manipulated at any time.

- To change to a right-handed coordinate system simply change CAMERA_WORLD_FORWARD to (0.0f, 0.0f, -1.0f)

- CameraQuaternions are well suited to represent 3D orientation, but they do not accumulate  
  many rotations in differing axies well and thus require occasional re-normalization.  
  To accomodate for this and for general performance reasons changes are accumulated and the  
  orientation quaternion is only updated when the view matrix is requested (once per frame).  

  Query functions only return the correct value AFTER pending changes have been applied. (i.e. calling `camera_view_matrix(..)`)  
  Example:  
  1. `camera_move(..)         // Changes NOT yet applied`  
  2. `camera_eye_position(..)  // Returned state does NOT yet include previous move`  
  3. `camera_view_matrix(..)   // Changes are now applied`  
  4. `camera_eye_position(..)  // Returned state DOES now include previous move`  

