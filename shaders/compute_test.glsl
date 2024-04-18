#version 430 core

layout (local_size_x = 10, local_size_y = 10, local_size_z = 1) in;

layout(rgba32f, binding = 0) writeonly uniform image2D imgOutput;

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

const int g_nSpheres = 2;
const float g_vHeight = 2.0f;

Sphere g_spheres[g_nSpheres] = {
    Sphere(vec3(0.0f, -100.5f, -g_focalLength), 100.0f),
    Sphere(vec3(0.0f,  0.0f, -g_focalLength), 0.5f),
    
};

vec3 lerp(vec3 a, vec3 b, float t)
{
    return a * (1.0f - t) + b * t;
}

float lerp(float a, float b, float t)
{
    return a * (1.0f - t) + b * t;
}

float rand(vec2 co) 
{
  float a = 12.9898;
  float b = 78.233;
  float c = 43758.5453;
  float dt = dot(co.xy, vec2(a, b));
  float sn = mod(dt, 3.1415926535);
  return fract(sin(sn) * c);
}

float randRange(vec2 co, float tMin, float tMax)
{
    return lerp(tMin, tMax, rand(co));
}

float randReal()
{
    return rand(vec2(0, 1));
}

vec4 pixelColor(Ray ray);
vec4 pixelColor2(Ray ray);
vec4 pixelColorNormal(Ray ray);

void main() 
{
    ivec2 texelCoord = ivec2(gl_GlobalInvocationID.xy);

        
    ivec2 dims = imageSize(imgOutput);
    float aspectRatio = float(dims.x) / float(dims.y);
    float vWidth = g_vHeight * aspectRatio;


    vec3 viepowrt_u = vec3(vWidth, 0.0f, 0.0f);
    vec3 viepowrt_v = vec3(0.0f, g_vHeight, 0.0f);

    vec3 lowerLeft = g_eyePos - vec3(0.0f, 0.0f, g_focalLength) - viepowrt_u / 2 - viepowrt_v / 2;


    Ray ray;
    ray.origin = g_eyePos;

    /**/
    float du = float(texelCoord.x) / dims.x;
    float dv = float(texelCoord.y) / dims.y;

    ray.dir = lowerLeft + du * viepowrt_u + dv * viepowrt_v;
    vec4 value = pixelColor2(ray);
    imageStore(imgOutput, texelCoord, value);
    /**/

    
    /*
    vec4 value = vec4(0.0f);
    for (int s = 0; s < 4; s++)
    {
        const float rx = randReal(vec2(float(texelCoord.x+s), float(texelCoord.y+s)));
		const float ry = randReal(vec2(float(texelCoord.y+s), float(texelCoord.x+s)));

		const float u = (float(texelCoord.x) + rx) / float(dims.x);
		const float v = (float(texelCoord.y) + ry) / float(dims.y);

        ray.dir = lowerLeft + u * viepowrt_u + v * viepowrt_v;
        value += pixelColor(ray);
    }
    
    imageStore(imgOutput, texelCoord, value / 4);
    */
}

vec3 getVecAtT(Ray ray, float t)
{
    return ray.origin + ray.dir * t;
}

vec3 getRandomPInUnitSphere(vec3 pos)
{
    vec3 p = vec3(0.0f);
    do 
    {
        //p = 2.0f * vec3(rand(-1, 1), rand(pos.yz + p.yz), rand(pos.xz + p.xz)) - vec3(1.0f);
        //p = vec3(rand(vec2(-1.0f, 1.0f)), rand(vec2(-1.0f, 1.0f)), rand(vec2(-1.0f, 1.0f)));
        p = 2.0f * vec3(rand(pos.xy + p.xy), rand(pos.xz + p.xz), rand(pos.yz + p.yz)) - vec3(1.0f);

    } 
    while (dot(p, p) >= 1);
    return p;
}

vec3 getRandomOnHemisphere(vec3 pos, vec3 normal)
{
    vec3 onUSphere = normalize(getRandomPInUnitSphere(pos));
    if (dot(onUSphere, normalize(normal)) > 0.0f)
    {
        return onUSphere;
    }
    return -onUSphere;
}

