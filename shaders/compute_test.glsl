#version 430 core

layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(rgba32f, binding = 0) uniform image2D imgOutput;

uniform vec3 g_eyePos;
uniform float g_focalLength;

float lerp(float a, float b, float t)
{
    return a * (1.0f - t) + b * t;
}

float lerp(vec3 a, vec3 b, float t)
{
    return a * (1.0f - t) + b * t;
}

vec3 getRayColor(vec3 ray);
float hitSphere(vec3 center, float radius, vec3 ray);

void main() 
{
    ivec2 texelCoord = ivec2(gl_GlobalInvocationID.xy);

    vec4 value = vec4(0.0f, 0.0f, 0.0f, 1.0f);
    value.x = float(texelCoord.x) / gl_NumWorkGroups.x;
    value.y = float(texelCoord.y) / gl_NumWorkGroups.y;

    vec3 ray = vec3(value.xy, -g_focalLength) - (g_eyePos);

    value = vec4(getRayColor(ray), 1.0f);

    imageStore(imgOutput, texelCoord, value);
}

float hitSphere(vec3 center, float radius, vec3 ray)
{
    vec3 oc = g_eyePos - center;
    float a = dot(ray, ray);
    float b = 2.0f * dot(ray, oc);
    float c = dot(oc, oc) - radius * radius;

    float discriminant = b*b - 4*a*c;

    if (discriminant < 0) 
    {
        return -1.0;
    } 
    else 
    {
        return (-b - sqrt(discriminant) ) / (2.0f*a);
    }
}

vec3 getVecAtT(vec3 v, float t)
{
    return normalize(v) * t;
}

vec3 getRayColor(vec3 ray)
{
    float t = hitSphere(vec3(0.5f, 0.5f, -g_focalLength), 0.2f, ray);
    if (t > 0.0f)
    {
        vec3 N = normalize(getVecAtT(ray, t) - vec3(0.5f, 0.5f, -g_focalLength));
        return 0.5f * vec3(N.x + 1.0f, N.y + 1.0f, N.z + 1.0f);
    }

    vec3 rayn = normalize(ray);
    float a = 0.5f * (rayn.y + 1.0f);
    return lerp(vec3(1.0f, 1.0f, 1.0f), vec3(0.5f, 0.7f, 1.0f), a);
}

