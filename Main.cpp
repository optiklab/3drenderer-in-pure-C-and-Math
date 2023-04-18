#include <iostream>
#include <cstdlib>
//#include <stdio.h>
#include "Display.h"
#include "Mesh.h"
#include "Camera.h"
#include "Clipping.h"
#include "Matrix.h"
#include "Light.h"
#include "lodepng.h"
#include "Texture.h"

bool is_running = false;

#define MAX_TRIANGLES_PER_MESH 10000
triangle_t static_triangles_to_render[MAX_TRIANGLES_PER_MESH];
int num_static_triangles_to_render = 0;

///////////////////////////////////////////////////////////////////////////////
// Declaration of our global transformation matrices
///////////////////////////////////////////////////////////////////////////////
vec3_t origin = { 0, 0, 0 };
mat4_t world_matrix;
mat4_t proj_matrix;
mat4_t view_matrix;

//float fov_scaling_factor = 640;

uint32_t previous_frame_time = 0;
float delta_time = 0;

void setup(void)
{
    // Initialize render mode and triangle culling method
    set_render_method(RENDER_WIRE);
    set_cull_method(CULL_BACKFACE);

    // Initialize the scene light direction
    init_light(vec3_new(0, 0, 1));

    // Initialize the perspective projection matrix
    float aspect_y = (float)get_window_height() / (float)get_window_width();
    float aspect_x = (float)get_window_width() / (float)get_window_height();
    float fov_y = M_PI / 3.0; // the same as 180/3, or 60deg
    float fov_x = atan(tan(fov_y / 2) * aspect_x) * 2;
    float znear = 0.1;
    float zfar = 100.0;
    proj_matrix = mat4_make_perspective(fov_y, aspect_y, znear, zfar);

    // Initialize frustum planes with a point and a normal
    init_frustum_planes(fov_x, fov_y, znear, zfar);

    //sphere
    //motorBike
    //cube
    //crab
    //drone
    //f117
    //f22
    //efa
    const char* runwayObj = "D:\\3dscene\\runway.obj";
    const char* f22Obj = "D:\\3dscene\\f22.obj";
    const char* efaObj = "D:\\3dscene\\efa.obj";
    const char* f117Obj = "D:\\3dscene\\f117.obj";

    const char* runwayPng = "D:\\3dscene\\runway.png";
    const char* f22Png = "D:\\3dscene\\f22.png";
    const char* efaPng = "D:\\3dscene\\efa.png";
    const char* f117Png = "D:\\3dscene\\f117.png";

    load_mesh(runwayObj, runwayPng, vec3_new(1, 1, 1), vec3_new(0, -1.5, +23), vec3_new(0, 0, 0));
    load_mesh(f22Obj, f22Png, vec3_new(1, 1, 1), vec3_new(0, -1.3, +5), vec3_new(0, -M_PI / 2, 0));
    load_mesh(efaObj, efaPng, vec3_new(1, 1, 1), vec3_new(-2, -1.3, +9), vec3_new(0, -M_PI / 2, 0));
    load_mesh(f117Obj, f117Png, vec3_new(1, 1, 1), vec3_new(+2, -1.3, +9), vec3_new(0, -M_PI / 2, 0));
}

