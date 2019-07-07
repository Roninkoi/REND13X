#include "SRC\RENDER.H"

int redraw = 0;
int itime = 0;
char vmode = 0;

int pg = 0;

void interrupt getTime()
{
	asm cli

	asm {
		sti
	}
	itime += 1;
	redraw = 1;
}

unsigned now() {
	return (unsigned)time(NULL);
}

void r_init()
{
	char p;
	p = pg;

	asm {
		mov ah, 0x0f
		int 0x10
		mov vmode, al // get current video mode

		mov ah, 0
		mov bh, p
		mov al, 0x13 // set 13
		int 0x10

		mov ah, 0x48
		mov bx, 0x10 // 64k
		int 0x21
	}
}

void r_exit()
{
	asm {
		mov ah, 0
		mov bh, 0
		mov bx, 0
		mov al, vmode // return original video mode
		int 0x10
	}
}

void r_flip()
{
	char p;
	p = pg;

	asm {
		mov ah, 05h
		mov al, p
		int 10h
	}
}

void r_drawpixel(int x, int y, unsigned char c)
{
	//y *= 320;
	y = (y + (y << 2)) << 6;
	x += y;
	asm {
		mov ax, 0xa000
		mov es, ax
		mov di, x
		mov dl, c
		mov [es:di], dl
	}
}

void r_swap()
{
	asm {
		mov ax, 0xa0a0
		mov bx, 0xd0a0
		mov es, ax
		mov ds, bx
		mov di, 0
		mov dl, 2
	}
	draw:
	asm {
		mov dl, [ds:di]
		mov [es:di], dl
		add di, 1
		cmp di, 0xf960
		jb draw
	}
}

void r_drawrect(int x, int y, int w, int h, unsigned char c)
{
	h += y;
	w += x;

	x = clamp(x, 0, 320);
	y = clamp(y, 0, 200);
	w = clamp(w, 0, 320);
	h = clamp(h, 0, 200);

	if (w <= x || h <= y) {
		return;
	}

	//h *= 320;
	h = (h + (h << 2)) << 6;
	//y *= 320;
	y = (y + (y << 2)) << 6;

	asm {
		push bp
		mov ax, 0xa000
		mov es, ax

		mov dl, c

		mov si, x
		mov ax, si
		mov bx, y
		mov cx, w
		mov bp, h
	}
	draw:
	asm {
		mov di, ax
		add di, bx
		mov [es:di], dl
		inc ax
		cmp ax, cx
		jb draw
		add bx, 320
		mov ax, si
		cmp bx, bp
		jb draw
	}
	asm pop bp
}

// line draw, pretty slow, implement in asm
void r_drawline(float (*v0)[2], float (*v1)[2], unsigned char c)
{
	int x;
	int y;
	int vx0 = (*v0)[0];
	int vy0 = (*v0)[1];
	float dx;
	float dy;
	float k;
	int i;
	int s;
	float kx;
	float ky;
	float diff;

	dx = (*v1)[0] - vx0;
	dy = (*v1)[1] - vy0;

	k = dy/dx;

	if (fabs(k) <= 1) {
		s = sign(dx);
		ky = k;
		kx = 1.0f;
		diff = fabs(dx);
	}
	else {
		s = sign(dy);
		kx = 1.0f/k;
		ky = 1.0f;
		diff = fabs(dy);
	}

	for (i = 0; i <= diff; i += 1) {
		x = vx0 + (float)i*s*kx;
		y = vy0 + (float)i*s*ky;
		//r_drawpixel(x, y, c);
		y = (y + (y << 2)) << 6;
		x += y;
		asm {
			mov ax, 0xa000
			mov es, ax
			mov di, x
			mov dl, c
			mov [es:di], dl
		}
	}
}

void r_drawlinef(float x0, float y0, float x1, float y1, unsigned char c)
{
	float v0[2];
	float v1[2];

	x0 = clamp(x0, 0, 320);
	x1 = clamp(x1, 0, 320);
	y0 = clamp(y0, 0, 200);
	y1 = clamp(y1, 0, 200);

	v0[0] = x0;
	v0[1] = y0;

	v1[0] = x1;
	v1[1] = y1;

	r_drawline(&v0, &v1, c);
}

