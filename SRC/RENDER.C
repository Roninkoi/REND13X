#include "SRC\RENDER.H"

byte pg = 0;
unsigned pgoffs = 0;

char vmode = 0;

int wireframe = 0;
int faceculling = 1;
int zsort = 1;
int clearscr = 1;
int clearcol = 0;

void r_waitRetrace()
{
	TRACESTART;
	TRACEEND;
}

void r_linefill(int a0, int b0, int a1, int b1, int s, byte c)
{
	int a;
	int b;
	int da;
	int db;
	int j = 1;
	int diff;

	da = a1 - a0;
	db = b1 - b0;

	if (db < 0) {
		j = -1;
		db = -db;
	}

	diff = 2 * db - da;
	a = a0;
	b = b0;

	// Bresenham line draw
	for (a = a0; a <= a1; ++a) {
		if (s < 0) {
			if (b > B)
				return;
			if (b < T || b > B || a > R || a < L)
				continue;
			r_putpixel(a, b, c);
		}
		else {
			if (a > B)
				return;
			if (a < T || a > B || b > R || b < L)
				continue;
			r_putpixel(b, a, c);
		}

		if (diff > 0) {
			b += j;
			diff += 2 * (db - da);
		}
		else {
			diff += 2 * db;
		}
	}
}

void r_drawline(int x0, int y0, int x1, int y1, byte c)
{
	int to;
	int p;
	int xd = x1 - x0;
	int yd = y1 - y0;
	int s = abs(yd) - abs(xd);
	int d0x, d0y, d1x, d1y;

	if (s < 0) { // horizontal line
		p = sign(x1 - x0);
		if (p < 0) { // sort by x
			to = x0;
			x0 = x1;
			x1 = to;

			to = y0;
			y0 = y1;
			y1 = to;
		}

		if (x0 > R || x1 < L ||
			min(y0, y1) < T || max(y0, y1) > B)
			return;

		r_linefill(x0, y0, x1, y1, s, c);
	}
	else { // vertical line
		p = sign(y1 - y0);
		if (p < 0) { // sort by y
			to = x0;
			x0 = x1;
			x1 = to;

			to = y0;
			y0 = y1;
			y1 = to;
		}

		if (y0 > B || y1 < T ||
			min(x0, x1) < L || max(x0, x1) > R)
			return;

		r_linefill(y0, x0, y1, x1, s, c);
	}
}

void r_trifillclip(float x0, float x1, int y, int dy, float k0, float k1, byte c)
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
}

void r_drawtri(float v[3][2], byte c)
{
	float to;
	int xl0;
	int xl1;
	int yl;

	float k0;
	float k1;
	float k2;

	float dx1;
	float dx2;

	float dy1;
	float dy2;

	// transform from gl to pix
	float x0 = (v[0][0]+1.0f)*W*0.5f;
	float y0 = (-v[0][1]+1.0f)*H*0.5f;

	float x1 = (v[1][0]+1.0f)*W*0.5f;
	float y1 = (-v[1][1]+1.0f)*H*0.5f;

	float x2 = (v[2][0]+1.0f)*W*0.5f;
	float y2 = (-v[2][1]+1.0f)*H*0.5f;

	int mar = 2; // margin
	int x0out = x0 > R-mar || x0 < L+mar;
	int x1out = x1 > R-mar || x1 < L+mar;
	int x2out = x2 > R-mar || x2 < L+mar;
	int y0out = y0 > B-mar || y0 < T+mar;
	int y1out = y1 > B-mar || y1 < T+mar;
	int y2out = y2 > B-mar || y2 < T+mar;

	int clipping = x0out || x1out || x2out;
	clipping = clipping || y0out || y1out || y2out;

	// bounds
	if (x0out && x1out && x2out && y0out && y1out && y2out)
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

	x0 = round(x0);
	x1 = round(x1);
	x2 = round(x2);

	y0 = round(y0);
	y1 = round(y1);
	y2 = round(y2);

	if (wireframe) {
		r_drawline(x0, y0, x1, y1, c);
		r_drawline(x1, y1, x2, y2, c);
		r_drawline(x0, y0, x2, y2, c);

		return;
	}

	// first - last
	dx1 = (x2 - x0);
	dy1 = (y2 - y0);
	if (dy1 != 0.0f)
		k0 = (dx1/dy1);

	// first - mid
	dx1 = (x1 - x0);
	dy1 = (y1 - y0);
	if (dy1 != 0.0f)
		k1 = (dx1/dy1);

	// mid - last
	dx2 = (x2 - x1);
	dy2 = (y2 - y1);
	if (dy2 != 0.0f)
		k2 = (dx2/dy2);

	x1 = x0; // sort x
	to = k0;
	if (k0 > k1) {
		k0 = k1;
		k1 = to;
	}

	// top
	if (clipping)
		r_trifillclip(x0, x1, (int) y0, (int) dy1 - 1, k0, k1, c);
	else
		r_trifill(x0, x1, (int) y0, (int) dy1 - 1, k0, k1, c);

	k0 = to;
	if (k0 < k2) {
			to = k0;
			k0 = k2;
			k2 = to;
	}
	dy2 += 1.0f;
	y2 = y2 - dy2;
	x1 = x2 - k0*(dy2);
	x2 = x2 - k2*(dy2);

	// bottom
	if (clipping)
		r_trifillclip(x1, x2, (int) y2, (int) dy2 - 1, k0, k2, c);
	else
		r_trifill(x1, x2, (int) y2, (int) dy2 - 1, k0, k2, c);

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

