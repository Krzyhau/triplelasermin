#ifndef __MATRIX_H__
#define __MATRIX_H__

#include "vector.h"
#include "quaternion.h"

typedef struct {
    float m[4][4];
} matrix_t;

void mat_identity(matrix_t* out);
void mat_translate(const vector_t v, matrix_t* out);
void mat_rotate(const quaternion_t q, matrix_t* out);
void mat_perspective(float fov, float aspectRatio, float near, float far, matrix_t* out);

void mat_mul(const matrix_t a, const matrix_t b, matrix_t* out);
void mat_transform_point(const matrix_t mat, const vector_t point, vector_t* out);
void mat_to_quaternion(const matrix_t mat, quaternion_t* out);

#endif