#include "SRC\RENDER.H"

Tri r_buffer[RBUFFERLEN];
unsigned r_sorted[RBUFFERLEN];
unsigned r_num = 0;

mat4 r_matrix;

unsigned drawCount = 0;

void r_add(vec3 *v0, vec3 *v1, vec3 *v2, byte c)
{
	Tri t;
	vec3 fc;

	if (r_num >= RBUFFERLEN) return;

	t.v0 = mat4vec3(&r_matrix, v0); // transform vertices
	t.v1 = mat4vec3(&r_matrix, v1);
	t.v2 = mat4vec3(&r_matrix, v2);

	//face culling
	if ((t.v1.x/t.v1.z-t.v0.x/t.v0.z)*(t.v2.y/t.v2.z-t.v0.y/t.v0.z) -
	    (t.v1.y/t.v1.z-t.v0.y/t.v0.z)*(t.v2.x/t.v2.z-t.v0.x/t.v0.z) < 0.0f
	    && faceculling)
		return;

	fc = t.v0;
	fc = vec3Add(fc, t.v1);
	fc = vec3Add(fc, t.v2);
	fc = vec3Scale(fc, 1.0f/3.0f);
	t.fc = vec3Len(&fc);

	t.c = c;
	t.tex = NULL; // untextured

	r_buffer[r_num] = t; // render buffer

	++r_num; // triangle index
}

void r_addf(float v0x, float v0y, float v0z,
		float v1x, float v1y, float v1z,
		float v2x, float v2y, float v2z, byte c)
{
	vec3 v0 = Vec3(v0x, v0y, v0z);
	vec3 v1 = Vec3(v1x, v1y, v1z);
	vec3 v2 = Vec3(v2x, v2y, v2z);

	r_add(&v0, &v1, &v2, c);
}

void r_addSprite(vec3 *v, float w, float h, Texture *tex)
{
	Tri t;

	if (r_num >= RBUFFERLEN) return;

	t.v0 = mat4vec3(&r_matrix, v); // transform vertex
	t.v1 = Vec3(w, h * ((float) W / (float) H), // preserve aspect ratio
			0.0f);

	t.fc = vec3Len(&t.v0);

	t.c = 0;
	t.tex = tex; // set texture for sprite draw

	r_buffer[r_num] = t; // render buffer

	++r_num; // index
}

void r_sort()
{
	int i;
	int j;
	int tmp;
	int largest_i;
	float largest;

	for (i = 0; i < r_num; ++i) {
		r_sorted[i] = i;
	}

	if (!zsort) return;

	// selection sort?
	for (i = 0; i < r_num; ++i) {
		largest_i = i;
		largest = r_buffer[r_sorted[i]].fc;

		for (j = i; j < r_num; ++j) {
			if (r_buffer[r_sorted[j]].fc > largest) {
				largest_i = j;
				largest = r_buffer[r_sorted[j]].fc;
			}
		}
		tmp = r_sorted[i];
		r_sorted[i] = r_sorted[largest_i];
		r_sorted[largest_i] = tmp;
	}
}

void r_draw()
{
	int i;

	drawCount = 0;

	// draw triangles back to front
	for (i = 0; i < r_num; ++i) {
		if (r_buffer[r_sorted[i]].tex) { // sprite
			r_drawSprite3D(&r_buffer[r_sorted[i]].v0,
					 r_buffer[r_sorted[i]].v1.x,
					 r_buffer[r_sorted[i]].v1.y,
					 r_buffer[r_sorted[i]].tex);
			continue;
		}
		if (filled)
			r_drawTri3D(&r_buffer[r_sorted[i]].v0,
					&r_buffer[r_sorted[i]].v1,
					&r_buffer[r_sorted[i]].v2,
					r_buffer[r_sorted[i]].c);
		if (wireframe) {
			r_drawLine3D(&r_buffer[r_sorted[i]].v0,
					 &r_buffer[r_sorted[i]].v1,
					 r_buffer[r_sorted[i]].c);
			r_drawLine3D(&r_buffer[r_sorted[i]].v0,
					 &r_buffer[r_sorted[i]].v2,
					 r_buffer[r_sorted[i]].c);
			r_drawLine3D(&r_buffer[r_sorted[i]].v1,
					 &r_buffer[r_sorted[i]].v2,
					 r_buffer[r_sorted[i]].c);
		}
	}

	r_num = 0;
}

