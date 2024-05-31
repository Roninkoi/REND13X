#include "SRC\RENDER.H"

#ifdef MODEX

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

