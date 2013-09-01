#version 400

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 color;

flat out vec4 out_color;

void main(void)
{
    out_color = color;
    gl_Position = position;
}
