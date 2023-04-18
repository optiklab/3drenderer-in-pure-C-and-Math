#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <stdbool.h>
#include <stdint.h>
#include "Texture.h"
#include "Vector.h"

typedef struct {
    int a;
    int b;
    int c;
    tex2_t a_uv;
    tex2_t b_uv;
    tex2_t c_uv;
    uint32_t color;
} face_t;

typedef struct {
    vec4_t points[3];
    tex2_t texcoords[3];
    uint32_t color;
    lodepng_texture_t* texture;
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// No more painters algorithm. No more bubble-sort. Use z-buffer instead for both Textures AND Colored triangles. //
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //float avg_depth; // For ordering triangles as part of painter algorithm.
} triangle_t;

vec3_t get_triangle_normal(vec4_t vertices[3]);

void draw_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color);

void draw_filled_triangle_painters_algorithm(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color);

void draw_filled_triangle(
    int x0, int y0, float z0, float w0,
    int x1, int y1, float z1, float w1,
    int x2, int y2, float z2, float w2,
    uint32_t color
);

void draw_textured_triangle(
    int x0, int y0, float z0, float w0, float u0, float v0,
    int x1, int y1, float z1, float w1, float u1, float v1,
    int x2, int y2, float z2, float w2, float u2, float v2,
    lodepng_texture_t* texture
);

#endif
