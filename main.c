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

void game_draw(struct WindowHandler* window) {
    float d = fabsf(fmodf(window->totalTime, 2.0) - 1.0);

    for (int x = 0; x < window->display->width; x++) {

        float d2 = fabsf(fmodf(window->totalTime + x * 0.01f, 2.0) - 1.0);

        color_t color = { 
            .r = colorTest.r * d2, 
            .g = colorTest.g * d2, 
            .b = colorTest.b * d2 
        };
        for (int y = 0; y < window->display->height; y++) {
            display_set_pixel(window->display, x, y, color);
        }
    }
}