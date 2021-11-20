# REND13X DOS software renderer

![cube](https://media.giphy.com/media/S57czNBuL5ZxcZmrbN/giphy.gif)

DOS software renderer written in ANSI C and x86 assembly. Mode 13h (320x200) or mode X (320x240), 256 colors.

The purpose of this project is to explore early game engine
development and x86 assembly graphics programming

Compiled using Turbo C++ 3.0 (.PRJ included)

## FEATURES

- "Fast" x86 assembly draw routines for line drawing and triangle fills

- Simple vector/matrix math and transforms

- 3D perspective projected triangle draw

- Page flipping in mode X

- Keyboard interrupt handler for smooth multi-key input

## CONTROLS

WASD - Move 

Arrows - Rotate camera

R - Fly up

F - Fly down

Esc - Exit to DOS