// horizontal line draw with x sort, usually fast
void r_drawlineh(int x0, int x1, int y, unsigned char c)
{
	int to;
	if (x0 > x1) {
		to = x0;
		x0 = x1;
		x1 = to;
	}

	/*y = (y + (y << 2)) << 6;
	x0 += y;
	x1 += y;*/

	asm {
		mov ax, 0xa000
		mov es, ax
		/*mov ax, x1
		mov dl, c
		mov di, x0*/
		mov dx, y
		mov ax, dx
		shl ax, 2
		add dx, ax
		shl dx, 6

		mov ax, x1
		mov di, x0
		add ax, dx
		add di, dx
		
		mov dl, c
	}
	draw:
	asm {
		mov [es:di], dl
		inc di
		cmp di, ax
		jb draw
	}
}

/*
 * triangle draw coordinate system
 * same as opengl with origin [0.0f, 0.0f]
 * screen range [-1.0f, 1.0f], [-1.0f, 1.0f]
*/
void r_drawtri(float (*v)[3][2], unsigned char c)
{
	float to;
	int i = 0;
	int cl = 0;
	int xl0;
	int xl1;
	int yl;

	float k0;
	float k1;
	float k2;

	float dx1;
	float dx2;

	float dy1;
	float dy2;

	float x0;
	float y0;

	float x1;
	float y1;

	float x2;
	float y2;

	// transform from gl to pix
	x0 = ((*v)[0][0]+1.0f)*160.0f;
	y0 = (-(*v)[0][1]+1.0f)*100.0f;

	x1 = ((*v)[1][0]+1.0f)*160.0f;
	y1 = (-(*v)[1][1]+1.0f)*100.0f;

	x2 = ((*v)[2][0]+1.0f)*160.0f;
	y2 = (-(*v)[2][1]+1.0f)*100.0f;

	// sort vertices
	if (y0 > y2) {
		to = y0;
		y0 = y2;
		y2 = to;
		to = x0;
		x0 = x2;
		x2 = to;
	}
	if (y1 > y2) {
		to = y1;
		y1 = y2;
		y2 = to;
		to = x1;
		x1 = x2;
		x2 = to;
	}
	if (y0 > y1) {
		to = y0;
		y0 = y1;
		y1 = to;
		to = x0;
		x0 = x1;
		x1 = to;
	}

	// first - last
	dx1 = x2 - x0;
	dy1 = y2 - y0;
	k0 = dx1/dy1;
	if (dy1 == 0) k0 = 0;

	// first - mid
	dx1 = x1 - x0;
	dy1 = y1 - y0;
	k1 = dx1/dy1;
	if (dy1 == 0) k1 = 0;

	// mid - last
	dx2 = x2 - x1;
	dy2 = y2 - y1;
	k2 = dx2/dy2;
	if (dy2 == 0) k2 = 0;

	dy1 = fabs(dy1);
	dy2 = fabs(dy2);

	y2 -= 1.0f; // float offs

	x1 = x0;
	to = k0;
	if (k0 > k1) {
		k0 = k1;
		k1 = to;
	}

	// top
	for (i = 0; i < dy1 - 1.0f; i += 1) {
		x0 += k0;
		x1 += k1;
		y0 += 1;

		xl0 = (int)x0;
		xl1 = (int)x1;
		yl = (int)y0;

		// clipping
		cl = xl1 < 0 || yl < 8;
		cl = cl || xl0 > 319 || yl > 199;
		if (cl)
			continue;
		xl0 = clamp(xl0, 0, 320);
		xl1 = clamp(xl1, 0, 320);

		asm {
			mov ax, 0xa000
			mov es, ax
			mov dx, yl
			mov ax, dx
			shl ax, 2
			add dx, ax
			shl dx, 6

			mov ax, xl1
			mov di, xl0
			add ax, dx
			add di, dx

			mov dl, c
		}
		draw1:
		asm {
			mov [es:di], dl
			inc di
			cmp di, ax
			jb draw1
		}
	}

	if (fabs(k2) <= 0.0031f)
		return;

	k0 = to;
	if (k0 < k2) {
			to = k0;
			k0 = k2;
			k2 = to;
	}
	x1 = x2 - k0*(dy2+1.0f);
	x2 = x2 - k2*(dy2+1.0f);
	y2 = y2 - dy2;

	// bottom
	for (i = 0; i < dy2-0.5f; i += 1) {
		x1 += k0;
		x2 += k2;
		y2 += 1;

		xl0 = (int)x1;
		xl1 = (int)x2;
		yl = (int)y2;

		// clipping
		cl = xl1 < 0 || yl < 8;
		cl = cl || xl0 > 319 || yl > 199;
		if (cl)
			continue;
		xl0 = clamp(xl0, 0, 320);
		xl1 = clamp(xl1, 0, 320);

		asm {
			mov ax, 0xa000
			mov es, ax
			mov dx, yl
			mov ax, dx
			shl ax, 2
			add dx, ax
			shl dx, 6

			mov ax, xl1
			mov di, xl0
			add ax, dx
			add di, dx

			mov dl, c
		}
		draw2:
		asm {
			mov [es:di], dl
			inc di
			cmp di, ax
			jb draw2
		}
	}
}