///////////////////////////////////////////////////////////////////////////////
// Poll system events and handle keyboard input
///////////////////////////////////////////////////////////////////////////////
void process_input(void)
{
    SDL_Event event;
    while (SDL_PollEvent(&event)) 
    {
        switch (event.type)
    	{
        case SDL_QUIT:
            is_running = false;
            break;
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE)
            {
                is_running = false;
                break;
            }
            if (event.key.keysym.sym == SDLK_1)
            {
                set_render_method(RENDER_WIRE_VERTEX);
                break;
            }
            if (event.key.keysym.sym == SDLK_2)
            {
                set_render_method(RENDER_WIRE);
                break;
            }
            if (event.key.keysym.sym == SDLK_3)
            {
                set_render_method(RENDER_FILL_TRIANGLE);
                break;
            }
            if (event.key.keysym.sym == SDLK_4)
            {
                set_render_method(RENDER_FILL_TRIANGLE_WIRE);
                break;
            }
            if (event.key.keysym.sym == SDLK_5)
            {
                set_render_method(RENDER_TEXTURED);
                break;
            }
            if (event.key.keysym.sym == SDLK_6) 
            {
                set_render_method(RENDER_TEXTURED_WIRE);
                break;
            }
            if (event.key.keysym.sym == SDLK_c)
            {
                set_cull_method(CULL_BACKFACE);
                break;
            }
            if (event.key.keysym.sym == SDLK_x)
            {
                set_cull_method(CULL_NONE);
                break;
            }
            if (event.key.keysym.sym == SDLK_w)
            {
                rotate_camera_pitch(+3.0 * delta_time); // Radians * seconds
                break;
            }
            if (event.key.keysym.sym == SDLK_s)
            {
                rotate_camera_pitch(-3.0 * delta_time); // Radians * seconds
                break;
            }
            if (event.key.keysym.sym == SDLK_RIGHT)
            {
                rotate_camera_yaw(+1.0 * delta_time); // Radians * seconds
                break;
            }
            if (event.key.keysym.sym == SDLK_LEFT)
            {
                rotate_camera_yaw(-1.0 * delta_time); // Radians * seconds
                break;
            }
            if (event.key.keysym.sym == SDLK_UP)
            {
                update_camera_forward_velocity(vec3_mul(get_camera_direction(), 5.0 * delta_time));
                update_camera_position(vec3_add(get_camera_position(), get_camera_forward_velocity()));
                break;
            }
            if (event.key.keysym.sym == SDLK_DOWN)
            {
                update_camera_forward_velocity(vec3_mul(get_camera_direction(), 5.0 * delta_time));
                update_camera_position(vec3_sub(get_camera_position(), get_camera_forward_velocity()));
                break;
            }
            break;
        }
    }
}

void keepStableFps(void)
{
    // CPU consuming way to implement Delays.
    //while (!SDL_TICKS_PASSED(SDL_GetTicks(), previous_frame_time + FRAME_TARGET_TIME));

    // Better way to implement Delays.
    // Wait some time until the reach the target frame time in milliseconds
    int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - previous_frame_time);
    // Only delay execution if we are running too fast
    if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME)
    {
        SDL_Delay(time_to_wait);
    }
    // This is still not ideal. Right now, if we increase our FPS the game objects will move faster,
    // and if we decrease the FPS our game objects will run slower.
    // We need "variable delta-time" that will help us get framerate-independent movement.
    delta_time = (SDL_GetTicks() - previous_frame_time) / 1000.0; // ( Milliseconds - Milliseconds ) / 1000 => SECONDS

    previous_frame_time = SDL_GetTicks();
}

// Unlink FPS from the number of updates
float setTransformation(float value)
{
    return value * delta_time;
}

