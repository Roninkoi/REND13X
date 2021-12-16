#include "SRC\VEC.H"

void vec3Print(vec3 *v)
{
	printf("(%.1f %.1f %.1f)\n", v->x, v->y, v->z);
}

void vec4Print(vec4 *v)
{
	printf("(%.1f %.1f %.1f %.1f)\n", v->x, v->y, v->z, v->w);
}

void mat4Print(mat4 *m)
{
	printf("(%.1f %.1f %.1f %.1f\n", m->m[0][0], m->m[0][1], m->m[0][2], m->m[0][3]);
	printf(" %.1f %.1f %.1f %.1f\n", m->m[1][0], m->m[1][1], m->m[1][2], m->m[1][3]);
	printf(" %.1f %.1f %.1f %.1f\n", m->m[2][0], m->m[2][1], m->m[2][2], m->m[2][3]);
	printf(" %.1f %.1f %.1f %.1f)\n", m->m[3][0], m->m[3][1], m->m[3][2], m->m[3][3]);
}

vec2 Vec2(float x, float y)
{
	vec2 v;
	v.x = x;
	v.y = y;
	return v;
}

vec3 Vec3(float x, float y, float z)
{
	vec3 v;
	v.x = x;
	v.y = y;
	v.z = z;
	return v;
}

vec4 Vec4(float x, float y, float z, float w)
{
	vec4 v;
	v.x = x;
	v.y = y;
	v.z = z;
	v.w = w;
	return v;
}

vec3 Zvec3()
{
	vec3 v;
	v.x = 0.0f;
	v.y = 0.0f;
	v.z = 0.0f;
	return v;
}

vec4 Zvec4()
{
	vec4 v;
	v.x = 0.0f;
	v.y = 0.0f;
	v.z = 0.0f;
	v.w = 0.0f;
	return v;
}

vec3 Vec43(vec4 *v)
{
	return Vec3(v->x, v->y, v->z);
}

vec4 Vec34(vec3 *v)
{
	return Vec4(v->x, v->y, v->z, 1.0f);
}

mat4 Mat4(float s)
{
	mat4 m;

	m.m[0][0] = s;
	m.m[0][1] = 0.0f;
	m.m[0][2] = 0.0f;
	m.m[0][3] = 0.0f;

	m.m[1][0] = 0.0f;
	m.m[1][1] = s;
	m.m[1][2] = 0.0f;
	m.m[1][3] = 0.0f;

	m.m[2][0] = 0.0f;
	m.m[2][1] = 0.0f;
	m.m[2][2] = s;
	m.m[2][3] = 0.0f;

	m.m[3][0] = 0.0f;
	m.m[3][1] = 0.0f;
	m.m[3][2] = 0.0f;
	m.m[3][3] = s;

	return m;
}

