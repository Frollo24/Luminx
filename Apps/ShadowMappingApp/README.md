# Shadow Mapping App
This application shows a basic shadow mapping implementation in a simple scene with a skybox and some sample objects, rendering shadows for a directional light.

## Configuration
The configuration process of this application is extensive as it adds some elements to the scene, but it is similar to other applications that use several pipelines: no default state is set as it will be modified with each render pass.

To set up the shadow pass, it is necessary to create a depth texture for the shadow map, bind it to the texture unit of choice and create a framebuffer with the previously mentioned depth texture as a depth-only framebuffer.

## Render Loop
Excluding the skybox, the render loop consists of two passes: a first depth pass where objects are rendered from the light perspective and a second pass where they are rendered normally, shading those regions that are occluded.

When rendering shadows for a directional light it is important to keep in mind that a directional light is not defined by a position, but a position is required by a camera in order to render objects. To overcome this, a fictitious position is defined for the light, which is where an orthographic camera will be placed and aligned with the direction of the light to simulate the shadows produced by the directional light.
