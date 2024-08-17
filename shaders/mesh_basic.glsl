#shader VERTEX

#version 430 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 color;
layout (location = 2) in int faceId;

const float g_fadeFactors[6] = {
    0.1,
    0.2,
    0.3,
    0.4,
    0.5,
    0.6
};

uniform mat4 u_projection;
uniform mat4 u_view;

uniform vec3 u_chunkPos;

out vec3 fragment_color;

void main()
{
    gl_Position = u_projection * u_view * vec4(u_chunkPos + pos, 1.0);
    fragment_color = color;
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
