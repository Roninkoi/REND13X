#include "SRC\RENDER.H"

#ifdef MODEX

extern int far r_init13();

extern int far r_init();

extern void far r_exit(int vmode);

extern void far r_waitretrace();

extern int far r_page(int pg);

extern int far r_flip(int pg);

extern void far r_putpixel(int x, int y, byte c);

extern void far r_fill(byte c);

extern void far r_vfill(int y0, int h, byte c);

extern void far r_planefill(int x, int y, int p, byte c);

extern void far r_hlinefill1(int x0, int x1, int y, byte c);

extern void far r_hlinefill2(int x0, int x1, int y, byte c);

extern void far r_hplanefill(int x0, int x1, int y, int p, byte c);

extern void far r_vplanefill(int x, int y0, int y1, int p, byte c);

extern void far r_linefill(int x0, int y0, int x1, int y1, byte c);

extern void far r_triplanefill(int x0, int dx0, int x1, int dx1,
					 int y, int dy, int p, byte c);

extern void far r_spritefill2(int x, int y, int w, int h, int tstart);

extern void far r_spriteplanefill(int x, int y, int w, int h, int p,
					    int tw, int th, int tp, int tstart);

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

void far r_hlinefill(int x0, int x1, int y, byte c)
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

void far r_vlinefill(int x, int y0, int y1, byte c)
{
	r_vplanefill(x, y0, y1, pixpx(x), c);
}

void far r_rectfill(int x, int y, int w, int h, byte c)
{
	int i;

	r_vplanefill(x, y, y+h, linepx(x, min((x/4)*4 + 3, x+w-1)), c);
	r_vplanefill(x+w, y, y+h, linepx(max(x, ((x+w)/4)*4), x+w-1), c);

	for (i = 0; i < (w-3)/4; ++i)
		r_vplanefill(x+4*i+3, y, y+h, 0x0f, c);
}

void far r_trifill(int x0, int dx0, int x1, int dx1, int y, int dy, byte c)
{
	// fill plane at a time
	r_triplanefill(x0+3, dx0, x1+3, dx1, y, dy, pixpx(0), c);
	r_triplanefill(x0+2, dx0, x1+2, dx1, y, dy, pixpx(1), c);
	r_triplanefill(x0+1, dx0, x1+1, dx1, y, dy, pixpx(2), c);
	r_triplanefill(x0+0, dx0, x1+0, dx1, y, dy, pixpx(3), c);
}

void far r_spritefill(int x, int y, int w, int h,
			int tx, int ty, int tw, int th, unsigned tstart)
{
	int p0 = ((tx&3) << 2);
	unsigned toffs = W / 4;
	toffs *= ty;
	toffs += tx / 4;
	toffs += tstart;

	r_spriteplanefill(x, y, w, h, pixpx(x+0),
				tw, th, 0 | p0, toffs);
	r_spriteplanefill(x, y, w, h, pixpx(x+1),
				tw, th, 1 | p0, toffs);
	r_spriteplanefill(x, y, w, h, pixpx(x+2),
				tw, th, 2 | p0, toffs);
	r_spriteplanefill(x, y, w, h, pixpx(x+3),
				tw, th, 3 | p0, toffs);
}

#endif

