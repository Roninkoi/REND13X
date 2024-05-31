#include "SRC\RENDER.H"

#ifdef MODE13

extern int r_init();

extern void r_exit(int vmode);

extern void r_waitretrace();

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

void r_clear() {
	r_scr(clearcol);
}

void r_sync() {
	r_waitretrace();
}

#endif

