#version 430 core

layout (local_size_x = 10, local_size_y = 10, local_size_z = 1) in;

layout(rgba32f, binding = 0) uniform image2D imgOutput;

uniform vec3 g_eyePos;
uniform float g_focalLength;

struct Ray
{
    vec3 origin;
    vec3 dir;
};

struct Sphere
{
    vec3 center;
    float radius;
};

struct HitRecord
{
    float t;
    vec3 pos;
    vec3 normal;
};

vec3 pixelColor(Ray ray);

const float vHeight = 2.0f;

void main() 
{
    ivec2 texelCoord = ivec2(gl_GlobalInvocationID.xy);

        
    ivec2 dims = imageSize(imgOutput);
    float aspectRatio = float(dims.x) / float(dims.y);
    float vWidth = vHeight * aspectRatio;

    vec3 lowerLeft = vec3(-aspectRatio, -1.0f, -g_focalLength);

    vec3 viepowrt_u = vec3(vWidth, 0.0f, 0);
    vec3 viepowrt_v = vec3(0.0f, vHeight, 0);

    float du = float(texelCoord.x) / dims.x;
    float dv = float(texelCoord.y) / dims.y;

    Ray ray;
    ray.origin = g_eyePos;
    ray.dir = lowerLeft + du * viepowrt_u + dv * viepowrt_v;

    vec4 value = vec4(pixelColor(ray), 1.0f);

    imageStore(imgOutput, texelCoord, value);
}

float lerp(vec3 a, vec3 b, float t)
{
    return a * (1.0f - t) + b * t;
}

vec3 getVecAtT(Ray ray, float t)
{
    return ray.origin + ray.dir * t;
}

bool hitSphere(Sphere sphere, Ray ray, float tMin, float tMax, out HitRecord hRecord)
{
    const vec3 oc = g_eyePos - sphere.center;
    const float a = pow(length(ray.dir), 2);
    const float h = dot(ray.dir, oc);
    const float c = pow(length(oc), 2) - sphere.radius * sphere.radius;

    const float discriminant = h*h - a*c;
    if (discriminant > 0.0f)
    {
        float t = (-h - sqrt(discriminant)) / a;
        if (t < tMin || t > tMax)
        {
            t = (-h + sqrt(discriminant)) / a;
            if (t < tMin || t > tMax)
            {
                return false;
            }
        }

        hRecord.t = t;
        hRecord.pos = getVecAtT(ray, hRecord.t);
        hRecord.normal = (hRecord.pos - sphere.center) / sphere.radius;
        return true;
    }

    return false;
}

const float g_minRayLen = 0.0f;
const float g_maxRayLen = 1000.0f;

vec3 pixelColor(Ray ray)
{
    Sphere s1;
    HitRecord rec1;
    s1.center = vec3(0.0f, 0.0f, -g_focalLength);
    s1.radius = 0.5f;

    Sphere s2;
    HitRecord rec2;
    s2.center = vec3(0.0f, -100.5f, -g_focalLength);
    s2.radius = 100.0f;

    bool hit = hitSphere(s1, ray, g_minRayLen, g_maxRayLen, rec1);
    if (hit)
    {
        return 0.5f * (rec1.normal + vec3(1.0f));
    }

    hit = hitSphere(s2, ray, g_minRayLen, g_maxRayLen, rec2);
    if (hit)
    {
        return 0.5f * (rec2.normal + vec3(1.0f));
    }

    vec3 rayn = normalize(ray.dir);
    float a = 0.5f * (rayn.y + 1.0f);
    return lerp(vec3(1.0f, 1.0f, 1.0f), vec3(0.5f, 0.7f, 1.0f), a);
}
