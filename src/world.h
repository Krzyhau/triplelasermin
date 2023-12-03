#ifndef __WORLD_H__
#define __WORLD_H__

#include "render/render.h"
#include "player.h"

#define WORLD_ROOMS_MAX_COUNT 32
#define WORLD_ROOMS_MAX_PROPAGATION 128

struct WorldPortalData {
    int roomFrom;
    int roomTo;
    int verticesCount;
    vector_t* vertices;
    vector_t plane;
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

struct World {
    struct WorldData* data;

    struct Player player;
    struct Camera camera;
};

void world_init(struct World* world, struct WorldData* data);
void world_update(struct World* world, struct WindowHandler* window);
void world_render(struct World* world, struct Display* display);

#endif