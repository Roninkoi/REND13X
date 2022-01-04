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
	asm {
		mov ax, VSTART
		mov es, ax

		mov di, W
		mov ax, y0
		mul di
		mov di, ax

		mov cx, W
		mov ax, h
		mul cx
		mov cx, ax

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


//#define RECTCLIP
void r_rectfill(int x, int y, int w, int h, byte c)
{
#ifdef RECTCLIP
	w = clamp(w + x, 0, W);
	h = clamp(h + y, 0, H);
	x = clamp(x, L, R);
	y = clamp(y, T, B);

	if (w <= x || h <= y) {
		return;
	}
#endif

	asm {
		mov ax, VSTART
		mov es, ax

		mov dx, y
		mov ax, W
		mul dx // y offset

		mov di, x // start points
		mov cx, di
		add cx, w

		sub cx, di
		mov si, cx
		add di, ax // calculate address

		mov dx, y
		add dx, h
		mov ax, W
		mul dx
		mov dx, ax // final line start address

		xor ah, ah
		mov al, c // color

		mov bx, W
		sub bx, cx
	}
	rfill:
	asm {
		rep stosb // fill line

		mov cx, si
		add di, bx // y += 1

		cmp di, dx
		jb rfill
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
	int k0 = dx0<<7;
	int k1 = dx1<<7;
	int ye = (y+dy)*W;

	if (dy <= 0) return;

	k0 /= dy;
	k1 /= dy;

	asm {
		mov ax, VSTART
		mov es, ax // video memory start

		mov bx, x0 // initial points
		shl bx, 7
		mov si, x1
		shl si, 7

		mov dx, W
		mov ax, y
		mul dx
		mov dx, ax // y offset
	}
	tfill:
	asm {
		mov di, bx
		shr di, 7 // divide to calculate coordinates

		mov cx, si
		shr cx, 7

		sub cx, di
		inc cx
		add di, dx // calculate final addresses

		xor ax, ax
		mov al, c // color

		rep stosb // fill line

		add bx, k0 // x0 += k0
		add si, k1 // x1 += k1
		add dx, W // y += 1

		mov ax, ye
		cmp dx, ax
		jb tfill // lines left?
	}
}

#endif

