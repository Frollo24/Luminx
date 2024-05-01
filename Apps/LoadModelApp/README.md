# Model Loading App
In this basic application we load a 3D model from disk and position it at the origin of coordinates to render it onto the screen. The result on screen is the model itself colored with the direction of the vertex normal, normalized between 0 and 1.

## Configuration and Render Loop
Luminx offers a fairly easy to use abstraction to display the model on screen: we create a reference to the model and render it in the render loop.
