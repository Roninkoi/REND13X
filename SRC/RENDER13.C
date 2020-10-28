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
	h += y;
	w += x;

	x = clamp(x, L, R);
	y = clamp(y, L, R);
	w = clamp(w, L, R);
	h = clamp(h, L, R);

	if (w <= x || h <= y) {
		return;
	}

	h *= W;
	y *= W;

	asm {
		push bp
		mov ax, vstart
		mov es, ax

		mov dl, c

		mov si, x
		mov ax, si
		mov bx, y
		mov cx, w
		mov bp, h
	}
	draw:
	asm {
		mov di, ax
		add di, bx
		mov [es:di], dl
		inc ax
		cmp ax, cx
		jb draw
		add bx, W
		mov ax, si
		cmp bx, bp
		jb draw
		pop bp
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

void r_drawlinef(float x0, float y0, float x1, float y1, BYTE c)
{
	float v0[2];
	float v1[2];

	v0[0] = x0;
	v0[1] = y0;

	v1[0] = x1;
	v1[1] = y1;

	r_drawline(&v0, &v1, c);
}

// horizontal line draw with x sort, usually fast
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
			mov ax, vstart
			mov es, ax

			mov dx, y
			mov ax, W
			imul dx

			mov si, xi1
			mov di, xi0
			add si, ax
			add di, ax

			mov dl, c
		}
		drawt:
		asm {
			mov [es:di], dl
			add di, 1
			cmp di, si
			jbe drawt
		}
	}
}

/*
	Asm triangle fill using integers
	no clipping, but slightly faster
*/
void r_nchalftrifill(float x0, float x1, int y,
										int dy, float k0, float k1,
										BYTE c)
{
	unsigned xi0, xi1, ki0, ki1, ye;
	unsigned err = 40; // correction

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
		mov ax, vstart // video memory start
		mov es, ax

		mov bx, xi0 // initial points
		add bx, err
		mov cx, xi1
		mov dx, y
	}
	drawt:
	asm {
		add bx, ki0 // x0 += k0
		add cx, ki1 // x1 += k1
		add dx, W // y += 1
	}
	asm {
		mov di, bx
		sub di, err // correction
		shr di, 7 // divide by 128

		mov si, cx
		add si, err
		shr si, 7
	}
	asm {
		add si, dx // calculate final addresses
		add di, dx

		mov al, c // color
	}
	drawl: // line draw
	asm {
		mov [es:di], al // write to screen
		add di, 1
		cmp di, si
		jb drawl // x < x1?

		mov ax, ye
		cmp dx, ax
		jb drawt // lines left?
	}
}

void r_clear()
{
	asm {
		mov ax, vstart
		mov es, ax
		xor di, di
		mov cx, W*H
		mov ax, 0
		rep stosw
	}
}

void r_scr()
{
	asm {
		mov ah, 0x6
		xor al, al
		mov bh, 0x3
		mov cx, 0x0100
		mov dx, 0x182a
		int 10h
	}
}

#endif

