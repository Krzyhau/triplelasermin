#include "core/window.h"
#include "render/render.h"
#include "render/camera.h"

#include "../assets/map.h"

void game_config(struct WindowHandler* window) {
    window->info = (struct WindowInfo){
        .className = L"tl.exe",
        .titleName = L"tl.exe",
        .width = 1280, .height = 720,
        .minWidth = 320, .minHeight = 180,
        .pixelSize = 4,
        .tickTimeMs = 10,
    };
}

struct Camera camera;

void game_init(struct WindowHandler* window) {
    camera_init(&camera);

    camera.transform.position = (vector_t){ 0.0f, 0.0f, -2.0f };

    window->input->mouseLocked = InputMouseLocked;
}

void game_update(struct WindowHandler* window) {

    // rotation
    float sensitivity = 0.2f;

    float pitchDelta = -window->input->deltaMouseY * sensitivity;
    float yawDelta = -window->input->deltaMouseX * sensitivity;

    quaternion_t pitchRot, yawRot;
    quaternion_axis_angle(pitchDelta, (vector_t) { 1.0f, 0.0f, 0.0f }, &pitchRot);
    quaternion_axis_angle(yawDelta, (vector_t) { 0.0f, 1.0f, 0.0f }, &yawRot);

    quaternion_multiply(camera.transform.rotation, yawRot, &camera.transform.rotation);
    quaternion_multiply(pitchRot, camera.transform.rotation, &camera.transform.rotation);

    // movement
    vector_t forwardVec;
    vector_t rightVec;
    transform_forward(camera.transform, &forwardVec);
    transform_right(camera.transform, &rightVec);

    vector_t mov = { 0.0f, 0.0f, 0.0f };

    if (input_key_held(window->input, 'W')) {
        vector_add(mov, forwardVec, &mov);
    }
    if (input_key_held(window->input, 'S')) {
        vector_sub(mov, forwardVec, &mov);
    }
    if (input_key_held(window->input, 'A')) {
        vector_sub(mov, rightVec, &mov);
    }
    if (input_key_held(window->input, 'D')) {
        vector_add(mov, rightVec, &mov);
    }
    if (input_key_held(window->input, 'Q')) {
        vector_add(mov, (vector_t) {0.0f, 1.0f, 0.0f}, &mov);
    }
    if (input_key_held(window->input, 'E')) {
        vector_add(mov, (vector_t) { 0.0f, -1.0f, 0.0f }, &mov);
    }

    vector_scale(mov, 2.0f * window->deltaTime, &mov);

    vector_add(camera.transform.position, mov, &camera.transform.position);
}

void game_draw(struct WindowHandler* window) {

    display_fill(window->display, (color_t) { .rgba = 0xff000000 });

    camera.aspectRatio = (float)window->display->width / (float)window->display->height;

    struct WorldData* world = &g_world;

    // find the first room to render with camera position and bounds
    int startingRoom = world->roomCount;
    vector_t cp = camera.transform.position;
    while (--startingRoom >= 0) {
        int boundId = world->rooms[startingRoom].boundsCount;
        while (--boundId >= 0) {
            vector_t bound = world->rooms[startingRoom].bounds[boundId];
            float dist = bound.x * cp.x + bound.y * cp.y + bound.z * cp.z - bound.w;
            if (dist < 0) break;
        }
        if (boundId < 0)  break;
    }

    // generate room masks lookups for batches by travelling from the main room
    uint32_t roomsProcessed = 0;
    uint32_t roomsToProcess = 0;
    uint32_t portalsLookup[WORLD_ROOMS_MAX_COUNT] = { 0 };
    uint32_t reversePortalsLookup[WORLD_ROOMS_MAX_COUNT] = { 0 };

    if (startingRoom >= 0) {
        roomsToProcess |= (1 << startingRoom);
    }
    while (roomsToProcess) {
        for (int i = 0; i < world->roomCount; i++) {
            if ((roomsToProcess & (1 << i)) == 0) continue;
            roomsToProcess &= ~(1 << i);
            if ((roomsProcessed & (1 << i)) != 0) continue;
            roomsProcessed |= (1 << i);
            
            for (int j = 0; j < world->portalsCount; j++) {
                struct WorldPortalData* portal = &world->portals[j];
                if (portal->roomFrom == i && (roomsProcessed & (1 << portal->roomTo)) == 0) {
                    portalsLookup[portal->roomTo] = portalsLookup[i] | (1 << j);
                    roomsToProcess |= (1 << portal->roomTo);
                }
                else if (portal->roomTo == i && (roomsProcessed & (1 << portal->roomFrom)) == 0) {
                    reversePortalsLookup[portal->roomFrom] = portalsLookup[i] | (1 << j);
                    roomsToProcess |= (1 << portal->roomFrom);
                }
            }

        }
    }

    // render all rooms

    struct RenderBatch batch;

    for (int i = 0; i < world->roomCount;i++) {
        render_batch_reset(&batch);

        // generate masks based on previously made lookups
        for (int j = 0; j < world->portalsCount; j++) {
            struct WorldPortalData* portal = &world->portals[j];
            uint8_t portalState = portalsLookup[i] & (1 << j);
            uint8_t reversePortalState = reversePortalsLookup[i] & (1 << j);
            if (portalState == 0 && reversePortalState == 0) continue;
            for (int k = 0; k < portal->verticesCount; k++) {
                vector_t p1 = portal->vertices[k];
                vector_t p2 = portal->vertices[(k + 1) % portal->verticesCount];

                if (reversePortalState) {
                    render_batch_add_mask_line(&batch, (line_t) { p1, p2 });
                }
                if (portalState) {
                    render_batch_add_mask_line(&batch, (line_t) { p2, p1 });
                }
            }
        }

        // add all lines
        for (int j = 0; j < world->rooms[i].linesCount; j++) {
            struct RenderData data = {
                .color = (color_t){.rgba = 0xffffffff},
                .line = world->rooms[i].lines[j]
            };
            render_batch_add_data(&batch, data);
        }

        // project and render
        render_batch_project(&batch, &camera);
        render_batch_draw(window->display, &batch);
    }
}