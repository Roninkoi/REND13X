#include "SRC\RENDER.H"

#if MODE13

void r_init()
{
	asm {
		mov ah, 0x0f
		int 0x10
		mov vmode, al // get current video mode

		xor ah, ah
		mov al, 0x13 // set mode 13h
		int 0x10

		mov ah, 0x48
		mov bx, 0x10 // allocate 64k
		int 0x21
	}
}

void r_exit()
{
	asm {
		xor ah, ah
		xor bx, bx
		mov al, vmode // return original video mode
		int 0x10
	}
}

// put pixel of color c at (x, y)
void r_putpixel(int x, int y, byte c)
{
	asm {
		mov ax, vstart
		mov es, ax

		mov dx, y
		mov ax, W
		mul dx
		add ax, x

		mov di, ax

		xor dh, dh
		mov dl, c
		mov [es:di], dl
	}
}

// fill screen area starting from vstart with color c
void r_clear(byte c)
{
	asm {
		mov es, vstart
		xor di, di
		mov cx, W*H/2
		xor ax, ax
		mov al, c
		mov ah, c
		rep stosw
	}
}

// vertical fill from top y0, height h, color c
void r_vfill(int y0, int h, byte c)
{
	y0 *= W;
	h *= W;
	asm {
		mov es, vstart
		mov di, y0
		mov cx, h
		xor ah, ah
		mov al, c
		rep stosb
	}
}

// clear screen with color c
void r_scr(byte c)
{
	asm {
		xor al, al
		mov ah, 0x6
		xor bl, bl
		mov bh, c
		mov cx, 0x0100
		mov dx, 0x182a
		int 10h
	}
}

// rectangle fill, left corner (x, y), size (w, h), color c
void r_rectfill(int x, int y, int w, int h, byte c)
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

		xor ah, ah
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

#define HLINESORT 0
#define HLINECLIP 0
// horizontal line draw with optional x sort and clipping
void r_hlinefill(int x0, int x1, int y, byte c)
{
#if HLINESORT
	int to;
	if (x0 > x1) {
		to = x0;
		x0 = x1;
		x1 = to;
	}
#endif

#if HLINECLIP
	if (x0 > R || x1 < L || y > B || y < T)
		return;
	if (x0 < L)
		x0 = L;
	if (x1 > R)
		x1 = R;
#endif

	asm {
		mov es, vstart

		mov dx, y
		mov ax, W
		mul dx // y offset

		mov cx, x1
		mov di, x0

		sub cx, di
		add cx, 1 // n = x1 - x0 + 1
		add di, ax // calculate address

		xor ah, ah
		mov al, c // color

		rep stosb
	}
}

// horizontal line fill two pixels at a time
void r_hlinefill2(int x0, int x1, int y, byte c)
{
	asm {
		mov es, vstart

		mov dx, y
		mov ax, W
		mul dx // y offset

		mov cx, x1
		mov di, x0

		sub cx, di
		shr cx, 1 // divide by 2, discard odd
		add cx, 1 // n = x1 - x0 + 1
		add di, ax // calculate address

		mov al, c // color
		mov ah, c

		rep stosw
	}
}

#define TRIMAR 64 // correction
/*
	asm half triangle fill using integers
	no clipping, but slightly faster
*/
void r_trifill(float x0, float x1, int y, int dy, float k0, float k1, byte c)
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
#ifdef TRIMAR
		sub bx, TRIMAR/2
		add si, TRIMAR/2
#endif
		mov dx, y
	}
	drawt:
	asm {
		add bx, ki0 // x0 += k0
		add si, ki1 // x1 += k1
		add dx, W // y += 1

		mov di, bx
#ifdef TRIMAR
		sub di, TRIMAR // correction
#endif
		shr di, 7 // divide by 128

		mov cx, si
#ifdef TRIMAR
		add cx, TRIMAR
#endif
		shr cx, 7

		sub cx, di
		add di, dx // calculate final addresses

		xor ah, ah
		mov al, c // color

		rep stosb

		mov ax, ye
		cmp dx, ax
		jb drawt // lines left?
	}
}

#endif

