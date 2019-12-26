#include "SRC\RENDER.H"

unsigned char keycode = 0;
unsigned char keycodeBuffer[256];
unsigned char keycodeTail = 0;

int running = 1;

int key;
int keydown[256];

void interrupt (*oldkb) ();

void interrupt (*oldtime) ();

// get code from keyboard
void interrupt getKeys()
{
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

void getInput()
{
	int i;
	for (i = 0; i < 256; ++i) {
		key = keycodeBuffer[i];
		keycodeBuffer[i] = 0;

		if (key == 1) {
			running = 0; // halt program
		}
			// key press and release
		if (key == 17) {
			keydown[(int)'w'] = 1;
		}
		if (key == 145) {
			keydown[(int)'w'] = 0;
		}
		if (key == 30) {
			keydown[(int)'a'] = 1;
		}
		if (key == 158) {
			keydown[(int)'a'] = 0;
		}
		if (key == 31) {
			keydown[(int)'s'] = 1;
		}
		if (key == 159) {
			keydown[(int)'s'] = 0;
		}
		if (key == 32) {
			keydown[(int)'d'] = 1;
		}
		if (key == 160) {
			keydown[(int)'d'] = 0;
		}
		if (key == 77) {
			keydown[(int)'0'] = 1; // right
		}
		if (key == 205) {
			keydown[(int)'0'] = 0;
		}
		if (key == 75) {
			keydown[(int)'1'] = 1; // left
		}
		if (key == 203) {
			keydown[(int)'1'] = 0;
		}
		if (key == 72) {
			keydown[(int)'2'] = 1; // up
		}
		if (key == 200) {
			keydown[(int)'2'] = 0;
		}
		if (key == 80) {
			keydown[(int)'3'] = 1; // down
		}
		if (key == 208) {
			keydown[(int)'3'] = 0;
		}
		if (key == 19) {
			keydown[(int)'r'] = 1;
		}
		if (key == 147) {
			keydown[(int)'r'] = 0;
		}
		if (key == 33) {
			keydown[(int)'f'] = 1;
		}
		if (key == 161) {
			keydown[(int)'f'] = 0;
		}
	}
}

void waitRetrace()
{
	while (inportb(0x3da) & 8); // wait around
	while (!(inportb(0x3da) & 8));
	//r_clear();
	//r_scr();
}

void addf(
float v0x, float v0y, float v0z,
float v1x, float v1y, float v1z,
float v2x, float v2y, float v2z, BYTE c)
{
	vec4 v0 = Vec4(v0x, v0y, v0z, 1.0f);
	vec4 v1 = Vec4(v1x, v1y, v1z, 1.0f);
	vec4 v2 = Vec4(v2x, v2y, v2z, 1.0f);

	r_add(&v0, &v1, &v2, c);
}

int main()
{
	unsigned i;

	long int t;
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
	//mat4 pm = projmat(PI*0.5f, 320.0f/200.0f, 100.0f, 0.1f);
	// camera matrix
	mat4 cm = Mat4(1.0);

	vec4 cube00 = Vec4(-0.5f, -0.5f, -0.5f, 1.0f);
	vec4 cube01 = Vec4(0.5f, -0.5f, -0.5f, 1.0f);
	vec4 cube02 = Vec4(0.5f, 0.5f, -0.5f, 1.0f);
	vec4 cube03 = Vec4(-0.5f, 0.5f, -0.5f, 1.0f);

	vec4 cube10 = Vec4(-0.5f, -0.5f, 0.5f, 1.0f);
	vec4 cube11 = Vec4(0.5f, -0.5f, 0.5f, 1.0f);
	vec4 cube12 = Vec4(0.5f, 0.5f, 0.5f, 1.0f);
	vec4 cube13 = Vec4(-0.5f, 0.5f, 0.5f, 1.0f);

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
	posz = -4.0f;

	roty = 0.0f;
	rotx = 0.0f;

	key = 0;

	dt = 1.0f;
	rt = 1.0f;
	rs = 0;

	for (i = 0; i < 256; ++i) {
		keydown[i] = 0;
	}

	r_init();

	oldkb = getvect(9);
	setvect(9, getKeys); // hook up kb

	oldtime = getvect(0x1c);
	setvect(0x1c, getTime);

	while (running) {
		++t;
		++frames;

		waitRetrace();
		//r_clear();
		r_scr();

		rs -= itime;

		cam = Vec4(-posx, -posy, -posz, 0.0f);

		cm = Mat4(1.0f);
		cm = rotateX(&cm, rotx);
		cm = rotateY(&cm, roty);
		cm = translate(&cm, cam);

		for (i = 0; i < 1; ++i) {
		rm = cm;

		//rm = scale(&rm, 1.0f);

		rm = translate(&rm, Vec4(i, 0.0f, -2.0f, 0.0f));

		rm = rotateY(&rm, t*0.03f/7.0f);
		rm = rotateX(&rm, t*0.02f/7.0f);

		r_add(&cube00, &cube01, &cube02, 48);
		r_add(&cube00, &cube02, &cube03, 48);

		r_add(&cube10, &cube12, &cube11, 38);
		r_add(&cube10, &cube13, &cube12, 38);

		r_add(&cube00, &cube10, &cube11, 45);
		r_add(&cube00, &cube11, &cube01, 45);

		r_add(&cube01, &cube11, &cube12, 35);
		r_add(&cube01, &cube12, &cube02, 35);

		r_add(&cube02, &cube12, &cube13, 55);
		r_add(&cube02, &cube13, &cube03, 55);

		r_add(&cube03, &cube13, &cube10, 42);
		r_add(&cube03, &cube10, &cube00, 42);
		}
		rm = Mat4(1.0f);

		/*addf(
		0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		42);*/
		/*addf(
		-1.0f, -1.0f*0.0f + sin(t*0.01f), 1.0f,
		1.0f, -1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		42);*/  /*
		addf(
		-1.0f, -1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		42);*/

		wireframe = 0;
		faceculling = 1;
		zsort = 1;

		r_sort();

		r_draw();

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

		printf("fps: %u, key: %i, rt: %.1f, rn: %i  \r",
		 fps, keycode, rt, drawcount);

		getInput();

		walk_spd = 0.04f;

		// game input
		if (keydown[(int)'w']) {
			posz += cos(roty)*walk_spd*dt;
			posx += -sin(roty)*walk_spd*dt;
		}
		if (keydown[(int)'a']) {
			posx -= cos(roty)*walk_spd*dt;
			posz -= sin(roty)*walk_spd*dt;
		}
		if (keydown[(int)'s']) {
			posz -= cos(roty)*walk_spd*dt;
			posx -= -sin(roty)*walk_spd*dt;
		}
		if (keydown[(int)'d']) {
			posx += cos(roty)*walk_spd*dt;
			posz += sin(roty)*walk_spd*dt;
		}
		if (keydown[(int)'0']) {
			roty -= 0.02f*dt;
		}
		if (keydown[(int)'1']) {
			roty += 0.02f*dt;
		}
		if (keydown[(int)'2']) {
			rotx += 0.02f*dt;
		}
		if (keydown[(int)'3']) {
			rotx -= 0.02f*dt;
		}
		if (keydown[(int)'r']) {
			posy += 0.04f*dt;
		}
		if (keydown[(int)'f']) {
			posy -= 0.04f*dt;
		}
	}

	r_exit();

	setvect(9, oldkb);
	setvect(0x1c, oldtime);

	return 0;
}
