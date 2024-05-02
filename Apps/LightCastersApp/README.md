# Light Casters Types App
This application shows how a directional light, a spot light and a spot light behave together in a scene with several objects, where you can configure materials, models and shaders as in other applications.

## Configuration
In addition to as in other applications, each of the three types of lights are configured by previously indicating what type of light it is, its direction and its position. Then, you upload the data of each light (color, intensity, direction, position...).

## Render Loop
The render loop calls are arranged in such a way that context switches are minimized as much as possible. As we want the spot light to move along with the camera, its position and direction are defined in camera space and uploaded to the shader transformed to world space.
