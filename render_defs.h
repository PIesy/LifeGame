#ifndef RENDER_DEFS_H
#define RENDER_DEFS_H

#include <GL/glew.h>

typedef struct Vertex
{
    float coord[4];
    float color[4];
//    float normal[4];
} vertex;

typedef struct Object
{
    vertex* vertices;
    unsigned short* indices;
    short coord_space;
    short vertex_count;
    short indices_count;
    short converted;
    float v_coord[3];
} obj_t;

typedef struct Render_data
{
    obj_t** objects;
    short* asociation;
    short draw_calls;
} rdr_t;
#endif // RENDER_DEFS_H