mat4 projMat(float fov, float ar, float fr, float nr)
{
	mat4 m;

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

mat4 rotMatX(float a)
{
	mat4 m;

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

mat4 rotMatY(float a)
{
	mat4 m;

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
	m.m[3][3] = 1.0f;

	return m;
}

mat4 rotMatZ(float a)
{
	mat4 m;

	m.m[0][0] = cos(a);
	m.m[0][1] = -sin(a);
	m.m[0][2] = 0.0f;
	m.m[0][3] = 0.0f;

	m.m[1][0] = sin(a);
	m.m[1][1] = cos(a);
	m.m[1][2] = 0.0f;
	m.m[1][3] = 0.0f;

	m.m[2][0] = 0.0f;
	m.m[2][1] = 0.0f;
	m.m[2][2] = 0.0f;
	m.m[2][3] = 0.0f;

	m.m[3][0] = 0.0f;
	m.m[3][1] = 0.0f;
	m.m[3][2] = 0.0f;
	m.m[3][3] = 1.0f;

	return m;
}

vec3 vec3Add(vec3 v0, vec3 v1)
{
	v0.x += v1.x;
	v0.y += v1.y;
	v0.z += v1.z;

	return v0;
}

vec3 vec3Sub(vec3 v0, vec3 v1)
{
	v0.x -= v1.x;
	v0.y -= v1.y;
	v0.z -= v1.z;

	return v0;
}

vec3 vec3Scale(vec3 v, float s)
{
	v.x *= s;
	v.y *= s;
	v.z *= s;

	return v;
}

float vec3Len(vec3 *v)
{
	return sqrt(v->x*v->x + v->y*v->y + v->z*v->z);
}

float vec3Dot(vec3 *v0, vec3 *v1)
{
	return v0->x*v1->x + v0->y*v1->y + v0->z*v1->z;
}

vec3 vec3Cross(vec3 *v0, vec3 *v1)
{
	vec3 v;

	v.x = v0->y*v1->z - v0->z*v1->y;
	v.y = v0->z*v1->x - v0->x*v1->z;
	v.z = v0->x*v1->y - v0->y*v1->x;

	return v;
}

vec3 vec3Normalize(vec3 *v)
{
	return vec3Scale(*v, 1.0f/vec3Len(v));
}

vec4 vec4Add(vec4 v0, vec4 v1)
{
	v0.x += v1.x;
	v0.y += v1.y;
	v0.z += v1.z;
	v0.w += v1.w;

	return v0;
}

vec4 vec4Sub(vec4 v0, vec4 v1)
{
	v0.x -= v1.x;
	v0.y -= v1.y;
	v0.z -= v1.z;
	v0.w -= v1.w;

	return v0;
}

vec4 vec4Scale(vec4 v, float s)
{
	v.x *= s;
	v.y *= s;
	v.z *= s;
	v.w *= s;

	return v;
}

float vec4Len(vec4 *v)
{
	return sqrt(v->x*v->x + v->y*v->y + v->z*v->z + v->w*v->w);
}

float vec4Dot(vec4 *v0, vec4 *v1)
{
	return v0->x*v1->x + v0->y*v1->y + v0->z*v1->z + v0->w*v1->w;
}

vec4 vec4Cross(vec4 *v0, vec4 *v1)
{
	vec4 v;

	v.x = v0->y*v1->z - v0->z*v1->y;
	v.y = v0->z*v1->x - v0->x*v1->z;
	v.z = v0->x*v1->y - v0->y*v1->x;

	return v;
}

vec4 vec4Normalize(vec4 *v)
{
	return vec4Scale(*v, 1.0f/vec4Len(v));
}

mat4 translate(mat4 *m, vec3 v)
{
	mat4 r;

	mat4 tm = Mat4(1.0f);

	tm.m[0][3] = v.x; // col 2, 3 not guaranteed
	tm.m[1][3] = v.y;
	tm.m[2][3] = v.z;

	r = mat4mat4(m, &tm);

	return r;
}

vec4 mat4GetRow(mat4 *m, int r)
{
	vec4 v;
	v.x = m->m[r][0];
	v.y = m->m[r][1];
	v.z = m->m[r][2];
	v.w = m->m[r][3];
	return v;
}

vec3 mat4vec3(mat4 *m, vec3 *v)
{
	vec3 r = Zvec3();

	r.x += m->m[0][0]*v->x;
	r.x += m->m[0][1]*v->y;
	r.x += m->m[0][2]*v->z;
	r.x += m->m[0][3];

	r.y += m->m[1][0]*v->x;
	r.y += m->m[1][1]*v->y;
	r.y += m->m[1][2]*v->z;
	r.y += m->m[1][3];

	r.z += m->m[2][0]*v->x;
	r.z += m->m[2][1]*v->y;
	r.z += m->m[2][2]*v->z;
	r.z += m->m[2][3];

	return r;
}

vec4 mat4vec4(mat4 *m, vec4 *v)
{
	vec4 r = Zvec4();
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

mat4 mat4mat4(mat4 *m0, mat4 *m1)
{
	mat4 r;

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

mat4 rotateX(mat4 *m, float a)
{
	mat4 r;

	mat4 rm; // rotation matrix

	rm = rotMatX(a); // construct x rot mat

	r = mat4mat4(m, &rm); // mul

	return r;
}

mat4 rotateY(mat4 *m, float a)
{
	mat4 r;

	mat4 rm; // rotation matrix

	rm = rotMatY(a); // construct y rot mat

	r = mat4mat4(m, &rm); // mul

	return r;
}

mat4 rotateZ(mat4 *m, float a)
{
	mat4 r;

	mat4 rm; // rotation matrix

	rm = rotMatZ(a); // construct z rot mat

	r = mat4mat4(m, &rm); // mul

	return r;
}

mat4 scale(mat4 *m, float s)
{
	mat4 r;

	mat4 sm;

	sm = Mat4(s);

	r = mat4mat4(m, &sm);

	return r;
}

mat4 lookAt(vec3 v0, vec3 v1)
{
	mat4 r;
	vec3 d = vec3Sub(v1, v0);
	vec3 up = Vec3(0.0f, 1.0f, 0.0f);
	float ya = 0.0f, xa = 0.0f;

	if (d.x != 0.0f)
		ya = atan2(d.z, d.x)-1.5708f;

	xa = acos(-vec3Dot(&up, &d)/vec3Len(&d))-1.5708f;

	r = rotMatX(xa);
	r = rotateY(&r, ya);

	return r;
}
