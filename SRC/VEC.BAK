#include "SRC\VEC.H"

struct vec4 vec4(float x, float y, float z, float w)
{
	struct vec4 v;
	v.x = x;
	v.y = y;
	v.z = z;
	v.w = w;
	return v;
}

// null vector
struct vec4 nvec4()
{
	struct vec4 v;
	v.x = 0.0f;
	v.y = 0.0f;
	v.z = 0.0f;
	v.w = 0.0f;
	return v;
}

struct vec3 vec3(float x, float y, float z)
{
	struct vec3 v;
	v.x = x;
	v.y = y;
	v.z = z;
	return v;
}

struct vec2 vec2(float x, float y)
{
	struct vec2 v;
	v.x = x;
	v.y = y;
	return v;
}

struct mat4 mat4(float s)
{
	struct mat4 m;

	m.m[0][0] = 1.0f*s;
	m.m[0][1] = 0.0f;
	m.m[0][2] = 0.0f;
	m.m[0][3] = 0.0f;

	m.m[1][0] = 0.0f;
	m.m[1][1] = 1.0f*s;
	m.m[1][2] = 0.0f;
	m.m[1][3] = 0.0f;

	m.m[2][0] = 0.0f;
	m.m[2][1] = 0.0f;
	m.m[2][2] = 1.0f*s;
	m.m[2][3] = 0.0f;

	m.m[3][0] = 0.0f;
	m.m[3][1] = 0.0f;
	m.m[3][2] = 0.0f;
	m.m[3][3] = 1.0f*s;

	return m;
}

struct mat4 projmat(float fov, float ar, float fr, float nr)
{
	struct mat4 m;

	m.m[0][0] = 1.0f/(ar*tan(0.5f*fov));
	m.m[0][1] = 0.0f;
	m.m[0][2] = 0.0f;
	m.m[0][3] = 0.0f;

	m.m[1][0] = 0.0f;
	m.m[1][1] = 1.0f/(tan(0.5f*fov));
	m.m[1][2] = 0.0f;
	m.m[1][3] = 0.0f;

	m.m[2][0] = 0.0f;
	m.m[2][1] = 0.0f;
	m.m[2][2] = (-nr - fr)/(-nr - fr);
	m.m[2][3] = (2.0f*fr*nr)/(-nr - fr);

	m.m[3][0] = 0.0f;
	m.m[3][1] = 0.0f;
	m.m[3][2] = 1.0f;
	m.m[3][3] = 0.0f;

	return m;
}

struct mat4 rotmatY(float a)
{
	struct mat4 m;

	m.m[0][0] = cos(a);
	m.m[0][1] = 0.0f;
	m.m[0][2] = sin(a);
	m.m[0][3] = 0.0f;

	m.m[1][0] = 0.0f;
	m.m[1][1] = 1.0f;
	m.m[1][2] = 0.0f;
	m.m[1][3] = 0.0f;

	m.m[2][0] = -sin(a);
	m.m[2][1] = 0.0f;
	m.m[2][2] = cos(a);
	m.m[2][3] = 0.0f;

	m.m[3][0] = 0.0f;
	m.m[3][1] = 0.0f;
	m.m[3][2] = 0.0f;
	m.m[3][3] = 1.0f; // should this be 1 or 0

	return m;
}

struct mat4 rotmatX(float a)
{
	struct mat4 m;

	m.m[0][0] = 1.0f;
	m.m[0][1] = 0.0f;
	m.m[0][2] = 0.0f;
	m.m[0][3] = 0.0f;

	m.m[1][0] = 0.0f;
	m.m[1][1] = cos(a);
	m.m[1][2] = -sin(a);
	m.m[1][3] = 0.0f;

	m.m[2][0] = 0.0f;
	m.m[2][1] = sin(a);
	m.m[2][2] = cos(a);
	m.m[2][3] = 0.0f;

	m.m[3][0] = 0.0f;
	m.m[3][1] = 0.0f;
	m.m[3][2] = 0.0f;
	m.m[3][3] = 1.0f; // should this be 1 or 0

