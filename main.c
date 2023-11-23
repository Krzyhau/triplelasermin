#include <windows.h>
#include <time.h>
#include <stdint.h>
#include <math.h>
#include <stdio.h>

#include "window.h"

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

struct point2d {
    float x;
    float y;
};

struct gameroom {
    int wallscount;
    int* wallsids;
    struct point2d* wallverts;
    float floor;
    float ceiling;
    int floorid;
    int ceilingid;
};

static color_t colorTest;

void game_init(struct WindowHandler* window) {
    colorTest.rgba = 0x006fde;
}

void game_update(struct WindowHandler* window) {
    if (input_key_held(window->input, VK_UP)) {
        colorTest.r += 5;
    }
    if (input_key_held(window->input, VK_DOWN)) {
        colorTest.r -= 5;
    }

    if (input_key_pressing(window->input, 'G')) {
        colorTest.g += 64;
    }

    if (input_key_typing(window->input, 'B')) {
        colorTest.b += 64;
    }
}

color_t texture_white_tile_big_generate(float ux, float uy) {
    const color_t borderColor = (color_t){ .rgba = 0x787b79 };
    const color_t backgroundColor = (color_t){ .rgba = 0xf0f0f1 };

    float squareDistX = fabsf(fmodf(ux, 1.0f) - 0.5f) * 2.0f;
    float squareDistY = fabsf(fmodf(uy, 1.0f) - 0.5f) * 2.0f;
    float squareDist = fmaxf(squareDistX, squareDistY);

    float gridx = fabsf(fmodf(ux * 20.0f, 1.0f) - 0.5f);
    float gridy = fabsf(fmodf(uy * 20.0f, 1.0f) - 0.5f);
    float grid = fabsf(gridx - gridy) * 0.3f;

    squareDist = fminf(fmaxf(((squareDist - 0.97f) * 40.0f), 0.0f), 1.0f) - grid;

    return (color_t) {
        .rgba = color_scale(borderColor, squareDist).rgba + color_scale(backgroundColor, 1.0f - squareDist).rgba
    };
}

void game_draw(struct WindowHandler* window) {

    for (int x = 0; x < window->display->width; x++) {

        for (int y = 0; y < window->display->height; y++) {

            const float tileSize = 64;
            float tileX = (float)x / tileSize;
            float tileY = (float)y / tileSize;

            float scroll = window->totalTime * 0.2f;
            tileX += scroll;

            display_set_pixel(window->display, x, y, texture_white_tile_big_generate(tileX, tileY));
        }
    }
}