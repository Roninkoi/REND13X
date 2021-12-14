/*
 * Vector/matrix math
 */

#ifndef VEC_H
#define VEC_H

#include <math.h>

typedef struct {
  float x;
  float y;
  float z;
  float w;
} vec4;

typedef struct {
  float x;
  float y;
  float z;
} vec3;

typedef struct {
	float x;
	float y;
} vec2;

typedef struct {
	float m[4][4];
} mat4;

vec2 Vec2(float x, float y);
vec3 Vec3(float x, float y, float z);
vec4 Vec4(float x, float y, float z, float w);

// zero vector
vec4 Zvec4();

mat4 Mat4(float s);

// projection matrix
mat4 projMat(float fov, float ar, float fr, float nr);

// construct rotation matrices, angle a
mat4 rotMatX(float a);
mat4 rotMatY(float a);
mat4 rotMatZ(float a);

// dot product
float vec4Dot(vec4 *v0, vec4 *v1);
// cross product
vec4 vec4Cross(vec4 *v0, vec4 *v1);

// vector add
vec4 vec4Add(vec4 v0, vec4 v1);
// vector subtract
vec4 vec4Sub(vec4 v0, vec4 v1);
// vector scale
vec4 vec4Scale(vec4 v, float s);
// vector length
float vec4Len(vec4 *v);

vec4 vec4Normalize(vec4 *v);

// rotate matrix by angle a
mat4 rotateX(mat4 *m, float a);
mat4 rotateY(mat4 *m, float a);
mat4 rotateZ(mat4 *m, float a);

// translate matrix by vector v
mat4 translate(mat4 *m, vec4 v);
// scale matrix by s
mat4 scale(mat4 *m, float s);

mat4 lookAt(vec4 v0, vec4 v1);

// mat4 get row
vec4 mat4GetRow(mat4 *m, int r);

// matrix multiplication, mat4 x vec4
vec4 mat4vec4(mat4 *m, vec4 *v);
// mat4 x vec4
mat4 mat4mat4(mat4 *m0, mat4 *m1);

void mat4Print(mat4* m);
void vec4Print(vec4* v);

#endif