// CUSTOM DRAW FUNCTIONS

int triCount = 0;

void triDemo()
{
	int i;
	float size = 1.0f;
	vec2 vt0, vt1, vt2;

	clearscr = 0;
	doublebuffer = 0;

	for (i = 0; i < 1; ++i) {
		vt0.x = size * (RANDF * 2.0f - 1.0f);
		vt0.y = size * (RANDF * 2.0f - 1.0f);

		vt1.x = size * (RANDF * 2.0f - 1.0f);
		vt1.y = size * (RANDF * 2.0f - 1.0f);

		vt2.x = size * (RANDF * 2.0f - 1.0f);
		vt2.y = size * (RANDF * 2.0f - 1.0f);

		r_drawTriClip(&vt0, &vt1, &vt2, (byte) (RANDF * 256.0f));

		++triCount;
	}

	drawCount = triCount;
}

void lineDemo()
{
	unsigned i;
	float a;
	unsigned n = 64000;
	for (i = 0; i < n; ++i) {
		a = (float) i / (float) n;

		r_drawLine(W/2, H/2,
			     round(W/2 + 0.45f*H*cos(2.0f*PI*a)),
			     round(H/2 + 0.45f*H*sin(2.0f*PI*a)),
			     256*a);
	}
	drawCount = n;
}

void drawIco(vec3 pos, mat4 *rot, float a, byte c, byte ci)
{
	float phi = 1.618034f;

	vec3 ico0 = Vec3(0.5f*a*phi, 0.5f*a, 0.0f*a);
	vec3 ico1 = Vec3(0.5f*a*phi, -0.5f*a, 0.0f*a);
	vec3 ico2 = Vec3(-0.5f*a*phi, -0.5f*a, 0.0f*a);
	vec3 ico3 = Vec3(-0.5f*a*phi, 0.5f*a, 0.0f*a);

	vec3 ico4 = Vec3(0.5f*a, 0.0f*a, 0.5f*a*phi);
	vec3 ico5 = Vec3(-0.5f*a, 0.0f*a, 0.5f*a*phi);
	vec3 ico6 = Vec3(-0.5f*a, 0.0f*a, -0.5f*a*phi);
	vec3 ico7 = Vec3(0.5f*a, 0.0f*a, -0.5f*a*phi);

	vec3 ico8 = Vec3(0.0f*a, 0.5f*a*phi, 0.5f*a);
	vec3 ico9 = Vec3(0.0f*a, 0.5f*a*phi, -0.5f*a);
	vec3 ico10 = Vec3(0.0f*a, -0.5f*a*phi, -0.5f*a);
	vec3 ico11 = Vec3(0.0f*a, -0.5f*a*phi, 0.5f*a);

	mat4 rm0 = r_matrix;

	r_matrix = scale(&r_matrix, a);
	r_matrix = translate(&r_matrix, pos);
	r_matrix = mat4mat4(&r_matrix, rot);

	r_add(&ico0, &ico1, &ico4, c+ci*0);
	r_add(&ico1, &ico0, &ico7, c+ci*1);

	r_add(&ico2, &ico3, &ico5, c+ci*2);
	r_add(&ico3, &ico2, &ico6, c+ci*3);

	r_add(&ico4, &ico5, &ico8, c+ci*4);
	r_add(&ico5, &ico4, &ico11, c+ci*5);

	r_add(&ico6, &ico7, &ico9, c+ci*6);
	r_add(&ico7, &ico6, &ico10, c+ci*7);

	r_add(&ico8, &ico9, &ico0, c+ci*8);
	r_add(&ico9, &ico8, &ico3, c+ci*9);

	r_add(&ico10, &ico11, &ico1, c+ci*10);
	r_add(&ico11, &ico10, &ico2, c+ci*11);

	r_add(&ico0, &ico4, &ico8, c+ci*12);
	r_add(&ico7, &ico0, &ico9, c+ci*13);

	r_add(&ico4, &ico1, &ico11, c+ci*14);
	r_add(&ico1, &ico7, &ico10, c+ci*15);

	r_add(&ico2, &ico5, &ico11, c+ci*16);
	r_add(&ico6, &ico2, &ico10, c+ci*17);

	r_add(&ico3, &ico6, &ico9, c+ci*18);
	r_add(&ico5, &ico3, &ico8, c+ci*19);

	r_matrix = rm0;
}

