#include "SRC\RENDER.H"

byte pg = 0;
unsigned pgoffs = 0;

char oldvmode = 0;

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
void r_drawLine(int x0, int y0, int x1, int y1, byte c)
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

void r_trihfill(int x0, int dx0, int x1, int dx1, int y, int dy, int p, byte c)
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

	// Bresenham
	while (y <= y0) {
		if (hlineVis(x0, x1, y)) {
#ifdef MODE13
			r_hlinefill(max(x0, L), min(x1, R), y, c);
#endif
#ifdef MODEX
			r_hplanefill(max(x0, L), min(x1, R), y, p, c);
#endif
		}

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


#ifdef MODE13

void r_trifillb(int x0, int dx0, int x1, int dx1, int y, int dy, byte c)
{
	r_trihfill(x0, dx0, x1, dx1, y, dy, 0, c);
}

#endif

#ifdef MODEX

void r_trifillb(int x0, int dx0, int x1, int dx1, int y, int dy, byte c)
{
	r_trihfill(x0+3, dx0, x1+3-3, dx1, y, dy, pixpx(0), c);
	r_trihfill(x0+2, dx0, x1+2-3, dx1, y, dy, pixpx(1), c);
	r_trihfill(x0+1, dx0, x1+1-3, dx1, y, dy, pixpx(2), c);
	r_trihfill(x0+0, dx0, x1+0-3, dx1, y, dy, pixpx(3), c);
}

#endif

void r_drawTri(int x0, int y0, int x1, int y1, int x2, int y2, byte c)
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

	if (dy0 == 0)
		return;

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
	if (dy1 > 0) {
#ifndef FASTFILL
		r_trifillb(x0, dx0, x0, dx1, y0, dy1, c);
#else
		r_trifill(x0, dx0, x0, dx1, y0, dy1, c);
#endif
	}

	if (x1 > x2) { // sort x
		to = x1;
		x1 = x2;
		x2 = to;

		to = dx01;
		dx01 = dx2;
		dx2 = to;
	}

	// bottom
	if (dy2 > 0) {
#ifndef FASTFILL
			r_trifillb(x1, dx2, x2, dx01, y2, dy2, c);
#else
			r_trifill(x1, dx2, x2, dx01, y2, dy2, c);
#endif
	}

	++drawCount;
}

#define zCross(ux, uy, vx, vy) ((ux) * (vy) - (uy) * (vx))

int pointInTri(long px, long py,
	long t0x, long t0y,
	long t1x, long t1y,
	long t2x, long t2y)
{
	long a, b;
	long det1, det2, det01, det02, det12;

	t1x -= t0x;
	t1y -= t0y;
	t2x -= t0x;
	t2y -= t0y;

	det1 = zCross(px, py, t1x, t1y);
	det2 = zCross(px, py, t2x, t2y);
	det01 = zCross(t0x, t0y, t1x, t1y);
	det02 = zCross(t0x, t0y, t2x, t2y);
	det12 = zCross(t1x, t1y, t2x, t2y);

	a = (det2 - det02) * sign(det12);
	b = (-det1 + det01) * sign(det12);

	return a > 0 && b > 0 && a + b < abs(det12);
}

int pointCloser(long x0, long y0, long x1, long y1, long x2, long y2)
{
	unsigned long dx01, dy01, dx02, dy02, dx12, dy12;
	unsigned long d01, d02, d12;

	dx01 = x1 - x0;
	dy01 = y1 - y0;
	dx02 = x2 - x0;
	dy02 = y2 - y0;
	dx12 = x2 - x1;
	dy12 = y2 - y1;

	d01 = dx01 * dx01 + dy01 * dy01;
	d02 = dx02 * dx02 + dy02 * dy02;
	d12 = dx12 * dx12 + dy12 * dy12;

	return d01 < d02 && d12 < d02;
}

pix clipLine(int x, int y, int dx, int dy)
{
	int x0, y0;
	pix p = Pix(x, y);

	if (dx != 0) {
		x0 = p.x;
		p.x = min(R, p.x);
		p.x = max(L, p.x);
		p.y += ((float) dy / (float) dx * (float) (p.x - x0));
	}
	if (dy != 0) {
		y0 = p.y;
		p.y = min(B, p.y);
		p.y = max(T, p.y);
		p.x += ((float) dx / (float) dy * (float) (p.y - y0));
	}

	return p;
}

