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
    vector_t cp = camera.transform.position;

    // find the first room to render with camera position and bounds
    int startingRoom = world->roomCount;
    while (--startingRoom >= 0) {
        int boundId = world->rooms[startingRoom].boundsCount;
        while (--boundId >= 0) {
            vector_t bound = world->rooms[startingRoom].bounds[boundId];
            float dist = bound.x * cp.x + bound.y * cp.y + bound.z * cp.z - bound.w;
            if (dist < 0) break;
        }
        if (boundId < 0)  break;
    }
    if (startingRoom < 0) return;
    
    // figure out which portals we can render through
    uint32_t portalDirectionsFlag = 0;
    for (int i = 0; i < world->portalsCount; i++) {
        vector_t mask = world->portals[i].plane;
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

        for (int portalId = 0; portalId < world->portalsCount; portalId++) {
            struct WorldPortalData* portal = &world->portals[portalId];
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

    for (int i = 0; i < propagationSteps;i++) {
        render_batch_reset(&batch);

        int room = roomPropagationList[i];
        uint32_t roomMask = portalMaskPropagationList[i];

        // generate masks based on previously made flag masks
        for (int portalId = 0; portalId < world->portalsCount; portalId++) {
            if ((roomMask & (1 << portalId)) == 0) continue;
            int reversed = (portalDirectionsFlag & (1 << portalId));
            struct WorldPortalData* portal = &world->portals[portalId];
            for (int vert = 0; vert < portal->verticesCount; vert++) {
                vector_t p1 = portal->vertices[vert];
                vector_t p2 = portal->vertices[(vert + 1) % portal->verticesCount];
                line_t maskLine = (reversed) ? (line_t) { p2, p1 } : (line_t) { p1, p2 };
                render_batch_add_mask_line_group(&batch, maskLine, portalId);
            }
        }

        // add all lines
        for (int line = 0; line < world->rooms[room].linesCount; line++) {
            struct RenderData data = {
                .color = (color_t){.rgba = 0xffffffff},
                .line = world->rooms[room].lines[line]
            };
            render_batch_add_data(&batch, data);
        }

        // project and render
        render_batch_project(&batch, &camera);
        render_batch_draw(window->display, &batch);
    }
}