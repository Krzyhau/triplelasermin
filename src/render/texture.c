#include "texture.h"

#include "texture_list.h"

color_t texture_sample(int sampleFuncIndex, float ux, float uy)
{
    return g_textureSampleFuncs[sampleFuncIndex](ux, uy);
}
