#shader VERTEX

#version 430 core

layout (location = 0) in vec3 pos;
layout (location = 1) in int texId;
layout (location = 2) in int uvId;

const float g_fadeFactors[6] = {
    0.1,
    0.2,
    0.3,
    0.4,
    0.5,
    0.6
};

const g_texCoords[4] = {
    vec2(0, 0),
    vec2(0, 1),
    vec2(1, 0),
    vec2(1, 1)
};

uniform mat4 u_projection;
uniform mat4 u_view;

uniform vec3 u_chunkPos;

out vec3 frag_texCoords;

void main()
{
    gl_Position = u_projection * u_view * vec4(u_chunkPos + pos, 1.0);
    fragment_color = vec3(g_texCoords[]);
}

#shader GEOMETRY
#shader FRAGMENT

#version 430

in vec3 frag_texCoords;
out vec4 o_color;

uniform sampler2DArray u_textureArray;

void main()
{
    o_color = vec4(texture(u_textureArray, frag_texCoords), 1.0);
}
