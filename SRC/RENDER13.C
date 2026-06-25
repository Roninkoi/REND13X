#include "SRC\RENDER.H"

#ifdef MODE13

extern int far r_init();

extern void far r_exit(int vmode);

extern void far r_waitretrace();

extern void far r_putpixel(int x, int y, byte c);

extern void far r_scr(byte c);

extern void far r_fill(byte c);

extern void far r_vfill(int y0, int h, byte c);

extern void far r_rectfill(int x, int y, int w, int h, byte c);

extern void far r_hlinefill(int x0, int x1, int y, byte c);

extern void far r_hlinefill2(int x0, int x1, int y, byte c);

extern void far r_vlinefill(int x, int y0, int y1, byte c);

extern void far r_linefill(int x0, int y0, int x1, int y1, byte c);

extern void far r_trifill(int x0, int dx0, int x1, int dx1, int y, int dy, byte c);

void r_clear() {
	r_scr(clearcol);
}

void r_sync() {
	r_waitretrace();
}

#endif

