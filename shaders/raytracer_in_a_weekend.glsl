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

const uint DIFFUSE = 1;
const uint METAL = 2;
const uint DIELECTRIC = 3;

struct Material
{
    vec3 color;
    uint type;
    float fuzziness;
    float refractionIndex;
};

struct Sphere
{
    vec3 center;
    float radius;
    Material material;
};

struct HitRecord
{
    float t;
    vec3 pos;
    vec3 normal;
    bool frontFace;
    Material material;
};

const int g_sphereCount = 5;

const Sphere[g_sphereCount] g_spheres = {
    Sphere(vec3(0.0f, 0.0f, -1.2f),     0.5f,   Material(vec3(0.1f, 0.2f, 0.5f),    DIFFUSE,       0.0f, 0.0f)),
    Sphere(vec3(0.0f, -100.5f, -1.0f),  100.0f, Material(vec3(0.8f, 0.8f, 0.0f),    DIFFUSE,       0.0f, 0.0f)),
    Sphere(vec3(-1.0f, 0.0f, -1.0f),    0.5f,   Material(vec3(1.0f),                DIELECTRIC,    0.0f, 1.5f)),
    Sphere(vec3(-1.0f, 0.0f, -1.0f),    0.4f,   Material(vec3(1.0f),                DIELECTRIC,    0.0f, 1.0f / 1.5f)),
    Sphere(vec3(1.0f, 0.0f, -1.0f),     0.5f,   Material(vec3(0.8f, 0.6f, 0.2f),    METAL,         1.0f, 0.0f)),
};

const int g_samples = 100;
const float g_samplesPerPixel = 1.0f / float(g_samples);
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
vec3 reflectRay(const vec3 v, const vec3 normal);
vec3 refractRay(const vec3 uv, const vec3 normal, float etaiOverEtat);
bool scatter(const Ray r, const HitRecord record, out vec3 attenuation, out Ray scattered);
bool nearZero(vec3 v);
bool canRefract(const vec3 vector, const vec3 normal, const float eta);
float reflectanceFactor(const vec3 vector, const vec3 normal, const float eta);

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
    vec3 deflectedColor = vec3(1.0f);
    vec3 lightSourceColor = vec3(0.0f);

    for (int i = 0; i < depths; i++)
    {
        HitRecord record;
        if (!worldHit(ray, g_rayMin, g_rayMax, record))
        {
            lightSourceColor = skyColor(ray.dir);
            break;
        }
        
        Ray scattered;
        vec3 attenuation;
        if (scatter(ray, record, attenuation, scattered))
        {
            deflectedColor *= attenuation;
            ray = Ray(scattered.start, scattered.dir);
        }
        else
        {
            lightSourceColor = vec3(0.0f);
            break;
        }
    }

    return deflectedColor * lightSourceColor;
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
    record.material = s.material;
    record.frontFace = true;
    if (dot(record.normal, r.dir) > 0)
    {
        record.normal = -record.normal;
        record.frontFace = false;
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
    return vec3(
        randomInInterval(-1.0f, 1.0f), 
        randomInInterval(-1.0f, 1.0f), 
        randomInInterval(-1.0f, 1.0f));
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

vec3 reflectRay(const vec3 v, const vec3 normal)
{
    return v - 2.0f * dot(v, normal) * normal;
}

vec3 refractRay(const vec3 uv, const vec3 normal, float etaiOverEtat)
{
    float cosTheta = min(dot(-uv, normal), 1.0f);
    vec3 rOutPerp = etaiOverEtat * (uv + cosTheta * normal);
    vec3 rOutParallel = -sqrt(abs(1.0 - dot(rOutPerp, rOutPerp))) * normal;
    return rOutPerp + rOutParallel;
}

bool nearZero(vec3 v)
{
    float val = 1e-8;
    return abs(v.x) < val && abs(v.y) < val && abs(v.z) < val;
}

bool scatter(const Ray r, const HitRecord record, out vec3 attenuation, out Ray scattered)
{
    switch(record.material.type)
    {
        case DIFFUSE: {
            vec3 scatterDir = record.normal + randomUnitVec();

            if (nearZero(scatterDir))
            {
                scatterDir = record.normal;
            }
            scattered = Ray(record.pos, scatterDir);
            attenuation = record.material.color;
            return true;
        }

        case METAL: {
            vec3 reflected = reflectRay(r.dir, record.normal);
            reflected = normalize(reflected) + (record.material.fuzziness * randomUnitVec());
            scattered = Ray(record.pos, reflected);
            attenuation = record.material.color;
            return dot(scattered.dir, record.normal) > 0.0f;
        }

        case DIELECTRIC: {
            attenuation = vec3(1.0f);
            float ri = 
                record.frontFace ? (1.0f / record.material.refractionIndex) 
                    : record.material.refractionIndex;

            vec3 unitDir = normalize(r.dir);
            float cosTheta = min(dot(-unitDir, record.normal), 1.0f);
            float sinTheta = sqrt(1.0f - cosTheta * cosTheta);
            const bool cannotRefract = ri * sinTheta > 1.0f;            
                
            vec3 direction;
            if (cannotRefract || reflectanceFactor(r.dir, record.normal, ri) > random())
            {
                direction = reflectRay(unitDir, record.normal);
            }
            else
            {
                direction = refractRay(unitDir, record.normal, ri);
            }

            scattered = Ray(record.pos, direction);
            return true;
        }
    }

    return false;
}

bool canRefract(const vec3 vector, const vec3 normal, const float eta) 
{
    const float cosTheta = dot(-vector, normal);
    return eta * sqrt(1.0f - cosTheta * cosTheta) <= 1.0f;
}

float reflectanceFactor(const vec3 vector, const vec3 normal, const float eta) 
{
    float r = pow((1.0f - eta) / (1.0f + eta), 2.0f);
    return r + (1.0f - r) * pow(1.0f - dot(-vector, normal), 5.0f);
}
