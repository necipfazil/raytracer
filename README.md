## Physically Based Path Tracer
A physically-based path tracer with many features, including:
- Anti-Aliasing through Multi-Sampling
- Refractive Object Shading
- Smooth Shading
- Distribution Ray Tracing
  - Area Light
  - Depth of Field
  - Glossy Reflection
  - Motion Blur
- Texture Mapping, Bump mapping, Perlin noise
- Different Lighting Methods
  - Point Light, Directional light, Spotlight
  - Object Lights (Sphere, Mesh), Visible Lights
  - Lighting through Environment Radiance Maps
- High Dynamic Range Imaging
  - Generating high dynamic range image outputs
  - Tonemapping, generating tonemapped HDR outputs
- BRDF (Bidirectional Reflectance Distribution Function)
  - Design that allows introducing any BRDF model easily
  - 7 different BRDF models
- Path Tracing (Indirect Illumination) with Monte Carlo methods
- Transformation and Instancing
- Acceleration Techniques
  - Bounding Volume Hierarchy
  - Multi-Threading

The project had 13 milestones, first of which included the principal design and the implementation of the ray tracer. With others, a different challenge was addressed that improves varying aspects of the project including its output quality, performance, and usability.

### Development Blog: Chase The Ray
Link to the blog: https://chasetheray.wordpress.com/

In each post, development of an additional feature was discussed briefly. Output images and performance evaluations were shared.

##### Contents
- [First Blood: Basic Ray Tracing](https://chasetheray.wordpress.com/2018/03/01/first-blood-basic-ray-tracer/): Object-oriented modelling of the problem. Class diagrams and the discussion on the interactions of the classes.
- [Accelerating Ray Tracing: Bounding Volume Hierarchy and Multi-Threading](https://chasetheray.wordpress.com/2018/03/28/accelerating-ray-tracing-bounding-volume-hierarchy-and-multi-threading/): Improving the performance of the project in terms of time efficiency by making use of a special data structure: bounding volume hiearchy, and multithreading.
- [Multisampling in Ray Tracing](https://chasetheray.wordpress.com/2018/03/28/multisampling-in-ray-tracing/): Improving the output image quality through multisampling (supersampling), which eliminates aliasing artifacts.
- [Shading Refractive (Transparent) Shapes](https://chasetheray.wordpress.com/2018/03/29/shading-refractive-transparent-shapes/): Shading transparent objects using Snell's Law for refraction geometry, Fresnal Equation for reflection and transmission, Schlick's approximation for simpler and faster solution, and Beer's Law for modelling attenuation.
- [Smooth Shading](https://chasetheray.wordpress.com/2018/04/07/smooth-shading/): Improving the output image quality through smooth shading, which assigns normal vectors to vertices rather than triangles and computes the surface normal at a point on a triangle as a weighted sum of normals of its vertices.
- [Transformation and Instancing](https://chasetheray.wordpress.com/2018/04/18/transformation-and-instancing/): Applying translation, rotation, and scaling transformations to scene objects, and instancing.
- [Distribution Ray Tracing](https://chasetheray.wordpress.com/2018/04/18/distribution-ray-tracing/): Introducing area light source depth of field, glossy reflection, motion blur effects through multi-sampling tricks.
- [All About Textures: Texture Mapping, Bump Mapping, Perlin Noise](https://chasetheray.wordpress.com/2018/05/02/all-about-textures-texture-mapping-bump-mapping-perlin-noise/): Texture mapping on different types of shapes. Bump Mapping. Perlin Noise.
- [Glittery Ray Tracer: Directional Light, Spotlight and HDR in Ray Tracing](https://chasetheray.wordpress.com/2018/05/07/glittery-ray-tracer-directional-light-spot-light-and-hdr-in-ray-tracing/): Different light sources: directional light and spotlight. Tonemapping high dynamic range outputs using [Reinhard et al.'s method](http://www.cmap.polytechnique.fr/~peyre/cours/x2005signal/hdr_photographic.pdf).
- [All About BRDF: Phong, Blinn-Phong, Torrance-Sparrow](https://chasetheray.wordpress.com/2018/05/26/all-about-brdf-phong-blinn-phong-torrance-sparrow/): Introducing BRDF (Bidirectional Reflectance Distribution Function). Design changes on light modelling to allow working with any BRDF. Implementation of 7 different BRDF models: Phong (original, modified, modified&normalized&energy-conserving), Blinn-Phong (original, modified, modified&normalized&energy-conserving), Torrance-Sparrow.
- [Object Lights: Sphere Light, Mesh Light and Environment Map](https://chasetheray.wordpress.com/2018/06/19/object-lights-sphere-light-mesh-light-and-environment-map/): Objects acting as light sources. Environment Map: object lights with texture as radiance map. Visible light sources.
- [Path Tracing with Monte Carlo Methods](https://chasetheray.wordpress.com/2018/06/19/path-tracing-with-monte-carlo-methods/): Turning the ray tracer into a path tracer: indirect lighting. Application of Monte Carlo methods.
- [Exporting Blender 3D Scenes](https://chasetheray.wordpress.com/2018/06/20/exporting-blender-3d-scenes/): Exporting Blender 3D scenes to be used as output to the path tracer.

### Sample Outputs

![Cornellbox Path Traced: Direct + Indirect Lighting](sample_outputs/cornellbox_path_uniform_100.png)

![Head Spherical Environment Light](sample_outputs/head_env_light.png)

![Sponza Path Traced: Direct + Indirect Lighting](sample_outputs/sponza_path.png)

See more in the blog posts.

### Acknowledgements
This project was developed while following the course: [CENG795 Special Topics: Advanced Ray Tracing](https://catalog.metu.edu.tr/course.php?prog=571&course_code=5710795), which was offered by [Assoc.Prof. Ahmet Oguz Akyuz](user.ceng.metu.edu.tr/~akyuz) at [CENG](http://ceng.metu.edu.tr) [METU](http://www.metu.edu.tr).

> ... the thoughest yet the most entertaining course of my BS adventure.