void drawCube(vec3 pos, mat4 *rot, float a, byte c, byte ci)
{
	vec3 cube00 = Vec3(-0.5f*a, -0.5f*a, -0.5f*a);
	vec3 cube01 = Vec3(0.5f*a, -0.5f*a, -0.5f*a);
	vec3 cube02 = Vec3(0.5f*a, 0.5f*a, -0.5f*a);
	vec3 cube03 = Vec3(-0.5f*a, 0.5f*a, -0.5f*a);

	vec3 cube10 = Vec3(-0.5f*a, -0.5f*a, 0.5f*a);
	vec3 cube11 = Vec3(0.5f*a, -0.5f*a, 0.5f*a);
	vec3 cube12 = Vec3(0.5f*a, 0.5f*a, 0.5f*a);
	vec3 cube13 = Vec3(-0.5f*a, 0.5f*a, 0.5f*a);

	mat4 rm0 = r_matrix;

	r_matrix = scale(&r_matrix, a);
	r_matrix = translate(&r_matrix, pos);
	r_matrix = mat4mat4(&r_matrix, rot);

	r_add(&cube00, &cube01, &cube02, c+ci*0);
	r_add(&cube00, &cube02, &cube03, c+ci*1);

	r_add(&cube10, &cube12, &cube11, c+ci*2);
	r_add(&cube10, &cube13, &cube12, c+ci*3);

	r_add(&cube00, &cube10, &cube11, c+ci*4);
	r_add(&cube00, &cube11, &cube01, c+ci*5);

	r_add(&cube01, &cube11, &cube12, c+ci*6);
	r_add(&cube01, &cube12, &cube02, c+ci*7);

	r_add(&cube02, &cube12, &cube13, c+ci*8);
	r_add(&cube02, &cube13, &cube03, c+ci*9);

	r_add(&cube03, &cube13, &cube10, c+ci*10);
	r_add(&cube03, &cube10, &cube00, c+ci*11);

	r_matrix = rm0;
}

void groundDemo(vec3 cam)
{
	int x, z;
	int y = -3.0f;
	int c1 = 49;
	int c2 = 2;
	int xn = 10, zn = 10;
	float size = 1.5f;

	for (x = 0; x < xn; ++x) {
		for (z = 0; z < zn; ++z) {
			r_addf(size*(x-round(cam.x/size)-xn/2.0f), y,
				 size*(z-round(cam.z/size)-zn/2.0f),
				 size*(1.0f+x-round(cam.x/size)-xn/2.0f), y,
				 size*(z-round(cam.z/size)-zn/2.0f),
				 size*(1.0f+x-round(cam.x/size)-xn/2.0f), y,
				 size*(1.0f+z-round(cam.z/size)-zn/2.0f), c1);
			r_addf(size*(x-round(cam.x/size)-xn/2.0f), y,
				 size*(z-round(cam.z/size)-zn/2.0f),
				 size*(1.0f+x-round(cam.x/size)-xn/2.0f), y,
				 size*(1.0f+z-round(cam.z/size)-zn/2.0f),
				 size*(x-round(cam.x/size)-xn/2.0f), y,
				 size*(1.0f+z-round(cam.z/size)-zn/2.0f), c2);
		}
	}
}

