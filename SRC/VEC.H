/*
 * Vector math
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

vec4 Vec4(float x, float y, float z, float w);

// null vector
vec4 nvec4();

vec3 Vec3(float x, float y, float z);

vec2 Vec2(float x, float y);

mat4 Mat4(float s);

mat4 projmat(float fov, float ar, float fr, float nr);

mat4 rotmatX(float a);

mat4 rotmatY(float a);

// dot product
float v4dot(vec4 *v0, vec4 *v1);

// cross product
vec4 v4cross(vec4 *v0, vec4 *v1);

// vector addition
vec4 v4a(vec4 v0, vec4 v1);

// vector scaling
vec4 v4s(vec4 v, float s);

float v4l(vec4 v);

mat4 rotateX(mat4 *m, float a);

mat4 rotateY(mat4 *m, float a);

mat4 translate(mat4 *m, vec4 v);

// mat4 get row
vec4 m4gr(mat4 m, int r);

// matrix multiplication, mat4 x vec4
vec4 m4xv4(mat4 *m, vec4 *v);

mat4 m4xm4(mat4 *m0, mat4 *m1);

void matpr(mat4* m);
void vecpr(vec4* v);

#endif
