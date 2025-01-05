#shader VERTEX

#version 450 core

layout (location = 0) in uint portion1;
layout (location = 1) in uint portion2;

uniform mat4 u_projection;
uniform mat4 u_view;

uniform vec3 u_chunkPos;

out vec3 fragment_color;

const float g_fadeFactors[6] = {
    0.1,
    0.2,
    0.3,
    0.4,
    0.5,
    0.6
};

void main()
{
    const vec3 pos = u_chunkPos + vec3(
        portion1 & 63,
        (portion1 >> 6) & 63,
        (portion1 >> 12) & 63 
    );
    
    const vec3 color = vec3(
        ((portion1 >> 18) & 255) / 255.0,
        ((((portion2 & 3) << 6) | ((portion1 >> 26) & 63))) / 255.0,
        ((portion2 >> 2) & 255) / 255.0 
    );

    const uint faceId = (portion2 >> 10) & 7;

    gl_Position = u_projection * u_view * vec4(pos, 1.0);
    fragment_color = g_fadeFactors[faceId] * color;
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
