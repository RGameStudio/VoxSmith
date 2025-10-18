# VoxSmith
For the moment my most ambitious project, which will be a high performant voxel engine, with heavy use of GPU computing using C++17 and OpenGL.

# Features in work

+ Frustum Culling, having them already but still some work needed. (To test how shadows will react to chunk clipping)

+ Random assembly of chunk meshes Multi-pass rendering (render passes/render phases) to separate the rendering of opaque/transparent objects (Earth/Water Glass/Static semi-transparent objects)

+ Support for multiple materials (Color, Diffuse, Diffuse+Emissive, Combined (Color+Diffuse, Color+Emissive, Color+Diffuse+Emissive))

+ Colored semi-static lighting (dynamics are only apparent in chunk updates) for many objects, including dynamics.
+ Shadows (Cascading/etc.)

+ Reflections (SSR)

+ Fog (A simple fog that extends about 10% of the distance from the far clip plane toward the near clip plane)

+ Ambient Occlusion in 2 variations (Static shading baked into the mesh; SSAO)

+ Anti-aliasing (FXAA / MSAA / SMAA). (TAA is not suitable due to its visual component in the form of ghosting.)

+ Bloom in 2 variations (The entire scene; only emissive materials)