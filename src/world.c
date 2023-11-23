#include "world.h"

void world_init(struct World* world, struct WorldData data) {
    world->roomCount = data.roomCount;

    world->rooms = malloc(data.roomCount * sizeof(struct Room));

    for (int i = 0; i < data.roomCount; i++) {
        room_init(&world->rooms[i], data.rooms[i]);
    }

    world->camera.fovRad = 90.0f * 3.1415926535 / 180.0f;
}

void world_draw(struct World* world, struct Display* display)
{
    for (int i = 0; i < world->roomCount; i++) {
        room_project_vertices(&world->rooms[i], &world->camera, display);
    }

    // TODO: find the room we're currently in to render it
    int currRoom = 0;

    for (int x = 0; x < display->width; x++) {
        room_draw_column(&world->rooms[currRoom], display, x);
    }
}
