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

// viewport dimensions
#define W 320
#define H 200
#define T 8
#define B 0

#define clamp(x, min, max) x > max ? max : (x < min ? min : x)

#define sign(x) (x) >= 0 ? 1 : -1;

#define PI 3.1415926

void interrupt (*oldkb) ();

void interrupt (*oldtime) ();

int redraw;
int itime;

void interrupt getTime()
{
	asm cli

	asm {
		sti
	}
	itime += 1;
	redraw = 1;
}

unsigned char keycode;
unsigned char keycodeBuffer[256];
unsigned char keycodeTail;

unsigned now() {
	return (unsigned)time(NULL);
}

int pg = 0;

void r_init();

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

void r_drawtri3d(struct vec4* v0, struct vec4* v1, struct vec4* v2, unsigned char c);

void r_fill();

void r_clear();

void r_scr();

#endif
