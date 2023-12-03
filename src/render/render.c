#include "render.h"

#include "../math/matrix.h"
#include <math.h>

void render_mask_line(struct RenderMask* mask, const line_t line, line_t* out)
{
    int linesInGroup[RENDER_MASK_MAX_LINES] = { 0 };
    int groupBehindZCount[RENDER_MASK_MAX_LINES] = { 0 };
    int maxGroup = 0;

    line_t maskedLine = line;
    for (int i = 0; i < mask->count; i++) {
        int group = mask->group[i];
        maxGroup = max(maxGroup, group);
        linesInGroup[group]++;
        line_t maskLine = mask->lines[i];
        if (maskLine.a.z < 0 && maskLine.b.z < 0) {
            groupBehindZCount[group]++;
            continue;
        }
        vector_t maskLineTangent;
        vector_sub(maskLine.b, maskLine.a, &maskLineTangent);
        vector_norm(maskLineTangent, &maskLineTangent);
        vector_t maskLineNormal = {-maskLineTangent.y, maskLineTangent.x};

        vector_t distVec;
        vector_sub(maskedLine.a, maskLine.a, &distVec);
        float dotA = vector_dot(distVec, maskLineNormal);
        vector_sub(maskedLine.b, maskLine.a, &distVec);
        float dotB = vector_dot(distVec, maskLineNormal);

        // both ends of the line masked, abort
        if (dotA < 0 && dotB < 0) {
            out->a = (vector_t){ -1.0f, -1.0f, -1.0f, -1.0f };
            out->b = (vector_t){ -1.0f, -1.0f, -1.0f, -1.0f };
            return;
        }

        // line not even masked, go to another mask line
        if (dotA >= 0 && dotB >= 0) {
            continue;
        }

        // masking only a part. figure out intersection
        // and try to cut the line off at that point
        vector_t cut;

        float lineDiffX = (line.b.x - line.a.x);
        float maskLineDiffX = (maskLine.b.x - maskLine.a.x);

        // lines are parallel horizontally, don't bother
        if (lineDiffX == 0.0f && maskLineDiffX == 0.0f) {
            continue;
        }

        float lineM = (line.b.y - line.a.y) / lineDiffX;
        float lineC = line.a.y - lineM * line.a.x;
        float maskM = (maskLine.b.y - maskLine.a.y) / maskLineDiffX;
        float maskC = maskLine.a.y - maskM * maskLine.a.x;

        // lines are parallel, don't bother
        if (lineM == maskM) {
            continue;
        }

        if (lineDiffX == 0.0f) {
            cut.x = line.a.x;
            cut.y = maskM * line.a.x + maskC;
        }
        else if (maskLineDiffX == 0.0f) {
            cut.x = maskLine.a.x;
            cut.y = lineM * maskLine.a.x + lineC;
        }
        else {
            cut.x = (maskC - lineC) / (lineM - maskM);
            cut.y = lineM * cut.x + lineC;
        }

        float d = (line.a.x == line.b.x)
            ? (cut.y - line.a.y) / (line.b.y - line.a.y)
            : (cut.x - line.a.x) / (line.b.x - line.a.x);
        cut.z = line.a.z + (line.b.z - line.a.z) * d;

        if (dotA < 0) {
            maskedLine.a = cut;
        }
        else if(dotB < 0) {
            maskedLine.b = cut;
        }
    }

    // at least one mask group is behind the camera.
    // we're assuming we want to mask stuff
    // so just hide that line
    for (int i = 0; i <= maxGroup; i++) {
        if (linesInGroup[i] > 0 && groupBehindZCount[i] == linesInGroup[i]) {
            out->a = (vector_t){ 1.0f, -1.0f, -1.0f, -1.0f };
            out->b = (vector_t){ -1.0f, -1.0f, -1.0f, -1.0f };
            return;
        }
    }

    *out = maskedLine;
}

void render_batch_reset(struct RenderBatch* batch)
{
    batch->dataCount = 0;
    batch->mask.count = 0;
}

void render_batch_add_data(struct RenderBatch* batch, struct RenderData data)
{
    if (batch->dataCount == RENDER_BATCH_MAX_DATA) return;
    batch->data[batch->dataCount++] = data;
}

void render_batch_add_mask_line(struct RenderBatch* batch, const line_t maskLine) {
    render_batch_add_mask_line_group(batch, maskLine, batch->mask.count);
}

void render_batch_add_mask_line_group(struct RenderBatch* batch, const line_t maskLine, int group)
{
    if (batch->mask.count == RENDER_MASK_MAX_LINES) return;
    batch->mask.group[batch->mask.count] = group;
    batch->mask.lines[batch->mask.count] = maskLine;
    batch->mask.count++;
}