void r_drawtri3d(vec4* v0, vec4* v1, vec4* v2, unsigned char c)
{
	float to;
	int i = 0;
	int cl = 0;
	int xl0;
	int xl1;
	int yl;

	int isz = 0;

	float k0;
	float k1;
	float k2;

	float dx1;
	float dx2;

	float dy1;
	float dy2;

	float x0;
	float y0;
	float z0;

	float x1;
	float y1;
	float z1;

	float x2;
	float y2;
	float z2;

	int zp = 0;
	int za = 0;
	float zd = 0.0f;
	float zdh = 0.0f;

	char dc = c;

	vec4 fv0;
	vec4 fv1;

	zdh = (z0 + z1 + z2) / ZFAR;

	if (c < 32 || 1) {
		dc = c;
	}
	else if (zdh <= 2.5f) {
		dc = 72 * ((int)(zdh) % 3) + (int)c;
	}
	else {
		dc = 17;
	}

	// face culling
/*	fv0 = v4s(*v0, -1.0f);
	fv0 = v4a(*v0, *v1);
	fv1 = v4s(*v0, -1.0f);
	fv1 = v4a(*v0, *v2);
	fv1 = v4cross(&fv0, &fv1);
	if (fv1.z < 0)
		return;                 */

	z0 = v0->z;
	z1 = v1->z;
	z2 = v2->z;

	if (z0 <= ZNEAR || z1 <= ZNEAR || z2 <= ZNEAR)
		return;

	if (z0 >= ZFAR && z1 >= ZFAR && z2 >= ZFAR)
		return;

	// projection
	x0 = v0->x;
	x0 /= z0;
	y0 = v0->y;
	y0 /= z0;

	x1 = v1->x;
	x1 /= z1;
	y1 = v1->y;
	y1 /= z1;

	x2 = v2->x;
	x2 /= z2;
	y2 = v2->y;
	y2 /= z2;

//	if ((x1-x0)*(y2-y0) - (y1-y0)*(x2-x0) > 0.0f)
//		return;

	// transform from gl to pix
	x0 = (x0+1.0f)*160.0f;
	y0 = (-y0+1.0f)*100.0f;

	x1 = (x1+1.0f)*160.0f;
	y1 = (-y1+1.0f)*100.0f;

	x2 = (x2+1.0f)*160.0f;
	y2 = (-y2+1.0f)*100.0f;

	// bounds
	if (x0 > 320 && x1 > 320 && x2 > 320)
		return;
	if (x0 < 0 && x1 < 0 && x2 < 0)
		return;
	if (y0 > 200 && y1 > 200 && y2 > 200)
		return;
	if (y0 < 0 && y1 < 0 && y2 < 0)
		return;

	// sort vertices
	if (y0 > y2) {
		to = y0;
		y0 = y2;
		y2 = to;
		to = x0;
		x0 = x2;
		x2 = to;
	}
	if (y1 > y2) {
		to = y1;
		y1 = y2;
		y2 = to;
		to = x1;
		x1 = x2;
		x2 = to;
	}
	if (y0 > y1) {
		to = y0;
		y0 = y1;
		y1 = to;
		to = x0;
		x0 = x1;
		x1 = to;
	}

	// first - last
	dx1 = x2 - x0;
	dy1 = y2 - y0;
	k0 = dx1/dy1;
	if (dy1 == 0) k0 = 0;

	// first - mid
	dx1 = x1 - x0;
	dy1 = y1 - y0;
	k1 = dx1/dy1;
	if (dy1 == 0) k1 = 0;

	// mid - last
	dx2 = x2 - x1;
	dy2 = y2 - y1;
	k2 = dx2/dy2;
	if (dy2 == 0) k2 = 0;

	dy1 = fabs(dy1);
	dy2 = fabs(dy2);

	y2 -= 1.0f; // float offs

	x1 = x0;
	to = k0;
	if (k0 > k1) {
		k0 = k1;
		k1 = to;
	}

	// top
	for (i = 0; i < dy1 - 1.0f; i += 1) {
		x0 += k0;
		x1 += k1;
		y0 += 1;

		xl0 = (int)x0;
		xl1 = (int)x1;
		yl = (int)y0;

/*		zdh = (dy1 - y1) / dy1; // 0 -> 1
		zdh = zdh * z1 + (1.0f - zdh) * z0;
		zdh = zdh/ZFAR;
	*/
		// view clipping
		cl = xl1 < 0 || yl < 8;
		cl = cl || xl0 > 319 || yl > 199;
		if (cl)
			continue;
		xl0 = clamp(xl0, 0, 320);
		xl1 = clamp(xl1, 0, 320);
		asm {
			mov ax, 0xa000
			mov es, ax
			mov dx, yl
			mov ax, dx
			shl ax, 2
			add dx, ax
			shl dx, 6

			mov bx, xl1
			mov di, xl0
			add bx, dx
			add di, dx

			mov dl, dc
		}
		draw1:
		asm {
			mov [es:di], dl
			add di, 1
			mov zp, di
			cmp di, bx
			jb draw1
		}
	}

	if (fabs(k2) <= 0.0031f)
		return;

	k0 = to;
	if (k0 < k2) {
			to = k0;
			k0 = k2;
			k2 = to;
	}
	x1 = x2 - k0*(dy2+1.0f);
	x2 = x2 - k2*(dy2+1.0f);
	y2 = y2 - dy2;

	// bottom
	for (i = 0; i < dy2-0.5f; i += 1) {
		x1 += k0;
		x2 += k2;
		y2 += 1;

		xl0 = (int)x1;
		xl1 = (int)x2;
		yl = (int)y2;

/*		zdh = (dy2 - y2) / dy2; // 0 -> 1
		zdh = zdh * z2 + (1.0f - zdh) * z1;
		zdh = zdh/ZFAR;*/

		// view clipping
		cl = xl1 < 0 || yl < 8;
		cl = cl || xl0 > 319 || yl > 199;
		if (cl)
			continue;
		xl0 = clamp(xl0, 0, 320);
		xl1 = clamp(xl1, 0, 320);

		asm {
			mov ax, 0xa000
			mov es, ax
			mov dx, yl
			mov ax, dx
			shl ax, 2
			add dx, ax
			shl dx, 6 // * 320

			mov si, xl1
			mov di, xl0
			add si, dx // calculate memory location
			add di, dx
			mov za, si

			mov dl, dc
		}
		draw2: // ax reg used
		asm {
			mov [es:di], dl
			add di, 1
			mov zp, di
			cmp di, si
			jb draw2
		}
	}
}

void r_fill()
{
	asm {
		mov ax, 0xa000
		mov es, ax
		mov dl, 2
		mov di, 0xfa00
	}
	draw:
	asm {
		sub di, 1
		mov [es:di], dl
		cmp di, 0
		ja draw
	}
}

void r_clear()
{
	/*asm {
		mov ax, 0xa0a0 // +16 pix
		mov es, ax
		mov dl, c
		xor di, di
	}
	draw:
	asm {
		mov [es:di], dl
		add di, 1
		cmp di, 0xfa00
		jb draw
	}*/
	asm {
		mov ax, 0xa0a0
		mov es, ax
		xor di, di
		mov cx, 0xf960
		mov ax, 0
		rep stosw
	}
}

void r_scr()
{
	asm {
		mov ah, 0x6
		xor al, al
		mov bh, 0x3
		mov cx, 0x0100
		mov dx, 0x182a
		int 10h
	}
}

