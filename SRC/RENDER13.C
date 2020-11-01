#include "SRC\RENDER.H"

#if MODE13

void r_init()
{
	asm {
		mov ah, 0x0f
		int 0x10
		mov vmode, al // get current video mode

		mov ah, 0
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
		mov ah, 0
		mov bh, 0
		mov bx, 0
		mov al, vmode // return original video mode
		int 0x10
	}
}

void r_drawpixel(int x, int y, BYTE c)
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

void r_drawrect(int x, int y, int w, int h, BYTE c)
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

// line draw, pretty slow, implement in asm
void r_drawline(float (*v0)[2], float (*v1)[2], BYTE c)
{
	int x;
	int y;
	int vx0;
	int vy0;
	int vx1;
	int vy1;
	float dx;
	float dy;
	float k;
	int i;
	int s;
	float kx;
	float ky;
	float diff;

	if ((*v0)[1] < (*v1)[1]) {
		vx0 = (*v0)[0];
		vy0 = (*v0)[1];
		vx1 = (*v1)[0];
		vy1 = (*v1)[1];
	}
	else {
		vx1 = (*v0)[0];
		vy1 = (*v0)[1];
		vx0 = (*v1)[0];
		vy0 = (*v1)[1];
	}

	dx = vx1 - vx0;
	dy = vy1 - vy0;

	k = dy/dx;

	if (fabs(k) <= 1) {
		s = sign(dx);
		ky = k;
		kx = 1.0f;
		diff = fabs(dx);
	}
	else {
		s = sign(dy);
		kx = 1.0f/k;
		ky = 1.0f;
		diff = fabs(dy);
	}

	for (i = 0; i <= diff; i += 1) {
		x = vx0 + (float)i*s*kx;
		y = vy0 + (float)i*s*ky;

		if (y > B)
			return;
		if (y < T)
			continue;
		if (x > R)
			continue;
		if (x < L)
			continue;

		if (x < L)
			x = L;
		if (x > R)
			x = R;

		y *= W;
		x += y;

		asm {
			mov ax, vstart
			mov es, ax
			mov di, x
			mov dl, c
			mov [es:di], dl
		}
	}
}

// horizontal line draw with x sort
void r_drawlineh(int x0, int x1, int y, BYTE c)
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
			add di, ax // calculate address

			mov al, c // color

			rep stosb
		}
	}
}

#define TERR 64 // correction
/*
	Asm triangle fill using integers
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
#ifdef TERR
		add bx, TERR
#endif
		mov si, xi1
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

#endif

