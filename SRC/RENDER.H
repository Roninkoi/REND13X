	/*
 * DOS Software renderer by Roninkoi 2020
 * Draw routines written in x86 assembly, mode 13h
 * 320x200, 256 colors
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

#define MODE13 0
#define MODEX 1

// viewport dimensions
#define W 320
#define L 0
#define R 319

#if MODE13
#define H 200
#define T 8
#define B 199
#endif

#if MODEX
#define H 240
#define T 0
#define B 239
#endif

#define VSTART 0xa000
#define EPSILON 0.0031f

#define ZNEAR 0.2
#define ZFAR 20.0

#define min(x, y) (x > y ? y : x)
#define max(x, y) (x < y ? y : x)
#define clamp(x, min, max) (x > max ? max : (x < min ? min : x))

#define sign(x) (x >= 0 ? 1 : -1)

#define abs(x) (x < 0 ? -(x) : x)

#define round(x) floor(x + 0.5f)

#define PI 3.1415926

#define TRACESTART while (inportb(0x3da) & 8)
#define TRACEEND while (!(inportb(0x3da) & 8))

extern unsigned vstart;

extern int redraw;
extern int itime;
extern char vmode;
extern int wireframe;
extern int faceculling;
extern int zsort;
extern int clearscr;
extern int clearcol;

extern byte pg;
extern int pgoffs;

struct tri {
	vec4 v0; // vertices
	vec4 v1;
	vec4 v2;

	float fc; // face center distance

	byte c; // color
};

extern struct tri r_buffer[256];

extern byte r_s[256]; // sorted
extern byte r_n;

extern unsigned drawcount;

// render matrix
extern mat4 rm;

extern byte keycode;
extern byte keycodeBuffer[256];
extern byte keycodeTail;

void interrupt getTime();

unsigned now();

void r_init();
void r_exit();
void r_scr(byte c);
void r_flip();
void r_clear(byte c);
void r_vfill(int y0, int h, byte c);
void r_hlinefill(int x0, int x1, int y, byte c);
void r_putpixel(int x, int y, byte c);
void r_rectfill(int x, int y, int w, int h, byte c);

void r_drawline(float (*v0)[2], float (*v1)[2], byte c);
void r_drawlinef(float x0, float y0, float x1, float y1, byte c);

void r_drawtri(float v[3][2], byte c);
void r_drawtri3d(vec4* v0, vec4* v1, vec4* v2, byte c);
void r_trifill(float x0, float x1, int y, int dy, float k0, float k1, byte c);
void r_trifillclip(float x0, float x1, int y, int dy, float k0, float k1, byte c);

// DRAW

void r_add(vec4* v0, vec4* v1, vec4* v2, byte c);
void r_addf(float v0x, float v0y, float v0z,
			float v1x, float v1y, float v1z,
			float v2x, float v2y, float v2z, byte c);
void r_sort();
void r_draw();

void r_waitRetrace();

#endif
