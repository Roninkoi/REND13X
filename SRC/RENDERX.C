#include "SRC\RENDER.H"

#ifdef MODEX

#define SCI 0x03c4
#define SCD 0x03c5
#define CRTI 0x03d4
#define MISCOUT 0x03c2

#define CRTPLEN 10
const unsigned CRTPARAM[CRTPLEN] = {
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

void r_init13()
{
	asm {
		mov ah, 0x0f // get current video mode
		int 0x10
		mov vmode, al

		mov ax, 0x13 // set 13
		int 0x10
	}
}

void r_init()
{
	r_init13();
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
		mov si, offset CRTPARAM
		mov cx, CRTPLEN
	}
	crtp:
	asm {
		lodsw
		out dx, ax
		loop crtp
	}

	//r_fill(clearcol);
}

#define MAP_MASK 0x02

void r_fill(byte c)
{
	asm {
		mov dx, SCI
		mov ax, 0x0f00 + MAP_MASK // all planes
		out dx, ax

		mov ax, VSTART
		mov es, ax
		//xor di, di
		mov di, pgoffs
		mov ah, c
		mov al, c
		mov cx, W*H/8
		rep stosw
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

void r_putpixel(int x, int y, byte c)
{
	asm {
		mov ax, W/4
		mul y
		mov bx, x
		shr bx, 2
		add bx, ax
		mov ax, pgoffs
		add bx, ax
		mov ax, VSTART
		mov es, ax

		mov cx, x
		and cx, 3 // plane of pixel
		mov ax, 0x0100 + MAP_MASK
		shl ah, cl
		mov dx, SCI
		out dx, ax

		mov al, c
		mov [es:bx], al
	}
}

#define CRTHI 0x0c
#define CRTLO 0x0d

void r_flip()
{
	int hi = CRTHI | (pgoffs & 0xff00);
	int lo = CRTLO | (pgoffs << 8);

	TRACESTART;

	asm {
		cli
		mov dx, CRTI
		mov ax, hi
		out dx, ax
		mov ax, lo
		out dx, ax
		sti
	}

	TRACEEND;

	if (pg > 0) {
		pg = 0;
		pgoffs = 0;
	}
	else {
		pg = 1;
		pgoffs = W/4; // has to be done like this
		pgoffs *= H;
	}
}

// fill all planes from x0 to x1 using stosw
void r_hlinefill2(int x0, int x1, int y, byte c)
{
	asm {
		mov ax, 0x0f00 + MAP_MASK // select all planes
		mov dx, SCI
		out dx, ax

		mov ax, VSTART
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
		add cx, 1 // n = x1 - x0 + 1
		shr cx, 1 // words
		add di, ax // calculate address
		add di, pgoffs // add page offset

		mov al, c // color
		mov ah, c

		rep stosw
	}
}

// fill all planes from x0 to x1 using stosb
void r_hlinefill1(int x0, int x1, int y, byte c)
{
	asm {
		mov ax, 0x0f00 + MAP_MASK // select all planes
		mov dx, SCI
		out dx, ax

		mov ax, VSTART
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
		add cx, 1 // n = x1 - x0 + 1
		add di, ax // calculate address
		add di, pgoffs // add page offset

		xor ax, ax
		mov al, c // color

		rep stosb
	}
}

void r_planefill(int x, int y, int p, byte c)
{
	asm {
		mov dx, 0x0100
		mov ax, p // select planes to draw 0-16
		mul dx
		add ax, MAP_MASK
		mov dx, SCI
		out dx, ax

		mov ax, VSTART
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

		mov [es:di], al
	}
}

// select planes from right x offset
#define rightp(x) (0x0f>>(x))
// select planes from left x offset
#define leftp(x) (0x0f&(0x0f<<(x)))
// combined left and right offsets
#define linep(x0, x1) (leftp(x0)&rightp(x1))
// select planes in coordinates [x0, x1]
#define linepx(x0, x1) (linep((x0)&3, 3-(x1)&3))
// select plane for one pixel coordinate x
#define pixpx(x) (1<<((x)&3))

void r_hlinefill(int x0, int x1, int y, byte c)
{
	// fill edges x0 and x1, selecting correct planes
	r_planefill(x0, y, linepx(x0, min((x0/4)*4 + 3, x1)), c);
	r_planefill(x1, y, linepx(max(x0, (x1/4)*4), x1), c);

	if (x1 - x0 <= 4) // no center area?
		return;

	// fill edge area not covered by fill2
	//r_planefill(max((x1/4-1)*4, x0), y,
	//	linepx(max((x1/4-1)*4, x0), (x1/4-1)*4+3), c);
	// fill center words at a time
	//r_hlinefill2(x0, x1, y, c);

	// fill center
	r_hlinefill1(x0, x1, y, c);
}

void r_vplanefill(int x, int y0, int y1, int p, byte c)
{
	asm {
		mov dx, 0x0100
		mov ax, p // select planes to draw 0-16
		mul dx
		add ax, MAP_MASK
		mov dx, SCI
		out dx, ax

		mov ax, VSTART
		mov es, ax

		mov si, W/4
		mov ax, y1
		inc ax // account for x
		mul si
		mov bx, ax // end point
		add bx, pgoffs // add page offset for cmp

		mov ax, y0
		mul si
		mov cx, ax // start point

		mov di, x
		shr di, 2

		add di, cx // calculate address
		add di, pgoffs

		xor ax, ax
		mov al, c // color
	}
	vfill:
	asm {
		mov [es:di], al
		add di, si

		cmp di, bx
		jb vfill
	}
}

void r_vlinefill(int x, int y0, int y1, byte c)
{
	r_vplanefill(x, y0, y1, pixpx(x), c);
}

void r_rectfill(int x, int y, int w, int h, byte c)
{
	int i;

	r_vplanefill(x, y, y+h, linepx(x, min((x/4)*4 + 3, x+w)), c);
	r_vplanefill(x+w, y, y+h, linepx(max(x, ((x+w)/4)*4), x+w), c);

	for (i = 0; i < w/4; ++i)
		r_vplanefill(x+4*i+3, y, y+h, 0x0f, c);
}

void r_trifill(int x0, int dx0, int x1, int dx1, int y, int dy, byte c)
{
	r_trifillclip(x0, dx0, x1, dx1, y, dy, c); // TODO: new asm fill for mode 13h
}

#endif

