#ifndef __TRANSFORM_H__
#define __TRANSFORM_H__

#include "vector.h"
#include "quaternion.h"
#include "matrix.h"

typedef struct {
    vector_t position;
    quaternion_t rotation;
} transform_t;

void transform_forward(const transform_t transform, vector_t* out);
void transform_right(const transform_t transform, vector_t* out);
void transform_up(const transform_t transform, vector_t* out);

void transform_world_to_local_matrix(const transform_t transform, matrix_t* out);
void transform_local_to_world_matrix(const transform_t transform, matrix_t* out);

void transform_apply_matrix(const transform_t transform, const matrix_t mat, transform_t* out);

#endif