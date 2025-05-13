# Heatmap Visualisation (Quake 3 Engine)

## Overview

The objective here was to modify the Quake 3 game engine to read player locations in real-time and output those positions to a heatmap visualization. 
The primary goal of this project was to identify player hotspots within the game that could indicate unfair advantages, such as advantageous camping spots, thereby aiding in game balancing efforts.

## Features

- **Real-Time Player Tracking**: Modifications to the Quake 3 engine enable real-time tracking of player locations, outputting this data into a `.txt` file for further analysis.
- **Heatmap Generation**: Utilizes OpenGL and GLEW to visualize player positions from the recorded data, allowing for easy identification of popular or advantageous areas in the game.
- **Gameplay Analysis**: The heatmap serves as a tool to theoretically balance the gameplay by highlighting areas that may offer unfair advantages to players.


## Welcome to the analytics program! 

<h3>Linking libraries it in Visual Studio:</h3>
• Step 1: Locate the 'OpenGL' folder<br>
• Step 2: Paste it in your documents<br>
• Step 3: In VS, add an Include Directory to OpenGL\Libraries\Includes<br>
• Step 4: Then add another Include Directory for OpenGL\LIbraries\Includes\glm\glm<br>
• Step 5: In `Linker->Input`, add `opengl32.lib`, `glew32s.lib` and `glfw3.lib`<br><br>

Hopefully the program will then produce no linking errors when built. 

<h3>Controlling the program: </h3>
Once the program succesfully runs, here are the keyboard controls: 

• 1: Toggle player 1 trajectory<br>
• 2: Toggle player 2 trajectory<br>
• 3: Toggle player 3 trajectory<br>
• 4: Toggle player 4 trajectory<br>
• 5: Toggle all trajectories<br>
• 6: Turn trajectories off/just have the heat map<br>

• W: zoom in<br>
• A: pan left<br>
• S: zoom out<br>
• D: pan right<br>

• Esc: close program
