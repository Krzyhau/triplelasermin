#ifndef __ROOM_H__
#define __ROOM_H__

#include "camera.h"
#include "core/display.h"

struct Wall {
    int texture;
};

struct RoomVertex {
    float x;
    float y;
    float z;
    float h;
};

struct RoomData {
    int wallCount;
    struct Wall* walls;
    struct RoomVertex* vertices;

    int floorTexture;
    int ceilingTexture;
};

struct Room {
    struct RoomData data;
    struct RoomVertex* projectedVertices;
};

void room_init(struct Room* room, struct RoomData data);

void room_project_vertices(struct Room* room, struct Camera* camera, struct Display* display);
void room_draw_column(struct Room* room, struct Display* display, int x);

#endif
