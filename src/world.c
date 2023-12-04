#include "world.h"

void world_init(struct World* world, struct WorldData* data)
{
    world->objectCount = 0;
    world->data = data;

    player_init(&world->player, world);
    camera_init(&world->camera);

    world_add_object(world, &world->player.object);

    portal_init(&world->primaryPortal, world, &world->secondaryPortal, (color_t) { .rgba = 0xff006fde });
    portal_init(&world->secondaryPortal, world, &world->primaryPortal, (color_t) { .rgba = 0xffde6f00 });

    world_add_object(world, &world->primaryPortal);
    world_add_object(world, &world->secondaryPortal);

    world->primaryPortal.object.transform.position = (vector_t){ -3.5, 0.5, -0.75 };
    world->secondaryPortal.object.transform.position = (vector_t){ -5, 0.5, -0.75 };
    quaternion_axis_angle(180.0f, (vector_t) { 0.0f, 1.0f, 0.0f }, & world->primaryPortal.object.transform.rotation);
    quaternion_axis_angle(180.0f, (vector_t) { 0.0f, 1.0f, 0.0f }, &world->secondaryPortal.object.transform.rotation);

    world->gravity = 5.0f;
}

void world_update(struct World* world, struct WindowHandler* window)
{
    for (int i = 0; i < world->objectCount; i++) {
        object_update(world->objects[i], window);
    }

    vector_t cam_offset = { 0.0f, 0.5f, 0.0f };
    vector_add(world->player.object.transform.position, cam_offset, &world->camera.transform.position);
    world->camera.transform.rotation = world->player.object.transform.rotation;
}

void world_add_object(struct World* world, struct Object* object)
{
    if (world->objectCount >= WORLD_MAX_OBJECTS) return;
    world->objects[world->objectCount] = object;
    world->objectCount++;
}

int world_get_room_at(struct World* world, vector_t pos)
{
    int room = world->data->roomCount;
    while (--room >= 0) {
        int boundId = world->data->rooms[room].boundsCount;
        while (--boundId >= 0) {
            vector_t bound = world->data->rooms[room].bounds[boundId];
            float dist = bound.x * pos.x + bound.y * pos.y + bound.z * pos.z - bound.w;
            if (dist < 0) break;
        }
        if (boundId < 0) break;
    }
    return room;
}

void world_render_custom(struct World* world, struct Display* display, struct Camera camera, struct WorldPortalData* mainPortal)
{
    camera.aspectRatio = (float)display->width / (float)display->height;

    vector_t cp = camera.transform.position;

    
    int startingRoom = 0;
    if (mainPortal != NULL) {
        // if given portal is valid, use its target room for rendering
        // otherwise, we won't see anything anyway, so just skip it
        vector_t pmask = mainPortal->plane;
        float dist = pmask.x * cp.x + pmask.y * cp.y + pmask.z * cp.z - pmask.w;
        if (dist <= 0) return;

        startingRoom = mainPortal->roomTo;
    }
    else {
        // find the first room to render
        startingRoom = world_get_room_at(world, cp);
    }

    // figure out which portals we can render through
    uint32_t portalDirectionsFlag = 0;
    for (int i = 0; i < world->data->portalsCount; i++) {
        vector_t mask = world->data->portals[i].plane;
        float dist = mask.x * cp.x + mask.y * cp.y + mask.z * cp.z - mask.w;
        if (dist > 0) {
            portalDirectionsFlag |= (1 << i);
        }
    }

    // prepare render states - propagate from the first room
    int propagationSteps = 1;
    int roomPropagationList[WORLD_ROOMS_MAX_PROPAGATION] = { 0 };
    uint32_t portalMaskPropagationList[WORLD_ROOMS_MAX_PROPAGATION] = { 0 };
    roomPropagationList[0] = startingRoom;

    for (int i = 0; i < propagationSteps; i++) {
        int room = roomPropagationList[i];
        uint32_t roomMask = portalMaskPropagationList[i];

        for (int portalId = 0; portalId < world->data->portalsCount; portalId++) {
            struct WorldPortalData* portal = &world->data->portals[portalId];
            if (portal->roomFrom != room && portal->roomTo != room) continue;

            // make sure this is a portal which propagates further in rendering
            int neededDirection = (portal->roomFrom == room) ? 1 : 0;
            int portalDirection = ((portalDirectionsFlag & (1 << portalId)) > 0) ? 1 : 0;
            if (portalDirection != neededDirection) continue;

            int passedToRoomId = (portal->roomTo == room) ? portal->roomFrom : portal->roomTo;

            if (propagationSteps >= WORLD_ROOMS_MAX_PROPAGATION) break;

            roomPropagationList[propagationSteps] = passedToRoomId;
            portalMaskPropagationList[propagationSteps] = roomMask | (1 << portalId);
            propagationSteps++;
        }

        if (propagationSteps >= WORLD_ROOMS_MAX_PROPAGATION) break;
    }


    // render propagation data
    struct RenderBatch* batch = malloc(sizeof(struct RenderBatch));
    render_batch_init(batch, display, &camera);

    for (int i = 0; i < propagationSteps; i++) {
        render_batch_reset(batch);

        int room = roomPropagationList[i];
        uint32_t roomMask = portalMaskPropagationList[i];

        // if using main portal, add it to mask
        if (mainPortal != NULL) {
            for (int vert = 0; vert < mainPortal->verticesCount; vert++) {
                vector_t p1 = mainPortal->vertices[vert];
                vector_t p2 = mainPortal->vertices[(vert + 1) % mainPortal->verticesCount];
                render_batch_add_mask_line_group(batch, (line_t) { p1, p2 }, 0);
            }
        }

        // generate masks based on previously made flag masks
        for (int portalId = 0; portalId < world->data->portalsCount; portalId++) {
            if ((roomMask & (1 << portalId)) == 0) continue;
            int reversed = (portalDirectionsFlag & (1 << portalId));
            struct WorldPortalData* portal = &world->data->portals[portalId];
            for (int vert = 0; vert < portal->verticesCount; vert++) {
                vector_t p1 = portal->vertices[vert];
                vector_t p2 = portal->vertices[(vert + 1) % portal->verticesCount];
                line_t maskLine = (reversed) ? (line_t) { p2, p1 } : (line_t) { p1, p2 };
                render_batch_add_mask_line_group(batch, maskLine, portalId+1);
            }
        }

        // add all lines
        for (int line = 0; line < world->data->rooms[room].linesCount; line++) {
            struct RenderData data = {
                .color = (color_t){.rgba = 0xffffffff},
                .line = world->data->rooms[room].lines[line]
            };
            render_batch_add_data(batch, data);
        }

        for (int i = 0; i < world->objectCount; i++) {
            object_render(world->objects[i], batch, room);
        }

        render_batch_draw(batch);
    }

    free(batch);
}

void world_render(struct World* world, struct Display* display)
{
    world_render_custom(world, display, world->camera, NULL);
}
