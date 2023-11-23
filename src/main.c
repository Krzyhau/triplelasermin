#include <windows.h>
#include <time.h>
#include <stdint.h>
#include <math.h>
#include <stdio.h>

#include "core/window.h"
#include "texture/texture_list.h"

#include "world.h"

void game_init(struct WindowHandler* window);
void game_update(struct WindowHandler* window);
void game_draw(struct WindowHandler* window);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {

    struct WindowHandler window;

    window.data = (struct WindowData){
        .instance = hInstance,
        .prevInstance = hPrevInstance,
        .cmdLine = pCmdLine,
        .cmdShow = nCmdShow,

        .className = L"tl.exe",
        .titleName = L"tl.exe",
        .width = 1280, .height = 720,
        .minWidth = 320, .minHeight = 180,
        .pixelSize = 8,
        .tickTimeMs = 16,

        .startCallback = game_init,
        .updateCallback = game_update,
        .drawCallback = game_draw,
    };

    window_init(&window);
    return window_run(&window);
}

struct World g_world;

struct Wall test_room_wall_data[] = {
     {.texture = TEXTURE_WHITE_WALL },
     {.texture = TEXTURE_WHITE_WALL },
     {.texture = TEXTURE_WHITE_WALL },
     {.texture = TEXTURE_WHITE_WALL },
};

struct RoomVertex test_room_vertices[] = {
    {.x = -1.0f, .y = -1.0f, .z = -1.0f, .h = 2.0f },
    {.x = -1.0f, .y = -1.0f, .z = 1.0f, .h = 2.0f },
    {.x = 1.0f, .y = -1.0f, .z = 1.0f, .h = 2.0f },
    {.x = 1.0f, .y = -1.0f, .z = -1.0f, .h = 2.0f },
};

struct RoomData test_world_rooms[] = {
    // test_room
    {
        .wallCount = 4,
        .walls = test_room_wall_data,
        .vertices = test_room_vertices,
        .floorTexture = TEXTURE_BLACK_WALL,
        .ceilingTexture = TEXTURE_BLACK_WALL,
    }
};

struct WorldData test_world_data = {
    .roomCount = 1,
    .rooms = test_world_rooms
};

void game_init(struct WindowHandler* window) {
    world_init(&g_world, test_world_data);
}

void game_update(struct WindowHandler* window) {
    if (input_key_held(window->input, VK_UP)) {
        g_world.camera.x += cos(g_world.camera.rotRad) * 2.0f * window->deltaTime;
        g_world.camera.z += sin(g_world.camera.rotRad) * 2.0f * window->deltaTime;
    }
    if (input_key_held(window->input, VK_DOWN)) {
        g_world.camera.x -= cos(g_world.camera.rotRad) * 2.0f * window->deltaTime;
        g_world.camera.z -= sin(g_world.camera.rotRad) * 2.0f * window->deltaTime;
    }

    if (input_key_held(window->input, VK_LEFT)) {
        g_world.camera.rotRad += 3.14f * window->deltaTime;
    }

    if (input_key_held(window->input, VK_RIGHT)) {
        g_world.camera.rotRad -= 3.14f * window->deltaTime;
    }
}

void game_draw(struct WindowHandler* window) {
    world_draw(&g_world, window->display);

    /*int cameraScreenX = (g_world.camera.x) * 10.0f + (float)window->display->width * 0.5f;
    int cameraScreenY = (-g_world.camera.z) * 10.0f + (float)window->display->height * 0.5f;

    for (int x = 0; x < window->display->width; x++) {
        for (int y = 0; y < window->display->height; y++) {
            display_set_pixel(window->display, x, y, (color_t){ .rgba = 0 });
        }
    }

    display_set_pixel(window->display, cameraScreenX, cameraScreenY, (color_t) { .rgba = 0x006fde });*/
}