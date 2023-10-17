#include "SRC\RENDER.H"

#ifdef MODEX

// use routines refined in .ASM? (else inline C)
//#define ASM

#ifdef ASM

extern int r_init13();

extern int r_init();

extern void r_exit(int vmode);

extern void r_waitretrace();

extern int r_flip(int pgo);

extern void r_putpixel(int x, int y, byte c);

extern void r_fill(byte c);

extern void r_planefill(int x, int y, int p, byte c);

extern void r_hlinefill1(int x0, int x1, int y, byte c);

extern void r_hlinefill2(int x0, int x1, int y, byte c);

extern void r_hplanefill(int x0, int x1, int y, int p, byte c);

extern void r_vplanefill(int x, int y0, int y1, int p, byte c);

extern void r_linefill(int x0, int y0, int x1, int y1, byte c);

extern void r_triplanefill(int x0, int dx0, int x1, int dx1, int y, int dy, int p, byte c);

#else

#define SCI 0x03c4
#define SCD 0x03c5
#define CRTI 0x03d4
#define MISCOUT 0x03c2

#define MAP_MASK 0x02
#define ALL_PLANES 0x0f00
#define PIX_PLANE 0x0100

#define CRTHI 0x0c
#define CRTLO 0x0d

int pgoffs; // offset of page being written

const unsigned crtparam[10] = {
		0x00d06, // vert tot
		0x03e07, // overflow
		0x04109, // cell height
		0x0ea10, // vsync start
		0x0ac11, // vsync end
		0x0df12, // display vert
		0x00014, // no dword
		0x0e715, // vblank start
		0x00616, // vblank end
		0x0e317 // byte mode
};

int r_init13()
{
	int vmode;
	asm {
		mov ah, 0x0f // get current video mode
		int 0x10
		xor ah, ah
		mov vmode, ax

		mov ax, 0x13 // set 13
		int 0x10
	}
	return vmode;
}

int r_init()
{
	int vmode = r_init13();
	asm {
		mov dx, SCI // sequence controller
		mov ax, 0x0604
		out dx, ax // no chain 4
		mov ax, 0x0100
		out dx, ax // sync reset
		mov dx, MISCOUT // misc output
		mov al, 0x0e3
		out dx, al // 25 MHz clock, 60 Hz scan
		mov dx, SCI
		mov ax, 0x0300
		out dx, ax // restart sequencer

		mov dx, CRTI // CRT controller
		mov al, 0x11 // vsync end reg contains write
		out dx, al

		inc dx // CRT controller data register
		in al, dx // get vsync end reg
		and al, 0x7f // no write protect
		out dx, al

		mov dx, CRTI
		cld
		mov si, offset crtparam
		mov cx, 10 // length of crtparam
	}
	crtp:
	asm {
		lodsw
		out dx, ax
		loop crtp
	}
	return vmode;
}

void r_exit(int vmode)
{
	asm {
		mov ah, 0
		mov bh, 0
		mov bx, 0
		mov ax, vmode // return original video mode
		int 0x10
	}
}

void r_waitretrace()
{
	asm cli
	
	twaits:
	asm {
		mov dx, 0x3da
		in al, dx
		and al, 8
		cmp al, 0
		jg twaits
	}
	twaite:
	asm {
		mov dx, 0x3da
		in al, dx
		and al, 8
		cmp al, 0
		jle twaite

		sti
	}
}

int r_flip(int pg)
{
	int newpg;
	
	asm cli

	ftwaits:
	asm {
		mov dx, 0x3da // trace start
		in al, dx
		and al, 8
		cmp al, 0
		jg ftwaits

		mov dx, CRTI

		mov cx, pgoffs
		and cx, 0xff00
		or cx, CRTHI
		mov ax, cx // hi = CRTHI | (pgo & 0xff00);

		out dx, ax

		mov cx, pgoffs
		shl cx, 8
		or cx, CRTLO
		mov ax, cx // lo = CRTLO | (pgo << 8);

		out dx, ax
	}
	ftwaite:
	asm {
		mov dx, 0x3da // trace end
		in al, dx
		and al, 8
		cmp al, 0
		jle ftwaite

		mov newpg, 0 // pg = 0
		mov pgoffs, 0 // pgoffs = 0

		mov ax, pg
		cmp ax, 0
		jg pge // pg > 0?

		mov newpg, 1 // pg = 1
		mov ax, W/4
		mov cx, H
		mul cx
		mov pgoffs, ax // pgoffs = W/4*H
	}
	pge:
	asm sti

	return newpg;
}

