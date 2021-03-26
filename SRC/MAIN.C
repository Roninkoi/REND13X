#include "SRC\RENDER.H"

#define RANDF ((rand() % 10000) / 10000.0f)

BYTE keycode = 0;
BYTE keycodeBuffer[256];
BYTE keycodeTail = 0;

int running = 1;

int key;
int keydown[256];

void interrupt (*oldkb) ();

void interrupt (*oldtime) ();

void interrupt getTime()
{
	itime += 1;
	redraw = 1;
}

unsigned now() {
	return (unsigned) time(NULL);
}

void interrupt getKeys()
{
	// get code from keyboard
	asm {
		cli

		in al, 0x060	// read code
		mov keycode, al
		in al, 0x061	// status
		mov bl, al
		or al, 0x080
		out 0x061, al
		mov al, bl
		out 0x061, al

		mov al, 0x020	// reset
		out 0x020, al

		sti
	}

	*(keycodeBuffer + keycodeTail) = keycode;

	++keycodeTail;
}

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

void getInput()
{
	int i;
	for (i = 0; i < 256; ++i) {
		key = keycodeBuffer[i];
		keycodeBuffer[i] = 0;

		// key press and release
		switch (key) {
			case 1:
				running = 0; // halt program
				break;
			case wDownCode:
				keydown[wDownCode] = 1;
				break;
			case wUpCode:
				keydown[wDownCode] = 0;
				break;
			case aDownCode:
				keydown[aDownCode] = 1;
				break;
			case aUpCode:
				keydown[aDownCode] = 0;
				break;
			case sDownCode:
				keydown[sDownCode] = 1;
				break;
			case sUpCode:
				keydown[sDownCode] = 0;
				break;
			case dDownCode:
				keydown[dDownCode] = 1;
				break;
			case dUpCode:
				keydown[dDownCode] = 0;
				break;
			case rightDownCode:
				keydown[rightDownCode] = 1;
				break;
			case rightUpCode:
				keydown[rightDownCode] = 0;
				break;
			case leftDownCode:
				keydown[leftDownCode] = 1;
				break;
			case leftUpCode:
				keydown[leftDownCode] = 0;
				break;
			case upDownCode:
				keydown[upDownCode] = 1;
				break;
			case upUpCode:
				keydown[upDownCode] = 0;
				break;
			case downDownCode:
				keydown[downDownCode] = 1;
				break;
			case downUpCode:
				keydown[downDownCode] = 0;
				break;
			case rDownCode:
				keydown[rDownCode] = 1;
				break;
			case rUpCode:
				keydown[rDownCode] = 0;
				break;
			case fDownCode:
				keydown[fDownCode] = 1;
				break;
			case fUpCode:
				keydown[fDownCode] = 0;
				break;
		}
	}
}

int tricount = 0;

void tridemo(long t)
{
	float vt[3][2];

	clearscr = 0;

	vt[0][0] = RANDF * 2.0f - 1.0f;
	vt[0][1] = RANDF * 2.0f - 1.0f;

	vt[1][0] = RANDF * 2.0f - 1.0f;
	vt[1][1] = RANDF * 2.0f - 1.0f;

	vt[2][0] = RANDF * 2.0f - 1.0f;
	vt[2][1] = RANDF * 2.0f - 1.0f;

	r_drawtri(vt, (BYTE) (RANDF * 256.0f));

	++tricount;

	drawcount = tricount;
}

void demo(long t)
{
	int i;
	float ln;
	float lf;
	int cn;

	vec4 cube00 = Vec4(-0.5f, -0.5f, -0.5f, 1.0f);
	vec4 cube01 = Vec4(0.5f, -0.5f, -0.5f, 1.0f);
	vec4 cube02 = Vec4(0.5f, 0.5f, -0.5f, 1.0f);
	vec4 cube03 = Vec4(-0.5f, 0.5f, -0.5f, 1.0f);

	vec4 cube10 = Vec4(-0.5f, -0.5f, 0.5f, 1.0f);
	vec4 cube11 = Vec4(0.5f, -0.5f, 0.5f, 1.0f);
	vec4 cube12 = Vec4(0.5f, 0.5f, 0.5f, 1.0f);
	vec4 cube13 = Vec4(-0.5f, 0.5f, 0.5f, 1.0f);

	mat4 rm0 = rm;

	cn = 3;
	for (i = 0; i < cn && 1; ++i) {
		rm = rm0;
		//rm = scale(&rm, sin(i+t*0.1f)*0.2f + 1.0f);

		rm = translate(&rm, Vec4(sin(t*0.03f/2.0f+3*i/PI)*2.0f,
			cos(t*0.03f/4.0f+3*i/PI)*2.0f,
			sin(i/PI*2.0f)*4.0f + 2.5f, 1.0f));
		rm = rotateY(&rm, sin(i/PI)*i/cn*4.0f*t*0.03f/4.0f);
		rm = rotateX(&rm, t*0.03f/2.0f);

/*		cube00.x += sin(t*0.02f+i/cn) * 0.1f;
		cube10.x -= cos(t*0.06f+i/cn) * 0.1f;
		cube01.x += sin(t*0.08f+i/cn) * 0.1f;
		cube11.x -= cos(t*0.02f+i/cn) * 0.1f;
		cube02.x -= sin(t*0.05f+i/cn) * 0.1f;
		cube03.x -= sin(t*0.05f+i/cn) * 0.1f;
		cube13.x += sin(t*0.02f+i/cn) * 0.1f;*/

		r_add(&cube00, &cube01, &cube02, 48);
		r_add(&cube00, &cube02, &cube03, 50);

		r_add(&cube10, &cube12, &cube11, 38);
		r_add(&cube10, &cube13, &cube12, 40);

		r_add(&cube00, &cube10, &cube11, 45);
		r_add(&cube00, &cube11, &cube01, 47);

		r_add(&cube01, &cube11, &cube12, 35);
		r_add(&cube01, &cube12, &cube02, 37);

		r_add(&cube02, &cube12, &cube13, 55);
		r_add(&cube02, &cube13, &cube03, 57);

		r_add(&cube03, &cube13, &cube10, 42);
		r_add(&cube03, &cube10, &cube00, 44);
	}
}

