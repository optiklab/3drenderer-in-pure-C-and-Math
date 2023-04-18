#include <iostream>
#include <string>
#include <cstdlib>
#include <vector>

#include "Mesh.h"

#define MAX_NUM_MESHES 10
static mesh_t meshes[MAX_NUM_MESHES];
static int mesh_count = 0;

void load_mesh(const char* obj_filename, const char* png_filename, vec3_t scale, vec3_t translation, vec3_t rotation)
{
    load_mesh_obj_data(&meshes[mesh_count], obj_filename);
    load_mesh_png_data(&meshes[mesh_count], png_filename);

    meshes[mesh_count].scale = scale;
    meshes[mesh_count].translation = translation;
    meshes[mesh_count].rotation = rotation;

    mesh_count++;
}

void load_mesh_obj_data(mesh_t* mesh, const char* obj_filename)
{
    FILE* file;
    // Linux:
    // file = fopen(filename, "r");
    if (fopen_s(&file, obj_filename, "r") == 0)
    {
        char line[1024];

        std::vector<tex2_t> texcoords;

        while (fgets(line, 1024, file))
        {
            // Vertex information
            if (strncmp(line, "v ", 2) == 0) 
            {
                vec3_t vertex;
                sscanf_s(line, "v %f %f %f", &vertex.x, &vertex.y, &vertex.z);
                mesh->vertices.push_back(vertex);
            }
            // Texture coordinate information
            if (strncmp(line, "vt ", 3) == 0) 
            {
                tex2_t texcoord;
                sscanf_s(line, "vt %f %f", &texcoord.u, &texcoord.v);
                texcoords.push_back(texcoord);
            }
            // Face information
            if (strncmp(line, "f ", 2) == 0) 
            {
                int vertex_indices[3];
                int texture_indices[3];
                int normal_indices[3];
                sscanf_s(line, "f %d/%d/%d %d/%d/%d %d/%d/%d",
                    &vertex_indices[0], &texture_indices[0], &normal_indices[0],
                    &vertex_indices[1], &texture_indices[1], &normal_indices[1],
                    &vertex_indices[2], &texture_indices[2], &normal_indices[2]
                );
                face_t face = {
                    .a = vertex_indices[0],
                    .b = vertex_indices[1],
                    .c = vertex_indices[2],
                    .a_uv = texcoords[texture_indices[0] - 1],
                    .b_uv = texcoords[texture_indices[1] - 1],
                    .c_uv = texcoords[texture_indices[2] - 1],
                    .color = 0xFFFFFFFF
                };
                mesh->faces.push_back(face);
            }
        }

        fclose(file);
    }
}

void load_mesh_png_data(mesh_t* mesh, const char* png_filename)
{
    std::vector<unsigned char> image; // The raw pixels, 4 bytes per pixel.
    unsigned char* png_texture = NULL;
    unsigned texture_width = 64;
    unsigned texture_height = 64;

    unsigned error = lodepng::decode(image, texture_width, texture_height, png_filename);

    mesh->texture = new lodepng_texture_t;

    if (mesh->texture)
    {
        mesh->texture->width = texture_width;
        mesh->texture->height = texture_height;

        // Convert 4 bytes per pixel into matrix.
        uint32_t* png_texture = new uint32_t[image.size()];
        int pointer = 0;
        for (size_t i = 0; i < image.size()-4 && pointer < image.size(); i+=4)
        {
            png_texture[pointer++] = (uint32_t)(
                (uint32_t)image[i] |
                (uint32_t)image[i+1] << 8 |
                (uint32_t)image[i+2] << 16 |
                (uint32_t)image[i+3] << 24);
        }
        mesh->texture->png_texture = png_texture;
    }
}

int get_num_meshes(void)
{
    return mesh_count;
}

mesh_t* get_mesh(int index)
{
    return &meshes[index];
}

void rotate_mesh_x(int mesh_index, float angle)
{
    meshes[mesh_index].rotation.x += angle;
}

void rotate_mesh_y(int mesh_index, float angle)
{
    meshes[mesh_index].rotation.y += angle;
}

void rotate_mesh_z(int mesh_index, float angle)
{
    meshes[mesh_index].rotation.z += angle;
}

void free_meshes(void)
{
    for (int i = 0; i < mesh_count; i++) 
    {
        free_texture(meshes[i].texture);
        meshes[i].faces.clear();
        meshes[i].vertices.clear();
    }
}