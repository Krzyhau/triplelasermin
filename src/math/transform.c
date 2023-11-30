#include "transform.h"

#include "matrix.h"

void transform_forward(const transform_t transform, vector_t* out)
{
    quaternion_rotate(transform.rotation, (vector_t) { 0.0f, 0.0f, 1.0f }, out);
}

void transform_right(const transform_t transform, vector_t* out)
{
    quaternion_rotate(transform.rotation, (vector_t) { 1.0f, 0.0f, 0.0f }, out);
}

void transform_up(const transform_t transform, vector_t* out)
{
    quaternion_rotate(transform.rotation, (vector_t) { 0.0f, 1.0f, 0.0f }, out);
}

void transform_world_to_local_matrix(const transform_t transform, matrix_t* out)
{
    vector_t pos_inv;
    quaternion_t rot_inv;

    vector_scale(transform.position, -1.0f, &pos_inv);
    quaternion_inverse(transform.rotation, &rot_inv);

    matrix_t translate_matrix;
    matrix_t rotate_matrix;
    mat_translate(pos_inv, &translate_matrix);
    mat_rotate(rot_inv, &rotate_matrix);

    mat_mul(rotate_matrix, translate_matrix, out);
}

void transform_local_to_world_matrix(const transform_t transform, matrix_t* out)
{
    matrix_t translate_matrix;
    matrix_t rotate_matrix;
    mat_translate(transform.position, &translate_matrix);
    mat_rotate(transform.rotation, &rotate_matrix);

    mat_mul(rotate_matrix, translate_matrix, out);
}