void render_batch_apply_matrix(struct RenderBatch* batch, const matrix_t matrix) {
    for (int i = 0; i < batch->dataCount; i++) {
        line_t line;
        mat_transform_point(matrix, batch->data[i].line.a, &line.a);
        mat_transform_point(matrix, batch->data[i].line.b, &line.b);
        batch->data[i].line = line;
    }
    for (int i = 0; i < batch->mask.count; i++) {
        line_t line;
        mat_transform_point(matrix, batch->mask.lines[i].a, &line.a);
        mat_transform_point(matrix, batch->mask.lines[i].b, &line.b);
        batch->mask.lines[i] = line;
    }
}

void render_line_screen_space_pass(struct Display* display, const line_t in, line_t* out) {
    *out = in;
    if (in.a.w >= 0 || in.b.w >= 0) {
        out->a.z = -1.0f;
        out->b.z = -1.0f;
        return;
    }
    
    out->a.x = (1.0f - in.a.x / in.a.w) * 0.5f * display->width;
    out->a.y = (1.0f + in.a.y / in.a.w) * 0.5f * display->height;
    out->b.x = (1.0f - in.b.x / in.b.w) * 0.5f * display->width;
    out->b.y = (1.0f + in.b.y / in.b.w) * 0.5f * display->height;
    out->a.z /= in.b.w;
    out->b.z /= in.b.w;
    out->a.w = 1.0f;
    out->b.w = 1.0f;
}

void render_batch_screen_space_pass(struct RenderBatch* batch, struct Display* display) {
    // transform lines into screen space
    for (int i = 0; i < batch->dataCount; i++) {
        render_line_screen_space_pass(display, batch->data[i].line, &batch->data[i].line);
    }
    for (int i = 0; i < batch->mask.count; i++) {
        render_line_screen_space_pass(display, batch->mask.lines[i], &batch->mask.lines[i]);
    }

    // adding screen edges as mask
    render_batch_add_mask_line(batch, (line_t) { {0, 0}, { display->width, 0 } });
    render_batch_add_mask_line(batch, (line_t) { {display->width, 0}, { display->width, display->height } });
    render_batch_add_mask_line(batch, (line_t) { { display->width, display->height }, { 0, display->height } });
    render_batch_add_mask_line(batch, (line_t) { {0, display->height}, { 0, 0 } });

    // mask lines after screen space conversion
    for (int i = 0; i < batch->dataCount; i++) {
        render_mask_line(&batch->mask, batch->data[i].line, &batch->data[i].line);
    }
}

void render_line_near_plane_clip(struct RenderBatch* batch, float nearPlane, const line_t in, line_t* out) {
    *out = in;
    if (in.a.z < nearPlane && in.b.z >= nearPlane) {
        float d = (nearPlane - in.b.z) / (in.a.z - in.b.z);
        out->a.x = in.b.x * (1.0f - d) + in.a.x * d;
        out->a.y = in.b.y * (1.0f - d) + in.a.y * d;
        out->a.z = in.b.z * (1.0f - d) + in.a.z * d;
    }
    if (in.b.z < nearPlane && in.a.z >= nearPlane) {
        float d = (nearPlane - in.a.z) / (in.b.z - in.a.z);
        out->b.x = in.a.x * (1.0f - d) + in.b.x * d;
        out->b.y = in.a.y * (1.0f - d) + in.b.y * d;
        out->b.z = in.a.z * (1.0f - d) + in.b.z * d;
    }
}

void render_batch_project(struct RenderBatch* batch, struct Camera* camera)
{
    // transform batch with view matrix
    matrix_t worldToLocalMatrix;
    transform_world_to_local_matrix(camera->transform, &worldToLocalMatrix);
    render_batch_apply_matrix(batch, worldToLocalMatrix);

    // clip lines
    for (int i = 0; i < batch->dataCount; i++) {
        render_line_near_plane_clip(batch, camera->nearPlane, batch->data[i].line, &batch->data[i].line);
    }
    for (int i = 0; i < batch->mask.count; i++) {
        render_line_near_plane_clip(batch, camera->nearPlane, batch->mask.lines[i], &batch->mask.lines[i]);
    }

    // transform batch with projection matrix
    matrix_t projectionMatrix;
    camera_projection_matrix(camera, &projectionMatrix);
    render_batch_apply_matrix(batch, projectionMatrix);
}

void render_batch_draw(struct Display* display, struct RenderBatch* batch)
{
    render_batch_screen_space_pass(batch, display);

    for (int i = 0; i < batch->dataCount; i++) {
        struct RenderData data = batch->data[i];
        if (data.line.a.z < 0 && data.line.b.z < 0) continue;
        render_line_draw(display, data);
    }
}

void render_line_draw(struct Display* display, struct RenderData data)
{
    line_t line = data.line;
    if (line.a.x == line.b.x && line.a.y == line.b.y) return;

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
        display_draw_pixel_depth(display, x1, y1, data.color, -depth);

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
