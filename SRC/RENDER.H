/*
 * DOS Software renderer by Roninkoi 2020
 * (ANSI?) C, draw routines written in x86 assembly
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

// video mode selection (ifdef)
//#define MODE13
#define MODEX

// enable faster fills with less precision
#define FASTFILL

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
#define r_sync() if (doublebuffer) r_flip(); else r_waitRetrace();
#endif

// start of video memory
#define VSTART 0xa000
// 1/320
#define EPSILON 0.0031f

// near and far planes
#define ZNEAR 0.2
#define ZFAR 20.0

#define min(x, y) ((x) > (y) ? (y) : (x))
#define max(x, y) ((x) < (y) ? (y) : (x))
#define clamp(x, min, max) ((x) > (max) ? \
	(max) : ((x) < (min) ? (min) : (x)))

#define sign(x) ((x) >= 0 ? 1 : -1)
#define abs(x) ((x) < 0 ? -(x) : (x))
#define round(x) floor((x) + 0.5f)

// determine visibility of geometric primitives
#define pointVis(x, y) ((x) >= L && (x) <= R && (y) >= T && (y) <= B)
#define lineVis(x0, y0, x1, y1) (\
	min(x0, x1) <= R && max(x0, x1) >= L && \
	min(y0, y1) <= B && max(y0, y1) >= T)
#define hlineVis(x0, x1, y) (\
	x0 <= R && x1 >= L && y >= T && y <= B)
#define vlineVis(x, y0, y1) (\
	x <= R && x >= L && y0 <= B && y1 >= T)
#define triVis(x0, y0, x1, y1, x2, y2) (\
	min(x0, min(x1, x2)) <= R && max(x0, max(x1, x2)) >= L && \
	min(y0, min(y1, y2)) <= B && max(y0, max(y1, y2)) >= T)
#define triClips(x0, y0, x1, y1, x2, y2, mar) (\
	(x0 > R-mar) || (x0 < L+mar) || \
	(x1 > R-mar) || (x1 < L+mar) || \
	(x2 > R-mar) || (x2 < L+mar) || \
	(y0 > B-mar) || (y0 < T+mar) || \
	(y1 > B-mar) || (y1 < T+mar) || \
	(y2 > B-mar) || (y2 < T+mar))

// planar video mode utilities
// select planes from right x offset
#define rightp(x) (0x0f>>(x))
// select planes from left x offset
#define leftp(x) (0x0f&(0x0f<<(x)))
// combined left and right offsets
#define linep(x0, x1) (leftp(x0)&rightp(x1))
// select planes in coordinates [x0, x1]
#define linepx(x0, x1) (linep((x0)&3, 3-(x1)&3))
// select plane for one pixel coordinate x
#define pixpx(x) (1<<((x)&3))

// random float in range [0, 1]
#define RANDF ((rand() % 10000) / (10000.0f-1.0f))

#define PI 3.1415926

// wait around for start/end of trace to sync
#define TRACESTART while (inportb(0x3da) & 8)
#define TRACEEND while (!(inportb(0x3da) & 8))

extern char oldvmode; // previous video mode
extern int wireframe; // draw wireframe triangles?
extern int filled; // fill triangles?
extern int faceculling; // cull back faces?
extern int zsort;  // sort triangles by z?
extern int clearscr; // screen clearing enabled
extern int clearcol; // screen clear color
extern int doublebuffer; // enable double buffering in MODEX

extern byte pg; // page number
extern unsigned pgoffs; // offset of page being written

struct tri {
	vec3 v0; // vertices
	vec3 v1;
	vec3 v2;

	float fc; // face center distance

	byte c; // color
};

#define RBUFFERLEN 512

// render buffer of triangles
extern struct tri r_buffer[RBUFFERLEN];

// indices of sorted triangles in r_buffer
extern unsigned r_sorted[RBUFFERLEN];
extern unsigned r_num; // number of triangles in buffer

// total triangle draw count
extern unsigned drawCount;

// render matrix for triangles used in r_add
extern mat4 r_matrix;

// primitive VGA routines

// initialize renderer, enter video mode
void r_init();
// return original video mode
void r_exit();
// initialize (clear) screen with color c by int 10h, slightly faster than fill
void r_scr(byte c);
// flip page
void r_flip();
// fill entire screen area with color c
void r_fill(byte c);
// vertical fill from top y0, height h, color c
void r_vfill(int y0, int h, byte c);
// horizontal line fill with color c (optional x sort and clipping)
void r_hlinefill(int x0, int x1, int y, byte c);
// vertical line fill with color c
void r_vlinefill(int x, int y0, int y1, byte c);

// put pixel of color c at (x, y)
void r_putpixel(int x, int y, byte c);
// rectangle fill, left corner (x, y), size (w, h), color c
void r_rectfill(int x, int y, int w, int h, byte c);
// fast asm triangle fill using integers, no clipping, color c
void r_trifill(int x0, int dx0, int x1, int dx1, int y, int dy, byte c);

// DRAW

// add triangle to render buffer
void r_add(vec3 *v0, vec3 *v1, vec3 *v2, byte c);
void r_addf(float v0x, float v0y, float v0z,
			float v1x, float v1y, float v1z,
			float v2x, float v2y, float v2z, byte c);

void r_sort(); // sort triangles by centroid z coordinate
void r_draw(); // draw all triangles in render buffer

// 2D draws
// draw line of color c from (x0, y0) to (x1, y1)
void r_drawLine(int x0, int y0, int x1, int y1, byte c);
// draw line of color c from v0 to v1 (clipped)
void r_drawLineClip(vec2 *v0, vec2 *v1, byte c);
// draw triangle of color c with vertices (x0, y0), (x1, y1), (x2, y2)
void r_drawTri(int x0, int y0, int x1, int y1, int x2, int y2, byte c);
// draw triangle of color c with vertices {v0, v1, v2} (clipped)
void r_drawTriClip(vec2 *v0, vec2 *v1, vec2 *v2, byte c);

// 3D perspective projected draws
// draw 3D point at v with color c
void r_drawPoint3D(vec3 *v0, byte c);
// draw 3D line from v0 to v1 with color c
void r_drawLine3D(vec3 *v0, vec3 *v1, byte c);
// draw 3D perspective projected triangle of color c with vertices {v0, v1, v2}
void r_drawTri3D(vec3 *v0, vec3 *v1, vec3 *v2, byte c);

void r_waitRetrace(); // wait for screen retrace to sync

// INPUT

#define SECOND 18.206
// convert itime 1/(18.206 s) to second
#define TOSECOND 0.054927f

extern int running; // program running?

extern int itime;  // time since start of program via int 0x1c (18.206/s)
void hookTime();
void unhookTime();
unsigned now(); // get system time

extern byte keycode; // keycode of currently pressed/released key
extern int keydown[256]; // key pressed down?
void hookKeys();
void unhookKeys();
void getInput(); // get which keys are pressed down

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
