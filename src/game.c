#include "core/window.h"
#include "render/render.h"

void game_config(struct WindowHandler* window) {
    window->info = (struct WindowInfo){
        .className = L"tl.exe",
        .titleName = L"tl.exe",
        .width = 1280, .height = 720,
        .minWidth = 320, .minHeight = 180,
        .pixelSize = 4,
        .tickTimeMs = 16,
    };
}

void game_init(struct WindowHandler* window) {

}

void game_update(struct WindowHandler* window) {
    if (input_key_held(window->input, VK_UP)) {

    }
    if (input_key_held(window->input, VK_DOWN)) {

    }

    if (input_key_held(window->input, VK_LEFT)) {

    }

    if (input_key_held(window->input, VK_RIGHT)) {

    }
}

void game_draw(struct WindowHandler* window) {

    display_fill(window->display, (color_t) { .rgba = 0xff000000 });
    render_line(window->display, 100, 100, 200, 200, (color_t) { .rgba = 0xff006fde });
    render_line(window->display, 50, 50, 50, 300, (color_t) { .rgba = 0xff006fde });
    render_line(window->display, 200, 20, 10, 20, (color_t) { .rgba = 0xff006fde });

    //display_draw_circle(window->display, 200, 100, 20, (color_t) { .rgba = 0x80de6f00 });
}