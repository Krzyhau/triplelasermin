#include "texture_list.h"

#include <math.h>

color_t texture_white_wall(float ux, float uy)
{
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

color_t texture_black_wall(float ux, float uy)
{
    const color_t borderColor = (color_t){ .rgba = 0x6f716f };
    const color_t backgroundColor = (color_t){ .rgba = 0x363736 };

    float squareDistX = fabsf(fmodf(ux, 1.0f) - 0.5f) * 2.0f;
    float squareDistY = fabsf(fmodf(uy, 1.0f) - 0.5f) * 2.0f;
    float squareDist = fmaxf(squareDistX, squareDistY);

    float sineTint = sinf(ux * 60.0f) * 0.04f;

    squareDist = fminf(fmaxf(((squareDist - 0.97f) * 40.0f), 0.0f), 1.0f) - sineTint;

    return (color_t) {
        .rgba = color_scale(borderColor, squareDist).rgba + color_scale(backgroundColor, 1.0f - squareDist).rgba
    };
}


TextureSampleFunc g_textureSampleFuncs[] = {
    texture_white_wall,
    texture_black_wall,
};