#ifndef __RENDER_H__
#define __RENDER_H__

#include "../core/display.h"
#include "../math/vector.h"
#include "../math/matrix.h"
#include "camera.h"

#define RENDER_BATCH_MAX_DATA 1024
#define RENDER_MASK_MAX_LINES 1024

typedef struct {
    vector_t a;
    vector_t b;
} line_t;

struct RenderMask {
    int count;
    line_t lines[RENDER_MASK_MAX_LINES];
};

struct RenderData{
    line_t line;
    color_t color;
};

struct RenderBatch{
    int dataCount;
    struct RenderData data[RENDER_BATCH_MAX_DATA];
    struct RenderMask mask;
};

void render_batch_reset(struct RenderBatch* batch);
void render_batch_add_data(struct RenderBatch* batch, struct RenderData data);
void render_batch_add_mask_line(struct RenderBatch* batch, const line_t maskLine);
void render_batch_apply_matrix(struct RenderBatch* batch, const matrix_t matrix);
void render_batch_project(struct RenderBatch* batch, struct Camera* camera);
void render_batch_draw(struct Display* display, struct RenderBatch* batch);
void render_line_draw(struct Display* display, struct RenderData data);

#endif