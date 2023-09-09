# REND13X DOS software renderer

<p align="middle">
  <img src="https://media.giphy.com/media/S57czNBuL5ZxcZmrbN/giphy.gif" width="54%" />
  <img src="https://user-images.githubusercontent.com/12766039/148410578-ced829ff-7760-494f-97f8-0395313d4496.gif" width="45%" /> 
</p>

DOS software renderer written in C and x86 assembly. Mode 13h (320x200) or mode X (320x240), 256 colors.

The purpose of this project is to explore early game engine
development and x86 assembly graphics programming.

Compiled using Turbo C++ 3.0 or equivalent (supporting inline assembly). The program can be compiled using the project file REND13X.PRJ or batch file BUILD.BAT.

## FEATURES

- "Fast" x86 assembly draw routines for line drawing and triangle fills.

    - Two video modes: mode 13h or mode X, define MODE13 or MODEX in RENDER.C to enable/disable.

    - Triangle filling is implemented in two ways: a non-clipped inaccurate fast fill and clipped accurate slow fill (using Bresenham's algorithm), define FASTFILL in RENDER.H to enable/disable. 

- 3D perspective projected triangle draw with z sorting, face culling and wireframe rendering.

- Simple vector/matrix math and transforms.

- Page flipping in mode X (double buffering).

- Keyboard interrupt handler for smooth multi-key input.

## CONTROLS

| Key | Action |
| --- | ------ |
| WASD | Move |
| Arrows | Rotate camera |
| R | Fly up |
| F | Fly down |
| Esc | Exit to DOS |

## TODO

- Implement clipping at geometry level (currently clipping is only implemented at line level for slow triangle fill and pixel level for line fill).
- Figure out a way to increase fast fill precision (error accumulates for long lines because of 16-bit precision).
- Improve mode X fills for better plane handling (naive line fill, rectangle fill).
- Wish list:
    - Sprite rendering
	- Text rendering
	- Bespoke floor/ceiling/sky draw routines
	- Sound
	- Texturing
	- Z-buffer