	return m;
}

// vector addition
struct vec4 v4a(struct vec4 v0, struct vec4 v1)
{
	v0.x += v1.x;
	v0.y += v1.y;
	v0.z += v1.z;
	v0.w += v1.w;

	return v0;
}

// vector scaling
struct vec4 v4s(struct vec4 v, float s)
{
	v.x *= s;
	v.y *= s;
	v.z *= s;
	v.w *= s;

	return v;
}

float v4l(struct vec4 v)
{
	return sqrt(v.x*v.x + v.y*v.y + v.z*v.z + v.w*v.w);
}

struct mat4 translate(struct mat4* m, struct vec4 v)
{
	struct mat4 r;

	struct mat4 tm = mat4(1.0f);

	tm.m[0][3] = v.x; // col 2, 3 not guaranteed
	tm.m[1][3] = v.y;
	tm.m[2][3] = v.z;

	r = m4xm4(m, &tm);

	return r;
}

// mat4 get row
struct vec4 m4gr(struct mat4 m, int r)
{
	struct vec4 v;
	v.x = m.m[r][0];
	v.y = m.m[r][1];
	v.z = m.m[r][2];
	v.w = m.m[r][3];
	return v;
}

// matrix multiplication, mat4 x vec4
struct vec4 m4xv4(struct mat4* m, struct vec4* v)
{
	struct vec4 r = nvec4();
/*
	r = v4a(r, v4s(m4gr(*m, 0), v->x));
	r = v4a(r, v4s(m4gr(*m, 1), v->y));
	r = v4a(r, v4s(m4gr(*m, 2), v->z));
	r = v4a(r, v4s(m4gr(*m, 3), v->w));*/

	r.x += m->m[0][0]*v->x;
	r.x += m->m[0][1]*v->y;
	r.x += m->m[0][2]*v->z;
	r.x += m->m[0][3]*v->w;

	r.y += m->m[1][0]*v->x;
	r.y += m->m[1][1]*v->y;
	r.y += m->m[1][2]*v->z;
	r.y += m->m[1][3]*v->w;

	r.z += m->m[2][0]*v->x;
	r.z += m->m[2][1]*v->y;
	r.z += m->m[2][2]*v->z;
	r.z += m->m[2][3]*v->w;

	r.w += m->m[3][0]*v->x;
	r.w += m->m[3][1]*v->y;
	r.w += m->m[3][2]*v->z;
	r.w += m->m[3][3]*v->w;

	return r;
}

struct mat4 m4xm4(struct mat4* m0, struct mat4* m1)
{
	struct mat4 r;

	// we don't use loops around here...

	// row 0
	r.m[0][0]  = m0->m[0][0] * m1->m[0][0]; // m0 row 0 dot m1 col 0
	r.m[0][0] += m0->m[0][1] * m1->m[1][0];
	r.m[0][0] += m0->m[0][2] * m1->m[2][0];
	r.m[0][0] += m0->m[0][3] * m1->m[3][0];

	r.m[0][1]  = m0->m[0][0] * m1->m[0][1];
	r.m[0][1] += m0->m[0][1] * m1->m[1][1];
	r.m[0][1] += m0->m[0][2] * m1->m[2][1];
	r.m[0][1] += m0->m[0][3] * m1->m[3][1];

	r.m[0][2]  = m0->m[0][0] * m1->m[0][2];
	r.m[0][2] += m0->m[0][1] * m1->m[1][2];
	r.m[0][2] += m0->m[0][2] * m1->m[2][2];
	r.m[0][2] += m0->m[0][3] * m1->m[3][2];

	r.m[0][3]  = m0->m[0][0] * m1->m[0][3];
	r.m[0][3] += m0->m[0][1] * m1->m[1][3];
	r.m[0][3] += m0->m[0][2] * m1->m[2][3];
	r.m[0][3] += m0->m[0][3] * m1->m[3][3];

	// row 1
	r.m[1][0]  = m0->m[1][0] * m1->m[0][0];
	r.m[1][0] += m0->m[1][1] * m1->m[1][0];
	r.m[1][0] += m0->m[1][2] * m1->m[2][0];
	r.m[1][0] += m0->m[1][3] * m1->m[3][0];