// these have to be macros?
#define init() \
	r_init(); \
	oldkb = getvect(9); \
	setvect(9, getKeys); \
	oldtime = getvect(0x1c); \
	setvect(0x1c, getTime);

#define exit() \
	r_exit(); \
	setvect(9, oldkb); \
	setvect(0x1c, oldtime);

int main()
{
	unsigned i;

	long t;
	unsigned lt;
	unsigned nt;

	unsigned frames;
	unsigned fps;

	float posx;
	float posy;
	float posz;

	float rotx;
	float roty;

	float dt;
	float rt;
	int rs;

	float walk_spd;

	// projection matrix
	mat4 pm = projmat(PI*0.5f, W/H, 100.0f, 0.1f);
	// camera matrix
	mat4 cm = Mat4(1.0f);

	// camera position
	vec4 cam;

	t = 0;
	running = 1;
	lt = 0;
	nt = 0;

	frames = 1000;
	fps = 1000;

	posx = 0.0f;
	posy = 0.0f;
	posz = -2.0f;

	roty = 0.0f;
	rotx = 0.0f;

	key = 0;

	dt = 1.0f;
	rt = 1.0f;
	rs = 0;

	for (i = 0; i < 256; ++i) {
		keydown[i] = 0;
	}

	// initialize renderer, hook up keyboard
	init();

	clearscr = 1;
	clearcol = 3;

	while (running) {
		++t;
		++frames;

		r_waitRetrace();

		if (clearscr) {
			r_scr(clearcol);
		}

		rs -= itime;

		cam = Vec4(-posx, -posy, -posz, 0.0f);

		cm = Mat4(1.0f);

		cm = rotateX(&cm, rotx);
		cm = rotateY(&cm, roty);

		cm = translate(&cm, cam);

		rm = m4xm4(&pm, &cm);

		demo(t);

		wireframe = 0;
		faceculling = 1;
		zsort = 1;

		r_sort();

		r_draw();

		//tridemo(t);

		rs += itime;

		nt = itime;
		if (nt - lt >= 16) { // runs every second
			rt = ((rs/16.0f)/(float)frames)*1000.0f*0.25f + rt*0.75f;
			rs = 0;
			lt = nt;
			fps = frames;
			frames = 0;
			dt = 60.0f/((float)fps);
		}

		printf("fps: %u, key: %i, rt: %.1f, dc: %u  \r",
				 fps, keycode, rt, drawcount);

		getInput();

		walk_spd = 0.04f;

		// game input
		if (keydown[wDownCode]) {
			posz += cos(roty)*walk_spd*dt;
			posx += -sin(roty)*walk_spd*dt;
		}
		if (keydown[aDownCode]) {
			posx -= cos(roty)*walk_spd*dt;
			posz -= sin(roty)*walk_spd*dt;
		}
		if (keydown[sDownCode]) {
			posz -= cos(roty)*walk_spd*dt;
			posx -= -sin(roty)*walk_spd*dt;
		}
		if (keydown[dDownCode]) {
			posx += cos(roty)*walk_spd*dt;
			posz += sin(roty)*walk_spd*dt;
		}
		if (keydown[rightDownCode]) {
			roty -= 0.02f*dt;
		}
		if (keydown[leftDownCode]) {
			roty += 0.02f*dt;
		}
		if (keydown[upDownCode]) {
			rotx += 0.02f*dt;
		}
		if (keydown[downDownCode]) {
			rotx -= 0.02f*dt;
		}
		if (keydown[rDownCode]) {
			posy += 0.04f*dt;
		}
		if (keydown[fDownCode]) {
			posy -= 0.04f*dt;
		}
	}

	exit();

	return 0;
}
