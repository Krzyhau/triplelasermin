#include "core/window.h"
#include "render/render.h"
#include "render/camera.h"

#include "../assets/map.h"
#include "world.h"

void game_config(struct WindowHandler* window) {
    window->info = (struct WindowInfo){
        .className = L"tl.exe",
        .titleName = L"tl.exe",
        .width = 1280, .height = 720,
        .minWidth = 320, .minHeight = 180,
        .pixelSize = 4,
        .tickTimeMs = 10,
    };
}

struct World world;

void game_init(struct WindowHandler* window) {
    window->input->mouseLocked = InputMouseLocked;
    world_init(&world, &g_world);
}

void game_update(struct WindowHandler* window) {
    world_update(&world, window);
}

void game_draw(struct WindowHandler* window) {

    display_fill(window->display, (color_t) { .rgba = 0xff000000 });

    world_render(&world, window->display);
}