///////////////////////////////////////////////////////////////////////////////
// Process the graphics pipeline stages for all the mesh triangles
///////////////////////////////////////////////////////////////////////////////
// +-------------+
// | Model space |  <-- original mesh vertices
// +-------------+
// |   +-------------+
// `-> | World space |  <-- multiply by world matrix
//     +-------------+
//     |   +--------------+
//     `-> | Camera space |  <-- multiply by view matrix
//         +--------------+
//         |    +------------+
//         `--> |  Clipping  |  <-- clip against the six frustum planes
//              +------------+
//              |    +------------+
//              `--> | Projection |  <-- multiply by projection matrix
//                   +------------+
//                   |    +-------------+
//                   `--> | Image space |  <-- apply perspective divide
//                        +-------------+
//                        |    +--------------+
//                        `--> | Screen space |  <-- ready to render
//                             +--------------+
///////////////////////////////////////////////////////////////////////////////
void process_graphics_pipeline_stages(mesh_t* mesh)
{
    // Create scale, rotation, and translation matrices that will be used to multiply the mesh vertices 
    mat4_t scale_matrix = mat4_make_scale(mesh->scale.x, mesh->scale.y, mesh->scale.z);
    mat4_t translation_matrix = mat4_make_translation(mesh->translation.x, mesh->translation.y, mesh->translation.z);
    mat4_t rotation_matrix_x = mat4_make_rotation_x(mesh->rotation.x);
    mat4_t rotation_matrix_y = mat4_make_rotation_y(mesh->rotation.y);
    mat4_t rotation_matrix_z = mat4_make_rotation_z(mesh->rotation.z);

    // Update camera look at target to create view matrix
    vec3_t target = get_camera_lookat_target();
    vec3_t up_direction = vec3_new(0, 1, 0);
    view_matrix = mat4_look_at(get_camera_position(), target, up_direction);

    // Loop all triangle faces of our mesh
    for (size_t i = 0; i < mesh->faces.size(); i++)
    {
        face_t mesh_face = mesh->faces[i];

        vec3_t face_vertices[3];
        face_vertices[0] = mesh->vertices[mesh_face.a - 1];
        face_vertices[1] = mesh->vertices[mesh_face.b - 1];
        face_vertices[2] = mesh->vertices[mesh_face.c - 1];

        //////////////////////////////////////////////////////////////////////////////////
        // Apply transformations (sclaing, factoring and rotations)
        ///////////////////////////////////////////////////////////////////////////////////
        vec4_t transformed_vertices[3];

        // Loop all three vertices of this current face and apply transformations
        for (int j = 0; j < 3; j++)
        {
            vec4_t transformed_vertex = vec4_from_vec3(face_vertices[j]);

            // ORDER OF OPERATIONS MATTERS! It HAS to go in order: Scale -> Rotate -> Translate.
            // This is because MATRIX operations are NOT COMMUTATIVE (A*B!=B*A): [T]*[R]*[S]*v
            //
            // Create a Single World Matrix combining scale, rotation, and translation matrices
            world_matrix = mat4_identity();
            world_matrix = mat4_mul_mat4(scale_matrix, world_matrix);
            world_matrix = mat4_mul_mat4(rotation_matrix_z, world_matrix);
            world_matrix = mat4_mul_mat4(rotation_matrix_y, world_matrix);
            world_matrix = mat4_mul_mat4(rotation_matrix_x, world_matrix);
            world_matrix = mat4_mul_mat4(translation_matrix, world_matrix);

            // Use a World Matrix to make SCALING, ROTATION and TRANSLATION in combination.
            // Multiply the world matrix by the original vector.
            transformed_vertex = mat4_mul_vec4(world_matrix, transformed_vertex);

            // Multiply the view matrix by the vector to transform the scene to camera space
            transformed_vertex = mat4_mul_vec4(view_matrix, transformed_vertex);

            // Save transformed vertex in the array of transformed vertices
            transformed_vertices[j] = transformed_vertex;
        }

        //////////////////////////////////////////////////////////////////////////////////
        // BACKFACE CULLING
        // Check backface culling to hide unnecessary triangles.
        //////////////////////////////////////////////////////////////////////////////////

        // Calculate the triangle face normal
        vec3_t face_normal = get_triangle_normal(transformed_vertices);

        if (should_cull_backface())
        {
            // Find the vector between vertex A in the triangle and the camera origin
            vec3_t vector_a = vec3_from_vec4(transformed_vertices[0]);
            vec3_t camera_ray = vec3_sub(origin, vector_a);

            // Calculate how aligned the camera ray is with the face normal (using dot product)
            float dot_normal_camera = vec3_dot(face_normal, camera_ray);

            // Bypass the triangles that are looking away from the camera
            if (dot_normal_camera < 0)
            {
                continue;
            }
        }

        ///////////////////////////////////////////////////////////////////////////////////////////////
        // CLIPPING
        ///////////////////////////////////////////////////////////////////////////////////////////////

        // Create a polygon from the original transformed triangle to be clipped
        polygon_t polygon = polygon_from_triangle(
            vec3_from_vec4(transformed_vertices[0]),
            vec3_from_vec4(transformed_vertices[1]),
            vec3_from_vec4(transformed_vertices[2]),
            mesh_face.a_uv,
            mesh_face.b_uv,
            mesh_face.c_uv
        );

        // Clip the polygon and returns a new polygon with potential new vertices
        clip_polygon(&polygon);

        // Break the clipped polygon apart back into individual triangles
        triangle_t triangles_after_clipping[MAX_NUM_POLY_TRIANGLES];
        int num_triangles_after_clipping = 0;

        triangles_from_polygon(&polygon, triangles_after_clipping, &num_triangles_after_clipping);

        // Loops all the assembled triangles after clipping
        for (int t = 0; t < num_triangles_after_clipping; t++)
        {
            triangle_t triangle_after_clipping = triangles_after_clipping[t];

            ///////////////////////////////////////////////////////////////////////////////////////////////
            // PROJECTION
            // Project triangles that are on front
            ///////////////////////////////////////////////////////////////////////////////////////////////
            vec4_t projected_points[3];

            // Loop all three vertices of this current face and apply transformations
            for (int j = 0; j < 3; j++)
            {
                // Project the current vertex using a perspective projection matrix
                projected_points[j] = mat4_mul_vec4(proj_matrix, triangle_after_clipping.points[j]);
                //projected_points[j] = mat4_mul_vec4_project(proj_matrix, transformed_vertices[j]);
                //projected_points[j] = project_v2(vec3_from_vec4(transformed_vertices[j]));

                // Perform perspective divide
                if (projected_points[j].w != 0) {
                    projected_points[j].x /= projected_points[j].w;
                    projected_points[j].y /= projected_points[j].w;
                    projected_points[j].z /= projected_points[j].w;
                }

                // SCALE into the view
                projected_points[j].x *= (get_window_width() / 2.0);
                projected_points[j].y *= (get_window_height() / 2.0);

                // Flip vertically since the y values of the 3D mesh grow bottom->up and in screen space y values grow top->down
                projected_points[j].y *= -1;

                // TRANSLATE the projected points to the middle of the screen
                projected_points[j].x += (get_window_width() / 2.0);
                projected_points[j].y += (get_window_height() / 2.0);
            }

            // Calculate DOT PRODUCT: the shade intensity based on how aliged is the face normal and the opposite of the light direction.
            // Why opposite? Because the VIEWER (we're) sees the reflected light coming FROM the object, not into it.
            // But we calculate DOT PRODUCT with light directed TO the object. So, finally, we need to use the opposite value.
            float light_intensity_factor = -vec3_dot(face_normal, get_light_direction());

            // Calculate the triangle color based on the light angle
            uint32_t triangle_color = light_apply_intensity(mesh_face.color, light_intensity_factor);

            triangle_t projected_triangle_to_render = {
                .points = {
                    { projected_points[0].x, projected_points[0].y, projected_points[0].z, projected_points[0].w },
                    { projected_points[1].x, projected_points[1].y, projected_points[1].z, projected_points[1].w },
                    { projected_points[2].x, projected_points[2].y, projected_points[2].z, projected_points[2].w },
                },
                .texcoords = {
                    { triangle_after_clipping.texcoords[0].u, triangle_after_clipping.texcoords[0].v },
                    { triangle_after_clipping.texcoords[1].u, triangle_after_clipping.texcoords[1].v },
                    { triangle_after_clipping.texcoords[2].u, triangle_after_clipping.texcoords[2].v }
                },
                .color = triangle_color,
                .texture = mesh->texture
            };

            if (num_static_triangles_to_render < MAX_TRIANGLES_PER_MESH)
            {
                static_triangles_to_render[num_static_triangles_to_render++] = projected_triangle_to_render;
            }
        }
    }
}

