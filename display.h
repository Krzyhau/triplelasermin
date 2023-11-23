#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include <windows.h>
#include <stdint.h>

struct Display {
    HWND windowHandle;
    int pixelSize;

    int width;
    int height;

    uint32_t* buffer;
};

typedef union Color {
    uint32_t rgba;
    struct {
        uint8_t b;
        uint8_t g;
        uint8_t r;
        uint8_t a;
    };
} color_t;

void display_init(struct Display* display, HWND handle, int pixelSize);
void display_begin(struct Display* display);
void display_end(struct Display* display);
void display_set_pixel(struct Display* display, int x, int y, color_t color);

#endif