void r_putpixel(int x, int y, byte c)
{
	asm {
		mov ax, W/4
		mul y
		mov di, x
		shr di, 2
		add di, ax // calculate address x>>2 + y * W/4
		mov ax, pgoffs // page offset
		add di, ax
		
		mov ax, VSTART // video memory start
		mov es, ax

		mov cx, x
		and cx, 3 // plane of pixel
		mov ax, PIX_PLANE // select pixel plane
		add ax, MAP_MASK
		shl ah, cl
		mov dx, SCI
		out dx, ax

		mov al, c
		mov [es:di], al // put pixel
	}
}

void r_fill(byte c)
{
	asm {
		cli

		mov dx, SCI
		mov ax, ALL_PLANES // select all planes
		add ax, MAP_MASK
		out dx, ax

		mov ax, VSTART // video memory start
		mov es, ax

		mov di, pgoffs // page offset

		mov ah, c // color
		mov al, c

		mov cx, W*H/8 // size
		
		rep stosw // fill

		sti
	}
}

void r_planefill(int x, int y, int p, byte c)
{
	asm {
		mov dx, PIX_PLANE
		mov ax, p // select planes to draw 0-16
		mul dx
		add ax, MAP_MASK
		mov dx, SCI
		out dx, ax

		mov ax, VSTART // video memory start
		mov es, ax

		mov dx, y
		mov ax, W/4
		mul dx // y offset

		mov di, x
		shr di, 2 // x coordinate of plane

		add di, ax // calculate address
		add di, pgoffs // add page offset

		xor ax, ax
		mov al, c // color

		mov [es:di], al // draw planes
	}
}

// fill all planes from x0 to x1 using stosb
void r_hlinefill1(int x0, int x1, int y, byte c)
{
	asm {
		cli

		mov ax, ALL_PLANES // select all planes
		add ax, MAP_MASK
		mov dx, SCI
		out dx, ax

		mov ax, VSTART // video memory start
		mov es, ax

		mov dx, y
		mov ax, W/4
		mul dx // y offset

		mov cx, x1
		shr cx, 2
		mov di, x0
		shr di, 2 // plane coordinates

		inc di // remove ends
		dec cx

		sub cx, di
		inc cx // n = x1 - x0 + 1
		add di, ax // calculate address x0>>2 + y * W/4
		add di, pgoffs // add page offset

		xor ax, ax
		mov al, c // color

		rep stosb // fill line

		sti
	}
}

// fill all planes from x0 to x1 using stosw
void r_hlinefill2(int x0, int x1, int y, byte c)
{
	asm {
		cli
		
		mov ax, ALL_PLANES // select all planes
		add ax, MAP_MASK
		mov dx, SCI
		out dx, ax

		mov ax, VSTART // video memory start
		mov es, ax

		mov dx, y
		mov ax, W/4
		mul dx // y offset

		mov cx, x1
		shr cx, 2
		mov di, x0
		shr di, 2 // plane coordinates

		inc di // remove ends
		dec cx

		sub cx, di
		inc cx // w = x1 - x0 + 1
		shr cx, 1 // words
		add di, ax // calculate address x0>>2 + y * W/4
		add di, pgoffs // add page offset

		mov al, c // color
		mov ah, c

		rep stosw // fill line

		sti
	}
}

// fill plane p from x0 to x1 using stosb
void r_hplanefill(int x0, int x1, int y, int p, byte c)
{
	asm {
		cli

		mov dx, PIX_PLANE
		mov ax, p // select planes to draw 0-16
		mul dx
		add ax, MAP_MASK
		mov dx, SCI
		out dx, ax

		mov ax, VSTART // video memory start
		mov es, ax

		mov dx, y
		mov ax, W/4
		mul dx // y offset

		mov cx, x1
		shr cx, 2
		mov di, x0
		shr di, 2 // plane coordinates

		sub cx, di
		inc cx // n = x1 - x0 + 1
		add di, ax // calculate address
		add di, pgoffs // add page offset

		xor ax, ax
		mov al, c // color

		rep stosb // fill line

		sti
	}
}

void r_vplanefill(int x, int y0, int y1, int p, byte c)
{
	asm {
		cli

		mov dx, PIX_PLANE
		mov ax, p // select planes to draw 0-16
		mul dx
		add ax, MAP_MASK
		mov dx, SCI
		out dx, ax

		mov ax, VSTART // video memory start
		mov es, ax

		mov si, W/4
		mov ax, y1
		mul si
		mov bx, ax // end point
		add bx, pgoffs // add page offset for cmp

		mov ax, y0
		mul si
		mov cx, ax // start point

		mov di, x
		shr di, 2
		add di, cx // calculate address x>>2 + y0 * W/4
		add di, pgoffs // page offset

		xor ax, ax
		mov al, c // color
	}
	vfill:
	asm {
		mov [es:di], al // put pixel
		add di, si // y += 1

		cmp di, bx // end?
		jb vfill

		sti
	}
}

