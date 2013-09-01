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
    glBufferData(GL_ARRAY_BUFFER, 10000 * sizeof(vertex), NULL, GL_DYNAMIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 10000 * sizeof(short), NULL, GL_DYNAMIC_DRAW);
}

void core_gl_fetch_data(core_t *core_data)
{
    GLenum err;
    gl_data_t* gl_data = &(core_data->graphics_data.gl_data);
    rdr_t* render_data;

    if (render_queue_dequeue(&render_data, core_data->graphics_data.window_data.current_render_queue))
    {
        core_data->graphics_data.gl_data.current_render_data = render_data;
        convert_to_glcoordspace(render_data->objects[0]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, render_data->objects[0]->vertex_count * sizeof(vertex), render_data->objects[0]->vertices);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(offsetof(vertex, coord)));
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(offsetof(vertex, color)));
        gl_data->indices_count = render_data->objects[0]->indices_count;
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, render_data->objects[0]->indices_count * sizeof(short), render_data->objects[0]->indices);
        glUseProgram(gl_data->program_id[0]);
        if ((err = glGetError()) != GL_NO_ERROR)
            fprintf (stderr, "Error %s\n", gluGetString(err));
    }
}

void core_gl_draw(core_t *core_data)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawElements(GL_TRIANGLES, core_data->graphics_data.gl_data.indices_count, GL_UNSIGNED_SHORT, (void*)0);
}
