#include "portal.h"

#include <math.h>
#include "world.h"
#include "core/window.h"
#include "render/render.h"

void portal_update(struct Object* obj, struct WindowHandler* window) {
    struct Portal* portal = obj->data;

    if (portal->state == PortalOpen) {
        portal->openedTime += window->deltaTime;
    }
    else if(portal->state == PortalClosed){
        portal->closedTime += window->deltaTime;
    }
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
    if (dist <= 0.0f) return;

    struct Portal* portal = obj->data;

    if (portal->state != PortalOpen && portal->closedTime >= 1.0f) return;

    vector_t upVec = { 0 };
    vector_t rightVec = { 0 };
    transform_up(obj->transform, &upVec);
    transform_right(obj->transform, &rightVec);

    float animDir = (portal->color.r > portal->color.b) ? 1.0f : -1.0f;
    float animSize = fmaxf(0.1f, sinf(fminf(portal->openedTime * 3.0f, 0.5f * M_PI)));

    // calculate reference points for portal frame
    vector_t portalPoints[PORTAL_RESOLUTION];
    for (int i = 0; i < PORTAL_RESOLUTION; i++) {
        vector_t upPart, rightPart;

        float ang = -2.0f * M_PI * ((float)i / (float)PORTAL_RESOLUTION);
        ang += fmodf(portal->openedTime * animDir, 2.0f * M_PI);

        float xDist = PORTAL_WIDTH * 0.5f * cosf(ang) * animSize;
        float yDist = PORTAL_HEIGHT * 0.5f * sinf(ang) * animSize;

        vector_scale(rightVec, xDist, &rightPart);
        vector_scale(upVec, yDist, &upPart);
        vector_add(rightPart, upPart, &portalPoints[i]);
        vector_add(portalPoints[i], obj->transform.position, &portalPoints[i]);
    }

    // generate ripples for rendering
    vector_t portalRipplesPoints[PORTAL_RESOLUTION * 3];
    
    for (int i = 0; i < PORTAL_RESOLUTION * 3; i++) {
        int v = i % PORTAL_RESOLUTION;
        int r = i / PORTAL_RESOLUTION;

        float randomnessScale = 0.02f;
        if (portal->state == PortalClosed) {
            randomnessScale = portal->closedTime * 0.2f;
        }

        vector_t randOffset = {
            sinf(portal->openedTime * fmodf(5.0f + i * 123.45f, 10.0f)) * r * randomnessScale,
            sinf(portal->openedTime * fmodf(5.0f + i * 234.56f, 10.0f)) * r * randomnessScale,
            sinf(portal->openedTime * fmodf(5.0f + i * 456.78f, 10.0f)) * r * randomnessScale,
        };

        vector_add(portalPoints[v], randOffset, &portalRipplesPoints[i]);
    }

    for (int i = 0; i < PORTAL_RESOLUTION * 3; i++) {
        int v = i % PORTAL_RESOLUTION;
        int r = i / PORTAL_RESOLUTION;

        vector_t p1 = portalRipplesPoints[r * PORTAL_RESOLUTION + v];
        vector_t p2 = portalRipplesPoints[r * PORTAL_RESOLUTION + ((v + 1) % PORTAL_RESOLUTION)];
        color_t color = portal->color;
        if (r == 1) color = color_mix(color, (color_t) { 0x44ffffff });
        else if (r == 2) color = color_mix(color, (color_t) { 0x44000000 });

        if (portal->state == PortalClosed) {
            color.a = (uint8_t)((1.0f - portal->closedTime) * 255.0f);
        }

        struct RenderData renderData = {
            .color = color,
            .line = {p1, p2},
        };
        render_batch_add_data(batch, renderData);
    }

    if (
        portal->state == PortalOpen && 
        portal->linked != NULL && 
        portal->linked->state == PortalOpen && 
        portal->currentDepth < PORTAL_MAX_DEPTH
    ) {
        matrix_t portalMatrix;
        portal_passage_matrix(portal, &portalMatrix);
        struct Camera portalCamera = *batch->camera;
        transform_apply_matrix(portalCamera.transform, portalMatrix, &portalCamera.transform);

        struct RenderMask* portalWorldMask = malloc(sizeof(struct RenderMask));
        portalWorldMask->count = batch->mask.count;
        portalWorldMask->nextGroup = batch->mask.nextGroup;
        for (int i = 0; i < portalWorldMask->count; i++) {
            portalWorldMask->group[i] = batch->mask.group[i];
            line_t maskLine;
            mat_transform_point(portalMatrix, batch->mask.lines[i].a, &maskLine.a);
            mat_transform_point(portalMatrix, batch->mask.lines[i].b, &maskLine.b);
            portalWorldMask->lines[i] = maskLine;
        }
        
        vector_t portalPointsTransformed[PORTAL_RESOLUTION];
        for (int i = 0; i < PORTAL_RESOLUTION; i++) {
            mat_transform_point(portalMatrix, portalPoints[PORTAL_RESOLUTION - 1 - i], &portalPointsTransformed[i]);
        }
        vector_t linkedportalplane;
        transform_forward(portal->linked->object.transform, &linkedportalplane);
        vector_t lpos = portal->linked->object.transform.position;
        linkedportalplane.w = lpos.x * linkedportalplane.x + lpos.y * linkedportalplane.y + lpos.z * linkedportalplane.z;
        vector_scale(linkedportalplane, -1.0f, &linkedportalplane);

        struct WorldPortalData worldPortal = {
            .roomFrom = portal->object.currentRoom,
            .roomTo = portal->linked->object.currentRoom,
            .verticesCount = PORTAL_RESOLUTION,
            .vertices = portalPointsTransformed,
            .plane = linkedportalplane,
        };

        portal->currentDepth++;
        world_render_custom(obj->world, batch->display, portalCamera, &worldPortal, portalWorldMask);
        portal->currentDepth--;

        free(portalWorldMask);
    }
}

