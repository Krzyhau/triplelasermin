#include "render.h"

#include "../math/matrix.h"
#include <math.h>

void render_batch_reset(struct RenderBatch* batch)
{
    batch->currentLength = 0;
}

void render_batch_add(struct RenderBatch* batch, struct RenderLine line)
{
    batch->lines[batch->currentLength++] = line;
}

void render_batch_apply_matrix(struct RenderBatch* batch, const matrix_t matrix) {
    for (int i = 0; i < batch->currentLength; i++) {
        vector_t a, b;
        mat_transform_point(matrix, batch->lines[i].a, &a);
        mat_transform_point(matrix, batch->lines[i].b, &b);
        batch->lines[i].a = a;
        batch->lines[i].b = b;
    }
}

void render_batch_project(struct RenderBatch* batch, struct Camera* camera)
{
    // transform batch with view matrix
    matrix_t worldToLocalMatrix;
    transform_world_to_local_matrix(camera->transform, &worldToLocalMatrix);
    render_batch_apply_matrix(batch, worldToLocalMatrix);

    // clip line
    for (int i = 0; i < batch->currentLength; i++) {
        struct RenderLine line = batch->lines[i];
        if (line.a.z < camera->nearPlane && line.b.z >= camera->nearPlane) {
            float d = (camera->nearPlane - line.b.z) / (line.a.z - line.b.z);
            line.a.x = line.b.x * (1.0f - d) + line.a.x * d;
            line.a.y = line.b.y * (1.0f - d) + line.a.y * d;
            line.a.z = line.b.z * (1.0f - d) + line.a.z * d;
        }
        if (line.b.z < camera->nearPlane && line.a.z >= camera->nearPlane) {
            float d = (camera->nearPlane - line.a.z) / (line.b.z - line.a.z);
            line.b.x = line.a.x * (1.0f - d) + line.b.x * d;
            line.b.y = line.a.y * (1.0f - d) + line.b.y * d;
            line.b.z = line.a.z * (1.0f - d) + line.b.z * d;
        }
        batch->lines[i] = line;
    }

    // transform batch with projection matrix
    matrix_t projectionMatrix;
    camera_projection_matrix(camera, &projectionMatrix);
    render_batch_apply_matrix(batch, projectionMatrix);
}

void render_batch_draw(struct Display* display, struct RenderBatch* batch)
{
    for (int i = 0; i < batch->currentLength; i++) {
        struct RenderLine line = batch->lines[i];

        if (line.a.w >= 0 || line.b.w >= 0) continue;

        // translate into a screen space
        line.a.x = (1.0f - line.a.x / line.a.w) * 0.5f * display->width;
        line.a.y = (1.0f + line.a.y / line.a.w) * 0.5f * display->height;
        line.b.x = (1.0f - line.b.x / line.b.w) * 0.5f * display->width;
        line.b.y = (1.0f + line.b.y / line.b.w) * 0.5f * display->height;

        render_line_draw(display, line);
    }
}

void render_line_draw(struct Display* display, struct RenderLine line)
{
    int x1 = line.a.x;
    int y1 = line.a.y;
    int x2 = line.b.x;
    int y2 = line.b.y;

    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = x1 < x2 ? 1 : -1;
    int sy = y1 < y2 ? 1 : -1;
    int err = dx - dy;

    while (1)
    {
        float t = 0.0f;
        if (line.b.x != line.a.x) {
            t = (x1 - line.a.x) / (line.b.x - line.a.x);
        }
        else if (line.b.y != line.a.y) {
            t = (y1 - line.a.y) / (line.b.y - line.a.y);
        }
        float depth = line.a.z * (1.0f - t) + line.b.z * t;
        display_draw_pixel_depth(display, x1, y1, line.color, -depth);

        if (x1 == x2 && y1 == y2) break;
        int err2 = 2 * err;
        if (err2 > -dy)
        {
            err -= dy;
            x1 += sx;
        }
        if (err2 < dx)
        {
            err += dx;
            y1 += sy;
        }
    }
}
