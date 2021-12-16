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
vec3 Zvec3();
vec4 Zvec4();

// vector conversion
vec3 Vec43(vec4 *v);
vec4 Vec34(vec3 *v);

mat4 Mat4(float s);

// projection matrix
mat4 projMat(float fov, float ar, float fr, float nr);

// construct rotation matrices, angle a
mat4 rotMatX(float a);
mat4 rotMatY(float a);
mat4 rotMatZ(float a);

// vector add
vec3 vec3Add(vec3 v0, vec3 v1);
// vector subtract
vec3 vec3Sub(vec3 v0, vec3 v1);
// vector scale
vec3 vec3Scale(vec3 v, float s);
// vector length
float vec3Len(vec3 *v);
// make unit vector
vec3 vec3Normalize(vec3 *v);
// dot product
float vec3Dot(vec3 *v0, vec3 *v1);
// cross product
vec3 vec3Cross(vec3 *v0, vec3 *v1);

// vector add
vec4 vec4Add(vec4 v0, vec4 v1);
// vector subtract
vec4 vec4Sub(vec4 v0, vec4 v1);
// vector scale
vec4 vec4Scale(vec4 v, float s);
// vector length
float vec4Len(vec4 *v);
// make unit vector
vec4 vec4Normalize(vec4 *v);
// dot product
float vec4Dot(vec4 *v0, vec4 *v1);
// cross product
vec4 vec4Cross(vec4 *v0, vec4 *v1);

// rotate matrix by angle a
mat4 rotateX(mat4 *m, float a);
mat4 rotateY(mat4 *m, float a);
mat4 rotateZ(mat4 *m, float a);

// translate matrix by vector v
mat4 translate(mat4 *m, vec3 v);
// scale matrix by s
mat4 scale(mat4 *m, float s);

mat4 lookAt(vec3 v0, vec3 v1);

// mat4 get row
vec4 mat4GetRow(mat4 *m, int r);

// matrix multiplication, mat4 x vec4
vec4 mat4vec4(mat4 *m, vec4 *v);
// matrix multiplication, mat4 x vec3
vec3 mat4vec3(mat4 *m, vec3 *v);
// mat4 x vec4
mat4 mat4mat4(mat4 *m0, mat4 *m1);

void vec3Print(vec3 *v);
void vec4Print(vec4 *v);
void mat4Print(mat4 *m);

#endif
