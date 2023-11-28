#include <windows.h>
#include <time.h>
#include <stdint.h>
#include <math.h>
#include <stdio.h>

#include "window.h"

void game_config(struct WindowHandler* window);
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

        .startCallback = game_init,
        .updateCallback = game_update,
        .drawCallback = game_draw,
    };

    game_config(&window);

    window_init(&window);
    return window_run(&window);
}