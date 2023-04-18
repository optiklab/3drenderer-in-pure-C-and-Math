#ifndef MESH_H
#define MESH_H

#include <stdbool.h>

#include "Vector.h"
#include "Triangle.h"
#include "Texture.h"

////////////////////////////////////////////////////////////////////////////////
// Define a struct for dynamic size meshes, with array of vertices and faces
////////////////////////////////////////////////////////////////////////////////
typedef struct {
    std::vector<vec3_t> vertices; // dynamic array of vertices
    std::vector<face_t> faces;    // dynamic array of faces
    lodepng_texture_t* texture;    // mesh PNG texture pointer
    vec3_t rotation;  // rotation with x, y, and z values
    vec3_t scale;       // scale with x, y, and z values
    vec3_t translation; // translation with x, y, and z values
} mesh_t;

void load_mesh(const char* obj_filename, const char* png_filename, vec3_t scale, vec3_t translation, vec3_t rotation);
void load_mesh_obj_data(mesh_t* mesh, const char* obj_filename);
void load_mesh_png_data(mesh_t* mesh, const char* png_filename);

int get_num_meshes(void);
mesh_t* get_mesh(int index);

void rotate_mesh_x(int mesh_index, float angle);
void rotate_mesh_y(int mesh_index, float angle);
void rotate_mesh_z(int mesh_index, float angle);

void free_meshes(void);

#endif