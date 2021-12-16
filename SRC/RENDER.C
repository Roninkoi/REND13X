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
	int x, y;
	int dx = x1 - x0;
	int dy = y1 - y0;
	int sx = 1, sy = 1;
	int diff;

	if (lineOut(x0, y0, x1, y1))
		return;

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
		if (i == maxd) {
			x = x1; // make sure last pixel exact
			y = y1;
		}
		if (inView(x, y))
			r_putpixel(x, y, c+x);

		if (2 * diff >= -dy) {
			diff -= dy;
			x += sx;
		}
		if (2 * diff <= dx) {
			diff += dx;
			y += sy;
		}
	}
}

/*void r_trifillclip(float x0, float x1, int y, int dy, float k0, float k1, byte c)
{
	int i, xi0, xi1;

	for (i = 0; i < dy; ++i) {
		x0 += k0;
		x1 += k1;
		y += 1;

		if (y > B)
			return;
		if (y < T)
			continue;
		if (x0 > R)
			continue;
		if (x1 < L)
			continue;

		xi0 = round(x0);
		xi1 = round(x1);

		if (x0 < L)
			xi0 = L;
		if (x1 > R)
			xi1 = R;

		r_hlinefill(xi0, xi1, y, c);
	}
}*/

void r_trifillclip(int x0, int dx0, int x1, int dx1, int y, int dy, byte c)
{
	int i, diff0, diff1;
	int s0 = 1, s1 = 1;
	int y0 = y;

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

	for (i = 0; y < y0+dy /*&& x0 < x1*/; ++i) {
		if (y > B)
			return;
		if (x0 > R || x1 < L || y < T)
			continue;

		if (x0 < L)
			x0 = L;
		if (x1 > R)
			x1 = R;

		r_hlinefill(x0, x1, y, c);

		if (2 * diff0 >= -dy) {
			diff0 -= dy;
			x0 += s0;
		}
		if (2 * diff1 >= -dy) {
			diff1 -= dy;
			x1 += s1;
		}
		if (2 * diff0 <= dx0 && 2 * diff1 <= dx1) {
			diff0 += dx0;
			diff1 += dx1;
			y += 1;
		}
	}
}

// TODO: clipping at triangle level
void r_drawtri(float v[3][2], byte c)
{
	int to;

	int dx0;
	int dx1;
	int dx2;

	int dy0;
	int dy1;
	int dy2;

	// transform from gl to pix
	int x0 = round((v[0][0]+1.0f)*W*0.5f);
	int y0 = round((-v[0][1]+1.0f)*H*0.5f);

	int x1 = round((v[1][0]+1.0f)*W*0.5f);
	int y1 = round((-v[1][1]+1.0f)*H*0.5f);

	int x2 = round((v[2][0]+1.0f)*W*0.5f);
	int y2 = round((-v[2][1]+1.0f)*H*0.5f);

	int mar = 2; // margin
	int x0out = x0 > R-mar || x0 < L+mar;
	int x1out = x1 > R-mar || x1 < L+mar;
	int x2out = x2 > R-mar || x2 < L+mar;
	int y0out = y0 > B-mar || y0 < T+mar;
	int y1out = y1 > B-mar || y1 < T+mar;
	int y2out = y2 > B-mar || y2 < T+mar;

	int clipping = x0out || x1out || x2out ||
		y0out || y1out || y2out;

	// bounds
	if (triOut(x0, y1, x1, y1, x2, y2))
		return;

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

	if (wireframe) {
		r_drawline(x0, y0, x1, y1, c);
		r_drawline(x1, y1, x2, y2, c);
		r_drawline(x0, y0, x2, y2, c);

		if (!filled) return;
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

//	if (dy0 > dy1)
		dx0 = dx0 * dy1 / dy0;

	if (dx0 > dx1) { // sort x
		to = dx0;
		dx0 = dx1;
		dx1 = to;
	}

	// top
//	if (clipping)
		r_trifillclip(x0, dx0, x0, dx1, y0, dy1, c);
//	else
//		r_trifill(x0, x1, (int) y0, (int) dy1 - 1, k0, k1, c);


//	if (dy0 > dy2)
	dx0 = (x2 - x0) * dy2 / dy0;

	x2 = x0 + (x2 - x0) * dy1 / dy0;

	y2 -= dy2;

	if (x1 > x2) { // sort x
			to = x1;
			x1 = x2;
			x2 = to;

			to = dx0;
			dx0 = dx2;
			dx2 = to;
	}

	// bottom
//	if (clipping)
		r_trifillclip(x1, dx2, x2, dx0, y2, dy2, c);
//	else
//		r_trifill(x1, x2, (int) y2, (int) dy2 - 1, k0, k2, c);

	++drawcount;
}

void r_drawpoint3d(vec4 v, byte c)
{
	if (v.z <= ZNEAR || v.z >= ZFAR)
		return;

	v.x /= v.z;
	v.y /= v.z;

	v.x = (v.x+1.0f)*W*0.5f;
	v.y = (-v.y+1.0f)*H*0.5f;

	if (v.x > R || v.x < L || v.y > B || v.y < T)
		return;

	r_putpixel(v.x, v.y, c);
}

void r_drawtri3d(vec4 *v0, vec4 *v1, vec4 *v2, byte c)
{
	float t[3][2];

	float z0 = v0->z;
	float z1 = v1->z;
	float z2 = v2->z;

	if (z0 <= ZNEAR || z1 <= ZNEAR || z2 <= ZNEAR)
		return;

	if (z0 >= ZFAR && z1 >= ZFAR && z2 >= ZFAR)
		return;

	// projection
	t[0][0] = v0->x;
	t[0][0] /= z0;
	t[0][1] = v0->y;
	t[0][1] /= z0;

	t[1][0] = v1->x;
	t[1][0] /= z1;
	t[1][1] = v1->y;
	t[1][1] /= z1;

	t[2][0] = v2->x;
	t[2][0] /= z2;
	t[2][1] = v2->y;
	t[2][1] /= z2;

	r_drawtri(t, c);
}

