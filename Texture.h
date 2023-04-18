#ifndef TEXTURE_H
#define TEXTURE_H
#include <stdint.h>
#include "lodepng.h"

typedef struct {
    float u;
    float v;
} tex2_t;

typedef struct
{
    unsigned int width;
    unsigned int height;
    uint32_t* png_texture;
} lodepng_texture_t;

tex2_t tex2_clone(tex2_t* t);

void free_texture(lodepng_texture_t* t);

#endif