#ifndef __PORTAL_H__
#define __PORTAL_H__

#include "object.h"
#include "core/display.h"

#define PORTAL_WIDTH 0.6f
#define PORTAL_HEIGHT 1.0f
#define PORTAL_RESOLUTION 12
#define PORTAL_MAX_DEPTH 3

struct Portal {
    struct Object object;

    struct Portal* linked;
    color_t color;
    int currentDepth;
};

void portal_init(struct Portal* portal, struct World* world, struct Portal* linked, color_t color);
void portal_passage_matrix(struct Portal* portal, matrix_t* out);

#endif