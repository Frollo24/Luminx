# Simple Fog Rendering App
This application shows 11 cubes being illuminated by various lights and a simple way of rendering fog, using squared exponential fog for the calculations.

## Configuration
This application is set up in a very similar way to other lighting applications (shader, model, camera, textures...), uploading data for each light and the fog data itself.

## Render Loop
The render loop of this application is very simple and similar to other applications. As we want the spot light and the point light to move along with the camera, their position and direction are defined in camera space and uploaded to the shader transformed to world space. Additionally, in order to obtain the depth of each object to the camera in a simplified way, the position of each object in camera coordinates is also calculated.

To see the effect and intensity of the fog, you can enable or disable both the fog and toggle the spot light using the following keys:

- **T**: Toggles the spot light
- **F**: Enables or disables fog rendering
