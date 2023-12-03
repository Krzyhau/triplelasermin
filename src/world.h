#ifndef __WORLD_H__
#define __WORLD_H__

#include "render/render.h"

#define WORLD_ROOMS_MAX_COUNT 32

struct WorldPortalData {
    int roomFrom;
    int roomTo;
    int verticesCount;
    vector_t* vertices;
};

struct WorldRoomData {
    int linesCount;
    line_t* lines;
    int boundsCount;
    vector_t* bounds;
};

struct WorldData {
    int roomCount;
    struct WorldRoomData* rooms;
    int portalsCount;
    struct WorldPortalData* portals;
};

#endif