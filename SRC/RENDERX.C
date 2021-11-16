#include "SRC\RENDER.H"

#ifdef MODEX

#define SCI 0x03c4
#define SCD 0x03c5
#define CRTI 0x03d4
#define MISCOUT 0x03c2

#define CRTPLEN 10
const unsigned int CRTPARAM[CRTPLEN] = {
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

void r_init()
{
	asm {
		mov ah, 0x0f // get current video mode
		int 0x10
		mov vmode, al

		mov ax, 0x13 // set 13
		int 0x10
	}
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
	CRTPLOOP:
	asm {
		lodsw
		out dx, ax
		loop CRTPLOOP
	}

	r_fill(clearcol);

}

void r_fill(byte c)
{
	asm {
		mov dx, SCI
		mov ax, 0x0f02 // all planes
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

#define MAP_MASK 0x02

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

int foff = 0;
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

void r_hlinefill(int x0, int x1, int y, byte c)
{
}

void r_trifill(float x0, float x1, int y, int dy, float k0, float k1, byte c)
{
}

#endif

