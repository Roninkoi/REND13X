#include "SRC\RENDER.H"

byte page = 0;

int oldvmode = 0;

int wireframe = 0;
int faceculling = 1;
int zsort = 1;
int clearscr = 1;
byte clearcol = 0;
int doublebuffer = 0;
int filled = 1;

TextureAtlas textureAtlas;

void lineFill(int x0, int y0, int x1, int y1, byte c)
{
	int i, maxd;
	int x = 0, y = 0;
	int dx, dy;
	int sx = 1, sy = 1;
	int diff, d;
	int s0, s1;

	if (!lineVis(x0, y0, x1, y1))
		return;

	dx = x1 - x0;
	dy = y1 - y0;

	sx = sign(dx);
	sy = sign(dy);

	dx = sx * dx;
	dy = sy * dy;

	maxd = max(dx, dy);

	diff = dx - dy;
	x = x0;
	y = y0;

	// Bresenham line draw
	for (i = 0; i <= maxd; ++i) {
		if (pointVis(x, y))
			r_putpixel(x, y, c);

#if 0
		// non-branchy version?
		s0 = -(2 * diff <= dx);
		s1 = -(2 * diff >= -dy);
		diff += (int)(dx & s0) - (int)(dy & s1);
		y += (int)(sy & s0);
		x += (int)(sx & s1);
#else
		d = diff;
		if (2 * d <= dx) {
			diff += dx;
			y += sy;
		}
		if (-2 * d <= dy) {
			diff -= dy;
			x += sx;
		}
#endif
	}
}

