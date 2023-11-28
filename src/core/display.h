#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include <windows.h>
#include <stdint.h>

typedef union Color {
    uint32_t rgba;
    struct {
        uint8_t b;
        uint8_t g;
        uint8_t r;
        uint8_t a;
    };
} color_t;

struct Display {
    HWND windowHandle;
    int pixelSize;

    int width;
    int height;

    color_t* buffer;
};

color_t color_scale(color_t color, float scalar);
color_t color_mix(color_t color, color_t overlay);

void display_init(struct Display* display, HWND handle, int pixelSize);
void display_begin(struct Display* display);
void display_end(struct Display* display);
void display_fill(struct Display* display, color_t color);
void display_draw_pixel(struct Display* display, int x, int y, color_t color);

#endif