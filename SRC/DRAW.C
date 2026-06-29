#include "SRC\RENDER.H"

Tri r_buffer[RBUFFERLEN];
unsigned r_sorted[RBUFFERLEN];
unsigned r_num = 0;

mat4 r_matrix;

unsigned drawCount = 0;
float drawTime = 0.0f;

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
	    && t.v0.z > 0.0f && t.v1.z > 0.0f && t.v2.z > 0.0f
	    && faceculling)
		return;

	fc = t.v0;
	fc = vec3Add(fc, t.v1);
	fc = vec3Add(fc, t.v2);
	fc = vec3Scale(fc, 0.33333333f);
	t.fc = vec3Len2(&fc);

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

	if (r_num >= RBUFFERLEN ||
	    !tex || tex->w == 0 || tex->h == 0 || !tex->data) return;

	t.v0 = mat4vec3(&r_matrix, v); // transform vertex
	t.v1 = Vec3(w, h * ((float) W / (float) H), // preserve aspect ratio
			0.0f);

	t.fc = vec3Len2(&t.v0);

	t.c = 0;
	t.tex = tex; // set texture for sprite draw

	r_buffer[r_num] = t; // render buffer

	++r_num; // index
}

void r_sort()
{
	unsigned i, k;
	float kv;
	int j;

	for (i = 0; i < r_num; ++i) {
		r_sorted[i] = i;
	}

	if (!zsort || r_num < 2) return;

	for (i = 1; i < r_num; ++i) {
		k = r_sorted[i];
		kv = r_buffer[k].fc;
		j = (int) i - 1;

		while (j >= 0 && r_buffer[r_sorted[j]].fc < kv) {
			r_sorted[j + 1] = r_sorted[j];
			--j;
		}
		r_sorted[j + 1] = k;
	}
}

void r_draw()
{
	unsigned i;
	unsigned start;

	drawCount = 0;
	drawTime = 0.0f;

	// draw triangles back to front
	for (i = 0; i < r_num; ++i) {
		start = itime;

		if (r_buffer[r_sorted[i]].tex) { // sprite
			r_drawSprite3D(&r_buffer[r_sorted[i]].v0,
					 r_buffer[r_sorted[i]].v1.x,
					 r_buffer[r_sorted[i]].v1.y,
					 r_buffer[r_sorted[i]].tex);
		}
		else if (filled) {
			r_drawTri3D(&r_buffer[r_sorted[i]].v0,
					&r_buffer[r_sorted[i]].v1,
					&r_buffer[r_sorted[i]].v2,
					r_buffer[r_sorted[i]].c);
		}
		else if (wireframe) {
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
		else {
		}
		
		drawTime += (float) (itime - start) * TOSECOND;
	}

	r_num = 0;
}

void r_drawString(int x, int y, char *str)
{
	int i;
	int fw = 8;
	int num = W/fw;

	for (i = 0; str[i] && i < num; ++i) {
		r_drawAtlasFont(x + fw * i, y, str[i]);
	}
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
			     (int) round((float)W/2.0f + 0.45f*(float)H*cosf(2.0f*PI*a)),
			     (int) round((float)H/2.0f + 0.45f*(float)H*sinf(2.0f*PI*a)),
			     (byte) (256.0f*a));
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

	r_add(&ico0, &ico1, &ico4, (byte) (c+ci*0));
	r_add(&ico1, &ico0, &ico7, (byte) (c+ci*1));

	r_add(&ico2, &ico3, &ico5, (byte) (c+ci*2));
	r_add(&ico3, &ico2, &ico6, (byte) (c+ci*3));

	r_add(&ico4, &ico5, &ico8, (byte) (c+ci*4));
	r_add(&ico5, &ico4, &ico11, (byte) (c+ci*5));

	r_add(&ico6, &ico7, &ico9, (byte) (c+ci*6));
	r_add(&ico7, &ico6, &ico10, (byte) (c+ci*7));

	r_add(&ico8, &ico9, &ico0, (byte) (c+ci*8));
	r_add(&ico9, &ico8, &ico3, (byte) (c+ci*9));

	r_add(&ico10, &ico11, &ico1, (byte) (c+ci*10));
	r_add(&ico11, &ico10, &ico2, (byte) (c+ci*11));

	r_add(&ico0, &ico4, &ico8, (byte) (c+ci*12));
	r_add(&ico7, &ico0, &ico9, (byte) (c+ci*13));

	r_add(&ico4, &ico1, &ico11, (byte) (c+ci*14));
	r_add(&ico1, &ico7, &ico10, (byte) (c+ci*15));

	r_add(&ico2, &ico5, &ico11, (byte) (c+ci*16));
	r_add(&ico6, &ico2, &ico10, (byte) (c+ci*17));

	r_add(&ico3, &ico6, &ico9, (byte) (c+ci*18));
	r_add(&ico5, &ico3, &ico8, (byte) (c+ci*19));

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

	r_add(&cube00, &cube01, &cube02, (byte) (c+ci*0));
	r_add(&cube00, &cube02, &cube03, (byte) (c+ci*1));

	r_add(&cube10, &cube12, &cube11, (byte) (c+ci*2));
	r_add(&cube10, &cube13, &cube12, (byte) (c+ci*3));

	r_add(&cube00, &cube10, &cube11, (byte) (c+ci*4));
	r_add(&cube00, &cube11, &cube01, (byte) (c+ci*5));

	r_add(&cube01, &cube11, &cube12, (byte) (c+ci*6));
	r_add(&cube01, &cube12, &cube02, (byte) (c+ci*7));

	r_add(&cube02, &cube12, &cube13, (byte) (c+ci*8));
	r_add(&cube02, &cube13, &cube03, (byte) (c+ci*9));

	r_add(&cube03, &cube13, &cube10, (byte) (c+ci*10));
	r_add(&cube03, &cube10, &cube00, (byte) (c+ci*11));

	r_matrix = rm0;
}

