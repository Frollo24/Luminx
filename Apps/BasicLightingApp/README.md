# Basic Phong Lighting App
In this application, two shaded cubes are shown using basic Phong lighting with a directional light. In addition, the background color is changed to give a more realistic lighting feel.

## Configuration
The setup of this application is very similar to other applications, but as it loads several textures at once, each of them must be binded to a different texture unit and set these bindings in the shader.

## Render Loop
In the render loop, we update the shading variables that are dependent on each object and on the camera transform: the camera position, the model matrix of each object...
