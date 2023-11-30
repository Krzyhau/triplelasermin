#ifndef __QUATERNION_H__
#define __QUATERNION_H__

#define M_PI (3.14159265358979323846264338327950288f)
#define DEG2RAD(x) ((x) * M_PI / 180.0f);
#define RAD2DEG(x) ((x) * 180.0f / M_PI);

#include "vector.h"

typedef struct {
    float w, x, y, z;
} quaternion_t;

void quaternion_identity(quaternion_t* out);
void quaternion_axis_angle(float angle, const vector_t axis, quaternion_t* out);
void quaternion_inverse(const quaternion_t in, quaternion_t* out);
void quaternion_conjugate(const quaternion_t in, quaternion_t* out);
void quaternion_multiply(const quaternion_t a, const quaternion_t b, quaternion_t* out);
void quaternion_rotate(const quaternion_t in, const vector_t v, vector_t* out);

#endif