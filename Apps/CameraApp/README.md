# Basic Camera App
This application allows you to control a basic perspective camera within Luminx.

## Configuration
The Camera class has two static functions to initialize a new camera either using lookAt parameters or using horizontal coordinates from the coordinate origin. In addition, it allows to modify the view and projection matrices whenever necessary.

## Render Loop
In the render loop you can update the camera position and rotation smoothly by pressing the following keys:

- **W**: Move forward.
- **S**: Move backwards.
- **A**: Move left.
- **D**: Move right.
- **Q**: Move down.
- **E**: Move up.
- **Arrow keys**: Rotate the camera
