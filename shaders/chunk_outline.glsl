#shader VERTEX

#version 430 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 color;
layout (location = 2) in int faceId;

uniform mat4 u_projection;
uniform mat4 u_view;

uniform vec3 u_chunkPos;

out vec3 fragment_color;

void main()
{
    gl_Position = u_projection * u_view * vec4(u_chunkPos + pos, 1.0);
    fragment_color = vec3(1.0, 0.0, 0.0);
}

#shader GEOMETRY
#shader FRAGMENT

#version 430

in vec3 fragment_color;
out vec4 o_color;

void main()
{
    o_color = vec4(fragment_color, 1.0);
}
