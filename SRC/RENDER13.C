#include "SRC\RENDER.H"

#ifdef MODE13

void r_init()
{
	asm {
		mov ah, 0x0f
		int 0x10
		mov oldvmode, al // get current video mode

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
		mov al, oldvmode // return original video mode
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
		inc cx // n = x1 - x0 + 1
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
		inc cx // n = x1 - x0 + 1
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
		inc cx // n = y1 - y0 + 1
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

#define FP_FACTOR 7
#define X_BIAS 100

void r_trifill(int x0, int dx0, int x1, int dx1, int y, int dy, byte c)
{
	int k0, k1, r0, r1;
	k1 = abs(dx1);
	k1 <<= FP_FACTOR;
	r1 = k1 % dy;
	k1 /= dy;
	k1 *= sign(dx1);
	if (r1 >= dy<<1) k1 += 10000;

	k0 = abs(dx0);
	k0 <<= FP_FACTOR;
	r0 = k0 % dy;
	k0 /= dy;
	k0 *= sign(dx0);
	if (r0 >= dy<<1) k0 += 10000;
	asm {
		cli
		push bp

		mov ax, VSTART
		mov es, ax // video memory start

		mov ax, y // final line
		add ax, dy
		mov cx, W
		mul cx
		push ax // final line to stack

		mov cx, dy // y diff
		mov ax, dx1 // x diff
		mov si, ax

		cwd // sign of ax to dx
		xor ax, dx
		sub ax, dx // ax = abs(ax)

		shl ax, FP_FACTOR // mul fixed point
		xor dx, dx
		div cx // calculate unsigned slope dx/dy

		cmp si, 0
		jg tk1
		neg ax // put sign back
	}
	tk1:
	asm {
		mov ax, k1
		push ax // k1 to stack

		mov si, x1 // right point
		shl si, FP_FACTOR // mul fixed point
		add si, X_BIAS // right bias

		mov ax, dx0 // x diff
		mov bx, ax

		cwd
		xor ax, dx
		sub ax, dx // ax = abs(ax)

		shl ax, FP_FACTOR // mul fixed point
		xor dx, dx
		div cx // calculate unsigned slope dx/dy

		cmp bx, 0
		jg tk0
		neg ax // put sign back
	}
	tk0:
	asm {
		mov ax, k0
		push ax // k0 to stack

		mov bx, x0 // left point
		shl bx, FP_FACTOR // mul fixed point
		sub bx, X_BIAS // left bias

		mov dx, W
		mov ax, y
		mul dx // y offset
		mov dx, ax

		xor ax, ax
		mov al, c // color

		mov bp, sp
	}
	tfill:
	asm {
		mov di, bx // x0
		shr di, FP_FACTOR // div to get coordinates

		mov cx, si // x1
		shr cx, FP_FACTOR

		sub cx, di
		add di, dx // calculate final address

		rep stosb // fill line

		add dx, W // y += 1

		mov di, [bp]
		add bx, di // x0 += k0
		mov cx, [bp+2]
		add si, cx // x1 += k1

		mov cx, [bp+4]
		cmp dx, cx

		jb tfill // lines left?

		pop ax
		pop ax
		pop ax
		pop bp
		sti
	}
}

#endif

