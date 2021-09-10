#include "SRC\RENDER.H"

#if MODE13

void r_init()
{
	asm {
		xor ah, ah
		int 0x10
		mov vmode, al // get current video mode

		xor ah, ah
		mov bh, pg
		mov al, 0x13 // set 13
		int 0x10

		mov ah, 0x48
		mov bx, 0x10 // 64k
		int 0x21
	}
}

void r_exit()
{
	asm {
		xor ah, ah
		xor bh, bh
		xor bx, bx
		mov al, vmode // return original video mode
		int 0x10
	}
}

// put pixel of color c at (x, y)
void r_putpixel(int x, int y, BYTE c)
{
	asm {
		mov ax, vstart
		mov es, ax

		mov dx, y
		mov ax, W
		imul dx
		add ax, x

		mov di, ax

		mov dl, c
		mov [es:di], dl
	}
}

// fill screen area starting from vstart with color c
void r_clear(int c)
{
	asm {
		mov es, vstart
		xor di, di
		mov cx, W*H
		mov ax, c
		rep stosb
	}
}

// vertical fill from top y0, height h, color c
void r_vfill(int y0, int h, int c)
{
	y0 *= W;
	h *= W;
	asm {
		mov es, vstart
		mov di, y0
		mov cx, h
		mov ax, c
		rep stosb
	}
}

// clear screen with color c
void r_scr(BYTE c)
{
	asm {
		mov ah, 0x6
		xor al, al
		mov bh, c
		mov cx, 0x0100
		mov dx, 0x182a
		int 10h
	}
}

// rectangle fill, left corner (x, y), size (w, h), color c
void r_rectfill(int x, int y, int w, int h, BYTE c)
{
	w = clamp(w + x, 0, W);
	h = clamp(h + y, 0, H);
	x = clamp(x, L, R);
	y = clamp(y, T, B);

	if (w <= x || h <= y) {
		return;
	}

	w -= x;

	h *= W;
	y *= W;

	asm {
		mov es, vstart

		mov dx, h

		mov di, y
		add di, x

		mov cx, w

		mov bx, W
		sub bx, cx

		mov al, c
	}
	draw:
	asm {
		rep stosb

		mov cx, w

		add di, bx
		cmp di, dx
		jb draw
	}
}

// horizontal line draw with x sort
void r_hlinefill(int x0, int x1, int y, BYTE c)
{
	int to;
	if (x0 > x1) {
		to = x0;
		x0 = x1;
		x1 = to;
	}

	asm {
		mov ax, vstart
		mov es, ax

		mov dx, y
		mov ax, W
		imul dx

		mov bx, x1
		mov di, x0
		add bx, ax
		add di, ax

		mov dl, c
	}
	draw:
	asm {
		mov [es:di], dl
		inc di
		cmp di, bx
		jb draw
	}
}

// half triangle fill with clipping
void r_halftrifill(float x0, float x1, int y,
										int dy, float k0, float k1,
										BYTE c)
{
	int i, xi0, xi1;

	for (i = 0; i < dy; ++i) {
		x0 += k0;
		x1 += k1;
		y += 1;

		if (y > B)
			return;
		if (y < T)
			continue;
		if (x0 > R)
			continue;
		if (x1 < L)
			continue;

		xi0 = round(x0);
		xi1 = round(x1);

		if (x0 < L)
			xi0 = L;
		if (x1 > R)
			xi1 = R;

		asm {
			mov es, vstart

			mov dx, y
			mov ax, W
			imul dx // y offset

			mov cx, xi1
			mov di, xi0

			sub cx, di
			add cx, 1 // n = x1 - x0 + 1
			add di, ax // calculate address

			mov al, c // color

			rep stosb
		}
	}
}

#define TERR 64 // correction
/*
	asm triangle fill using integers
	no clipping, but slightly faster
*/
void r_nchalftrifill(float x0, float x1, int y,
										int dy, float k0, float k1,
										BYTE c)
{
	unsigned xi0, xi1, ki0, ki1, ye;

	xi0 = (unsigned)(x0*128.0f);
	xi1 = (unsigned)(x1*128.0f);
	ki0 = (unsigned)(k0*128.0f);
	ki1 = (unsigned)(k1*128.0f);

	if (dy <= 0)
		return;

	ye = y + dy;

	y *= W;
	ye *= W;

	asm {
		mov es, vstart // video memory start

		mov bx, xi0 // initial points
		mov si, xi1
#ifdef TERR
		sub bx, TERR/2
		add si, TERR/2
#endif
		mov dx, y
	}
	drawt:
	asm {
		add bx, ki0 // x0 += k0
		add si, ki1 // x1 += k1
		add dx, W // y += 1

		mov di, bx
#ifdef TERR
		sub di, TERR // correction
#endif
		shr di, 7 // divide by 128

		mov cx, si
#ifdef TERR
		add cx, TERR
#endif
		shr cx, 7

		sub cx, di
		add di, dx // calculate final addresses

		mov al, c // color

		rep stosb

		mov ax, ye
		cmp dx, ax
		jb drawt // lines left?
	}
}

#endif

