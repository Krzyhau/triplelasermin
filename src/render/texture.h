#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include "../core/display.h"

typedef color_t(*TextureSampleFunc)(float ux, float uy);

color_t texture_sample(int sampleFuncIndex, float ux, float uy);

#endif
