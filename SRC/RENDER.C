#include "SRC\RENDER.H"

byte pg = 0;
unsigned pgoffs = 0;

char vmode = 0;

int wireframe = 0;
int faceculling = 1;
int zsort = 1;
int clearscr = 1;
int clearcol = 0;
int doublebuffer = 0;
int filled = 1;

void r_waitRetrace()
{
	TRACESTART;
	TRACEEND;
}

// TODO: clipping at line level
void r_drawline(int x0, int y0, int x1, int y1, byte c)
{
	int i, maxd;
	int x = 0, y = 0;
	int dx, dy;
	int sx = 1, sy = 1;
	int diff, d;

	if (!lineVis(x0, y0, x1, y1))
		return;

	dx = x1 - x0;
	dy = y1 - y0;

	dx = x1 - x0;
	dy = y1 - y0;

	if (dx < 0) {
		sx = -1;
		dx = -dx;
	}

	if (dy < 0) {
		sy = -1;
		dy = -dy;
	}

	maxd = max(dx, dy);

	diff = dx - dy;
	x = x0;
	y = y0;

	// Bresenham line draw
	for (i = 0; i <= maxd; ++i) {
		if (pointVis(x, y))
			r_putpixel(x, y, c);

		d = diff;
		if (2 * d <= dx) {
			diff += dx;
			y += sy;
		}
		if (-2 * d <= dy) {
			diff -= dy;
			x += sx;
		}
	}
}

#ifdef MODE13

void r_trihfillclip(int x0, int dx0, int x1, int dx1, int y, int dy, byte c)
{
	int i, diff0, diff1, d0, d1;
	int s0 = 1, s1 = 1;
	int y0 = y + dy;

	if (dx0 < 0) {
		s0 = -1;
		dx0 = -dx0;
	}
	if (dx1 < 0) {
		s1 = -1;
		dx1 = -dx1;
	}

	diff0 = dx0 - dy;
	diff1 = dx1 - dy;

	while (y <= y0) {
		if (hlineVis(x0, x1, y))
			r_hlinefill(max(x0, L), min(x1, R), y, c);

		if (y > B || y >= y0)
			return;

		d0 = diff0;
		d1 = diff1;
		if (2 * d0 >= -dy) {
			diff0 -= dy;
			x0 += s0;
		}
		if (2 * d1 >= -dy) {
			diff1 -= dy;
			x1 += s1;
		}

		if (2 * d0 <= dx0 && 2 * d1 <= dx1) {
			diff0 += dx0;
			diff1 += dx1;
			y += 1;
		}
	}
}

void r_drawhtri(int x0, int y0, int x1, int y1, int x2, int y2, byte c)
{
	int to;

	int dx0, dx01 = 0;
	int dx1;
	int dx2;

	int dy0;
	int dy1;
	int dy2;

	// sort vertices by y
	if (y0 > y2) {
		to = y0;
		y0 = y2;
		y2 = to;
		to = x0;
		x0 = x2;
		x2 = to;
	}
	if (y1 > y2) {
		to = y1;
		y1 = y2;
		y2 = to;
		to = x1;
		x1 = x2;
		x2 = to;
	}
	if (y0 > y1) {
		to = y0;
		y0 = y1;
		y1 = to;
		to = x0;
		x0 = x1;
		x1 = to;
	}

	// first -> last
	dx0 = x2 - x0;
	dy0 = y2 - y0;

	// first -> mid
	dx1 = x1 - x0;
	dy1 = y1 - y0;

	// mid -> last
	dx2 = x2 - x1;
	dy2 = y2 - y1;

	dx01 = dx0 * (float) dy2 / (float) dy0;
	dx0 = dx0 * (float) dy1 / (float) dy0;

	x2 = x0 + dx0;
	y2 -= dy2;

	if (dx0 > dx1) { // sort x
		to = dx0;
		dx0 = dx1;
		dx1 = to;
	}

	// top
	r_trihfillclip(x0, dx0, x0, dx1, y0, dy1, c);

	if (x1 > x2) { // sort x
			to = x1;
			x1 = x2;
			x2 = to;

			to = dx01;
			dx01 = dx2;
			dx2 = to;
	}

	// bottom
	r_trihfillclip(x1, dx2, x2, dx01, y2, dy2, c);

	++drawcount;
}

#endif

#ifdef MODEX

void r_trivfillclip(int x, int dx, int y0, int dy0, int y1, int dy1, byte c)
{
	int i, diff0, diff1, d0, d1;
	int s0 = 1, s1 = 1;
	int x0 = x + dx;

	if (dy0 < 0) {
		s0 = -1;
		dy0 = -dy0;
	}
	if (dy1 < 0) {
		s1 = -1;
		dy1 = -dy1;
	}

	diff0 = dy0 - dx;
	diff1 = dy1 - dx;

	while (x <= x0) {
		if (vlineVis(x, y0, y1))
			r_vlinefill(x, max(y0, T), min(y1, B), c);

		if (x > R || x >= x0)
			return;

		d0 = diff0;
		d1 = diff1;
		if (2 * d0 >= -dx) {
			diff0 -= dx;
			y0 += s0;
		}
		if (2 * d1 >= -dx) {
			diff1 -= dx;
			y1 += s1;
		}

		if (2 * d0 <= dy0 && 2 * d1 <= dy1) {
			diff0 += dy0;
			diff1 += dy1;
			x += 1;
		}
	}
}