void r_drawTriClip(vec2 *v, byte c)
{
	pix p[10];
	pix pc;
	int ps[10];
	int smallest_i, tmp;
	float smallest, ftmp;
	int pn;
	int i, j;
	int dx01, dy01, dx02, dy02, dx12, dy12;
	int clip0, clip1, clip2, clipn;

	int xc, yc;

	// transform from gl to pix
	int x0 = round((v[0].x+1.0f)*W*0.5f);
	int y0 = round((-v[0].y+1.0f)*H*0.5f);

	int x1 = round((v[1].x+1.0f)*W*0.5f);
	int y1 = round((-v[1].y+1.0f)*H*0.5f);

	int x2 = round((v[2].x+1.0f)*W*0.5f);
	int y2 = round((-v[2].y+1.0f)*H*0.5f);

	// bounds
	if (!triVis(x0, y0, x1, y1, x2, y2))
		return;

#ifndef FASTFILL
	r_drawTri(x0, y0, x1, y1, x2, y2, c);
	return;
#endif

	if (!triClips(x0, y0, x1, y1, x2, y2, 0)) {
		r_drawTri(x0, y0, x1, y1, x2, y2, c);
		return;
	}

	clip0 = !pointVis(x0, y0);
	clip1 = !pointVis(x1, y1);
	clip2 = !pointVis(x2, y2);

	p[0] = Pix(x0, y0);
	p[1] = Pix(W, H);
	p[2] = Pix(x1, y1);
	p[3] = Pix(W, H);
	p[4] = Pix(x2, y2);
	p[5] = Pix(W, H);
	p[6] = Pix(W, H);
	p[7] = Pix(W, H);
	p[8] = Pix(W, H);
	p[9] = Pix(W, H);

	dx01 = x1 - x0;
	dy01 = y1 - y0;

	dx02 = x2 - x0;
	dy02 = y2 - y0;

	dx12 = x2 - x1;
	dy12 = y2 - y1;

	if (clip0) {
		pc = clipLine(x0, y0, dx01, dy01);
		if (pointCloser(x0, y0, pc.x, pc.y, x1, y1))
			p[1] = pc;
		pc = clipLine(x0, y0, dx02, dy02);
		if (pointCloser(x0, y0, pc.x, pc.y, x2, y2))
			p[0] = pc;
	}
	if (clip1) {
		pc = clipLine(x1, y1, dx01, dy01);
		if (pointCloser(x1, y1, pc.x, pc.y, x0, y0))
			p[2] = pc;
		pc = clipLine(x1, y1, dx12, dy12);
		if (pointCloser(x1, y1, pc.x, pc.y, x2, y2))
			p[3] = pc;
	}
	if (clip2) {
		pc = clipLine(x2, y2, dx02, dy02);
		if (pointCloser(x2, y2, pc.x, pc.y, x0, y0))
			p[5] = pc;
		pc = clipLine(x2, y2, dx12, dy12);
		if (pointCloser(x2, y2, pc.x, pc.y, x1, y1))
			p[4] = pc;
	}

	if (pointInTri(L, T, x0, y0, x1, y1, x2, y2)) {
		p[6] = Pix(L, T);
	}
	if (pointInTri(R, T, x0, y0, x1, y1, x2, y2)) {
		p[7] = Pix(R, T);
	}
	if (pointInTri(R, B, x0, y0, x1, y1, x2, y2)) {
		p[8] = Pix(R, B);
	}
	if (pointInTri(L, B, x0, y0, x1, y1, x2, y2)) {
		p[9] = Pix(L, B);
	}

	xc = 0, yc = 0;
	pn = 0;
	for (i = 0; i < 10; ++i) {
		if (pointVis(p[i].x, p[i].y)) {
			ps[pn++] = i;
			xc += p[i].x;
			yc += p[i].y;
		}
	}
	xc = round((float) xc / (float) pn);
	yc = round((float) yc / (float) pn);

	if (pn == 3) {
		r_drawTri(
			p[ps[0]].x, p[ps[0]].y,
			p[ps[1]].x, p[ps[1]].y,
			p[ps[2]].x, p[ps[2]].y, c);
		return;
	}

	for (i = 0; i < pn; ++i) {
		smallest = 10.0f;
		smallest_i = i;
		for (j = i; j < pn; ++j) {
			if (p[ps[j]].y-yc == 0 && p[ps[j]].x-xc == 0)
				continue;
			ftmp = atan2(p[ps[j]].y-yc, p[ps[j]].x-xc);
			if (ftmp < smallest) {
				smallest = ftmp;
				smallest_i = j;
			}
		}
		if (smallest != 10.0f) {
			tmp = ps[i];
			ps[i] = ps[smallest_i];
			ps[smallest_i] = tmp;
		}
	}

	if (pn == 4) {
		r_drawTri(
			p[ps[0]].x, p[ps[0]].y,
			p[ps[1]].x, p[ps[1]].y,
			p[ps[2]].x, p[ps[2]].y, c);
		r_drawTri(
			p[ps[0]].x, p[ps[0]].y,
			p[ps[2]].x, p[ps[2]].y,
			p[ps[3]].x, p[ps[3]].y, c);
		return;
	}

	for (i = 0; i < pn; ++i) {
		r_drawTri(xc, yc,
			p[ps[(i)%pn]].x, p[ps[(i)%pn]].y,
			p[ps[(i+1)%pn]].x, p[ps[(i+1)%pn]].y, c);
	}

	/*r_putpixel(xc, yc, 5);
	for (i = 0; i < pn; ++i) {
		r_putpixel(p[ps[i]].x, p[ps[i]].y, 5);
	}*/
}

void r_drawPoint3D(vec3 v, byte c)
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

void r_drawLine3D(vec3 v0, vec3 v1, byte c)
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

	r_drawLine(x0, y0, x1, y1, c);
}

void r_drawTri3D(vec3 *v0, vec3 *v1, vec3 *v2, byte c)
{
	vec2 v[3];

	float z0 = v0->z;
	float z1 = v1->z;
	float z2 = v2->z;

	v[0] = Vec2(v0->x, v0->y);
	v[1] = Vec2(v1->x, v1->y);
	v[2] = Vec2(v2->x, v2->y);

	if (z0 <= ZNEAR || z1 <= ZNEAR || z2 <= ZNEAR)
		return;

	if (z0 >= ZFAR && z1 >= ZFAR && z2 >= ZFAR)
		return;

	// projection
	v[0].x /= z0;
	v[0].y /= z0;

	v[1].x /= z1;
	v[1].y /= z1;

	v[2].x /= z2;
	v[2].y /= z2;

	r_drawTriClip(v, c);
}