void r_lineplanefill(int x0, int y0, int x1, int y1, int p, byte c)
{
	asm {
		cli
		push bp

		mov dx, PIX_PLANE
		mov ax, p // select planes to draw 0-16
		push ax // plane to stack +10
		mul dx
		add ax, MAP_MASK
		mov dx, SCI
		out dx, ax

		mov ax, VSTART
		mov es, ax // video memory start

		mov cx, x1
		mov bx, x0
		sub cx, bx // $cx = dx = x1 - x0

		mov dx, y1
		mov bx, y0
		sub dx, bx // $dx = dy = y1 - y0

		mov si, 1 // sx
		mov di, W/4 // sy

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
		mov cx, dx
	}
	lfmaxd:
	asm {
		push cx // maxd to stack

		mov cx, y0 // start y
		mov ax, W/4
		mul cx
		add ax, pgoffs // add page offset
		mov bx, ax // $bx = y = y0
		mov dx, x0 // $bx = x = x0

		xor ax, ax
		mov al, c // color

		mov bp, sp

		xor sp, sp // $sp = i
	}
	lfill:
	asm {
		mov cx, 3 // skip other planes
		and cx, dx
		mov si, 1
		shl si, cl
		mov cx, [bp+10] // plane
		xor cx, si
		cmp cx, 0
		jne lfskip

		mov si, dx
		sar si, 2
		add si, bx // calculate address x + y * W/4

		mov [es:si], al // write pixel
	}
	lfskip:
	asm {
		mov si, di
		sal si, 1 // $si *= 2
		mov cx, [bp+8] // dx
		cmp si, cx
		jg lfx

		// if 2*d <= dx
		add di, cx // diff += dx
		mov cx, [bp+2] // sy
		add bx, cx // y += sy
	}
	lfx:
	asm {
		mov cx, [bp+6] // dy
		neg cx
		cmp si, cx
		jl lfy

		// if 2*d >= -dy
		add di, cx // diff += dy
		mov cx, [bp+4] // sx
		add dx, cx // x += sx
	}
	lfy:
	asm {
		mov cx, [bp] // maxd
		inc sp // ++i
		cmp sp, cx // end?
		jbe lfill

		mov sp, bp

		pop ax
		pop ax
		pop ax
		pop ax
		pop ax
		pop ax

		pop bp
		sti
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
		mov di, W/4 // sy

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
		push cx // dx to stack +10
		push dx // dy to stack +8
		push si // sx to stack +6
		push di // sy to stack +4

		mov di, cx
		sub di, dx // $di = diff

		cmp cx, dx
		jg lfmaxd

		// if dy > dx
		mov cx, dx
	}
	lfmaxd:
	asm {
		push cx // maxd to stack +2

		mov cx, y0 // start y
		mov ax, W/4
		mul cx
		add ax, pgoffs // add page offset
		mov bx, ax // $bx = y = y0
		mov dx, x0 // $dx = x = x0

		xor ax, ax
		mov al, c // color
		push ax // color to stack

		mov bp, sp

		xor sp, sp // $sp = i
	}
	lfill:
	asm {
		mov cx, dx
		and cx, 3 // plane of pixel
		mov ax, PIX_PLANE
		add ax, MAP_MASK
		shl ah, cl
		mov cx, dx
		mov dx, SCI
		out dx, ax // select plane
		mov dx, cx

		mov si, dx
		sar si, 2
		add si, bx // calculate address x + y * W/4

		mov ax, [bp]
		mov [es:si], al // write pixel

		mov si, di
		sal si, 1 // $si *= 2
		mov cx, [bp+10] // dx
		cmp si, cx
		jg lfx

		// if 2*d <= dx
		add di, cx // diff += dx
		mov cx, [bp+4] // sy
		add bx, cx // y += sy
	}
	lfx:
	asm {
		mov cx, [bp+8] // dy
		neg cx
		cmp si, cx
		jl lfy

		// if 2*d >= -dy
		add di, cx // diff += dy
		mov cx, [bp+6] // sx
		add dx, cx // x += sx
	}
	lfy:
	asm {
		mov cx, [bp+2] // maxd
		inc sp // ++i
		cmp sp, cx // end?
		jbe lfill

		mov sp, bp

		pop ax
		pop ax
		pop ax
		pop ax
		pop ax
		pop ax

		pop bp
		sti
	}
}

