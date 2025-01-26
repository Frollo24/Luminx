# Deferred Shading App
This application shows a basic implementation of the deferred shading technique, where all the information needed to render the scene (position, normals, albedo...) is pre-calculated and stored in a framebuffer known as G-Buffer. The formats used for the textures in this application are the simplest ones that meet the purpose of this application.

## Configuration
This application configures 8 cubes, a plane and a number of point lights which defaults to 256 lights. This number can be increased, without modifying any shader, up to 1024 lights without significantly increasing the computational cost thanks to the usage of deferred shading. The different G-Buffer textures and their use are reflected in the following tables:

| Attachment    | Format  | RGB                  | A                |
|---------------|---------|----------------------|------------------|
| RenderTarget0 | RGBA16F | World Position (XYZ) | ViewPosition (Z) |
| RenderTarget1 | RGBA16F | World Normal (XYZ)   | Unused           |
| RenderTarget2 | RGBA8   | Albedo               | Unused           |
| RenderTarget3 | RGBA8   | Specular             | Glossiness       |
| RenderTarget4 | RGBA8   | Emissive             | Unused           |

| Attachment    | Format  | Content|
|---------------|---------|--------|
| RenderTarget5 | DEPTH32 | Depth  |

## Render Loop
To render using deferred shading, first a pass is made where the cubes and the plane are rendered, being the output of this pass the G-Buffer textures. Then, in the next pass, the final render is drawn using only a 6-vertex quad defined in the shader itself. The rest of the scene elements (lights and skybox) are rendered as in other applications. Additionally, you can choose what the final rendering will show with the following keys:

- **0**: Rendered scene (shading calculation)
- **1**: Position buffer
- **2**: Normal buffer
- **3**: Albedo buffer
- **4**: Specular buffer
- **5**: Emissive buffer
