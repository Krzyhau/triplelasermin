#ifndef __VECTOR_H__
#define __VECTOR_H__

typedef struct{
    float x;
    float y;
    float z;
    float w;
} vector_t;

void vector_add(const vector_t a, const vector_t b, vector_t* out);
void vector_sub(const vector_t a, const vector_t b, vector_t* out);
void vector_scale(const vector_t v, const float scale, vector_t* out);
void vector_cross(const vector_t a, const vector_t b, vector_t* out);
float vector_dot(const vector_t a, const vector_t b);
float vector_len(const vector_t v);
void vector_norm(const vector_t v, vector_t* out);

#endif
