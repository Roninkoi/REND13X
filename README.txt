REND13X DOS software renderer
=============================
By Roninkoi 2020

DOS software renderer written in C and x86 assembly. Mode 13h (320x200)
or mode X (320x240), 256 colors.

The purpose of this project is to explore early game engine
development and x86 assembly graphics programming

Compiled using Turbo C++ 3.0 or equivalent (supporting inline assembly).
Open the project file REND13X.PRJ using TC, select Run -> Run (Ctrl + F9).

--- Features ---

- "Fast" x86 assembly draw routines for line drawing and triangle fills

- 3D perspective projected triangle draw with z sorting, face culling and
  wireframe rendering

- Simple vector/matrix math and transforms

- Page flipping in mode X (double buffering)

- Keyboard interrupt handler for smooth multi-key input

--- Controls ---

WASD - Move 

Arrows - Rotate camera

R - Fly up

F - Fly down

Esc - Exit to DOS

