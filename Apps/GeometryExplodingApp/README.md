# Geometry Exploding App
This application shows one of the uses that the geometry stage can have by exploding the model into its different triangles and moving them in a random direction.

## Configuration
This application is kept fairly simple in its configuration, by only loading textures, shaders and two models for the scene. By not adding fog and only having a single directional light, the creation of Uniform Buffers has been omitted to keep the shaders as simple as possible, so the light is uploaded as plain uniforms for each shader.

## Render Loop
The render loop varies between two very similar pipelines, one that includes the geometry stage for the model to be exploded and one that does not include it for the plane. The random generation of a vector in the shader is the same function used in [The Book of Shaders](https://thebookofshaders.com/10/). Finally, a collision with the plane is simulated in the explode function of the geometry stage. The skybox is rendered as in other applications. You can choose whether or not to exploit the model with the following keys:

- **Space**: Explode the model
- **R**: Reset the model to not being exploded
