# Uniform Buffers App
This application shows how uniform buffers can be used to store data that can be shared by multiple pipelines, such as light data and fog data.

## Configuration
This application (excluding the configuration of the uniform buffers) is configured very similar to other applications. To configure the uniform buffer containing the light data, since each light will have different properties, first the necessary memory is reserved and then the information for each light is uploaded separately. On the other hand, as the fog information can be known in advance, this information is uploaded at the same time as the corresponding uniform buffer.

## Render Loop
The render loop binds three pipelines: the first one for rendering the lights (shown as spheres with the color and intensity of each light), the second one to render the plane (which behaves as in other applications) and the other to render the skybox. Notice that it is not necessary to specify the light data and the fog data in each shader, as these are read from the uniform buffers directly.

In addition we can select which lights are visible using the following keys:

- **1**: Only directional lights
- **2**: Only point lights
- **3**: Only spot lights
- **4**: All lights
