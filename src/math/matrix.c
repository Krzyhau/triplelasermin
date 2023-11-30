#include "matrix.h"

#include <math.h>

void mat_identity(matrix_t* out)
{
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            out->m[i][j] = (i == j) ? 1.0f : 0.0f;
        }
    }
}

void mat_translate(const vector_t v, matrix_t* out)
{
    mat_identity(out);

    out->m[0][3] = v.x;
    out->m[1][3] = v.y;
    out->m[2][3] = v.z;
}

void mat_rotate(const quaternion_t q, matrix_t* out)
{
    mat_identity(out);

    float xx = q.x * (q.x + q.x);
    float xy = q.x * (q.y + q.y);
    float xz = q.x * (q.z + q.z);

    float yy = q.y * (q.y + q.y);
    float yz = q.y * (q.z + q.z);
    float zz = q.z * (q.z + q.z);

    float wx = q.w * (q.x + q.x);
    float wy = q.w * (q.y + q.y);
    float wz = q.w * (q.z + q.z);

    out->m[0][0] = 1.0f - (yy + zz);
    out->m[1][0] = xy - wz;
    out->m[2][0] = xz + wy;

    out->m[0][1] = xy + wz;
    out->m[1][1] = 1.0f - (xx + zz);
    out->m[2][1] = yz - wx;

    out->m[0][2] = xz - wy;
    out->m[1][2] = yz + wx;
    out->m[2][2] = 1.0f - (xx + yy);
}

void mat_perspective(float fov, float aspectRatio, float near, float far, matrix_t* out)
{
    mat_identity(out);

    float fovRad = DEG2RAD(fov);
    float tanHalfFOV = tanf(fovRad * 0.5f);

    out->m[0][0] = 1.0f / (aspectRatio * tanHalfFOV);
    out->m[1][1] = 1.0f / tanHalfFOV;
    out->m[2][2] = -(far + near) / (far - near);
    out->m[3][2] = -1.0f;
    out->m[2][3] = -(2.0f * far * near) / (far - near);
    out->m[3][3] = 0.0f;
}

void mat_mul(const matrix_t a, const matrix_t b, matrix_t* out)
{
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            out->m[i][j] = 0.0f;
            for (int k = 0; k < 4; ++k) {
                out->m[i][j] += a.m[i][k] * b.m[k][j];
            }
        }
    }
}

void mat_transform_point(const matrix_t mat, const vector_t point, vector_t* out)
{
    out->x = mat.m[0][0] * point.x + mat.m[0][1] * point.y + mat.m[0][2] * point.z + mat.m[0][3];
    out->y = mat.m[1][0] * point.x + mat.m[1][1] * point.y + mat.m[1][2] * point.z + mat.m[1][3];
    out->z = mat.m[2][0] * point.x + mat.m[2][1] * point.y + mat.m[2][2] * point.z + mat.m[2][3];
    out->w = mat.m[3][0] * point.x + mat.m[3][1] * point.y + mat.m[3][2] * point.z + mat.m[3][3];
}
