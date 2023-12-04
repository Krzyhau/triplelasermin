#include "portal.h"

#include <math.h>
#include "world.h"
#include "render/render.h"

void portal_update(struct Object* obj, struct WindowHandler* window) {

}

void portal_draw(struct Object* obj, struct RenderBatch* batch, int room) {
    if (room != obj->currentRoom) return;

    vector_t portalplane;
    transform_forward(obj->transform, &portalplane);
    vector_t pos = obj->transform.position;
    portalplane.w = pos.x * portalplane.x + pos.y * portalplane.y + pos.z * portalplane.z;

    // don't render portal at all if we're on the other side of it
    vector_t cp = batch->camera->transform.position;
    float dist = portalplane.x * cp.x + portalplane.y * cp.y + portalplane.z * cp.z - portalplane.w;
    if (dist >= 0.0f) return;

    struct Portal* portal = obj->data;

    vector_t upVec = { 0 };
    vector_t rightVec = { 0 };
    transform_up(obj->transform, &upVec);
    transform_right(obj->transform, &rightVec);

    vector_t portalPoints[PORTAL_RESOLUTION];
    for (int i = 0; i < PORTAL_RESOLUTION; i++) {
        vector_t upPart, rightPart;

        float ang = 2.0f * M_PI * ((float)i / (float)PORTAL_RESOLUTION);

        float xDist = PORTAL_WIDTH * 0.5f * cosf(ang);
        float yDist = PORTAL_HEIGHT * 0.5f * sinf(ang);

        vector_scale(rightVec, xDist, &rightPart);
        vector_scale(upVec, yDist, &upPart);
        vector_add(rightPart, upPart, &portalPoints[i]);
        vector_add(portalPoints[i], obj->transform.position, &portalPoints[i]);
    }

    for (int i = 0; i < PORTAL_RESOLUTION; i++) {
        vector_t p1 = portalPoints[i];
        vector_t p2 = portalPoints[(i + 1) % PORTAL_RESOLUTION];
        color_t color = portal->color;
        struct RenderData renderData = {
            .color = color,
            .line = {p1, p2},
        };
        render_batch_add_data(batch, renderData);
    }

    if (portal->linked != NULL && portal->currentDepth < PORTAL_MAX_DEPTH) {

        struct WorldPortalData worldPortal = {
            .roomFrom = portal->object.currentRoom,
            .roomTo = portal->linked->object.currentRoom,
            .verticesCount = PORTAL_RESOLUTION,
            .vertices = portalPoints,
            .plane = portalplane,
        };

        matrix_t portalMatrix;
        portal_passage_matrix(portal, &portalMatrix);
        struct Camera portalCamera = *batch->camera;
        transform_apply_matrix(portalCamera.transform, portalMatrix, &portalCamera.transform);

        portal->currentDepth++;
        world_render_custom(obj->world, batch->display, portalCamera, &worldPortal);
        portal->currentDepth--;
    }
}

void portal_init(struct Portal* portal, struct World* world, struct Portal* linked, color_t color)
{
    portal->linked = linked;
    portal->color = color;
    portal->currentDepth = 0;
    object_init(&portal->object, portal, world, portal_update, portal_draw);
}

void portal_passage_matrix(struct Portal* portal, matrix_t* out)
{
    mat_identity(out);
    if (portal->linked == NULL) {
        return;
    }

    matrix_t worldToLocalIn, rotateYMat, localToWorldOut;

    transform_world_to_local_matrix(portal->object.transform, &worldToLocalIn);
    transform_local_to_world_matrix(portal->linked->object.transform, &localToWorldOut);
    quaternion_t rotateY;
    quaternion_axis_angle(180.0f, (vector_t) { 0.0f, 1.0f, 0.0f }, & rotateY);
    mat_rotate(rotateY, &rotateYMat);

    matrix_t temp;
    mat_mul(localToWorldOut, rotateYMat, &temp);
    mat_mul(temp, worldToLocalIn, out);

}
