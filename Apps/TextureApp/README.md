# Texture App
This application shows how we can load a texture from disk using the Luminx framework.

## Configuration
To configure the texture we make use of the render device, to which we pass a description of the properties of our texture (image extent, image format...). Once the texture is created, we pass it the data of our image and bind the texture to the necessary texture unit.

## Render Loop
We can bind the texture to the texture unit we need, although in this application it is not necessary. We can choose the final result on the screen by pressing the following keys:

- **C**: Color tint the texture or not.
- **T**: Shade based on the texture coordinates or using the texture itself.
