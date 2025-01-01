#shader VERTEX

#version 450 core

layout (location = 0) in int posData;
layout (location = 1) in uint texColLightData;

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
    const int x = posData & 0x1F;
    const int y = (posData >> 5) & 0x1F;
    const int z = (posData >> 10) & 0x1F;
    
    const vec3 color = vec3(
        ((texColLightData) & 255) / 255.0,
        ((texColLightData >> 8) & 255) / 255.0,
        ((texColLightData >> 16) & 255) / 255.0 
    );

    const uint faceId = (texColLightData >> 24) & 255;

    gl_Position = u_projection * u_view * vec4(u_chunkPos + vec3(x, y, z), 1.0);
    fragment_color = vec3(0, 0.5, 0);
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