void portal_init(struct Portal* portal, struct World* world, struct Portal* linked, color_t color)
{
    portal->linked = linked;
    portal->color = color;
    portal->currentDepth = 0;
    portal->openedTime = 0.0f;
    portal->closedTime = 1.0f;
    portal->state = PortalClosed;

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
    quaternion_axis_angle(180.0f, (vector_t) { 0.0f, 1.0f, 0.0f }, &rotateY);
    mat_rotate(rotateY, &rotateYMat);

    matrix_t temp;

    mat_mul(localToWorldOut, rotateYMat, &temp);
    mat_mul(temp, worldToLocalIn, out);
}

void portal_set_state(struct Portal* portal, enum PortalState state)
{
    if (state == PortalOpen) {
        portal->openedTime = 0.0f;
    }
    else if(state == PortalClosed) {
        portal->closedTime = 0.0f;
    }
    portal->state = state;
}

void portal_place(struct Portal* portal, transform_t transform)
{
    if (portal->state == PortalOpen) {
        portal_set_state(portal, PortalClosed);
    }
    portal_set_state(portal, PortalOpen);

    vector_t newPosition;

    vector_t normal;
    transform_forward(transform, &normal);
    vector_scale(normal, 0.01f, &newPosition);
    vector_add(newPosition, transform.position, &transform.position);
    portal->object.transform = transform;
}

void portal_shoot(struct Portal* portal, vector_t origin, vector_t dir)
{
    struct RayCastHit hit;
    world_raycast(portal->object.world, origin, dir, 1024, &hit);

    if (hit.type != HitWorld) return;
  
    vector_t forwardVec = hit.normal;
    vector_t upVec = { 0.0f, 1.0f, 0.0f };

    // TODO: fix floor/ceiling portal rendering lol
    if (fabs(vector_dot(forwardVec, upVec)) > 0.1) {
        return; // lol no fuck you :)
    }

    if (fabsf(vector_dot(forwardVec, upVec)) > 0.99) {
        vector_t rightVec;
        vector_cross(forwardVec, dir, &rightVec);
        vector_cross(rightVec, forwardVec, &upVec);
    }

    quaternion_t rotation;
    quaternion_look_rotation(forwardVec, upVec, &rotation);

    portal_place(portal, (transform_t) {
        .position = hit.point,
        .rotation = rotation
    });
}
