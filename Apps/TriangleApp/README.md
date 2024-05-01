# First Triangle App
This basic application displays a triangle on the screen in a similar way to how the first triangle would be drawn using OpenGL.

## Configuration
To configure the application we follow the steps below:

1. Create the shader
2. Create the Vertex Array Object
3. Create the Vertex Buffer
4. Specify the layout of the Vertex Buffer and add it to the Vertex Array Object together with the Vertex Buffer.

## Render Loop
In the render loop, we do the following:

1. Specify which shader we are going to use
2. Bind the Vertex Array Object previously configured.
3. Send to draw 3 vertices
