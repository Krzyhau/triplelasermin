#include "room.h"

#include <math.h>
#include "texture/texture_list.h"

void room_init(struct Room* room, struct RoomData data)
{
    room->data = data;

    room->projectedVertices = malloc(room->data.wallCount * sizeof(struct RoomVertex));
}

void room_project_vertices(struct Room* room, struct Camera* camera, struct Display* display)
{
    float screenRatio = (float)display->width / (float)display->height;

    for (int i = 0; i < room->data.wallCount; i++) {
        // translate the vertex to the local origin
        struct RoomVertex translated = room->data.vertices[i];
        translated.x -= camera->x;
        translated.y -= camera->y;
        translated.z -= camera->z;

        // rotate the vertex around origin (y axis)
        struct RoomVertex rotated = translated;
        float revAng = 1.570796325f - camera->rotRad;
        rotated.x = translated.x * cosf(revAng) + translated.z * sinf(revAng);
        rotated.z = translated.x * sinf(revAng) + translated.z * cosf(revAng);

        // apply perspective projection to the vertex
        struct RoomVertex projected = rotated;
        float perspFactor = tanf(camera->fovRad) * projected.z;
        projected.x /= perspFactor * screenRatio;
        projected.y /= perspFactor;
        projected.h /= perspFactor;

        // transform it to screen-space
        projected.x = ((projected.x + 1.0f) * 0.5f) * (float)display->width;
        projected.y = ((-projected.y + 1.0f) * 0.5f) * (float)display->height;
        projected.h *= -0.5f * (float)display->height;

        room->projectedVertices[i] = projected;
    }
}

void room_draw_column(struct Room* room, struct Display* display, int x)
{
    int wallIndex = -1;
    float wallH = 0.0f;
    int floorY = 0;
    int ceilingY = 0;
    float wallDist = 0;

    for (int i = 0; i < room->data.wallCount; i++) {
        struct RoomVertex projVert1 = room->projectedVertices[i];
        struct RoomVertex projVert2 = room->projectedVertices[(i + 1) % room->data.wallCount];

        if (projVert2.x < projVert1.x) continue; // backface culling
        if (x < projVert1.x || x > projVert2.x) continue; // column not overlapping the wall

        // this is the one, let's calculate shit

        wallIndex = i;
        wallH = ((float)x - projVert1.x) / (projVert2.x - projVert1.x);

        floorY = projVert1.y * (1.0f - wallH) + projVert2.y * wallH;
        ceilingY = floorY + projVert1.h * (1.0f - wallH) + projVert2.h * wallH;

        wallDist = projVert1.z * (1.0f - wallH) + projVert2.z * wallH;

        break;
    }

    if (wallIndex == 0 || wallDist <= 0) {
        // no wall found, clear black
        for (int y = 0; y < display->height; y++) {
            display_set_pixel(display, x, y, (color_t) {.rgba = 0});
        }
        return;
    }
    struct Wall wall = room->data.walls[wallIndex];

    struct RoomVertex vert1 = room->data.vertices[wallIndex];
    struct RoomVertex vert2 = room->data.vertices[(wallIndex + 1) % room->data.wallCount];

    float wallHorizontalLength = sqrtf(powf(vert2.x - vert1.x, 2.0f) + powf(vert2.z - vert1.z, 2.0f));
    float wallX = wallHorizontalLength * wallH;
    float wallHeight = vert1.h * (1.0f - wallH) + vert2.h * wallH;

    floorY = fminf(floorY, display->height);
    ceilingY = fmaxf(ceilingY, 0.0f);

    for (int y = ceilingY; y <= floorY; y++) {
        float wallY = (float)(y - ceilingY) / (float)(floorY - ceilingY) * wallHeight;
        display_set_pixel(display, x, y, texture_sample(wall.texture, wallX, wallY));
    }

    if (ceilingY > 0) {
        for (int y = 0; y < ceilingY; y++) {
            display_set_pixel(display, x, y, (color_t) { .rgba = 0 });
        }
    }

    if (floorY + 1 < display->height) {
        for (int y = floorY + 1; y < display->height; y++) {
            display_set_pixel(display, x, y, (color_t) { .rgba = 0 });
        }
    }
}