void updateShape(void)
{
    num_static_triangles_to_render = 0;

    // Loop all the meshes of our scene
    for (int mesh_index = 0; mesh_index < get_num_meshes(); mesh_index++)
    {
        mesh_t* mesh = get_mesh(mesh_index);

        // Change the mesh scale, rotation, and translation values per animation frame
        // rotate_mesh_x(mesh_index, mesh->rotation_velocity.x * delta_time);
        // rotate_mesh_y(mesh_index, mesh->rotation_velocity.y * delta_time);
        // rotate_mesh_z(mesh_index, mesh->rotation_velocity.z * delta_time);

        // Process graphics pipeline stages for each mesh
        process_graphics_pipeline_stages(mesh);
    }
}

void update(void)
{
    keepStableFps();

    updateShape();
}

void render_shape(void)
{
    // Draw cube mesh. Loop all projected triangles and render them
    int trianglesNum = num_static_triangles_to_render;

    for (int i = 0; i < trianglesNum; i++)
    {
        triangle_t triangle = static_triangles_to_render[i];

        if (should_render_filled_triangles())
        {
            draw_filled_triangle(
                triangle.points[0].x, triangle.points[0].y, triangle.points[0].z, triangle.points[0].w, // vertex A
                triangle.points[1].x, triangle.points[1].y, triangle.points[1].z, triangle.points[1].w, // vertex B
                triangle.points[2].x, triangle.points[2].y, triangle.points[2].z, triangle.points[2].w, // vertex C
                triangle.color
            );
        }
        // Draw textured triangle
        if (should_render_textured_triangles()) 
        {
            draw_textured_triangle(
                triangle.points[0].x, triangle.points[0].y, triangle.points[0].z, triangle.points[0].w, triangle.texcoords[0].u, triangle.texcoords[0].v, // vertex A
                triangle.points[1].x, triangle.points[1].y, triangle.points[1].z, triangle.points[1].w, triangle.texcoords[1].u, triangle.texcoords[1].v, // vertex B
                triangle.points[2].x, triangle.points[2].y, triangle.points[2].z, triangle.points[2].w, triangle.texcoords[2].u, triangle.texcoords[2].v, // vertex C
                triangle.texture
            );
        }

        if (should_render_wireframe())
        {
            // Draw unfilled triangle
            draw_triangle(
                triangle.points[0].x, triangle.points[0].y, // vertex A
                triangle.points[1].x, triangle.points[1].y, // vertex B
                triangle.points[2].x, triangle.points[2].y, // vertex C
                0xFFFFFFFF
            );
        }

        if (should_render_wire_vertex())
        {
            // Draw triangle vertex points
            draw_rect(triangle.points[0].x, triangle.points[0].y, 3, 3, 0xFFFFFF00);
            draw_rect(triangle.points[1].x, triangle.points[1].y, 3, 3, 0xFFFFFF00);
            draw_rect(triangle.points[2].x, triangle.points[2].y, 3, 3, 0xFFFFFF00);
        }
    }
}

void render(void)
{
    // Clear all the arrays to get ready for the next frame
    clear_color_buffer(0xFF000000);
    clear_z_buffer();

    draw_grid();
    //draw_grid_dots();
    //draw_rect(300, 200, 300, 150, 0xFFFF00FF);

    render_shape();

    render_color_buffer();
}

void free_resources(void)
{
    free_meshes();
}

int main(void)
{
    is_running = initialize_window();

    setup();

    while (is_running)
    {
        process_input();
        update();
        render();
    }

    destroy_window();
    free_resources();

    return 0;
}