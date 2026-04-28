# Raycaster

A simple raycasting engine using OpenGL and modern C++.

## Overview

This project implements a simple raycasting engine using algorithms similar to those in Wolfenstein 3D to generate a pseudo-3D maze with texture mapping and sprites. It uses GLFW to create a window and handle events, and OpenGL as the primary graphics API.

Although OpenGL is used for rendering, this project does not utilize traditional geometry to render the maze. Instead, it uses OpenGL to render a single textured quad that covers the screen. All pixel operations are performed on a buffer, which updates the quad's texture every frame.

The purpose of this project is to refamiliarize myself with ray casting. Even though this is an old technique, building a simple raycasting engine offers valuable lessons for modern graphics programming. Concepts like casting rays, dealing with projection planes, and revisiting basic trigonometry remain relevant.

Note that this project doesn’t follow the popular [Lode’s Raycasting Tutorial](https://lodev.org/cgtutor/raycasting.html). The primary difference is that I’m using polar coordinates to represent some positions and directions, which is more aligned with the original [Wolfenstein3D source-code](https://github.com/id-Software/wolf3d).
