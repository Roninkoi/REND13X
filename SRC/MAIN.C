#include "SRC\RENDER.H"

int running = 1;

int tricount = 0;

void tridemo()
{
	float vt[3][2];

	clearscr = 0;

	vt[0][0] = RANDF * 2.0f - 1.0f;
	vt[0][1] = RANDF * 2.0f - 1.0f;

	vt[1][0] = RANDF * 2.0f - 1.0f;
	vt[1][1] = RANDF * 2.0f - 1.0f;

	vt[2][0] = RANDF * 2.0f - 1.0f;
	vt[2][1] = RANDF * 2.0f - 1.0f;

	r_drawtri(vt, (byte) (RANDF * 256.0f));

	++tricount;

	drawcount = tricount;
}

void demo(float t)
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
	for (i = 0; i < cn; ++i) {
		rm = rm0;
		//rm = scale(&rm, sin(i+t*0.1f)*0.2f + 1.0f);

		rm = translate(&rm, Vec4(sin(t/2.0f+3*i/PI)*2.0f,
			cos(t/4.0f+3*i/PI)*2.0f,
			sin(i/PI*2.0f)*4.0f + 2.5f, 1.0f));
		rm = rotateY(&rm, sin(i/PI)*i/cn*4.0f*t/4.0f);
		rm = rotateX(&rm, t/2.0f);

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

#define init() \
	r_init(); \
	hookKeys(); \
	hookTime();

#define exit() \
	r_exit(); \
	unhookKeys(); \
	unhookTime();

int main()
{
	unsigned i, n;

	float t;
	unsigned lt;

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

	float walk_spd, rot_spd;

	// projection matrix
	mat4 pm = projmat(PI*0.5f, W/H, 100.0f, 0.1f);
	// camera matrix
	mat4 cm = Mat4(1.0f);

	// camera position
	vec4 cam;

	t = 0.0f;
	running = 1;
	lt = 0;

	frames = 0;
	fps = 70;

	posx = 0.0f;
	posy = 0.0f;
	posz = -2.0f;

	roty = 0.0f;
	rotx = 0.0f;

	walk_spd = 5.6f;
	rot_spd = 1.4f;

	dt = 1.0f/(float) fps;
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
		++frames;
		t += dt;

		if (clearscr) {
			r_clear();
		}

		rs -= itime;

		cam = Vec4(-posx, -posy, -posz, 0.0f);

		cm = Mat4(1.0f);

		cm = rotateX(&cm, rotx);
		cm = rotateY(&cm, roty);

		cm = translate(&cm, cam);

		rm = m4xm4(&pm, &cm);

		demo(t*3.0f);

		wireframe = 0;
		faceculling = 1;
		zsort = 1;

		r_sort();

		r_draw();

		//tridemo();

		rs += itime;

		if (itime - lt >= SECOND) { // runs every second
			rt = (float) rs / (float) frames * TOSECOND * 1000.0f * 0.25f + rt*0.75f;
			rs = 0;
			dt = min((float) (itime - lt), 2.0f * SECOND) * TOSECOND / (float) frames;
			lt = itime;
			fps = frames;
			frames = 0;
		}

		printf("fps: %u, key: %i, rt: %.1f, dc: %u   \r",
				 fps, keycode, rt, drawcount);

		r_sync();

		getInput();

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
			roty -= rot_spd*dt;
		}
		if (keydown[leftDownCode]) {
			roty += rot_spd*dt;
		}
		if (keydown[upDownCode]) {
			rotx += rot_spd*dt;
		}
		if (keydown[downDownCode]) {
			rotx -= rot_spd*dt;
		}
		if (keydown[rDownCode]) {
			posy += walk_spd*dt;
		}
		if (keydown[fDownCode]) {
			posy -= walk_spd*dt;
		}
	}

	exit();

	return 0;
}
