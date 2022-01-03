#include "SRC\RENDER.H"

#ifdef MODE13

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

void r_putpixel(int x, int y, byte c)
{
	asm {
		mov ax, VSTART
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

void r_fill(byte c)
{
	asm {
		mov ax, VSTART
		mov es, ax
		xor di, di
		mov cx, W*H/2
		xor ax, ax
		mov al, c
		mov ah, c
		rep stosw
	}
}

void r_vfill(int y0, int h, byte c)
{
	y0 *= W;
	h *= W;
	asm {
		mov ax, VSTART
		mov es, ax
		mov di, y0
		mov cx, h
		xor ah, ah
		mov al, c
		rep stosb
	}
}

void r_scr(byte c)
{
	asm {
		xor al, al
		mov ah, 0x6
		xor bl, bl
		mov bh, c
		mov cx, 0x0100
		mov dx, 0x182a
		int 0x10
	}
}

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
		mov ax, VSTART
		mov es, ax

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

//#define HLINESORT
//#define HLINECLIP
void r_hlinefill(int x0, int x1, int y, byte c)
{
#ifdef HLINESORT
	int to;
	if (x0 > x1) {
		to = x0;
		x0 = x1;
		x1 = to;
	}
#endif

#ifdef HLINECLIP
	if (x0 > R || x1 < L || y > B || y < T)
		return;
	if (x0 < L)
		x0 = L;
	if (x1 > R)
		x1 = R;
#endif

	asm {
		mov ax, VSTART
		mov es, ax

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
		mov ax, VSTART
		mov es, ax

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

void r_vlinefill(int x, int y0, int y1, byte c)
{
	asm {
		mov ax, VSTART
		mov es, ax

		mov di, y0
		mov ax, W
		mul di
		mov di, ax
		add di, x // starting address

		mov cx, y1
		add cx, 1 // n = y1 - y0 + 1
		mov ax, W
		mul cx

		mov dx, ax
		add dx, x // calculate final address

		xor ah, ah
		mov al, c // color
	}
	vfill:
	asm {
		mov [es:di], al
		add di, W
		cmp di, dx
		jb vfill
	}
}

void r_trifill(int x0, int dx0, int x1, int dx1, int y, int dy, byte c)
{
	int i, diff0, diff1, d0, d1;
	int s0 = 1, s1 = 1;
	int y0 = y + dy;

	if (dx0 < 0) {
		s0 = -1;
		dx0 = -dx0;
	}
	if (dx1 < 0) {
		s1 = -1;
		dx1 = -dx1;
	}

	diff0 = dx0 - dy;
	diff1 = dx1 - dy;

	asm {
		mov ax, VSTART
		mov es, ax

		mov dx, y
		mov ax, W
		mul dx // y offset
		mov dx, ax

		mov cx, x1
		mov di, x0

		sub cx, di
		add cx, 1 // n = x1 - x0 + 1
		add di, ax // calculate address

		xor ah, ah
		mov al, c // color
	}
	tfill:
	asm {
		rep stosb

		add di, W
		cmp di, dx
		jb tfill
	}
}

#endif

