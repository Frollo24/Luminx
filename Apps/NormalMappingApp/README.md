# Normal Mapping App
This application shows a scene lit with a directional light using Blinn-Phong shading and normal mapping on the textured cubes.

## Configuration
The configuration of this application is identical to the Blinn Phong application, but with some additional settings to add the normal texture to the textured material.

## Render Loop
The render loop of this application is identical to the Blinn Phong application. Notice that the order of the render state changes are arranged in a way that minimizes context switches. To see the effect on the lighting of cubes, you can enable or disable it using the **N** key.