	r.m[1][1]  = m0->m[1][0] * m1->m[0][1];
	r.m[1][1] += m0->m[1][1] * m1->m[1][1];
	r.m[1][1] += m0->m[1][2] * m1->m[2][1];
	r.m[1][1] += m0->m[1][3] * m1->m[3][1];

	r.m[1][2]  = m0->m[1][0] * m1->m[0][2];
	r.m[1][2] += m0->m[1][1] * m1->m[1][2];
	r.m[1][2] += m0->m[1][2] * m1->m[2][2];
	r.m[1][2] += m0->m[1][3] * m1->m[3][2];

	r.m[1][3]  = m0->m[1][0] * m1->m[0][3];
	r.m[1][3] += m0->m[1][1] * m1->m[1][3];
	r.m[1][3] += m0->m[1][2] * m1->m[2][3];
	r.m[1][3] += m0->m[1][3] * m1->m[3][3];

	// row 2
	r.m[2][0]  = m0->m[2][0] * m1->m[0][0];
	r.m[2][0] += m0->m[2][1] * m1->m[1][0];
	r.m[2][0] += m0->m[2][2] * m1->m[2][0];
	r.m[2][0] += m0->m[2][3] * m1->m[3][0];

	r.m[2][1]  = m0->m[2][0] * m1->m[0][1];
	r.m[2][1] += m0->m[2][1] * m1->m[1][1];
	r.m[2][1] += m0->m[2][2] * m1->m[2][1];
	r.m[2][1] += m0->m[2][3] * m1->m[3][1];

	r.m[2][2]  = m0->m[2][0] * m1->m[0][2];
	r.m[2][2] += m0->m[2][1] * m1->m[1][2];
	r.m[2][2] += m0->m[2][2] * m1->m[2][2];
	r.m[2][2] += m0->m[2][3] * m1->m[3][2];

	r.m[2][3]  = m0->m[2][0] * m1->m[0][3];
	r.m[2][3] += m0->m[2][1] * m1->m[1][3];
	r.m[2][3] += m0->m[2][2] * m1->m[2][3];
	r.m[2][3] += m0->m[2][3] * m1->m[3][3];

	// row 3
	r.m[3][0]  = m0->m[3][0] * m1->m[0][0];
	r.m[3][0] += m0->m[3][1] * m1->m[1][0];
	r.m[3][0] += m0->m[3][2] * m1->m[2][0];
	r.m[3][0] += m0->m[3][3] * m1->m[3][0];

	r.m[3][1]  = m0->m[3][0] * m1->m[0][1];
	r.m[3][1] += m0->m[3][1] * m1->m[1][1];
	r.m[3][1] += m0->m[3][2] * m1->m[2][1];
	r.m[3][1] += m0->m[3][3] * m1->m[3][1];

	r.m[3][2]  = m0->m[3][0] * m1->m[0][2];
	r.m[3][2] += m0->m[3][1] * m1->m[1][2];
	r.m[3][2] += m0->m[3][2] * m1->m[2][2];
	r.m[3][2] += m0->m[3][3] * m1->m[3][2];

	r.m[3][3]  = m0->m[3][0] * m1->m[0][3];
	r.m[3][3] += m0->m[3][1] * m1->m[1][3];
	r.m[3][3] += m0->m[3][2] * m1->m[2][3];
	r.m[3][3] += m0->m[3][3] * m1->m[3][3];

	return r;
}

struct mat4 rotateY(struct mat4* m, float a)
{
	struct mat4 r;

	struct mat4 rm; // rotation matrix

	rm = rotmatY(a); // construct y rot mat

	r = m4xm4(m, &rm); // mul

	return r;
}

struct mat4 rotateX(struct mat4* m, float a)
{
	struct mat4 r;

	struct mat4 rm; // rotation matrix

	rm = rotmatX(a); // construct y rot mat

	r = m4xm4(m, &rm); // mul

	return r;
}
