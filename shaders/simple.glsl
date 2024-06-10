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

vec3 LinearToSRGB(in vec3 color)
{
    vec3 ret;
    if (any(greaterThan(color, vec3(0.0031308)))) ret = 1.055 * pow(color, vec3(1.0 / 2.4)) - vec3(0.055);
    if (color.r <= 0.0031308) { ret.r = color.r < 0.0 ? color.r : color.r * 12.92; }
    if (color.g <= 0.0031308) { ret.g = color.g < 0.0 ? color.g : color.g * 12.92; }
    if (color.b <= 0.0031308) { ret.b = color.b < 0.0 ? color.b : color.b * 12.92; }
    return ret;
}

void main()
{
    vec4 color = vec4(texture(u_texture, frag_texCoords).rgb, 1.0f);
    o_color = color; //vec4(texture(u_texture, frag_texCoords).rgb, color.a);
}
