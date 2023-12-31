#ifndef __RENDER_H__
#define __RENDER_H__

#include "../core/display.h"
#include "../math/vector.h"
#include "../math/matrix.h"
#include "camera.h"

#define RENDER_BATCH_MAX_DATA 1024
#define RENDER_MASK_MAX_LINES 128

typedef struct {
    vector_t a;
    vector_t b;
} line_t;

struct RenderMask {
    int count;
    line_t lines[RENDER_MASK_MAX_LINES];
    int nextGroup;
    int group[RENDER_MASK_MAX_LINES];
};

struct RenderData{
    line_t line;
    color_t color;
};

struct RenderBatch{
    int dataCount;
    struct RenderData data[RENDER_BATCH_MAX_DATA];
    struct RenderMask mask;

    struct Display* display;
    struct Camera* camera;
};

void render_batch_init(struct RenderBatch* batch, struct Display* display, struct Camera* camera);
void render_batch_reset(struct RenderBatch* batch);
void render_batch_add_data(struct RenderBatch* batch, struct RenderData data);
void render_batch_add_mask_line(struct RenderBatch* batch, const line_t maskLine);
void render_batch_add_mask_line_group(struct RenderBatch* batch, const line_t maskLine, int group);
void render_batch_draw(struct RenderBatch* batch);

void render_data_single_draw(struct RenderData data, struct Display* display, struct Camera camera);

#endif