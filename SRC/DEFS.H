#ifndef DEFS_H
#define DEFS_H

#include <math.h>

typedef unsigned char byte;

// video mode selection (ifdef)
//#define MODE13
#define MODEX

// enable faster fills with less precision
#define FASTFILL

// viewport dimensions
#define L 0
#define R 319
#define W 320

#ifdef MODE13
#define T 8
#define B 199
#define H 200
#endif

#ifdef MODEX
#define T 12
#define B 227
#define H 240
#endif

// start of video memory
#define VSTART 0xa000
// 1/320
#define EPSILON 0.0031f

// near and far planes
#define ZNEAR 0.3f
#define ZFAR 22.0f

#undef min
#undef max
#undef abs
#define min(x, y) ((x) > (y) ? (y) : (x))
#define max(x, y) ((x) < (y) ? (y) : (x))
#define clamp(x, min, max) ((x) > (max) ? \
	(max) : ((x) < (min) ? (min) : (x)))

#define sign(x) ((x) >= 0 ? 1 : -1)
#define abs(x) ((x) < 0 ? -(x) : (x))
static int abs16(int x) {
	const int y = x >> 15;
	return (x ^ y) - y;
}
#define fabsf(x) ((float) fabs((float) (x)))
//#undef floor
#define floorf(x) (float)((int) (x))
#define round(x) floorf((x) + 0.5f)
#undef fmod
#define fmod(x, y) ((x) - floorf((x) / (y)) * (y))

#define TAU 6.2831853f
#define PI 3.1415926f
#define PI2 1.5707963f

static float sin_approx(float x)
{
	//float z = fmod(x / PI + 1.0f, 2.0f) - 1.0f;
	//return 4.0f * z * (1.0f - abs(z));
	float y = x / TAU;
	float z = y - (float) floor(y) - 0.5f;
	return 8.0f * z * (fabsf(2.0f * z) - 1.0f);
}

//#define sinf(x) (sin_approx((x)))
//#define cosf(x) (sin_approx((x)+PI2))
#define sinf(x) ((float) sin((float) (x)))
#define cosf(x) ((float) cos((float) (x)))
#define tanf(x) ((float) tan((float) (x)))
#define asinf(x) ((float) asin((float) (x)))
#define acosf(x) ((float) acos((float) (x)))
#define atan2f(y, x) ((float) atan2((float) (y), (float) (x)))

#define sqrtf(x) ((float) sqrt((float) (x)))

// random float in range [0, 1]
#define RANDF ((rand() % 10000) / (10000.0f-1.0f))

// determine visibility of geometric primitives
#define pointVis(x, y) ((x) >= L && (x) <= R && (y) >= T && (y) <= B)
#define lineVis(x0, y0, x1, y1) (\
	(x0 >= L) && (x1 >= L) && \
	(x0 <= R) && (x1 <= R) && \
	(y0 >= T) && (y1 >= T) && \
	(y0 <= B) && (y1 <= B))
#define lineNotVis(x0, y0, x1, y1) (\
	(x0 < L) && (x1 < L) || \
	(x0 > R) && (x1 > R) || \
	(y0 < T) && (y1 < T) ||\
	(y0 > B) && (y1 > B) \
)
#define hlineVis(x0, x1, y) (\
	x0 <= R && x1 >= L && y >= T && y <= B)
#define vlineVis(x, y0, y1) (\
	x <= R && x >= L && y0 <= B && y1 >= T)
#define triVis(x0, y0, x1, y1, x2, y2) (\
	(x0 >= L) && (x1 >= L) && (x2 >= L) && \
	(x0 <= R) && (x1 <= R) && (x2 <= R) && \
	(y0 >= T) && (y1 >= T) && (y2 >= T) && \
	(y0 <= B) && (y1 <= B) && (y2 <= B) \
)
#define triNotVis(x0, y0, x1, y1, x2, y2) (\
	(x0 < L) && (x1 < L) && (x2 < L) || \
	(x0 > R) && (x1 > R) && (x2 > R) || \
	(y0 < T) && (y1 < T) && (y2 < T) || \
	(y0 > B) && (y1 > B) && (y2 > B) \
)
#define triClips(x0, y0, x1, y1, x2, y2, mar) (\
	(x0 > R-mar) || (x0 < L+mar) || \
	(x1 > R-mar) || (x1 < L+mar) || \
	(x2 > R-mar) || (x2 < L+mar) || \
	(y0 > B-mar) || (y0 < T+mar) || \
	(y1 > B-mar) || (y1 < T+mar) || \
	(y2 > B-mar) || (y2 < T+mar))

// planar video mode utilities
// select planes from right x offset
#define rightp(x) (0x0f>>(x))
// select planes from left x offset
#define leftp(x) (0x0f&(0x0f<<(x)))
// combined left and right offsets
#define linep(x0, x1) (leftp(x0)&rightp(x1))
// select planes in coordinates [x0, x1]
#define linepx(x0, x1) (linep((x0)&3, 3-(x1)&3))
// select plane for one pixel coordinate x
#define pixpx(x) (1<<((x)&3))

// render buffer size
#define RBUFFERLEN 256

// second in clock ticks
#define SECOND 5013
// convert itime to second
#define TOSECOND 0.000199466f
#define SECOND0 18 //.206
#define TOSECOND0 0.054927f
#define TIMERF 275

// keyboard codes

#define wDownCode 17
#define wUpCode 145
#define aDownCode 30
#define aUpCode 158
#define sDownCode 31
#define sUpCode 159
#define dDownCode 32
#define dUpCode 160
#define rightDownCode 77
#define rightUpCode 205
#define leftDownCode 75
#define leftUpCode 203
#define upDownCode 72
#define upUpCode 200
#define downDownCode 80
#define downUpCode 208
#define rDownCode 19
#define rUpCode 147
#define fDownCode 33
#define fUpCode 161

#endif
