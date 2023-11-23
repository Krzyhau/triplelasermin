#ifndef __WINDOW_H__
#define __WINDOW_H__

#include <windows.h>
#include <stdint.h>
#include <time.h>

#include "input.h"
#include "display.h"

typedef void (*StartFunc)(struct WindowHandler* window);
typedef void (*UpdateFunc)(struct WindowHandler* window);
typedef void (*DrawFunc)(struct WindowHandler* window);

struct WindowData {
    HINSTANCE instance;
    HINSTANCE prevInstance;
    PWSTR cmdLine;
    int cmdShow;

    LPCWSTR className;
    LPCWSTR titleName;
    int width;
    int height;
    int minWidth;
    int minHeight;
    int pixelSize;
    int tickTimeMs;

    StartFunc startCallback;
    UpdateFunc updateCallback;
    DrawFunc drawCallback;
};

struct WindowHandler {
    HWND handle;
    struct WindowData data;

    struct InputState* input;
    struct Display* display;
    
    float totalTime;
    float deltaTime;
    clock_t lastClock;
};

void window_init(struct WindowHandler* window);
int window_run(struct WindowHandler* window);

#endif