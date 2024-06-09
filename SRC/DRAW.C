#include "SRC\RENDER.H"

struct tri r_buffer[RBUFFERLEN];
unsigned r_sorted[RBUFFERLEN];
unsigned r_num = 0;

mat4 r_matrix;

unsigned drawCount = 0;

void r_add(vec3 *v0, vec3 *v1, vec3 *v2, byte c)
{
	struct tri t;
	vec3 fc;
	vec4 m;

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

