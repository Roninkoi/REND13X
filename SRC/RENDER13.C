#include "SRC\RENDER.H"

#ifdef MODE13

// use routines refined in .ASM? (else inline C)
//#define ASM

#ifdef ASM

extern int r_init();

extern void r_exit(int vmode);

extern void r_putpixel(int x, int y, byte c);

extern void r_scr(byte c);

extern void r_fill(byte c);

extern void r_vfill(int y0, int h, byte c);

extern void r_rectfill(int x, int y, int w, int h, byte c);

extern void r_hlinefill(int x0, int x1, int y, byte c);

extern void r_hlinefill2(int x0, int x1, int y, byte c);

extern void r_vlinefill(int x, int y0, int y1, byte c);

extern void r_linefill(int x0, int y0, int x1, int y1, byte c);

extern void r_trifill(int x0, int dx0, int x1, int dx1, int y, int dy, byte c);

#else

int r_init()
{
	int vmode;
	asm {
		mov ah, 0x0f
		int 0x10
		xor ah, ah
		mov vmode, ax // get current video mode

		xor ah, ah
		mov al, 0x13 // set mode 13h
		int 0x10

		mov ah, 0x48
		mov bx, 0x10 // allocate 64k
		int 0x21
	}
	return vmode;
}

void r_exit(int vmode)
{
	asm {
		xor bx, bx
		mov ax, vmode // return original video mode
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

void r_rectfill(int x, int y, int w, int h, byte c)
{
	asm {
		mov ax, VSTART
		mov es, ax

		mov dx, y
		mov ax, W
		mul dx // y offset

		mov di, x // start points
		mov cx, di
		mov cx, w // right edge

		mov si, cx
		add di, ax // start address

		mov dx, y
		add dx, h
		mov ax, W
		mul dx
		mov dx, ax // final line address

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

void r_hlinefill(int x0, int x1, int y, byte c)
{
	asm {
		mov ax, VSTART
		mov es, ax

		mov dx, y
		mov ax, W
		mul dx // y offset

		mov cx, x1
		mov di, x0

		sub cx, di // x1 - x0
		inc cx // w = x1 - x0 + 1
		add di, ax // start address = x0 + y * W

		xor ah, ah
		mov al, c // color

		rep stosb // fill line
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

		sub cx, di // x1 - x0
		inc cx // w = x1 - x0 + 1
		shr cx, 1 // divide by 2, discard odd
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

void r_linefill(int x0, int y0, int x1, int y1, byte c)
{
	asm {
		cli
		push bp

		mov ax, VSTART
		mov es, ax // video memory start

		mov cx, x1
		mov bx, x0
		sub cx, bx // $cx = dx = x1 - x0

		mov dx, y1
		mov bx, y0
		sub dx, bx // $dx = dy = y1 - y0

		mov si, 1 // sx
		mov di, W // sy

		cmp cx, 0
		jge lfdxs

		// if dx < 0
		neg si
		neg cx
	}
	lfdxs:
	asm {
		cmp dx, 0
		jge lfdys

		// if dy < 0
		neg di
		neg dx
	}
	lfdys:
	asm {
		push cx // dx to stack +8
		push dx // dy to stack +6
		push si // sx to stack +4
		push di // sy to stack +2

		mov di, cx
		sub di, dx // $di = diff

		cmp cx, dx
		jg lfmaxd

		// if dy > dx
		mov cx, dx // maxd = dy
	}
	lfmaxd:
	asm {
		push cx // maxd to stack

		mov bx, x0 // $bx = x = x0
		mov cx, y0 // $cx = y = y0
		mov ax, W
		mul cx
		mov cx, ax
		add cx, bx // start address x0 + y0 * W

		xor ax, ax
		mov al, c // color

		mov bp, sp

		xor bx, bx // i = $bx
	}
	lfill:
	asm {
		mov si, cx // pixel address x + y * W

		mov [es:si], al // put pixel

		mov si, di
		sal si, 1 // $si *= 2
		mov dx, [bp+8] // dx
		cmp si, dx
		jg lfx

		// if 2*d <= dx
		add di, dx
		mov dx, [bp+2] // sy
		add cx, dx // y += 1
	}
	lfx:
	asm {
		mov dx, [bp+6] // dy
		neg dx
		cmp si, dx
		jl lfy

		// if 2*d >= -dy
		add di, dx
		mov dx, [bp+4] // sx
		add cx, dx // x += 1
	}
	lfy:
	asm {
		mov dx, [bp] // maxd
		inc bx // ++i
		cmp bx, dx // end?
		jbe lfill

		pop ax
		pop ax
		pop ax
		pop ax
		pop ax
		pop bp
		sti
	}
}

void r_trifill(int x0, int dx0, int x1, int dx1, int y, int dy, byte c)
{
	asm {
		cli
		push bp

		mov ax, VSTART
		mov es, ax // video memory start

		mov ax, y // final line
		add ax, dy
		mov cx, W
		mul cx
		push ax // final line start address y * W to stack

		mov cx, dy
			
		mov ax, dx1 // $ax = slope 1
		mov si, ax
		
		cmp si, 0 // dx1 > 0?
		jg tfs1
		neg ax // $ax = |dx1|
	}
	tfs1:
	asm {
		shl ax, 7 // 128 * |dx1|
		xor dx, dx
		div cx // |k1| = (128 * |dx1|) / dy
		cmp si, 0 // dx1 > 0?
		jg tfss1
		neg ax // k1 = -|k1|
	}
	tfss1:
	asm {
		push ax // k1 to stack

		mov si, x1 // right point
		inc si
		shl si, 7
		add si, 80 // right bias

		mov ax, dx0 // $ax = slope 0
		mov bx, ax

		cmp bx, 0 // dx0 > 0?
		jg tfs0
		neg ax // $ax = |dx0|
	}
	tfs0:
	asm {
		shl ax, 7 // 128 * |dx0|
		xor dx, dx
		div cx // |k0| = (128 * |dx0|) / dy
		cmp bx, 0 // dx0 > 0?
		jg tfss0
		neg ax // k0 = -|k0|
	}
	tfss0:
	asm {
		push ax // k0 to stack

		mov bx, x0 // left point
		inc bx
		shl bx, 7
		sub bx, 80 // left bias

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
		shr di, 7 // divide to calculate coordinate

		mov cx, si // x1
		shr cx, 7 // divide to calculate coordinate

		sub cx, di
		cmp cx, 0 // check if x backwards
		jl tfend
		add di, dx // calculate final address

		rep stosb // fill line

		add dx, W // y += 1

		mov di, [bp]
		add bx, di // x0 += k0
		mov cx, [bp+2]
		add si, cx // x1 += k1

		mov cx, [bp+4] // yend
		cmp dx, cx
		jbe tfill // lines left?
	}
	tfend:
	asm {
		pop ax
		pop ax
		pop ax
		pop bp
		sti
	}
}

#endif

#endif

