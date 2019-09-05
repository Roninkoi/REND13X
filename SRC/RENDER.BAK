/*
 * DOS Software renderer by Ronin748 2018
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

typedef unsigned char BYTE;

// viewport dimensions
#define W 320
#define H 200
#define T 8
#define B 0

#define ZNEAR 0.2

#define ZFAR 10.0

#define clamp(x, min, max) x > max ? max : (x < min ? min : x)

#define sign(x) (x) >= 0 ? 1 : -1;

#define abs(x) x < 0 ? -x : x

#define PI 3.1415926

extern int redraw;
extern int itime;
extern char vmode;

extern int pg;

struct tri {
	vec4 v0; // vertices
	vec4 v1;
	vec4 v2;

	float fc; // face center distance

	BYTE c; // color
};

extern struct tri r_buffer[256];

extern BYTE r_s[256]; // sorted

extern BYTE r_n;

// render matrix
extern mat4 rm;

extern unsigned char keycode;
extern unsigned char keycodeBuffer[256];
extern unsigned char keycodeTail;

void interrupt getTime();

unsigned now();

void r_init();

void r_exit();

void r_flip();

void r_drawpixel(int x, int y, unsigned char c);

void r_swap();

void r_drawrect(int x, int y, int w, int h, unsigned char c);

// line draw, pretty slow, implement in asm
void r_drawline(float (*v0)[2], float (*v1)[2], unsigned char c);

void r_drawlinef(float x0, float y0, float x1, float y1, unsigned char c);

// horizontal line draw with x sort, usually fast
void r_drawlineh(int x0, int x1, int y, unsigned char c);

/*
 * triangle draw coordinate system
 * same as opengl with origin [0.0f, 0.0f]
 * screen range [-1.0f, 1.0f], [-1.0f, 1.0f]
 */
void r_drawtri(float (*v)[3][2], unsigned char c);

void r_drawtri3d(vec4* v0, vec4* v1, vec4* v2, unsigned char c);

void r_fill();

void r_clear();

void r_scr();

#endif
