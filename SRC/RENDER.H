/*
 * DOS Software renderer by Roninkoi 2020
 * ANSI C, draw routines written in x86 assembly
 * Compiled using Turbo C++ 3.0 or equivalent
 * Mode 13h (320x200) or mode X (320x240), 256 colors
 */

#ifndef RENDER_H
#define RENDER_H

#include <stdio.h>
//#include <bios.h>
#include <math.h>
#include <time.h>
#include <dos.h>

#include "SRC\VEC.H"

typedef unsigned char byte;

// video mode selection
#define MODE13
//#define MODEX

// viewport dimensions
#define L 0
#define R 319
#define W 320

#ifdef MODE13
#define T 8
#define B 199
#define H 200

#define r_clear() r_scr(clearcol);
#define r_sync() r_waitRetrace();
#endif

#ifdef MODEX
#define T 0
#define B 239
#define H 240

#define r_clear() r_fill(clearcol);
#define r_sync() r_flip();
#endif

// start of video memory
#define VSTART 0xa000
// 1/320
#define EPSILON 0.0031f

// near and far planes
#define ZNEAR 0.2
#define ZFAR 20.0

#define min(x, y) (x > y ? y : x)
#define max(x, y) (x < y ? y : x)
#define clamp(x, min, max) (x > max ? max : (x < min ? min : x))

#define sign(x) (x >= 0 ? 1 : -1)
#define abs(x) (x < 0 ? -(x) : x)
#define round(x) floor(x + 0.5f)

// random float in range [0, 1]
#define RANDF ((rand() % 10000) / 10000.0f)

#define SECOND 18.206
// convert itime 1/(18.206 s) to second
#define TOSECOND 0.054927f

#define PI 3.1415926

#define TRACESTART while (inportb(0x3da) & 8)
#define TRACEEND while (!(inportb(0x3da) & 8))

extern char vmode; // previous video mode
extern int wireframe; // draw wireframe triangles?
extern int faceculling; // cull back faces?
extern int zsort;  // sort triangles by z?
extern int clearscr; // screen clearing enabled
extern int clearcol; // screen clear color

extern byte pg; // page number
extern unsigned pgoffs; // offset of page being written

struct tri {
	vec4 v0; // vertices
	vec4 v1;
	vec4 v2;

	float fc; // face center distance

	byte c; // color
};

// render buffer of triangles
extern struct tri r_buffer[256];

// indices of sorted triangles in r_buffer
extern byte r_s[256]; // sorted
extern byte r_n;

extern unsigned drawcount;

// render matrix
extern mat4 rm;

extern int running; // program running?

extern int itime;  // time since start of program via int 0x1c (18.206/s)
extern byte keycode; // keycode of currently pressed/released key
extern int keydown[256]; // key pressed down?

void interrupt getTime();
unsigned now(); // get system time

void hookKeys();
void unhookKeys();
void getInput(); // get which keys are pressed down

void r_init(); // initialize renderer, enter video mode
void r_exit(); // return original video mode
void r_scr(byte c); // clear screen with color c by int 10h, slightly faster than fill
void r_flip(); // flip page
void r_fill(byte c); // fill entire screen area with color c
void r_vfill(int y0, int h, byte c); // vertical fill from top y0, height h, color c
void r_hlinefill(int x0, int x1, int y, byte c); // horizontal line draw with optional x sort and clipping
void r_putpixel(int x, int y, byte c); // put pixel of color c at (x, y)
void r_rectfill(int x, int y, int w, int h, byte c); // rectangle fill, left corner (x, y), size (w, h), color c

void r_trifill(float x0, float x1, int y, int dy, float k0, float k1, byte c); // fast asm triangle fill using integers, no clipping
void r_trifillclip(float x0, float x1, int y, int dy, float k0, float k1, byte c); // triangle will with clipping at pixel level

// DRAW

void r_add(vec4* v0, vec4* v1, vec4* v2, byte c); // add triangle to render buffer
void r_addf(float v0x, float v0y, float v0z,
			float v1x, float v1y, float v1z,
			float v2x, float v2y, float v2z, byte c);

void r_drawline(float (*v0)[2], float (*v1)[2], byte c); // draw line of color c from v0 to v1
void r_drawlinef(float x0, float y0, float x1, float y1, byte c);

void r_drawtri(float v[3][2], byte c); // draw triangle of color c with vertices v
void r_drawtri3d(vec4* v0, vec4* v1, vec4* v2, byte c); // draw 3D perspective projected triangle of color with vertices {v0, v1, v2}

void r_sort(); // sort triangles by centroid z coordinate
void r_draw(); // draw all triangles in render buffer

void r_waitRetrace(); // wait for screen retrace to sync

// keyboard codes

#define wDownCode 17
#define wUpCode 145
#define aDownCode 30
#define aUpCode 158
#define sDownCode 31
#define sUpCode 159
#define dDownCode 32
#define dUpCode 160
#define rightDownCode 77
#define rightUpCode 205
#define leftDownCode 75
#define leftUpCode 203
#define upDownCode 72
#define upUpCode 200
#define downDownCode 80
#define downUpCode 208
#define rDownCode 19
#define rUpCode 147
#define fDownCode 33
#define fUpCode 161

#endif
