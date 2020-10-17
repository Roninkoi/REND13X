#include "SRC\RENDER.H"

struct tri r_buffer[256];

BYTE r_s[256]; // sorted

BYTE r_n = 0;

int drawcount = 0;

// render matrix
mat4 rm;

void r_add(vec4* v0, vec4* v1, vec4* v2, BYTE c)
{
	struct tri t;
	vec4 fc;

	t.v0 = m4xv4(&rm, v0); // transform vertices
	t.v1 = m4xv4(&rm, v1);
	t.v2 = m4xv4(&rm, v2);

	//face culling
	if ((t.v1.x/t.v1.z-t.v0.x/t.v0.z)*(t.v2.y/t.v2.z-t.v0.y/t.v0.z) -
		(t.v1.y/t.v1.z-t.v0.y/t.v0.z)*(t.v2.x/t.v2.z-t.v0.x/t.v0.z) < 0.0f
		&& faceculling)
		return;

	fc = t.v0;
	fc = v4a(fc, t.v1);
	fc = v4a(fc, t.v2);
	fc = v4s(fc, 1.0f/3.0f);
	t.fc = v4l(fc);

	t.c = c;

	r_buffer[r_n] = t; // render buffer

	++r_n; // triangle index
}

void r_addf(float v0x, float v0y, float v0z,
		  float v1x, float v1y, float v1z,
		  float v2x, float v2y, float v2z, BYTE c)
{
	vec4 v0 = Vec4(v0x, v0y, v0z, 1.0f);
	vec4 v1 = Vec4(v1x, v1y, v1z, 1.0f);
	vec4 v2 = Vec4(v2x, v2y, v2z, 1.0f);

	r_add(&v0, &v1, &v2, c);
}

void r_sort() // selection sort or something
{
	int i;
	int j;
	int c;
	int largest;
	float largest_v;

	for (i = 0; i < r_n; ++i) {
		r_s[i] = i;
	}

	if (!zsort) return;

	for (i = 0; i < r_n; ++i) {
		largest = i;
		largest_v = r_buffer[r_s[i]].fc;

		for (j = i; j < r_n; ++j) {
			if (r_buffer[r_s[j]].fc > largest_v) {
				largest = j;
				largest_v = r_buffer[r_s[j]].fc;
			}
		}
		c = r_s[i];
		r_s[i] = r_s[largest];
		r_s[largest] = c;
	}
}

void r_draw()
{
	int i;

	for (i = 0; i < r_n; ++i) {
		r_drawtri3d(&r_buffer[r_s[i]].v0,
			&r_buffer[r_s[i]].v1,
			&r_buffer[r_s[i]].v2,
			r_buffer[r_s[i]].c);
	}

	drawcount = r_n;

	r_n = 0;
}
