#include "world.h"

void world_init(struct World* world, struct WorldData* data)
{
    world->data = data;
    player_init(&world->player, world);
    camera_init(&world->camera);

    world->gravity = 5.0f;
}

void world_update(struct World* world, struct WindowHandler* window)
{
    player_update(&world->player, window);

    vector_t cam_offset = { 0.0f, 0.5f, 0.0f };
    vector_add(world->player.transform.position, cam_offset, &world->camera.transform.position);
    world->camera.transform.rotation = world->player.transform.rotation;
}

void world_render(struct World* world, struct Display* display)
{
    world->camera.aspectRatio = (float)display->width / (float)display->height;

    vector_t cp = world->camera.transform.position;

    // find the first room to render with camera position and bounds
    int startingRoom = world->data->roomCount;
    while (--startingRoom >= 0) {
        int boundId = world->data->rooms[startingRoom].boundsCount;
        while (--boundId >= 0) {
            vector_t bound = world->data->rooms[startingRoom].bounds[boundId];
            float dist = bound.x * cp.x + bound.y * cp.y + bound.z * cp.z - bound.w;
            if (dist < 0) break;
        }
        if (boundId < 0)  break;
    }
    if (startingRoom < 0) return;

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
    struct RenderBatch batch;

    for (int i = 0; i < propagationSteps; i++) {
        render_batch_reset(&batch);

        int room = roomPropagationList[i];
        uint32_t roomMask = portalMaskPropagationList[i];

        // generate masks based on previously made flag masks
        for (int portalId = 0; portalId < world->data->portalsCount; portalId++) {
            if ((roomMask & (1 << portalId)) == 0) continue;
            int reversed = (portalDirectionsFlag & (1 << portalId));
            struct WorldPortalData* portal = &world->data->portals[portalId];
            for (int vert = 0; vert < portal->verticesCount; vert++) {
                vector_t p1 = portal->vertices[vert];
                vector_t p2 = portal->vertices[(vert + 1) % portal->verticesCount];
                line_t maskLine = (reversed) ? (line_t) { p2, p1 } : (line_t) { p1, p2 };
                render_batch_add_mask_line_group(&batch, maskLine, portalId);
            }
        }

        // add all lines
        for (int line = 0; line < world->data->rooms[room].linesCount; line++) {
            struct RenderData data = {
                .color = (color_t){.rgba = 0xffffffff},
                .line = world->data->rooms[room].lines[line]
            };
            render_batch_add_data(&batch, data);
        }

        // project and render
        render_batch_project(&batch, &world->camera);
        render_batch_draw(display, &batch);
    }
}
