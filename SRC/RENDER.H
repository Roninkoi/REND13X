/*
 * DOS Software renderer by Roninkoi 2020
 * (ANSI?) C, draw routines written in x86 assembly
 * Compiled using Turbo C++ 3.0 or equivalent
 * Mode 13h (320x200) or mode X (320x240), 256 colors
 */

#ifndef RENDER_H
#define RENDER_H

#include <stdio.h>
#include <math.h>
#include <time.h>
#include <dos.h>

#include "SRC\VEC.H"
#include "SRC\TEX.H"
#include "SRC\DEFS.H"

extern int running; // program running?

extern int oldvmode; // previous video mode
extern int wireframe; // draw wireframe triangles?
extern int filled; // fill triangles?
extern int faceculling; // cull back faces?
extern int zsort;  // sort triangles by z?
extern int clearscr; // screen clearing enabled
extern byte clearcol; // screen clear color
extern int doublebuffer; // enable double buffering in MODEX

extern byte page; // page number

typedef struct {
	vec3 v0; // vertices
	vec3 v1;
	vec3 v2;

	float fc; // face center distance

	byte c; // color
	Texture *tex; // texture (sprite)
} Tri;

// render buffer of triangles
extern Tri r_buffer[RBUFFERLEN];

// indices of sorted triangles in r_buffer
extern unsigned r_sorted[RBUFFERLEN];
extern unsigned r_num; // number of triangles in buffer

// total triangle/sprite draw count
extern unsigned drawCount;
// triangle/sprite draw time
extern float drawTime;

// render matrix for triangles used in r_add
extern mat4 r_matrix;

extern TextureAtlas textureAtlas;

// primitive VGA routines

// initialize renderer, enter video mode
int far r_init(void);
// return original video mode
void far r_exit(int vmode);
// wait for screen retrace
void far r_waitretrace(void);
// select page to draw
int far r_page(int pg);
// clear screen
void r_clear(void);
// synchronize screen (wait/flip)
void r_sync(void);

// put pixel of color c at (x, y)
void far r_putpixel(int x, int y, byte c);

// fill entire screen area with color c
void far r_fill(byte c);
// vertical fill from top y0, height h, color c
void far r_vfill(int y0, int h, byte c);
// horizontal line fill with color c (optional x sort and clipping)
void far r_hlinefill(int x0, int x1, int y, byte c);
// vertical line fill with color c
void far r_vlinefill(int x, int y0, int y1, byte c);
// fill plane p from x0 to x1
void far r_hplanefill(int x0, int x1, int y, int p, byte c);
// line fill from (x0, y0) to (x1, y1) with color c
void far r_linefill(int x0, int y0, int x1, int y1, byte c);

// rectangle fill, left corner (x, y), size (w, h), color c
void far r_rectfill(int x, int y, int w, int h, byte c);
// fast asm triangle fill using integers, no clipping, color c
void far r_trifill(int x0, int dx0, int x1, int dx1, int y, int dy, byte c);

// fill (w, h) sprite at (x, y) with (tw, th) texture at tstart + (tx, ty)
void far r_spritefill(int x, int y, int w, int h,
			    int tx, int ty, int tw, int th, int tw0, int th0,
			    unsigned char far *tstart);
void far r_spritefill2(int x, int y, int w, int h, int tstart);

// DRAW

// add triangle to render buffer
void r_add(vec3 *v0, vec3 *v1, vec3 *v2, byte c);
void r_addf(float v0x, float v0y, float v0z,
			float v1x, float v1y, float v1z,
			float v2x, float v2y, float v2z, byte c);
// add sprite to render buffer
void r_addSprite(vec3 *v, float w, float h, Texture *tex);

void r_sort(void); // sort triangles by centroid z coordinate
void r_draw(void); // draw all triangles in render buffer

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
// draw 3D perspective projected textured sprite at v with dimensions (w, h)
void r_drawSprite3D(vec3 *v, float w, float h, Texture *tex);

// draw 2D textured sprite at (x, y)
void r_drawSprite(int x, int y, int w, int h, Texture *tex);
// draw 2D textured sprite id at (x, y) from atlas
void r_drawAtlasSprite(int x, int y, TextureAtlas *atlas, int id);
// draw character from texture atlas font
void r_drawAtlasFont(int x, int y, char c);
// draw string using font
void r_drawString(int x, int y, char *str);

// INPUT

extern FILE *outfile;

extern unsigned volatile itime;  // time since start of program via int 0x1c (18.206/s)
void hookTime(void);
void unhookTime(void);
unsigned now(void); // get system time
void timerStart(void); // accurate timer
float timerStop(void);

extern byte volatile keycode; // keycode of currently pressed/released key
extern int keyDown[256]; // key pressed down?
extern pix mousePos; // mouse position
extern unsigned mouseLeft; // mouse left button pressed down?
extern unsigned mouseRight; // mouse right button pressed down?
extern unsigned hasMouse; // mouse driver present?

void hookKeys(void); // attach keyboard interrupt handler
void unhookKeys(void);
void hookMouse(void); // attach mouse interrupt handler
void unhookMouse(void);
void getInput(void); // get which keys are pressed down

#endif
