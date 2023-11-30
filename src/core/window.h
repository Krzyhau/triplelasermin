#ifndef __WINDOW_H__
#define __WINDOW_H__

#include <windows.h>
#include <windowsx.h>
#include <stdint.h>
#include <time.h>

#include "input.h"
#include "display.h"

typedef void (*WindowStartFunc)(struct WindowHandler* window);
typedef void (*WindowUpdateFunc)(struct WindowHandler* window);
typedef void (*WindowDrawFunc)(struct WindowHandler* window);

struct WindowInfo {
    LPCWSTR className;
    LPCWSTR titleName;
    int width;
    int height;
    int minWidth;
    int minHeight;
    int pixelSize;
    int tickTimeMs;
};

struct WindowData {
    HINSTANCE instance;
    HINSTANCE prevInstance;
    PWSTR cmdLine;
    int cmdShow;

    WindowStartFunc startCallback;
    WindowUpdateFunc updateCallback;
    WindowDrawFunc drawCallback;
};

struct WindowHandler{
    HWND handle;
    struct WindowInfo info;
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