#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "../math/vector.h"
#include "../math/quaternion.h"
#include "../math/matrix.h"
#include "../math/transform.h"

struct Camera {
    transform_t transform;
    float fieldOfView;
    float aspectRatio;
    float nearPlane;
    float farPlane;
};


void camera_init(struct Camera* camera);
void camera_projection_matrix(struct Camera* camera, matrix_t* out);

#endif
