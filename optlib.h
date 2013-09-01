#ifndef OPTLIB_H
#define OPTLIB_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <GL/glew.h>
#include "render_defs.h"

void append_to_object(obj_t* target, obj_t* source, float offset_x, float offset_y);
void convert_to_glcoordspace(obj_t* object);
void destroy_render_data_object(rdr_t* render_data_obj);
GLuint create_gl_program(GLuint* shaders, short count, GLboolean delete_shaders);
void read_shader(FILE *source, char* output);
GLuint load_shader(char* shader, GLenum type);
GLuint create_shader(char* src_path, GLenum type);
#endif // OPTLIB_H
