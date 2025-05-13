Created by Ryan Docherty | DOC13409696

Welcome to the analytics program! 
-----------------------------------------------
Linking libraries it in Visual Studio:
-----------------------------------------------

Step 1: Locate the 'OpenGL' folder
Step 2: Paste it in your documents
Step 3: In VS, add an Include Directory to OpenGL\Libraries\Includes 
Step 4: Then add another Include Directory for OpenGL\LIbraries\Includes\glm\glm
Step 5: In Linker->Input, add 'opengl32.lib', 'glew32s.lib' and 'glfw3.lib' 

Hopefully the program will then produce no linking errors when built. 

-----------------------------------------------
Controlling the program: 
-----------------------------------------------

Once the program succesfully runs, here are the keyboard controls: 

1: Toggle player 1 trajectory
2: Toggle player 2 trajectory
3: Toggle player 3 trajectory
4: Toggle player 4 trajectory
5: Toggle all trajectories
6: Turn trajectories off/just have the heat map

W: zoom in
A: pan left
S: zoom out
D: pan right

Esc: close program