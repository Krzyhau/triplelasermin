#include "display.h"

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

void display_set_pixel(struct Display* display, int x, int y, color_t color) {
    if (x < 0 || y < 0 || x >= display->width || y >= display->height) return;
    display->buffer[x + y * display->width] = color.rgba;
}