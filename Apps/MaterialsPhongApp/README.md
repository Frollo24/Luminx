# Blinn Phong Material Showcase App
This application renders 11 red spheres with Phong materials illuminated with a directional light in which the glossiness parameter goes from 0 to 1. In this application both Phong shading and Blinn-Phong shading (default) are used.

## Configuration
To set up this application, you create two shaders (one for each lighting model) and a reference to the shader you choose to shade with. Then, each material and each transform of each sphere is initialized in a for loop to place them in order in a row.

## Render Loop
Each sphere is rendered in the corresponding position and with the corresponding material, from the lowest to the highest glossiness value. The actual glossiness value needed for Phong and Blinn-Phong shading is calculated in the shader itself, in order to use a more intuitive 0 to 1 parameter for the user. You can select between Phong and Blinn-Phong with the following keys:

- **P**: Phong shading
- **B**: Blinn-Phong shading

### Energy Conservation
Additionally, energy conservation factors have been tested with the two shaders, which can be activated or deactivated with the **C** key. You can find more information about this at this link: [Energy Conserving in Games](https://www.rorydriscoll.com/2009/01/25/energy-conservation-in-games/). As a small optimization, in each shader we multiply by the inverse of the denominator (using the inverse of pi as constant) to avoid using divisions.
