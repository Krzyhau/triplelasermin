#include "vector.h"

#include <math.h>

void vector_add(const vector_t a, const vector_t b, vector_t* out)
{
    out->x = a.x + b.x;
    out->y = a.y + b.y;
    out->z = a.z + b.z;
    out->w = a.w + b.w;
}

void vector_sub(const vector_t a, const vector_t b, vector_t* out)
{
    out->x = a.x - b.x;
    out->y = a.y - b.y;
    out->z = a.z - b.z;
    out->w = a.w - b.w;
}

void vector_scale(const vector_t v, const float scale, vector_t* out)
{
    out->x = v.x * scale;
    out->y = v.y * scale;
    out->z = v.z * scale;
    out->w = v.w * scale;
}

void vector_cross(const vector_t v1, const vector_t v2, vector_t* out)
{
    out->x = v1.y * v2.z - v1.z * v2.y;
    out->y = v1.z * v2.x - v1.x * v2.z;
    out->z = v1.x * v2.y - v1.y * v2.x;
}

float vector_dot(const vector_t v1, const vector_t v2)
{
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w;
}

float vector_len(const vector_t v)
{
    return sqrtf(vector_dot(v, v));
}

void vector_norm(const vector_t v, vector_t* out)
{
    vector_scale(v, 1.0f / vector_len(v), out);
}