void r_drawLine(int x0, int y0, int x1, int y1, byte c)
{
#ifdef FASTFILL
	r_linefill(x0, y0, x1, y1, c);
#else
	lineFill(x0, y0, x1, y1, c);
#endif
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

#define coordToPix(vx, vy, x, y)			\
	x = round((vx+1.0f)*W*0.5f);			\
	y = round((-vy+1.0f)*H*0.5f);

#define dimToPix(vw, vh, w, h)			\
	w = round(vw*W*0.5f);				\
	h = round(vh*H*0.5f);

void r_drawLineClip(vec2 *v0, vec2 *v1, byte c)
{
	int x0, y0, x1, y1;
	int dx, dy;
	int clip0, clip1;
	pix p0, p1, pc;

	// transform from gl to pix
	coordToPix(v0->x, v0->y, x0, y0);
	coordToPix(v1->x, v1->y, x1, y1);

	p0 = Pix(x0, y0);
	p1 = Pix(x1, y1);

	dx = x1 - x0;
	dy = y1 - y0;

	if (lineNotVis(x0, y0, x1, y1))
		return;

	clip0 = !pointVis(x0, y0);
	clip1 = !pointVis(x1, y1);

	if (!clip0 && !clip1) {
		r_drawLine(p0.x, p0.y, p1.x, p1.y, c);
		return;
	}

	if (clip0) {
		pc = clipLine(x0, y0, dx, dy);
		if (pointCloser(x0, y0, pc.x, pc.y, x1, y1))
			p0 = pc;
	}
	if (clip1) {
		pc = clipLine(x1, y1, dx, dy);
		if (pointCloser(x1, y1, pc.x, pc.y, x0, y0))
			p1 = pc;
	}

	if (lineVis(p0.x, p0.y, p1.x, p1.y))
		r_drawLine(p0.x, p0.y, p1.x, p1.y, c);
}

void triFill(int x0, int dx0, int x1, int dx1, int y, int dy, int p, byte c)
{
	int diff0, diff1, d0, d1;
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

void fillTri(int x0, int dx0, int x1, int dx1, int y, int dy, byte c)
{
#ifdef FASTFILL
	r_trifill(x0, dx0, x1, dx1, y, dy, c);
#else
#ifdef MODE13
	triFill(x0, dx0, x1, dx1, y, dy, 0, c);
#endif
#ifdef MODEX
	triFill(x0+3, dx0, x1+3-3, dx1, y, dy, pixpx(0), c);
	triFill(x0+2, dx0, x1+2-3, dx1, y, dy, pixpx(1), c);
	triFill(x0+1, dx0, x1+1-3, dx1, y, dy, pixpx(2), c);
	triFill(x0+0, dx0, x1+0-3, dx1, y, dy, pixpx(3), c);
#endif
#endif
}

#define sorty(x0, y0, x1, y1, tmp)			\
	if (y0 > y1) {					\
		tmp = y0;					\
		y0 = y1;					\
		y1 = tmp;					\
		tmp = x0;					\
		x0 = x1;					\
		x1 = tmp;					\
	}

void r_drawTri(int x0, int y0, int x1, int y1, int x2, int y2, byte c)
{
	int tmp;

	int dx0, dx01 = 0;
	int dx1;
	int dx2;

	int dy0;
	int dy1;
	int dy2;

	// sort vertices by y
	sorty(x0, y0, x2, y2, tmp);
	sorty(x1, y1, x2, y2, tmp);
	sorty(x0, y0, x1, y1, tmp);

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
		tmp = dx0;
		dx0 = dx1;
		dx1 = tmp;
	}

	// top
	if (dy1 > 0)
		fillTri(x0, dx0, x0, dx1, y0, dy1, c);

	if (x1 > x2) { // sort x
		tmp = x1;
		x1 = x2;
		x2 = tmp;

		tmp = dx01;
		dx01 = dx2;
		dx2 = tmp;
	}

	// bottom
	if (dy2 > 0)
		fillTri(x1, dx2, x2, dx01, y2, dy2, c);

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

void r_drawTriClip(vec2 *v0, vec2 *v1, vec2 *v2, byte c)
{
	pix p[10];
	pix pc;
	int ps[10];
	int smallest_i, tmp;
	float smallest, ftmp;
	int pn;
	int i, j;
	int dx01, dy01, dx02, dy02, dx12, dy12;
	int clip0, clip1, clip2;

	int xc, yc;

	float x0, y0, x1, y1, x2, y2;

	// transform from gl to pix
	coordToPix(v0->x, v0->y, x0, y0);
	coordToPix(v1->x, v1->y, x1, y1);
	coordToPix(v2->x, v2->y, x2, y2);

	if (triNotVis(x0, y0, x1, y1, x2, y2))
		return;

#ifndef FASTFILL
	r_drawTri(x0, y0, x1, y1, x2, y2, c);
	return;
#endif

	clip0 = !pointVis(x0, y0);
	clip1 = !pointVis(x1, y1);
	clip2 = !pointVis(x2, y2);

	if (!clip0 && !clip1 && !clip2) {
		r_drawTri(x0, y0, x1, y1, x2, y2, c);
		return;
	}

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

	if (pn < 3) return;

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
}

void r_drawPoint3D(vec3 *v0, byte c)
{
	vec2 v;
	pix p;

	float z = v0->z;

	v = Vec2From3(v0);

	if (z < ZNEAR || z > ZFAR)
		return;

	v.x /= z;
	v.y /= z;

	// transform from gl to pix
	coordToPix(v.x, v.y, p.x, p.y);

	if (!pointVis(p.x, p.y))
		return;

	r_putpixel(p.x, p.y, c);
}

void r_drawLine3D(vec3 *v0, vec3 *v1, byte c)
{
	vec2 p0, p1;

	float z0 = v0->z;
	float z1 = v1->z;

	p0 = Vec2From3(v0);
	p1 = Vec2From3(v1);

	if (z0 < ZNEAR || z1 < ZNEAR)
		return;

	if (z0 > ZFAR && z1 > ZFAR)
		return;

	// projection
	p0.x /= z0;
	p0.y /= z0;

	p1.x /= z1;
	p1.y /= z1;

	r_drawLineClip(&p0, &p1, c);
}

void r_drawTri3D(vec3 *v0, vec3 *v1, vec3 *v2, byte c)
{
	vec2 p0, p1, p2;

	float z0 = v0->z;
	float z1 = v1->z;
	float z2 = v2->z;

	p0 = Vec2From3(v0);
	p1 = Vec2From3(v1);
	p2 = Vec2From3(v2);

	if (z0 < ZNEAR || z1 < ZNEAR || z2 < ZNEAR)
		return;

	if (z0 > ZFAR && z1 > ZFAR && z2 > ZFAR)
		return;

	// projection
	p0.x /= z0;
	p0.y /= z0;

	p1.x /= z1;
	p1.y /= z1;

	p2.x /= z2;
	p2.y /= z2;

	r_drawTriClip(&p0, &p1, &p2, c);
}

void r_drawSprite(int x, int y, int w, int h, Texture *tex)
{
	unsigned xx, yy, xx0, yy0, ww, hh;
	byte c;
	
	if (!pointVis(x, y) && !pointVis(x+w, y) &&
	    !pointVis(x, y+h) && !pointVis(x+w, y+h))
		return;

	xx0 = clamp(x, L, R) - x;
	yy0 = clamp(y, T, B) - y;
	ww = clamp(x + w - 1, L, R) - xx0 + 1;
	hh = clamp(y + h - 1, T, B) - yy0 + 1;

	for (yy = yy0; yy < hh; ++yy) {
		for (xx = xx0; xx < ww; ++xx) {
			c = getTexture(tex, xx, yy, w, h);
			r_putpixel(x+xx, y+yy, c);
		}
	}
}

void r_drawSprite3D(vec3 *v, float w, float h, Texture *tex)
{
	vec2 p;
	int x0, y0, x, y;
	int ww0, hh0, ww, hh, tw, th;
	float z = v->z;
	float pw = w;
	float ph = h;
	
	p = Vec2From3(v);
	
	if (z < ZNEAR || z > ZFAR)
		return;

	// center
	p.x -= w / 2.0f;
	p.y += h / 2.0f;
	
	// projection
	p.x /= z;
	p.y /= z;
	pw /= z;
	ph /= z;

	// transform from gl to pix
	coordToPix(p.x, p.y, x, y);
	dimToPix(pw, ph, ww, hh);

	if (!pointVis(x, y) && !pointVis(x+ww, y) &&
	    !pointVis(x, y+hh) && !pointVis(x+ww, y+hh))
		return;

	x0 = x;
	y0 = y;
	ww0 = ww;
	hh0 = hh;
	
	x = clamp(x0, L, R);
	y = clamp(y0, T, B);
	ww = clamp(x0 + ww0 - 1, L, R) - x + 1;
	hh = clamp(y0 + hh0 - 1, T, B) - y + 1;

	if (ww < 4 || hh < 4)
		return;
#ifdef MODE13
	r_drawSprite(x0, y0, ww0, hh0, tex);
#endif
#ifdef MODEX
	tw = (float) tex->w * ((float) ww / (float) ww0);
	th = (float) tex->h * ((float) hh / (float) hh0);
	
	r_spritefill(x, y, ww, hh,
			 x > x0 ? tex->w - tw : 0,
			 y > y0 ? tex->h - th : 0,
			 tw, th,
			 getAtlasTextureStart(&textureAtlas, tex->id));
#endif
}

unsigned getAtlasTextureStart(TextureAtlas *atlas, int i)
{
	unsigned id, xa, ya, x, y, tstart;
	id = atlas->textures[i]->id;
	xa = id % ATLAS_W;
	ya = id / ATLAS_W;
	x = xa * ATLAS_TW;
	y = ya * ATLAS_TH;
	tstart = W / 4;
	tstart *= atlas->page * H + y;
	tstart += x / 4;
	return tstart;
}

void r_drawAtlasSprite(int x, int y, TextureAtlas *atlas, int id)
{
	int tstart;
#ifdef MODE13
	r_drawSprite(x, y, atlas->textures[id]->w, atlas->textures[id]->h,
			 atlas->textures[id]);
#endif
#ifdef MODEX
	tstart = getAtlasTextureStart(atlas, id);
	r_spritefill2(x, y, atlas->textures[id]->w, atlas->textures[id]->h,
			 tstart);
#endif
}

void writeAtlasTextures(TextureAtlas *atlas)
{
	unsigned i, id;
	unsigned x, y, xa, ya;
	
#ifdef MODEX
	r_page(atlas->page);
	r_fill(atlas->alpha);

	for (i = 0; i < atlas->num; ++i) {
		id = atlas->textures[i]->id;
		xa = id % ATLAS_W;
		ya = id / ATLAS_W;
		x = xa * ATLAS_TW;
		y = ya * ATLAS_TH;
		r_drawSprite(x, y, atlas->textures[i]->w, atlas->textures[i]->h,
				 atlas->textures[i]);
	}

	r_page(page);
#endif
}