void r_drawvtri(int x0, int y0, int x1, int y1, int x2, int y2, byte c)
{
	int to;

	int dy0, dy01 = 0;
	int dy1;
	int dy2;

	int dx0;
	int dx1;
	int dx2;

	// sort vertices by x
	if (x0 > x2) {
		to = x0;
		x0 = x2;
		x2 = to;
		to = y0;
		y0 = y2;
		y2 = to;
	}
	if (x1 > x2) {
		to = x1;
		x1 = x2;
		x2 = to;
		to = y1;
		y1 = y2;
		y2 = to;
	}
	if (x0 > x1) {
		to = x0;
		x0 = x1;
		x1 = to;
		to = y0;
		y0 = y1;
		y1 = to;
	}

	// first -> last
	dx0 = x2 - x0;
	dy0 = y2 - y0;

	// first -> mid
	dx1 = x1 - x0;
	dy1 = y1 - y0;

	// mid -> last
	dx2 = x2 - x1;
	dy2 = y2 - y1;

	dy01 = dy0 * (float) dx2 / (float) dx0;
	dy0 = dy0 * (float) dx1 / (float) dx0;

	y2 = y0 + dy0;
	x2 -= dx2;

	if (dy0 > dy1) { // sort y
		to = dy0;
		dy0 = dy1;
		dy1 = to;
	}

	// top
	r_trivfillclip(x0, dx1, y0, dy0, y0, dy1, c);

	if (y1 > y2) { // sort y
			to = y1;
			y1 = y2;
			y2 = to;

			to = dy01;
			dy01 = dy2;
			dy2 = to;
	}

	// bottom
	r_trivfillclip(x2, dx2, y1, dy2, y2, dy01, c);

	++drawcount;
}

#endif

// TODO: clipping at triangle level
void r_drawtri(float *v, byte c)
{
	// transform from gl to pix
	int x0 = round((v[0]+1.0f)*W*0.5f);
	int y0 = round((-v[1]+1.0f)*H*0.5f);

	int x1 = round((v[2]+1.0f)*W*0.5f);
	int y1 = round((-v[3]+1.0f)*H*0.5f);

	int x2 = round((v[4]+1.0f)*W*0.5f);
	int y2 = round((-v[5]+1.0f)*H*0.5f);

	//int clipping = triClips(x0, y0, x1, y1, x2, y2, 2);

	// bounds
	if (!triVis(x0, y0, x1, y1, x2, y2))
		return;

#ifdef MODE13
	r_drawhtri(x0, y0, x1, y1, x2, y2, c);
#endif

#ifdef MODEX
	r_drawvtri(x0, y0, x1, y1, x2, y2, c);
#endif
}

void r_drawpoint3d(vec3 v, byte c)
{
	if (v.z <= ZNEAR || v.z >= ZFAR)
		return;

	v.x /= v.z;
	v.y /= v.z;

	v.x = (v.x+1.0f)*W*0.5f;
	v.y = (-v.y+1.0f)*H*0.5f;

	if (!pointVis(v.x, v.y))
		return;

	r_putpixel(v.x, v.y, c);
}

void r_drawline3d(vec3 v0, vec3 v1, byte c)
{
	int x0, y0, x1, y1;

	if (v0.z <= ZNEAR || v1.z <= ZNEAR)
		return;

	if (v0.z >= ZFAR && v1.z >= ZFAR)
		return;

	// projection
	v0.x /= v0.z;
	v0.y /= v0.z;

	v1.x /= v1.z;
	v1.y /= v1.z;

	// transform from gl to pix
	x0 = round((v0.x+1.0f)*W*0.5f);
	y0 = round((-v0.y+1.0f)*H*0.5f);

	x1 = round((v1.x+1.0f)*W*0.5f);
	y1 = round((-v1.y+1.0f)*H*0.5f);

	r_drawline(x0, y0, x1, y1, c);
}

void r_drawtri3d(vec3 *v0, vec3 *v1, vec3 *v2, byte c)
{
	float t[6];

	float z0 = v0->z;
	float z1 = v1->z;
	float z2 = v2->z;

	if (z0 <= ZNEAR || z1 <= ZNEAR || z2 <= ZNEAR)
		return;

	if (z0 >= ZFAR && z1 >= ZFAR && z2 >= ZFAR)
		return;

	// projection
	t[0] = v0->x;
	t[0] /= z0;
	t[1] = v0->y;
	t[1] /= z0;

	t[2] = v1->x;
	t[2] /= z1;
	t[3] = v1->y;
	t[3] /= z1;

	t[4] = v2->x;
	t[4] /= z2;
	t[5] = v2->y;
	t[5] /= z2;

	r_drawtri(t, c);
}