#define groundLineX(x0, x1, z0, z1)						\
	for (x = x0; x < x1; ++x) {						\
		z = z0;								\
		v0 = Vec3(size*(0.5f+x-round(cam.x/size)-xoffs), y,	\
			    size*(-0.5f+z-round(cam.z/size)-zoffs));	\
		v0 = mat4vec3(&r_matrix, &v0);				\
		z = z1;								\
		v1 = Vec3(size*(0.5f+x-round(cam.x/size)-xoffs), y,	\
			    size*(-0.5f+z-round(cam.z/size)-zoffs));	\
		v1 = mat4vec3(&r_matrix, &v1);				\
		r_drawLine3D(&v0, &v1, c);					\
	}

#define groundLineZ(z0, z1, x0, x1)					\
	for (z = z0; z < z1; ++z) {					\
		x = x0;							\
		v0 = Vec3(size*(x-round(cam.x/size)-xoffs), y,	\
			    size*(z-round(cam.z/size)-zoffs));	\
		v0 = mat4vec3(&r_matrix, &v0);			\
		x = x1;							\
		v1 = Vec3(size*(x-round(cam.x/size)-xoffs), y,	\
			    size*(z-round(cam.z/size)-zoffs));	\
		v1 = mat4vec3(&r_matrix, &v1);			\
		r_drawLine3D(&v0, &v1, c);				\
	}

#define groundLineXC(x0, x1, z0, z1)						\
	for (x = x0; x < x1; ++x) {							\
		zz = (x - (float) (x0 + x1)/2 + 1) * (x - (float) (x0 + x1)/2); \
		zz = z0 + zz / (float) (2 * (z1 - z0));				\
		v0 = Vec3(size*(0.5f+x-round(cam.x/size)-xoffs), y,		\
			    size*(-0.5f+zz-round(cam.z/size)-zoffs));		\
		v0 = mat4vec3(&r_matrix, &v0);					\
		zz = z1;									\
		v1 = Vec3(size*(0.5f+x-round(cam.x/size)-xoffs), y,		\
			    size*(-0.5f+zz-round(cam.z/size)-zoffs));		\
		v1 = mat4vec3(&r_matrix, &v1);					\
		r_drawLine3D(&v0, &v1, c);						\
	}

#define groundLineZC(z0, z1, x0, x1)						\
	for (z = z0; z < z1; ++z) {							\
		xx = (z - (float) (z0 + z1)/2 + 1) * (z - (float) (z0 + z1)/2); \
		xx = x0 + xx / (float) (2 * (x1 - x0));				\
		v0 = Vec3(size*(xx-round(cam.x/size)-xoffs), y,			\
			    size*(z-round(cam.z/size)-zoffs));			\
		v0 = mat4vec3(&r_matrix, &v0);					\
		xx = x1;									\
		v1 = Vec3(size*(xx-round(cam.x/size)-xoffs), y,			\
			    size*(z-round(cam.z/size)-zoffs));			\
		v1 = mat4vec3(&r_matrix, &v1);					\
		r_drawLine3D(&v0, &v1, c);						\
	}

