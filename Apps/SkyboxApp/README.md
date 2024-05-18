# Skybox Rendering App
This application shows a small scene with a skybox and some sample objects, integrating also lights and fog rendering.

## Configuration
The configuration process of this application is quite extensive, due to the variety of elements that make up the scene: different objects, different materials, different lights, etcetera. However, it is quite similar to how other applications have been configured. Notice that no default state is configured in the renderer, since we are going to change pipeline several times.

The cubemap is loaded as 6 different images and configured inside a Cubemap texture. In addition, a vertex array is created which will be in charge of rendering the skybox. In addition, to correctly integrate the fog with the skybox, we have chosen a light blue color and a very low intensity, to simulate haze in the distance.

## Render Loop
The render loop binds two pipelines: one to render the objects (which behaves as in other applications) and the other to render the skybox. Notice that the skybox is rendered at the end, to ensure that it is only drawn in those pixels where it will be visible. To do this, the depth test is set to accept values less than or equal to 1.0, which is the default value of the depth buffer.

To see the effect and intensity of the haze, you can enable or disable it using the **F** key.
