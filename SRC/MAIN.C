#include "SRC\RENDER.H"

int running = 1;

int tricount = 0;

void tridemo()
{
	int i;
	float vt[6];
#define TRIDEMOA 1.0f

	clearscr = 0;
	doublebuffer = 0;

	//for (i = 0; i < 1; ++i) {
		vt[0] = TRIDEMOA * (RANDF * 2.0f - 1.0f);
		vt[1] = TRIDEMOA * (RANDF * 2.0f - 1.0f);

		vt[2] = TRIDEMOA * (RANDF * 2.0f - 1.0f);
		vt[3] = TRIDEMOA * (RANDF * 2.0f - 1.0f);

		vt[4] = TRIDEMOA * (RANDF * 2.0f - 1.0f);
		vt[5] = TRIDEMOA * (RANDF * 2.0f - 1.0f);

		r_drawtri(vt, (byte) (RANDF * 256.0f));

		++tricount;
	//}

	drawcount = tricount;
}

void linedemo()
{
	unsigned i;
	float a;
	unsigned n = 62832;
	for (i = 0; i < n; ++i) {
		a = (float) i / (float) n;
		r_drawline(W/2, H/2,
			round(W/2 + 0.45f*H*cos(2.0f*PI*a)),
			round(H/2 + 0.45f*H*sin(2.0f*PI*a)),
			256*a);
	}
	drawcount = n;
}

void drawcube(vec3 pos, mat4 *rot, float a, byte c, byte ci)
{
	vec3 cube00 = Vec3(-0.5f*a, -0.5f*a, -0.5f*a);
	vec3 cube01 = Vec3(0.5f*a, -0.5f*a, -0.5f*a);
	vec3 cube02 = Vec3(0.5f*a, 0.5f*a, -0.5f*a);
	vec3 cube03 = Vec3(-0.5f*a, 0.5f*a, -0.5f*a);

	vec3 cube10 = Vec3(-0.5f*a, -0.5f*a, 0.5f*a);
	vec3 cube11 = Vec3(0.5f*a, -0.5f*a, 0.5f*a);
	vec3 cube12 = Vec3(0.5f*a, 0.5f*a, 0.5f*a);
	vec3 cube13 = Vec3(-0.5f*a, 0.5f*a, 0.5f*a);

	mat4 rm0 = rm;

	rm = scale(&rm, a);
	rm = translate(&rm, pos);
	rm = mat4mat4(&rm, rot);

	r_add(&cube00, &cube01, &cube02, c+ci*0);
	r_add(&cube00, &cube02, &cube03, c+ci*1);

	r_add(&cube10, &cube12, &cube11, c+ci*2);
	r_add(&cube10, &cube13, &cube12, c+ci*3);

	r_add(&cube00, &cube10, &cube11, c+ci*4);
	r_add(&cube00, &cube11, &cube01, c+ci*5);

	r_add(&cube01, &cube11, &cube12, c+ci*6);
	r_add(&cube01, &cube12, &cube02, c+ci*7);

	r_add(&cube02, &cube12, &cube13, c+ci*8);
	r_add(&cube02, &cube13, &cube03, c+ci*9);

	r_add(&cube03, &cube13, &cube10, c+ci*10);
	r_add(&cube03, &cube10, &cube00, c+ci*11);

	rm = rm0;
}

