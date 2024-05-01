# Blinn Phong Lighting App
This application shows a scene lit with a directional light using Blinn-Phong shading, with the specular component slightly edited to make it slightly more realistic.

## Configuration
This application is set up very similar to how other lighting applications are set up, but uses the Phong materials from the Luminx framework to upload the variables to the shader they use.

## Render Loop
To select which material you want to use to render the objects, the Luminx materials provide an easy-to-use interface thath takes care of selecting the colors and textures within the shader. Notice that the order of the render state changes are arranged in a way that minimizes context switches.
