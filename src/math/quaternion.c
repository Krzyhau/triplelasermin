#include "quaternion.h"

#include <math.h>
#include "matrix.h"

void quaternion_identity(quaternion_t* out) {
    out->w = 1.0f;
    out->x = 0.0f;
    out->y = 0.0f;
    out->z = 0.0f;
}

void quaternion_axis_angle(float angle, const vector_t axis, quaternion_t* out)
{
    float angleRad = DEG2RAD(angle);
    float s = sinf(angleRad / 2.0f) / vector_len(axis);

    out->x = axis.x * s;
    out->y = axis.y * s;
    out->z = axis.z * s;
    out->w = cosf(angleRad / 2.0f);
}

void quaternion_inverse(const quaternion_t in, quaternion_t* out)
{
    float norm = in.w * in.w + in.x * in.x + in.y * in.y + in.z * in.z;

    if (norm > 0.0) {
        float invNorm = 1.0 / norm;
        out->w = in.w * invNorm;
        out->x = -in.x * invNorm;
        out->y = -in.y * invNorm;
        out->z = -in.z * invNorm;
    }
    else {
        quaternion_identity(out);
    }
}

void quaternion_conjugate(const quaternion_t in, quaternion_t* out)
{
    out->w = in.w;
    out->x = -in.x;
    out->y = -in.y;
    out->z = -in.z;
}

void quaternion_multiply(const quaternion_t a, const quaternion_t b, quaternion_t* out)
{
    out->w = a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z;
    out->x = a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y;
    out->y = a.w * b.y - a.x * b.z + a.y * b.w + a.z * b.x;
    out->z = a.w * b.z + a.x * b.y - a.y * b.x + a.z * b.w;
}

void quaternion_rotate(const quaternion_t q, const vector_t v, vector_t* out) {
    float xx = q.x * (q.x + q.x);
    float xy = q.x * (q.y + q.y);
    float xz = q.x * (q.z + q.z);

    float yy = q.y * (q.y + q.y);
    float yz = q.y * (q.z + q.z);
    float zz = q.z * (q.z + q.z);

    float wx = q.w * (q.x + q.x);
    float wy = q.w * (q.y + q.y);
    float wz = q.w * (q.z + q.z);

    out->x = (1.0f - (yy + zz)) * v.x + (xy + wz) * v.y + (xz - wy) * v.z;
    out->y = (xy - wz) * v.x + (1.0f - (xx + zz)) * v.y + (yz + wx) * v.z;
    out->z = (xz + wy) * v.x + (yz - wx) * v.y + (1.0f - (xx + yy)) * v.z;
}