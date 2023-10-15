# REND13X software renderer

<p align="middle">
  <img src="https://media.giphy.com/media/S57czNBuL5ZxcZmrbN/giphy.gif" width="54%" />
  <img src="https://user-images.githubusercontent.com/12766039/148410578-ced829ff-7760-494f-97f8-0395313d4496.gif" width="45%" /> 
</p>

VGA software renderer written in C and x86 assembly. Mode 13h (320x200) or mode X (320x240), 256 colors.

The purpose of this project is to explore early game engine development and x86 assembly graphics programming.

Compiled using Turbo C++ 3.0 or equivalent (supporting inline assembly). The program can be compiled using the project file REND13X.PRJ or batch file BUILDC.BAT. Runs on real hardware (DOS, i486 or equivalent) or in DOSBox.

## FEATURES

- "Fast" x86 assembly draw routines for line drawing and triangle fills.
    - Two video modes: mode 13h or mode X, define MODE13 or MODEX in RENDER.H to enable/disable.
    - Line drawing is implemented using Bresenham's algorithm (mode 13h line draw is faster).
    - Triangle filling is implemented in two ways: a non-clipped inaccurate fast fill (using fixed-point) and clipped accurate slow fill (using Bresenham's algorithm), define FASTFILL in RENDER.H to enable/disable.
	- Graphics routines written in assembly are available inline in C (16-bit) or assembled and linked separately (32-bit), define ASM in the corresponding C file.

- 3D perspective projected triangle draw with z sorting, geometry clipping, face culling and wireframe rendering.

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

- Split inline assembly from RENDER13.C, RENDERX.C and INPUT.C to separate .ASM files.

- Figure out a way to increase fast fill precision (error accumulates for long lines because of 16-bit precision).

- Improve mode X fills for better plane handling.

- Clip geometry partially behind the camera.

- Wish list:
    - Sprite rendering
	- Text rendering
	- Bespoke floor/ceiling/sky draw routines
	- Sound
	- Texturing
	- Z-buffer

