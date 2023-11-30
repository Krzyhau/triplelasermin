#ifndef __RENDER_H__
#define __RENDER_H__

#include "../core/display.h"
#include "../math/vector.h"
#include "../math/matrix.h"
#include "camera.h"

#define RENDER_BATCH_MAX_LINES 1024;

struct RenderLine{
    vector_t a;
    vector_t b;
    color_t color;
};

struct RenderBatch{
    int currentLength;
    struct RenderLine lines[1024];
};

void render_batch_reset(struct RenderBatch* batch);
void render_batch_add(struct RenderBatch* batch, struct RenderLine line);
void render_batch_apply_matrix(struct RenderBatch* batch, const matrix_t matrix);
void render_batch_project(struct RenderBatch* batch, struct Camera* camera);
void render_batch_draw(struct Display* display, struct RenderBatch* batch);
void render_line_draw(struct Display* display, struct RenderLine line);

#endif