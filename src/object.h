#ifndef __OBJECT_H__
#define __OBJECT_H__

#include "math/transform.h"

typedef void (*ObjectUpdateFunc)(struct Object* obj, struct WindowHandler* window);
typedef void (*ObjectRenderFunc)(struct Object* obj, struct RenderBatch* batch, int room);

struct Object {
    transform_t transform;
    vector_t velocity;
    int currentRoom;

    struct World* world;
    void* data;

    ObjectUpdateFunc updateCallback;
    ObjectRenderFunc renderCallback;
};

void object_init(struct Object* obj, void* data, struct World* world, ObjectUpdateFunc updateFunc, ObjectRenderFunc drawFunc);
void object_update(struct Object* obj, struct WindowHandler* window);
void object_render(struct Object* obj, struct RenderBatch* batch, int room);

#endif