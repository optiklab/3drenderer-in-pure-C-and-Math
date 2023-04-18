#include "Texture.h"

#include <stdlib.h>

tex2_t tex2_clone(tex2_t* t)
{
    tex2_t result = { t->u, t->v };
    return result;
}

void free_texture(lodepng_texture_t* t)
{
    delete[] t->png_texture;
    delete t;
}