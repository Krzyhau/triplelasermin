#include "core/window.h"
#include "render/render.h"
#include "render/camera.h"

void game_config(struct WindowHandler* window) {
    window->info = (struct WindowInfo){
        .className = L"tl.exe",
        .titleName = L"tl.exe",
        .width = 1280, .height = 720,
        .minWidth = 320, .minHeight = 180,
        .pixelSize = 4,
        .tickTimeMs = 16,
    };
}

struct Camera camera;
struct RenderBatch batch;

struct RenderLine cubeLines[12] = {
    {{-1,-1,-1}, {-1,-1, 1}, {.rgba = 0xff0000ff}},
    {{-1,-1, 1}, { 1,-1, 1}, {.rgba = 0xffffffff}},
    {{ 1,-1, 1}, { 1,-1,-1}, {.rgba = 0xffffffff}},
    {{ 1,-1,-1}, {-1,-1,-1}, {.rgba = 0xffff0000}},
    {{-1, 1,-1}, {-1, 1, 1}, {.rgba = 0xffffffff}},
    {{-1, 1, 1}, { 1, 1, 1}, {.rgba = 0xffffffff}},
    {{ 1, 1, 1}, { 1, 1,-1}, {.rgba = 0xffffffff}},
    {{ 1, 1,-1}, {-1, 1,-1}, {.rgba = 0xffffffff}},
    {{-1,-1,-1}, {-1, 1,-1}, {.rgba = 0xff00ff00}},
    {{ 1,-1,-1}, { 1, 1,-1}, {.rgba = 0xffffffff}},
    {{-1,-1, 1}, {-1, 1, 1}, {.rgba = 0xffffffff}},
    {{ 1,-1, 1}, { 1, 1, 1}, {.rgba = 0xffffffff}},
};

void game_init(struct WindowHandler* window) {
    camera_init(&camera);
}

void game_update(struct WindowHandler* window) {

    // rotation
    float pitchDelta = 0.0f;
    float yawDelta = 0.0f;

    if (input_key_held(window->input, 'W')) {
        pitchDelta += 1.0f;
    }
    if (input_key_held(window->input, 'S')) {
        pitchDelta -= 1.0f;
    }
    if (input_key_held(window->input, 'A')) {
        yawDelta += 1.0f;
    }
    if (input_key_held(window->input, 'D')) {
        yawDelta -= 1.0f;
    }

    pitchDelta *= 180.0f * window->deltaTime;
    yawDelta *= 180.0f * window->deltaTime;

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

    if (input_key_held(window->input, VK_UP)) {
        vector_add(mov, forwardVec, &mov);
    }
    if (input_key_held(window->input, VK_DOWN)) {
        vector_sub(mov, forwardVec, &mov);
    }
    if (input_key_held(window->input, VK_LEFT)) {
        vector_sub(mov, rightVec, &mov);
    }
    if (input_key_held(window->input, VK_RIGHT)) {
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

    render_batch_reset(&batch);
    for (int i = 0; i < 12; i++) {
        render_batch_add(&batch, cubeLines[i]);
    }

    matrix_t worldToLocalMatrix;
    matrix_t projectionMatrix;
    transform_world_to_local_matrix(camera.transform, &worldToLocalMatrix);
    camera_projection_matrix(&camera, &projectionMatrix);

    matrix_t finalMatrix;
    mat_mul(projectionMatrix, worldToLocalMatrix, &finalMatrix);

    render_batch_apply_matrix(&batch, finalMatrix);

    render_batch_draw(window->display, &batch);
}