#version 440

layout(location = 1) in vec3 vert_color;
layout(location = 0) out vec4 frag_color;

void main()
{
    frag_color = vec4(vert_color, 1.0);
}