#version 430 core

layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(rgba32f, binding = 0) writeonly uniform image2D imgOutput;

uniform float g_focalLength;
uniform float g_screenWidth;
uniform float g_screenHeight;
uniform vec3 g_eyePos;

#define M_PI 3.1415926535897932384626433832795

struct Ray
{
    vec3 start;
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
    bool frontFace;
};

const int g_sphereCount = 2;

const Sphere[g_sphereCount] g_spheres = {
    Sphere(vec3(0.0f, 0.0f, -1.0f), 0.5f),
    Sphere(vec3(0.0f, -100.5f, -1.0f), 100.0f),
};

const int g_samples = 100;
const float g_samplesPkerPixel = 1.0f / float(g_samples);
const float g_rayMin = 0.0001f;
const float g_rayMax = 1000000.0f;

bool hitSphere(Sphere s, Ray r, const float tMin, const float tMax, out HitRecord record);
bool worldHit(Ray ray, const float tMin, const float tMax, out HitRecord record);
vec3 pixelColor(Ray ray);
vec3 rayAtT(Ray ray, float t);
vec3 lerp(vec3 a, vec3 b, const float t);
uint hash(uint x);
float random();
float randomInInterval(const float min, const float max);
vec3 randomVec();
vec3 randomUnitVec();
vec3 randomOnHemisphere(const vec3 normal);

void main()
{
    const ivec2 texelCoord = ivec2(gl_GlobalInvocationID.xy);

    const float viewportHeight = 2.0f;
    const float viewportWidth = viewportHeight * (g_screenWidth / g_screenHeight); 

    const vec3 viewportU = vec3(viewportWidth, 0, 0);
    const vec3 viewportV = vec3(0, viewportHeight, 0);

    const vec3 du = viewportU / g_screenWidth;
    const vec3 dv = viewportV / g_screenHeight;

    const vec3 viepowrtBottomLeft = 
        g_eyePos - vec3(0, 0, g_focalLength) - viewportU / 2 - viewportV / 2;

    const vec3 pixel00Loc = viepowrtBottomLeft + 0.5f * (du + dv);

    vec3 finalColor = vec3(0.0f);

    for (int i = 0; i < g_samples; i++)
    {
        vec3 pixelSample = pixel00Loc 
            + (texelCoord.x + random()) * du
            + (texelCoord.y + random()) * dv;
        
        Ray ray = Ray(g_eyePos, pixelSample - g_eyePos);
        finalColor += pixelColor(ray);
    }
    
    imageStore(imgOutput, texelCoord, vec4(finalColor, 1.0f) / g_samples);
}

vec3 lerp(vec3 a, vec3 b, const float t)
{
    return a * (1 - t) + b * t;
}

vec3 rayAtT(Ray ray, float t)
{
    return ray.start + ray.dir * t;
}

bool worldHit(Ray ray, const float tMin, const float tMax, out HitRecord record)
{
    bool hit = false;
    float closestSoFar = tMax;
    HitRecord temp;
    for (int iSphere = 0; iSphere < g_sphereCount; iSphere++)
    {
        if (hitSphere(g_spheres[iSphere], ray, tMin, closestSoFar, temp))
        {
            hit = true;
            closestSoFar = temp.t;
            record = temp;
        }
    }

    return hit;
}

vec3 skyColor(vec3 dir)
{
    vec3 v = normalize(dir);
    float t = 0.5f * (v.y + 1.0f);
    return lerp(vec3(1.0f), vec3(0.5f, 0.7f, 1.0f), t);
}

vec3 pixelColor(Ray ray)
{
    HitRecord record;    
    int depths = 50;
    float coeff = 1.0f;
    Ray tempRay = ray;

    for (int i = 0; i < depths; i++)
    {
        HitRecord record;
        if (worldHit(tempRay, g_rayMin, g_rayMax, record))
        {
            coeff *= 0.6f;
            tempRay = Ray(record.pos, record.normal + randomUnitVec());
            continue;
        }
        return coeff * skyColor(tempRay.dir);
    }

    return vec3(0.0f);
}

bool hitSphere(Sphere s, Ray r, const float tMin, const float tMax, out HitRecord record)
{
    const vec3 oc = s.center - r.start;

    const float a = dot(r.dir, r.dir);
    const float h = dot(r.dir, oc); 
    const float c = dot(oc, oc) - s.radius * s.radius;
    
    const float D = h * h - a * c;  
    if (D < 0.0f)
    {
        return false;
    }

    const float sqrtD = sqrt(D);

    float root = (h - sqrtD) / a;
    if (root <= tMin || tMax <= root)
    {
        root = (h + sqrtD) / a;
        if (root <= tMin || tMax <= root)
        {
            return false;
        }
    }

    record.t = root;
    record.pos = rayAtT(r, record.t);
    record.normal = (record.pos - s.center) / s.radius;
    if (dot(record.normal, r.dir) > 0)
    {
        record.normal = -record.normal;
    }

    return true;
}

uint hash(uint x) 
{
    x += (x << 10u);
    x ^= (x >>  6u);
    x += (x <<  3u);
    x ^= (x >> 11u);
    x += (x << 15u);
    return x;
}

uint currentRandomOffset = 0;

float random() 
{
    currentRandomOffset += 1;
    const uvec3 v = floatBitsToUint(vec3(gl_GlobalInvocationID.xy, currentRandomOffset));

    uint m = hash(v.x ^ hash(v.y) ^ hash(v.z));
    m &= 0x007FFFFFu;
    m |= 0x3F800000u;
    return uintBitsToFloat(m) - 1.0f;
}

float randomInInterval(const float min, const float max)
{
    return min + (max - min) * random();
}

vec3 randomVec()
{
    return vec3(randomInInterval(-1.0f, 1.0f), randomInInterval(-1.0f, 1.0f), randomInInterval(-1.0f, 1.0f));
}

vec3 randomUnitVec()
{
    return normalize(randomVec());
}

vec3 randomOnHemisphere(const vec3 normal)
{
    vec3 unitVec = randomUnitVec();
    if (dot(normal, unitVec) > 0.0f)
    {
        return unitVec;
    }
    return -unitVec;
}