void groundLines(vec3 cam)
{
	int x, z;
	float xx, zz;
	vec3 v0, v1;
	int neard, nearx0, nearx1, nearz0, nearz1;
	float xoffs, zoffs;
	int y = -3.0f;
	int c = 49;
	int xn = (int) ZFAR, zn = (int) ZFAR;
	float size = 2.0f;
	neard = 3;
	xoffs = (xn)/2.0f;
	zoffs = (zn-1)/2.0f;
	nearx0 = xn/2 - neard;
	nearx1 = xn/2 + neard;
	nearz0 = zn/2 - neard;
	nearz1 = zn/2 + neard;

	// draw crosses closest fneard to camera (TODO: z clipping)
	for (x = nearx0; x < nearx1; x++) {
		for (z = nearz0; z < nearz1; z++) {
			v0 = Vec3(size*(x-round(cam.x/size)-xoffs), y,
				    size*(z-round(cam.z/size)-zoffs));
			v0 = mat4vec3(&r_matrix, &v0);
			v1 = Vec3(size*(1.0f+x-round(cam.x/size)-xoffs), y,
				    size*(z-round(cam.z/size)-zoffs));
			v1 = mat4vec3(&r_matrix, &v1);
			r_drawLine3D(&v0, &v1, c);

			v0 = Vec3(size*(0.5f+x-round(cam.x/size)-xoffs), y,
				    size*(-0.5f+z-round(cam.z/size)-zoffs));
			v0 = mat4vec3(&r_matrix, &v0);
			v1 = Vec3(size*(0.5f+x-round(cam.x/size)-xoffs), y,
				    size*(0.5f+z-round(cam.z/size)-zoffs));
			v1 = mat4vec3(&r_matrix, &v1);
			r_drawLine3D(&v0, &v1, c);
		}
	}

	// draw long lines far away
	groundLineXC(0, xn, 0, nearz0); // "circular" edge
	groundLineXC(0, xn, zn, nearz1);
	groundLineZ(0, nearz0, nearx0, xn/2);
	groundLineZ(0, nearz0, xn/2, nearx1);
	groundLineZ(nearz1, zn, nearx0, xn/2);
	groundLineZ(nearz1, zn, xn/2, nearx1);

	groundLineZC(0, zn, 0, nearx0);
	groundLineZC(0, zn, xn, nearx1);
	groundLineX(0, nearx0, nearz0, zn/2);
	groundLineX(0, nearx0, zn/2, nearz1);
	groundLineX(nearx1, xn, nearz0, zn/2);
	groundLineX(nearx1, xn, zn/2, nearz1);
}

void cubeDemo(float t)
{
	int i;
	int cn;

	vec3 cube00 = Vec3(-0.5f, -0.5f, -0.5f);
	vec3 cube01 = Vec3(0.5f, -0.5f, -0.5f);
	vec3 cube02 = Vec3(0.5f, 0.5f, -0.5f);
	vec3 cube03 = Vec3(-0.5f, 0.5f, -0.5f);

	vec3 cube10 = Vec3(-0.5f, -0.5f, 0.5f);
	vec3 cube11 = Vec3(0.5f, -0.5f, 0.5f);
	vec3 cube12 = Vec3(0.5f, 0.5f, 0.5f);
	vec3 cube13 = Vec3(-0.5f, 0.5f, 0.5f);

	mat4 rm0 = r_matrix;

	cn = 5;
	for (i = 0; i < cn; ++i) {
		r_matrix = translate(&r_matrix,
					   Vec3(10.0f*sin(t+2*i/PI),
						  10.0f*cos(t/5.0f+i/PI/5.0f),
						  10.0f+5.0f*sin(t/5.0f+i/PI/5.0f)));
		r_matrix = rotateY(&r_matrix, sin(i/PI)*i/cn*t);
		r_matrix = rotateX(&r_matrix, t);

		r_add(&cube00, &cube01, &cube02, 48+i%30);
		r_add(&cube00, &cube02, &cube03, 50+i%30);

		r_add(&cube10, &cube12, &cube11, 38+i%30);
		r_add(&cube10, &cube13, &cube12, 40+i%30);

		r_add(&cube00, &cube10, &cube11, 45+i%30);
		r_add(&cube00, &cube11, &cube01, 47+i%30);

		r_add(&cube01, &cube11, &cube12, 35+i%30);
		r_add(&cube01, &cube12, &cube02, 37+i%30);

		r_add(&cube02, &cube12, &cube13, 55+i%30);
		r_add(&cube02, &cube13, &cube03, 57+i%30);

		r_add(&cube03, &cube13, &cube10, 42+i%30);
		r_add(&cube03, &cube10, &cube00, 44+i%30);

		r_matrix = rm0;
	}
}

