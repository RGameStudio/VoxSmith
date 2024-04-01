#shader VERTEX

#version 430 core

layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec2 in_texCoords;

out vec2 frag_texCoords;

void main()
{
    frag_texCoords = in_texCoords;
    gl_Position = vec4(in_pos, 1.0f);
}

#shader GEOMETRY
#shader FRAGMENT

#version 430 core

in vec2 frag_texCoords;
out vec4 o_color;

uniform sampler2D u_texture;

void main()
{
    o_color = vec4(texture(u_texture, frag_texCoords).rgb, 1.0f);
}
