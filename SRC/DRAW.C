#include "SRC\RENDER.H"

struct tri r_buffer[256];

BYTE r_s[256]; // sorted

BYTE r_n = 0;

// render matrix
struct mat4 rm;

void r_add(struct vec4* v0, struct vec4* v1, struct vec4* v2, BYTE c)
{
	struct tri t;
	struct vec4 fc;

	t.v0 = m4xv4(&rm, v0); // transform vertices
	t.v1 = m4xv4(&rm, v1);
	t.v2 = m4xv4(&rm, v2);

	//face culling
	if ((t.v1.x-t.v0.x)*(t.v2.y-t.v0.y) -
		(t.v1.y-t.v0.y)*(t.v2.x-t.v0.x) > 1.0f)
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

void r_sort() // selection sort of something
{
	int i;
	int j;
	int c;
	int largest;
	float largest_v;

	for (i = 0; i < r_n; ++i) {
		r_s[i] = i;
	}

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

	r_n = 0;
}