bool hitSphere(Sphere sphere, Ray ray, float tMin, float tMax, out HitRecord hRecord)
{
    const vec3 oc = g_eyePos - sphere.center;
    const float a = dot(ray.dir, ray.dir);
    const float h = dot(ray.dir, oc);
    const float c = dot(oc, oc) - sphere.radius * sphere.radius;

    const float discriminant = h*h - a*c;
    if (discriminant > 0.0f)
    {
        float t = (-h - sqrt(discriminant)) / a;
        if (t <= tMin || t >= tMax)
        {
            t = (-h + sqrt(discriminant)) / a;
            if (t <= tMin || t >= tMax)
            {
                return false;
            }
        }

        hRecord.t = t;
        hRecord.pos = getVecAtT(ray, hRecord.t);
        hRecord.normal = (hRecord.pos - sphere.center) / sphere.radius;
        if (dot(hRecord.normal, ray.dir) > 0.0f)
        {
            hRecord.normal *= -1;
        }
        return true;
    }

    return false;
}

const float g_minRayLen = 0.1f;
const float g_maxRayLen = 1000.0f;

vec3 skyColor(vec3 dir)
{
    vec3 rayn = normalize(dir);
    float t = 0.5f * (rayn.y + 1.0f);
    return mix(vec3(1.0), vec3(0.3, 0.5, 0.9), t);
}

vec3 normalColor(vec3 normal)
{
    return 0.5f * (normal + vec3(1.0f));
}

bool worldHit(Ray ray, float tMin, float tMax, out HitRecord record)
{
    HitRecord temp;
    float closestSoFar = tMax;
    bool hit = false;
    for (int iSphere = 0; iSphere < g_nSpheres; iSphere++)
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

vec4 pixelColor(Ray ray)
{
    HitRecord hRecord;
    bool hit = worldHit(ray, g_minRayLen, g_maxRayLen, hRecord);
    if (hit)
    {
        const int maxSecondRays = 10;
        vec3 rayColor[maxSecondRays];
        int rayCount = 0;
        do 
        {
            vec3 target = getRandomOnHemisphere(hRecord.pos, hRecord.normal);
            Ray secondaryRay = { hRecord.pos, target };
            hit = worldHit(secondaryRay, g_minRayLen, g_maxRayLen, hRecord);
            if (hit)
            {
                rayColor[rayCount] = vec3(0.0f);
                rayCount++;
            }
            else
            {
                rayColor[rayCount] = skyColor(secondaryRay.dir).rgb;
            } 
        }
        while(hit && rayCount < maxSecondRays); 

        vec3 color = vec3(0.0f);
        do
        {
            color = 0.5f * (color + rayColor[rayCount]);
            rayCount--;
        }
        while(rayCount >= 0);
        return vec4(color, 1.0f);
    }

    return vec4(skyColor(ray.dir), 1.0f);
}

vec4 pixelColor2(Ray ray)
{
    const int maxSecondRays = 15;
    Ray tempRay = Ray(ray.origin, ray.dir);
    vec3 color = vec3(1.0f);
    int rayCount = 0;
    while (true)
    {
        HitRecord record;
        if (rayCount >= maxSecondRays)
        {
            return vec4(skyColor(tempRay.dir), 1.0f);
        }
        if (worldHit(tempRay, g_minRayLen, g_maxRayLen, record))
        {
            vec3 target = getRandomOnHemisphere(record.pos, record.normal);
            tempRay = Ray(record.pos, target);
            rayCount++;
            color *= 0.5f;
        }
        else
        {
            color *= vec4(skyColor(tempRay.dir), 1.0f);
            return color;
        }
    }
}

vec4 pixelColorNormal(Ray ray)
{
    HitRecord hRecord;
    if (worldHit(ray, g_minRayLen, g_maxRayLen, hRecord))
    {
        return vec4(normalColor(hRecord.normal), 1.0f);
    }

    return vec4(skyColor(ray.dir), 1.0f);
}
