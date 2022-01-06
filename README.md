# REND13X DOS software renderer

<p align="middle">
  <img src="https://media.giphy.com/media/S57czNBuL5ZxcZmrbN/giphy.gif" width="54%" />
  <img src="https://user-images.githubusercontent.com/12766039/148410578-ced829ff-7760-494f-97f8-0395313d4496.gif" width="45%" /> 
</p>

DOS software renderer written in C and x86 assembly. Mode 13h (320x200) or mode X (320x240), 256 colors.

The purpose of this project is to explore early game engine
development and x86 assembly graphics programming.

Compiled using Turbo C++ 3.0 or equivalent (supporting inline assembly). Open the project file REND13X.PRJ using TC, select Run → Run (Ctrl + F9).

## FEATURES

- "Fast" x86 assembly draw routines for line drawing and triangle fills

- 3D perspective projected triangle draw with z sorting, face culling and wireframe rendering

- Simple vector/matrix math and transforms

- Page flipping in mode X (double buffering)

- Keyboard interrupt handler for smooth multi-key input

## CONTROLS

WASD - Move 

Arrows - Rotate camera

R - Fly up

F - Fly down

Esc - Exit to DOS

