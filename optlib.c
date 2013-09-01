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
