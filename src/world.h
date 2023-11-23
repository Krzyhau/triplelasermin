#ifndef __WORLD_H__
#define __WORLD_H__

#include "room.h"
#include "camera.h"


struct WorldData {
    int roomCount;
    struct RoomData* rooms;
};

struct World {
    int roomCount;
    struct Room* rooms;

    struct Camera camera;
};

void world_init(struct World* world, struct WorldData data);
void world_draw(struct World* world, struct Display* display);

#endif