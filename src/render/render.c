#include "render.h"

#include "../math/matrix.h"


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

void render_batch_draw(struct Display* display, struct RenderBatch* batch)
{
    for (int i = 0; i < batch->currentLength; i++) {
        struct RenderLine line = batch->lines[i];

        if (line.a.w > -0.001f) {
            line.a.w = -0.001f;
        }
        if (line.b.w > -0.001f) {
            line.b.w = -0.001f;
        }

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
        display_draw_pixel(display, x1, y1, line.color);

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
