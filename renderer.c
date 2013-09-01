#include "renderer.h"


void core_gl_init(core_t* core_data)
{
    gl_data_t* gl_data = &(core_data->graphics_data.gl_data);

    glGenBuffers(1,&(gl_data->VBO[0]));
    glGenBuffers(1,&(gl_data->IBO[0]));
    glGenVertexArrays(1, gl_data->VAO);
    glBindVertexArray(gl_data->VAO[0]);
    glBindBuffer(GL_ARRAY_BUFFER, gl_data->VBO[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gl_data->IBO[0]);
}

void core_gl_fetch_data(core_t *core_data)
{
    gl_data_t* gl_data = &(core_data->graphics_data.gl_data);
    rdr_t* render_data;

    if (render_queue_dequeue(&render_data, core_data->graphics_data.window_data.current_render_queue))
    {
        fprintf (stderr, "Indices %d %d %d %d %d %d\n", render_data->objects[0]->indices[0], render_data->objects[0]->indices[1], render_data->objects[0]->indices[2], render_data->objects[0]->indices[3], render_data->objects[0]->indices[4], render_data->objects[0]->indices[5]);
        glBufferData(GL_ARRAY_BUFFER, render_data->objects[0]->vertices_size, render_data->objects[0]->vertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(offsetof(vertex, coord)));
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(offsetof(vertex, color)));
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(short), render_data->objects[0]->indices, GL_STATIC_DRAW);
        glUseProgram(gl_data->program_id[0]);
    }
}

void core_gl_draw(core_t *core_data)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (void*)0);
}