void demo(float t)
{
	int i;
	float ln;
	float lf;
	int cn;

	vec3 cube00 = Vec3(-0.5f, -0.5f, -0.5f);
	vec3 cube01 = Vec3(0.5f, -0.5f, -0.5f);
	vec3 cube02 = Vec3(0.5f, 0.5f, -0.5f);
	vec3 cube03 = Vec3(-0.5f, 0.5f, -0.5f);

	vec3 cube10 = Vec3(-0.5f, -0.5f, 0.5f);
	vec3 cube11 = Vec3(0.5f, -0.5f, 0.5f);
	vec3 cube12 = Vec3(0.5f, 0.5f, 0.5f);
	vec3 cube13 = Vec3(-0.5f, 0.5f, 0.5f);

	mat4 rm0 = rm;

	cn = 5;
	for (i = 0; i < cn; ++i) {
		//rm = scale(&rm, sin(i+t*0.1f)*0.2f + 1.0f);

		/*rm = translate(&rm, Vec4(sin(t/2.0f+3*i/PI)*2.0f,
			cos(t/4.0f+3*i/PI)*2.0f,
			sin(i/PI*2.0f)*4.0f + 2.5f, 1.0f));*/

		rm = translate(&rm, Vec3(10.0f*sin(t+2*i/PI), 10.0f*cos(t/5.0f+i/PI/5.0f), 10.0f+5.0f*sin(t/5.0f+i/PI/5.0f)));
		rm = rotateY(&rm, sin(i/PI)*i/cn*t);
		rm = rotateX(&rm, t);

/*		cube00.x += sin(t*0.02f+i/cn) * 0.1f;
		cube10.x -= cos(t*0.06f+i/cn) * 0.1f;
		cube01.x += sin(t*0.08f+i/cn) * 0.1f;
		cube11.x -= cos(t*0.02f+i/cn) * 0.1f;
		cube02.x -= sin(t*0.05f+i/cn) * 0.1f;
		cube03.x -= sin(t*0.05f+i/cn) * 0.1f;
		cube13.x += sin(t*0.02f+i/cn) * 0.1f;*/

		r_add(&cube00, &cube01, &cube02, 48+i%30);
		r_add(&cube00, &cube02, &cube03, 50+i%30);

		r_add(&cube10, &cube12, &cube11, 38+i%30);
		r_add(&cube10, &cube13, &cube12, 40+i%30);

		r_add(&cube00, &cube10, &cube11, 45+i%30);
		r_add(&cube00, &cube11, &cube01, 47+i%30);

		r_add(&cube01, &cube11, &cube12, 35+i%30);
		r_add(&cube01, &cube12, &cube02, 37+i%30);

		r_add(&cube02, &cube12, &cube13, 55+i%30);
		r_add(&cube02, &cube13, &cube03, 57+i%30);

		r_add(&cube03, &cube13, &cube10, 42+i%30);
		r_add(&cube03, &cube10, &cube00, 44+i%30);

		rm = rm0;
	}
}

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
	mat4 pm = projMat(PI*0.5f, W/H, 100.0f, 0.1f);
	// camera matrix
	mat4 cm = Mat4(1.0f);
	// object matrix
	mat4 om = Mat4(1.0f);

	// camera position
	vec3 cam;

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
	r_init();
	hookKeys();
	hookTime();

	doublebuffer = 1;
	clearscr = 1;
	clearcol = 3;

	while (running) {
		++frames;
		t += dt;

		if (clearscr) {
			r_clear();
		}

		rs -= itime;

		cam = Vec3(-posx, -posy, -posz);

		cm = rotMatX(rotx);
		cm = rotateY(&cm, roty);

		cm = translate(&cm, cam);

		rm = mat4mat4(&pm, &cm);

		//demo(3.0f*t);

		//r_addf(0.0f, 0.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 4);
		om = rotMatY(t);
		om = rotateX(&om, t);
		//drawcube(Vec3(0.0f, 0.0f, 1.0f), &om, 1.0f, 42, 12);

		wireframe = 0;
		filled = 1;
		faceculling = 1;
		zsort = 1;

		r_sort();

		r_draw();

		//tridemo();
		//linedemo();

		rs += itime;

		if (itime - lt >= SECOND) { // runs every second
			rt = (float) rs / (float) frames * TOSECOND * 1000.0f /* 0.25f + rt*0.75f*/;
			rs = 0;
			dt = min((float) (itime - lt), 2.0f * SECOND) * TOSECOND / (float) frames;
			lt = itime;
			fps = frames;
			frames = 0;
		}

#ifdef MODE13
		printf("fps: %u, key: %i, rt: %.1f, dc: %u   \r",
				 fps, keycode, rt, drawcount);
#endif

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

	// return previous
	r_exit();
	unhookKeys();
	unhookTime();

	return 0;
}