void cubeDemo(float t)
{
	unsigned i;
	unsigned cn;

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
					   Vec3(10.0f*sinf(t+2*(float)i/PI),
						  10.0f*cosf(t/5.0f+(float)i/PI/5.0f),
						  10.0f+5.0f*sinf(t/5.0f+(float)i/PI/5.0f)));
		r_matrix = rotateY(&r_matrix, sinf((float)i/PI)*(float)i/(float)cn*t);
		r_matrix = rotateX(&r_matrix, t);

		r_add(&cube00, &cube01, &cube02, (byte) (48+i%30));
		r_add(&cube00, &cube02, &cube03, (byte) (50+i%30));

		r_add(&cube10, &cube12, &cube11, (byte) (38+i%30));
		r_add(&cube10, &cube13, &cube12, (byte) (40+i%30));

		r_add(&cube00, &cube10, &cube11, (byte) (45+i%30));
		r_add(&cube00, &cube11, &cube01, (byte) (47+i%30));

		r_add(&cube01, &cube11, &cube12, (byte) (35+i%30));
		r_add(&cube01, &cube12, &cube02, (byte) (37+i%30));

		r_add(&cube02, &cube12, &cube13, (byte) (55+i%30));
		r_add(&cube02, &cube13, &cube03, (byte) (57+i%30));

		r_add(&cube03, &cube13, &cube10, (byte) (42+i%30));
		r_add(&cube03, &cube10, &cube00, (byte) (44+i%30));

		r_matrix = rm0;
	}
}

void lineTest(float t) {
	unsigned i;
	unsigned num = 64;
	float o;
	vec2 v0, v1;
	for (i = 0; i < num; ++i) {
		o = 2.0f * PI / (float) (num * i);
		v0 = Vec2(0.0f, 0.0f);
		v1 = Vec2(0.9f*cosf(t+o), 0.9f*sinf(t+o));
		r_drawLineClip(&v0, &v1, (byte) (i+32));
	}
}

void drawWall(float x, float y, float z, float w, float h, int d, int n, byte c1, byte c2)
{
	int i;
	byte c = c1;
	vec3 wall0 = Vec3(x, y, z);
	vec3 wall1 = Vec3(x, y, z);
	vec3 wall2 = Vec3(x, y+h, z);
	vec3 wall3 = Vec3(x, y+h, z);
	vec3 step = Vec3(w, 0.0f, 0.0f);

	switch (d) {
	case 0:
	case 1: // x direction wall
		wall0.x -= w/2.0f;
		wall1.x += w/2.0f;
		wall2.x += w/2.0f;
		wall3.x -= w/2.0f;
		step = Vec3(w, 0.0f, 0.0f);
		break;
	case -1: // flip face
		wall0.x += w/2.0f;
		wall1.x -= w/2.0f;
		wall2.x -= w/2.0f;
		wall3.x += w/2.0f;
		step = Vec3(-w, 0.0f, 0.0f);
		break;
	case 2: // z direction wall
		wall0.z -= w/2.0f;
		wall1.z += w/2.0f;
		wall2.z += w/2.0f;
		wall3.z -= w/2.0f;
		step = Vec3(0.0f, 0.0f, w);
		break;
	case -2:
		wall0.z += w/2.0f;
		wall1.z -= w/2.0f;
		wall2.z -= w/2.0f;
		wall3.z += w/2.0f;
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

