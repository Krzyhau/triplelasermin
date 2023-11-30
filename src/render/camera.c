#include "camera.h"

void camera_init(struct Camera* camera)
{
    camera->transform.position = (vector_t){ 0.0f, 0.0f, 0.0f };
    quaternion_identity(&camera->transform.rotation);
    camera->fieldOfView = 90.0f;
    camera->aspectRatio = 1.0f;
    camera->nearPlane = 0.01f;
    camera->farPlane = 1000.0f;
}

void camera_projection_matrix(struct Camera* camera, matrix_t* out)
{
    mat_perspective(camera->fieldOfView, camera->aspectRatio, camera->nearPlane, camera->farPlane, out);
}