void lineTest(float t) {
	int i;
	float o;
	vec2 v0, v1;
	int num = 64;
	for (i = 0; i < num; ++i) {
		o = 2.0f * PI / (float) num * i;
		v0 = Vec2(0.0f, 0.0f);
		v1 = Vec2(0.9f*cos(t+o), 0.9f*sin(t+o));
		r_drawLineClip(&v0, &v1, i+32);
	}
}

void drawFloor(vec3 camPos, float rotY, byte c1, byte c2, byte co)
{
	int x, y, c, s, a, b;
	int xx, xx0, yy, zz, zz0;
	int cx, cz;
	float cosy, siny;

	s = H / 2;
	a = 2;
	b = 2;
	cx = (int) (camPos.x * s);
	cz = (int) (camPos.z * s);
	cx = cx >> b;
	cz = cz >> b;
	cosy = cos(rotY);
	siny = sin(rotY);
	
	for (x = 0; x < W; x++) {
		for (y = H / 2 + 1; y < H; y++) {
			xx = x - W / 2;
			yy = y - H / 2;
			
			xx = (s * xx) / yy;
			zz = (W / 2 * s) / yy;

			xx0 = xx;
			zz0 = zz;
			xx = xx0 * cosy + zz0 * siny;
			zz = xx0 * siny - zz0 * cosy;

			xx = xx >> b;
			zz = zz >> b;
			xx -= cx % s;
			zz -= cz % s;
			xx *= a;
			zz *= a;

			c = xx % s > sign(xx) * s / 2;
			c = c ^ (zz % s > sign(zz) * s / 2);

			c = c1 * c + c2 * (1 - c);
			
			r_putpixel(x, y, c);

			c += co;
			yy = H - 1 - y;
			r_putpixel(x, yy, c);
		}
	}
}

void drawWall(float x, float y, float z, float w, float h, int d, int n, int c1, int c2)
{
	int i;
	int c = c1;
	vec3 wall0 = Vec3(x, y, z);
	vec3 wall1 = Vec3(x, y, z);
	vec3 wall2 = Vec3(x, y+h, z);
	vec3 wall3 = Vec3(x, y+h, z);
	vec3 step = Vec3(w, 0.0f, 0.0f);

	switch (d) {
	case 0:
	case 1: // x direction wall
		wall0.x -= w/2;
		wall1.x += w/2;
		wall2.x += w/2;
		wall3.x -= w/2;
		step = Vec3(w, 0.0f, 0.0f);
		break;
	case -1: // flip face
		wall0.x += w/2;
		wall1.x -= w/2;
		wall2.x -= w/2;
		wall3.x += w/2;
		step = Vec3(-w, 0.0f, 0.0f);
		break;
	case 2: // z direction wall
		wall0.z -= w/2;
		wall1.z += w/2;
		wall2.z += w/2;
		wall3.z -= w/2;
		step = Vec3(0.0f, 0.0f, w);
		break;
	case -2:
		wall0.z += w/2;
		wall1.z -= w/2;
		wall2.z -= w/2;
		wall3.z += w/2;
		step = Vec3(0.0f, 0.0f, -w);
		break;
	}

	for (i = 0; i < n; ++i) {
		r_add(&wall0, &wall1, &wall2, c);
		r_add(&wall0, &wall2, &wall3, c);
		wall0 = vec3Add(wall0, step);
		wall1 = vec3Add(wall1, step);
		wall2 = vec3Add(wall2, step);
		wall3 = vec3Add(wall3, step);
		c = (c == c1 ? c2 : c1);
	}
}

