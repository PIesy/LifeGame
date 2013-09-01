#include "optlib.h"

GLuint create_gl_program(GLuint *shaders, short count, GLboolean delete_shaders)
{
    short i;
    GLuint program_id;

    program_id = glCreateProgram();
    for (i = 0; i < count; i++)
        glAttachShader(program_id, shaders[i]);
    glLinkProgram(program_id);
    if (delete_shaders == GL_TRUE)
        for (i = 0; i < count; i++)
            glDeleteShader(shaders[i]);
    return program_id;
}

GLuint create_shader(char *src_path, GLenum type)
{
    FILE* source;
    char* shader_buff;
    int length;
    GLuint shader_id;

    if (!(source = fopen(src_path,"rb")))
    {
        fprintf(stderr,"File is unreachable or doesnt exist.\n");
        return  0;
    }
    fseek(source, 0, 2);
    length = ftell(source);
    freopen(src_path,"r",source);
    shader_buff = (char*)calloc(length + 1, sizeof(char));
    read_shader(source, shader_buff);
    shader_id = load_shader(shader_buff, type);
    fclose(source);
    return shader_id;
}

void read_shader(FILE* source, char *output)
{
    char temp[201];
    int max_str_length = 200, length = 0, offset = 0;

    while (1)
    {
        fgets(temp,max_str_length,source);
        if (feof(source))
            break;
        length = strlen(temp);
        strncpy(&(output[offset]),temp,length);
        offset += length;
    }
    output[offset] = '\0';
}

GLuint load_shader(char *shader, GLenum type)
{
    GLuint id;

    id = glCreateShader(type);
    glShaderSource(id, 1, &shader, NULL);
    glCompileShader(id);
    free(shader);
    return id;
}

void convert_to_glcoordspace(obj_t *object)
{
    int i = 0;

    if (!object->converted)
        for (i = 0; i < object->vertex_count; i++)
        {
            object->vertices[i].coord[0] = (object->vertices[i].coord[0] - 500)/500;
            object->vertices[i].coord[1] = (object->vertices[i].coord[1] - 300)/300;
            object->converted = 1;
        }
}

void append_to_object(obj_t *target, obj_t *source, float offset_x, float offset_y)
{
    int i = 0;

    for (i = 0; i < source->vertex_count; i++)
    {
        target->vertices[target->vertex_count + i] = source->vertices[i];
        target->vertices[target->vertex_count + i].coord[0] += offset_x;
        target->vertices[target->vertex_count + i].coord[1] += offset_y;
    }
    for (i = 0; i < source->indices_count; i++)
        target->indices[target->indices_count + i] = source->indices[i] + target->vertex_count;
    target->vertex_count += source->vertex_count;
    target->indices_count += source->indices_count;
}