void r_triplanefill(int x0, int dx0, int x1, int dx1, int y, int dy, int p, byte c)
{
	asm {
		cli
		push bp

		mov dx, PIX_PLANE
		mov ax, p // select planes to draw 0-16
		mul dx
		add ax, MAP_MASK
		mov dx, SCI
		out dx, ax

		mov ax, VSTART
		mov es, ax // video memory start

		mov ax, y // final line
		add ax, dy
		mov cx, W/4
		mul cx
		add ax, pgoffs // page offset for cmp
		push ax // end to stack

		mov cx, dy

		mov ax, dx1 // $ax = slope 1
		mov bx, ax

		cmp bx, 0 // dx1 > 0?
		jg tfs1
		neg ax // $ax = |dx1|
	}
	tfs1:
	asm {
		shl ax, 7
		xor dx, dx
		div cx // |k1| = (128 * |dx1|) / dy

		cmp bx, 0 // dx1 > 0?
		jg tfss1
		neg ax // k1 = -|k1|
	}
	tfss1:
	asm {
		push ax // k1 to stack

		mov ax, dx0 // $ax = slope 0
		mov bx, ax

		cmp bx, 0 // dx0 > 0?
		jg tfs0
		neg ax // $ax = |dx0|
	}
	tfs0:
	asm {
		shl ax, 7
		xor dx, dx
		div cx // |k0| = (128 * |dx0|) / dy

		cmp bx, 0 // dx0 > 0?
		jg tfss0
		neg ax // k0 = -|k0|
	}
	tfss0:
	asm {
		push ax // k0 to stack

		mov dx, W/4
		mov ax, y
		mul dx // y offset
		add ax, pgoffs // add page offset
		mov si, ax

		mov bx, x0 // left point
		inc bx
		shl bx, 7
		sub bx, 80 // left bias

		mov dx, x1 // right point
		inc dx
		shl dx, 7
		add dx, 80 // right bias

		xor ax, ax
		mov al, c // color

		mov bp, sp
	}
	tfill:
	asm {
		mov di, bx // x0
		shr di, 9 // divide to calculate coordinates

		mov cx, dx // x1
		shr cx, 9

		sub cx, di
		cmp cx, 0 // check if x backwards
		jl tfend
		add di, si // calculate final address

		rep stosb // fill line

		add si, W/4 // y += 1

		mov cx, [bp]
		add bx, cx // x0 += k0
		mov cx, [bp+2]
		add dx, cx // x1 += k1

		mov cx, [bp+4]
		cmp si, cx
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

void r_clear() {
	r_fill(clearcol);
}

void r_sync() {
	if (doublebuffer) {
		page = r_flip(page);
	}
	else {
		r_waitretrace();
	}
}

void r_hlinefill(int x0, int x1, int y, byte c)
{
	int px;
	// fill edges x0 and x1, selecting correct planes
	px = min((x0/4)*4 + 3, x1);
	if (x0 <= px)
		r_planefill(x0, y, linepx(x0, px), c);
	px = max(x0, (x1/4)*4);
	if (px <= x1)
		r_planefill(x1, y, linepx(px, x1), c);

	if (x1 - x0 <= 4) // no center area?
		return;

	// fill center
	r_hlinefill1(x0, x1, y, c);
}

void r_vlinefill(int x, int y0, int y1, byte c)
{
	r_vplanefill(x, y0, y1, pixpx(x), c);
}

void r_rectfill(int x, int y, int w, int h, byte c)
{
	int i;

	r_vplanefill(x, y, y+h, linepx(x, min((x/4)*4 + 3, x+w-1)), c);
	r_vplanefill(x+w, y, y+h, linepx(max(x, ((x+w)/4)*4), x+w-1), c);

	for (i = 0; i < (w-3)/4; ++i)
		r_vplanefill(x+4*i+3, y, y+h, 0x0f, c);
}

void r_trifill(int x0, int dx0, int x1, int dx1, int y, int dy, byte c)
{
	// fill plane at a time
	r_triplanefill(x0+3, dx0, x1+3, dx1, y, dy, pixpx(0), c);
	r_triplanefill(x0+2, dx0, x1+2, dx1, y, dy, pixpx(1), c);
	r_triplanefill(x0+1, dx0, x1+1, dx1, y, dy, pixpx(2), c);
	r_triplanefill(x0+0, dx0, x1+0, dx1, y, dy, pixpx(3), c);
}

#endif

