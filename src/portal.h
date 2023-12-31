#ifndef __PORTAL_H__
#define __PORTAL_H__

#include "object.h"
#include "core/display.h"

#define PORTAL_WIDTH 0.5f
#define PORTAL_HEIGHT 0.9f
#define PORTAL_RESOLUTION 12
#define PORTAL_MAX_DEPTH 3

enum PortalState {
    PortalOpen,
    PortalClosed,
};

struct Portal {
    struct Object object;
    struct Object ghostObject;

    struct Portal* linked;
    color_t color;
    int currentDepth;

    float openedTime;
    float closedTime;
    enum PortalState state;
};

void portal_init(struct Portal* portal, struct World* world, struct Portal* linked, color_t color);
void portal_passage_matrix(struct Portal* portal, matrix_t* out);
void portal_set_state(struct Portal* portal, enum PortalState state);
void portal_place(struct Portal* portal, transform_t transform);
void portal_shoot(struct Portal* portal, vector_t origin, vector_t dir);

#endif