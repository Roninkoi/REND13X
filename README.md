# REND13X software renderer

<p align="middle">
  <img src="https://media.giphy.com/media/S57czNBuL5ZxcZmrbN/giphy.gif" width="54%" />
  <img src="https://user-images.githubusercontent.com/12766039/148410578-ced829ff-7760-494f-97f8-0395313d4496.gif" width="45%" /> 
</p>

VGA software renderer written in C and x86 assembly. Mode 13h (320x200) or mode X (320x240), 256 colors.

The purpose of this project is to explore early game engine development and x86 assembly graphics programming.

Runs on real hardware (DOS, i486 or equivalent) or in DOSBox.

## Features

- "Fast" x86 assembly draw routines for line drawing and triangle fills.
    - Two video modes: mode 13h or mode X, define MODE13 or MODEX in RENDER.H to enable/disable.
    - Line drawing is implemented using Bresenham's algorithm (mode 13h line draw is faster).
    - Triangle filling is implemented in two ways: a non-clipped inaccurate fast fill (using fixed-point) and clipped accurate slow fill (using Bresenham's algorithm), define FASTFILL in RENDER.H to enable/disable.
    - Graphics routines for mode 13h and mode X assembled and linked separately (32-bit).

- 3D perspective projected triangle draw with z sorting, geometry clipping, face culling and wireframe rendering.

- Simple vector/matrix math and transforms.

- Page flipping in mode X (double buffering).

- Keyboard interrupt handler for smooth multi-key input.

## Controls

| Key | Action |
| --- | ------ |
| WASD | Move |
| Arrows | Rotate camera |
| R | Fly up |
| F | Fly down |
| Esc | Exit to DOS |

## Building

Compiled using Turbo C++ 3 in DOS, Borland C++ 4 or Microsoft Visual C++ 6 in Windows or equivalent. Assembled using Turbo Assembler 5 or Microsoft Macro Assembler 6. File paths in projects may need to be adjusted depending on where the project is placed.

### Turbo C++

Building requires Turbo Assembler. The project can also be completely built using the given batch files.

Project file: `REND13X.PRJ`

Build Mode 13: `BUILD13.BAT`

Build Mode X: `BUILDX.BAT`

### Borland C++

Building requires Turbo Assembler. Include either `R13.ASM` or `RX.ASM` in the project (not both).

Project file: `REND13X.IDE`

### Visual C++

Building requires Microsoft Macro Assembler. Visual C++ won't assemble the .ASM files for you, so they need to be assembled separately before building the project. Include the object files `IN.OBJ` and either `R13.OBJ` or `RX.OBJ`.

Project file: `REND13X.MAK`

Assemble Mode 13: `MASM13.BAT`

Assemble Mode X: `MASMX.BAT`

## TODO

- Improve mode X fills for better plane handling.

- Clip geometry partially behind the camera.

- Set custom color palette.

- Wish list:
    - Sprite rendering
	- Text rendering
	- Bespoke floor/ceiling/sky draw routines
	- Sound
	- Texturing
	- Z-buffer

