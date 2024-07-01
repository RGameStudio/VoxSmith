#shader VERTEX

#version 430 core

layout (location = 0) in vec3 pos;

uniform mat4 u_projection;
uniform mat4 u_view;

uniform vec3 u_chunkPos;

void main()
{
    gl_Position = u_projection * u_view * vec4(u_chunkPos + pos, 1.0);
}

#shader GEOMETRY
#shader FRAGMENT

#version 430

out vec4 o_color;

void main()
{
    o_color = vec4(0.0, 1.0, 0.0, 1.0);
}
