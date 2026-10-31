#shader VERTEX

#version 430 core

layout (location = 0) in uint portion1;
layout (location = 1) in uint portion2;

const vec2 g_texCoords[4] = {
    vec2(0, 0),
    vec2(1, 0),
    vec2(0, 1),
    vec2(1, 1)
};

const float g_occlusionValues[4] = {
    0.1,
    0.2,
    0.6,
    1.0
};

uniform mat4 u_projection;
uniform mat4 u_view;

uniform vec3 u_chunkPos;

out vec3 frag_data;
out float frag_occlusionFactor;

void main()
{
    const vec3 pos = u_chunkPos + vec3(
        portion1 & 63,
        (portion1 >> 6) & 63,
        (portion1 >> 12) & 63 
    );

    const uint uvId = (portion1 >> 18) & 3;
    const uint texId = (portion1 >> 20) & 63;

    const uint occlusionCase = (portion1 >> 28) & 7;
    
    gl_Position = u_projection * u_view * vec4(pos, 1.0);

    frag_data = vec3(g_texCoords[uvId], texId);
    frag_occlusionFactor = g_occlusionValues[occlusionCase];
}

#shader GEOMETRY
#shader FRAGMENT

#version 430

in vec3 frag_data;
in float frag_occlusionFactor;

uniform bool u_bUseAO;

out vec4 o_color;

uniform sampler2DArray u_textureArray;

void main()
{
    float factorAO = 1.0;
    if (u_bUseAO)
    {
        factorAO = frag_occlusionFactor;
    }
    
    o_color = factorAO * texture(u_textureArray, frag_data);
}
