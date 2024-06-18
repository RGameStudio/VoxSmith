#shader VERTEX

#version 430 core

layout (location = 0) in vec3 pos;

uniform mat4 u_projection;
uniform mat4 view;

void main()
{
    gl_Position = vec4(pos, 1.0f) * u_view * u_projection;
}

#shader GEOMETRY
#shader FRAGMENT

#version 430

out vec4 o_color;

void main()
{
    o_color = vec4(0.0f, 1.0f, 0.0f, 1.0f);
}
