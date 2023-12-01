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
        .tickTimeMs = 10,
    };
}

struct Camera camera;
struct RenderBatch batch;

struct RenderData cubeData[12] = {
    {{{-1,-1,-1}, {-1,-1, 1}}, {.rgba = 0xff0000ff}},
    {{{-1,-1, 1}, { 1,-1, 1}}, {.rgba = 0xffffffff}},
    {{{ 1,-1, 1}, { 1,-1,-1}}, {.rgba = 0xffffffff}},
    {{{ 1,-1,-1}, {-1,-1,-1}}, {.rgba = 0xffff0000}},
    {{{-1, 1,-1}, {-1, 1, 1}}, {.rgba = 0xffffffff}},
    {{{-1, 1, 1}, { 1, 1, 1}}, {.rgba = 0xffffffff}},
    {{{ 1, 1, 1}, { 1, 1,-1}}, {.rgba = 0xffffffff}},
    {{{ 1, 1,-1}, {-1, 1,-1}}, {.rgba = 0xffffffff}},
    {{{-1,-1,-1}, {-1, 1,-1}}, {.rgba = 0xff00ff00}},
    {{{ 1,-1,-1}, { 1, 1,-1}}, {.rgba = 0xffffffff}},
    {{{-1,-1, 1}, {-1, 1, 1}}, {.rgba = 0xffffffff}},
    {{{ 1,-1, 1}, { 1, 1, 1}}, {.rgba = 0xffffffff}},
};

line_t testMask[4] = {
    {{-1,-1,-1}, {-1, 1,-1}},
    {{-1, 1,-1}, { 1, 1,-1}},
    {{ 1, 1,-1}, { 1,-1,-1}},
    {{ 1,-1,-1}, {-1,-1,-1}},
};

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

    render_batch_reset(&batch);

    // big cube with colored axes
    for (int i = 0; i < 12; i++) {
        render_batch_add_data(&batch, cubeData[i]);
    }

    for (int c = 0; c < 90; c++) {
        // make a small spinning yellow cube in the middle
        quaternion_t pitchCubeRot, yawCubeRot, cubeRot;
        quaternion_axis_angle(window->totalTime * 50.0f + c, (vector_t) { 1.0f, 0.0f, 0.0f }, & pitchCubeRot);
        quaternion_axis_angle(window->totalTime * 50.0f + c, (vector_t) { 0.0f, 1.0f, 0.0f }, & yawCubeRot);
        quaternion_multiply(pitchCubeRot, yawCubeRot, &cubeRot);

        matrix_t rotation_matrix;
        mat_rotate(cubeRot, &rotation_matrix);

        color_t color = {
            .r = (c / 90.0f) * 255,
            .g = 255,
            .b = 255 - (c / 90.0f) * 255,
            .a = 128
        };

        for (int i = 0; i < 12; i++) {
            struct RenderData data = cubeData[i];
            vector_scale(data.line.a, 0.5f, &data.line.a);
            vector_scale(data.line.b, 0.5f, &data.line.b);
            mat_transform_point(rotation_matrix, data.line.a, &data.line.a);
            mat_transform_point(rotation_matrix, data.line.b, &data.line.b);
            data.color = color;
            render_batch_add_data(&batch, data);
        }
    }

    

    for (int i = 0; i < 4; i++) {
        render_batch_add_mask_line(&batch, testMask[i]);
    }

    render_batch_project(&batch, &camera);

    render_batch_draw(window->display, &batch);
}