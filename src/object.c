#include "object.h"

#include "world.h"
#include "core/window.h"

void object_init(struct Object* obj, void* data, struct World* world, ObjectUpdateFunc updateFunc, ObjectRenderFunc drawFunc)
{
    quaternion_identity(&obj->transform.rotation);
    obj->world = world;
    obj->data = data;
    obj->updateCallback = updateFunc;
    obj->renderCallback = drawFunc;
}

void object_update(struct Object* obj, struct WindowHandler* window)
{
    if (obj->updateCallback == NULL) return;
    obj->updateCallback(obj, window);

    vector_t moveStep;
    vector_scale(obj->velocity, window->deltaTime, &moveStep);
    vector_add(obj->transform.position, moveStep, &obj->transform.position);
    obj->currentRoom = world_get_room_at(obj->world, obj->transform.position);
}

void object_render(struct Object* obj, struct RenderBatch* batch, int room)
{
    if (obj->renderCallback == NULL) return;
    obj->renderCallback(obj, batch, room);
}
