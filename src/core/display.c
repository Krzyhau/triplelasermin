#include "display.h"
#include <math.h>

color_t color_scale(color_t color, float scalar) {
    float r = (float)color.r * scalar;
    float g = (float)color.b * scalar;
    float b = (float)color.b * scalar;

    return (color_t) {
        .r = (uint8_t)fminf(r, 255),
        .g = (uint8_t)fminf(g, 255),
        .b = (uint8_t)fminf(b, 255),
    };
}

color_t color_mix(color_t color, color_t overlay) {
    float a = overlay.a / 255.0;
    float na = (1.0f - a);

    return (color_t) {
        .r = (uint8_t)(color.r * na + overlay.r * a),
        .g = (uint8_t)(color.g * na + overlay.g * a),
        .b = (uint8_t)(color.b * na + overlay.b * a),
        .a = (uint8_t)(color.a * na + overlay.a * a),
    };
}

void display_init(struct Display* display, HWND handle, int pixelSize)
{
    display->pixelSize = pixelSize;
    display->buffer = 0;
    display->windowHandle = handle;
}

void display_begin(struct Display* display)
{
    RECT r;
    GetClientRect(display->windowHandle, &r);
    if (r.bottom == 0) return;
    int scaledWidth = (r.right - r.left) / display->pixelSize;
    int scaledHeight = (r.bottom - r.top) / display->pixelSize;
    int windowSize = scaledWidth * scaledHeight;

    int displaySize = display->width * display->height;

    if (display->buffer == NULL || displaySize != windowSize) {
        displaySize = windowSize;
        display->width = scaledWidth;
        display->height = scaledHeight;
        if (display->buffer != NULL) free(display->buffer);
        display->buffer = malloc(displaySize * sizeof(uint32_t));
    }
}

void display_end(struct Display* display)
{
    if (display->buffer == NULL) return;

    RECT r;
    GetClientRect(display->windowHandle, &r);
    if (r.bottom == 0) return;
    int windowWidth = (r.right - r.left);
    int windowHeight = (r.bottom - r.top);

    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(display->windowHandle, &ps);

    BITMAPINFOHEADER bmih = {
        .biSize = sizeof(BITMAPINFOHEADER),
        .biWidth = display->width,
        .biHeight = display->height,
        .biPlanes = 1,
        .biBitCount = 32
    };
    BITMAPINFO dbmi = { .bmiHeader = bmih };

    StretchDIBits(
        hdc, 0, 0,
        windowWidth, windowHeight,
        0, 0, 
        display->width, display->height, 
        display->buffer, &dbmi, 0, SRCCOPY
    );
    EndPaint(display->windowHandle, &ps);
}

void display_fill(struct Display* display, color_t color) {
    int size = display->width * display->height;
    for (int i = 0; i < size; i++) {
        display->buffer[i] = color;
    }
}

void display_draw_pixel(struct Display* display, int x, int y, color_t color) {
    if (x < 0 || y < 0 || x >= display->width || y >= display->height) return;
    int index = x + (display->height - 1 - y) * display->width;
    display->buffer[index] = color.a == 255 ? color : color_mix(display->buffer[index], color);
}