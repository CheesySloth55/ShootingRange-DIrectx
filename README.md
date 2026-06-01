# Introduction
This project is a showcase of my personal project for my semester at open learning. It is intended to become a game with simple game logic but with a full rendering pipeline, that includes all the steps of rendering a model to putting shadows on your objects. 

# Explanation
The engine is based on Rastertek tutorials, including my own implementations and extensions learned along the way. https://rastertek.com/tutdx11win10.html
The structure of the engine was originally based on a C4 diagram, but it is only supposed to be a rough outline. It does not represent the final result, only a primitive outlook on what I thought I needed at the beginning of the project.

<img width="5660" height="3928" alt="image" src="https://github.com/user-attachments/assets/9d8a9e7e-e67a-4d29-9921-9db6998fb740" />

# Folder structure
After having implemented most of the code that was necessary for rendering I structured my folders in the following way:

## core
### ApplicationClass
- It is the application class where the rendering happens and the whole game loop is getting run. All the logic and rendering eventually come from here.

### SystemClass
- Also the system class which initializes all the necessary code, keeps the application in a loop. It gets called by main.cpp

## data
- Holds all of the image-, mesh-, model- and shader(.ps/.vs, i.e. pixel shader vertex shader) files. It is also a temp spot for the debug data.

## graphicsAPI
- All of the rendering pipeline. This includes: Camera, DirectX interfacing, font, frustum culling (for performance), lighting, models/meshes, shaders and textures. The shaders get managed by the shader manager. It basically initializes and manages the resources for all of the shaders and creates an easy interface for calling the different shaders. This is based on the flyweight pattern. https://refactoring.guru/design-patterns/flyweight

## peripheral
- Has all of the peripherals and interaction classes like a timer or managing input etc.
