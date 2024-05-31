REND13X software renderer
=============================
By Roninkoi 2020

VGA software renderer written in C and x86 assembly. Mode 13h (320x200)
or mode X (320x240), 256 colors.

The purpose of this project is to explore early game engine
development and x86 assembly graphics programming

Compiled using Turbo C++ 3 in DOS, Borland C++ 4.5 in Windows or equivalent.
Assembled using Turbo Assembler 5. The program can be compiled using the project
files REND13X.PRJ or REND13X.IDE. The program can also be built using the batch
files BUILD13.BAT and BUILDX.BAT. Runs on real hardware
(DOS, i486 or equivalent) or in DOSBox.

--- Features ---

- "Fast" x86 assembly draw routines for line drawing and triangle fills.
    - Two video modes: mode 13h or mode X, define MODE13 or MODEX in RENDER.H
      to enable/disable.
    - Line drawing is implemented using Bresenham's algorithm (mode 13h line
      draw is faster).
    - Triangle filling is implemented in two ways: a non-clipped inaccurate
      fast fill (using fixed-point) and clipped accurate slow fill (using
	Bresenham's algorithm), define FASTFILL in RENDER.H to enable/disable.
    - Graphics routines for mode 13h and mode X assembled and linked separately
      (32-bit).

- 3D perspective projected triangle draw with z sorting, geometry clipping,
  face culling and wireframe rendering.

- Simple vector/matrix math and transforms.

- Page flipping in mode X (double buffering).

- Keyboard interrupt handler for smooth multi-key input.

--- Controls ---

WASD - Move 

Arrows - Rotate camera

R - Fly up

F - Fly down

Esc - Exit to DOS

