#include "SRC\RENDER.H"

int running = 1;

int triCount = 0;

#define TRIDEMOA 1.0f

void triDemo()
{
	int i;
	float vt[6];

	clearscr = 0;
	doublebuffer = 0;

	for (i = 0; i < 1; ++i) {
		vt[0] = TRIDEMOA * (RANDF * 2.0f - 1.0f);
		vt[1] = TRIDEMOA * (RANDF * 2.0f - 1.0f);

		vt[2] = TRIDEMOA * (RANDF * 2.0f - 1.0f);
		vt[3] = TRIDEMOA * (RANDF * 2.0f - 1.0f);

		vt[4] = TRIDEMOA * (RANDF * 2.0f - 1.0f);
		vt[5] = TRIDEMOA * (RANDF * 2.0f - 1.0f);

		r_drawTriClip(vt, (byte) (RANDF * 256.0f));

		++triCount;
	}

	drawCount = triCount;
}

void lineDemo()
{
	unsigned i;
	float a;
	unsigned n = 62832;
	for (i = 0; i < n; ++i) {
		a = (float) i / (float) n;
		r_drawLine(W/2, H/2,
			round(W/2 + 0.45f*H*cos(2.0f*PI*a)),
			round(H/2 + 0.45f*H*sin(2.0f*PI*a)),
			256*a);
	}
	drawCount = n;
}

void drawIco(vec3 pos, mat4 *rot, float a, byte c, byte ci)
{
	float phi = 1.618034f;

	vec3 ico0 = Vec3(0.5f*a*phi, 0.5f*a, 0.0f*a);
	vec3 ico1 = Vec3(0.5f*a*phi, -0.5f*a, 0.0f*a);
	vec3 ico2 = Vec3(-0.5f*a*phi, -0.5f*a, 0.0f*a);
	vec3 ico3 = Vec3(-0.5f*a*phi, 0.5f*a, 0.0f*a);

	vec3 ico4 = Vec3(0.5f*a, 0.0f*a, 0.5f*a*phi);
	vec3 ico5 = Vec3(-0.5f*a, 0.0f*a, 0.5f*a*phi);
	vec3 ico6 = Vec3(-0.5f*a, 0.0f*a, -0.5f*a*phi);
	vec3 ico7 = Vec3(0.5f*a, 0.0f*a, -0.5f*a*phi);

	vec3 ico8 = Vec3(0.0f*a, 0.5f*a*phi, 0.5f*a);
	vec3 ico9 = Vec3(0.0f*a, 0.5f*a*phi, -0.5f*a);
	vec3 ico10 = Vec3(0.0f*a, -0.5f*a*phi, -0.5f*a);
	vec3 ico11 = Vec3(0.0f*a, -0.5f*a*phi, 0.5f*a);

	mat4 rm0 = r_matrix;

	r_matrix = scale(&r_matrix, a);
	r_matrix = translate(&r_matrix, pos);
	r_matrix = mat4mat4(&r_matrix, rot);

	r_add(&ico0, &ico1, &ico4, c+ci*0);
	r_add(&ico1, &ico0, &ico7, c+ci*1);

	r_add(&ico2, &ico3, &ico5, c+ci*2);
	r_add(&ico3, &ico2, &ico6, c+ci*3);

	r_add(&ico4, &ico5, &ico8, c+ci*4);
	r_add(&ico5, &ico4, &ico11, c+ci*5);

	r_add(&ico6, &ico7, &ico9, c+ci*6);
	r_add(&ico7, &ico6, &ico10, c+ci*7);

	r_add(&ico8, &ico9, &ico0, c+ci*8);
	r_add(&ico9, &ico8, &ico3, c+ci*9);

	r_add(&ico10, &ico11, &ico1, c+ci*10);
	r_add(&ico11, &ico10, &ico2, c+ci*11);

	r_add(&ico0, &ico4, &ico8, c+ci*12);
	r_add(&ico7, &ico0, &ico9, c+ci*13);

	r_add(&ico4, &ico1, &ico11, c+ci*14);
	r_add(&ico1, &ico7, &ico10, c+ci*15);

	r_add(&ico2, &ico5, &ico11, c+ci*16);
	r_add(&ico6, &ico2, &ico10, c+ci*17);

	r_add(&ico3, &ico6, &ico9, c+ci*18);
	r_add(&ico5, &ico3, &ico8, c+ci*19);

	r_matrix = rm0;
}

void drawCube(vec3 pos, mat4 *rot, float a, byte c, byte ci)
{
	vec3 cube00 = Vec3(-0.5f*a, -0.5f*a, -0.5f*a);
	vec3 cube01 = Vec3(0.5f*a, -0.5f*a, -0.5f*a);
	vec3 cube02 = Vec3(0.5f*a, 0.5f*a, -0.5f*a);
	vec3 cube03 = Vec3(-0.5f*a, 0.5f*a, -0.5f*a);

	vec3 cube10 = Vec3(-0.5f*a, -0.5f*a, 0.5f*a);
	vec3 cube11 = Vec3(0.5f*a, -0.5f*a, 0.5f*a);
	vec3 cube12 = Vec3(0.5f*a, 0.5f*a, 0.5f*a);
	vec3 cube13 = Vec3(-0.5f*a, 0.5f*a, 0.5f*a);

	mat4 rm0 = r_matrix;

	r_matrix = scale(&r_matrix, a);
	r_matrix = translate(&r_matrix, pos);
	r_matrix = mat4mat4(&r_matrix, rot);

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

	r_matrix = rm0;
}

#define GROUNDA 1.2f

void groundDemo(vec3 cam)
{
	int x, z;
	int y = -2.0f;
	int c1 = 50;
	int c2 = 49;
	int xn = 10, zn = 10;

	for (x = 0; x < xn; ++x) {
		for (z = 0; z < zn; ++z) {
			r_addf(GROUNDA*(x-round(cam.x/GROUNDA)-xn/2.0f), y,
				GROUNDA*(z-round(cam.z/GROUNDA)-zn/2.0f),
				GROUNDA*(1.0f+x-round(cam.x/GROUNDA)-xn/2.0f), y,
				GROUNDA*(z-round(cam.z/GROUNDA)-zn/2.0f),
				GROUNDA*(1.0f+x-round(cam.x/GROUNDA)-xn/2.0f), y,
				GROUNDA*(1.0f+z-round(cam.z/GROUNDA)-zn/2.0f), c1);
			r_addf(GROUNDA*(x-round(cam.x/GROUNDA)-xn/2.0f), y,
				GROUNDA*(z-round(cam.z/GROUNDA)-zn/2.0f),
				GROUNDA*(1.0f+x-round(cam.x/GROUNDA)-xn/2.0f), y,
				GROUNDA*(1.0f+z-round(cam.z/GROUNDA)-zn/2.0f),
				GROUNDA*(x-round(cam.x/GROUNDA)-xn/2.0f), y,
				GROUNDA*(1.0f+z-round(cam.z/GROUNDA)-zn/2.0f), c2);
		}
	}
}

void cubeDemo(float t)
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

	mat4 rm0 = r_matrix;

	cn = 5;
	for (i = 0; i < cn; ++i) {
		r_matrix = translate(&r_matrix, Vec3(10.0f*sin(t+2*i/PI), 10.0f*cos(t/5.0f+i/PI/5.0f), 10.0f+5.0f*sin(t/5.0f+i/PI/5.0f)));
		r_matrix = rotateY(&r_matrix, sin(i/PI)*i/cn*t);
		r_matrix = rotateX(&r_matrix, t);

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

		r_matrix = rm0;
	}
}

int main()
{
	unsigned i, n;

	float t;
	unsigned lt;

	unsigned frames;
	unsigned fps;

	float posX;
	float posY;
	float posZ;

	float rotX;
	float rotY;

	float dt; // time between frames
	float rt; // render time
	int rit; // render itime

	float walkSpd, rotSpd;

	// projection matrix
	mat4 projMatrix = projMat(PI*0.5f, W/H, 100.0f, 0.1f);
	// camera matrix
	mat4 camMatrix = Mat4(1.0f);
	// object matrix
	mat4 objMatrix = Mat4(1.0f);

	// camera position
	vec3 camPos;

	FILE *outfile = fopen("out.log", "w");

	t = 0.0f;
	running = 1;
	lt = 0;

	frames = 0;
	fps = 75;

	posX = 0.0f;
	posY = 0.0f;
	posZ = -2.0f;

	rotX = 0.0f;
	rotY = 0.0f;

	walkSpd = 5.6f;
	rotSpd = 1.4f;

	dt = 1.0f/(float) fps;
	rt = 1.0f;
	rit = 0;

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

		rit -= itime;

		camPos = Vec3(-posX, -posY, -posZ);

		camMatrix = rotMatX(rotX);
		camMatrix = rotateY(&camMatrix, rotY);

		camMatrix = translate(&camMatrix, camPos);

		r_matrix = mat4mat4(&projMatrix, &camMatrix);

		cubeDemo(3.0f*t);
		//groundDemo(camPos);

		objMatrix = rotMatY(t);
		objMatrix = rotateX(&objMatrix, t);
		//drawCube(Vec3(0.0f, 2.0f, 1.0f), &objMatrix, 1.0f, 42, 12);
		drawIco(Vec3(0.0f, 0.0f, 0.3f), &objMatrix, 1.0f, 64, 1);

		wireframe = 0;
		filled = 1;
		faceculling = 1;
		zsort = 1;

		r_sort();

		r_draw();

		//triDemo();
		//lineDemo();

		rit += itime;

		if (itime - lt >= SECOND) { // runs every second
			rt = (float) rit / (float) frames * TOSECOND * 1000.0f /* 0.25f + rt*0.75f*/;
			rit = 0;
			dt = min((float) (itime - lt), 2.0f * SECOND) * TOSECOND / (float) frames;
			lt = itime;
			fps = frames;
			frames = 0;
		}

#ifdef MODE13
		printf("fps: %u, key: %i, rt: %.1f, dc: %u   \r",
				 fps, keycode, rt, drawCount);
#endif
		fprintf(outfile, "fps: %u, key: %i, rt: %.1f, dc: %u   \r",
				 fps, keycode, rt, drawCount);

		r_sync();

		getInput();

		// game input
		if (keydown[wDownCode]) {
			posZ += cos(rotY)*walkSpd*dt;
			posX += -sin(rotY)*walkSpd*dt;
		}
		if (keydown[aDownCode]) {
			posX -= cos(rotY)*walkSpd*dt;
			posZ -= sin(rotY)*walkSpd*dt;
		}
		if (keydown[sDownCode]) {
			posZ -= cos(rotY)*walkSpd*dt;
			posX -= -sin(rotY)*walkSpd*dt;
		}
		if (keydown[dDownCode]) {
			posX += cos(rotY)*walkSpd*dt;
			posZ += sin(rotY)*walkSpd*dt;
		}
		if (keydown[rightDownCode]) {
			rotY -= rotSpd*dt;
		}
		if (keydown[leftDownCode]) {
			rotY += rotSpd*dt;
		}
		if (keydown[upDownCode]) {
			rotX += rotSpd*dt;
		}
		if (keydown[downDownCode]) {
			rotX -= rotSpd*dt;
		}
		if (keydown[rDownCode]) {
			posY += walkSpd*dt;
		}
		if (keydown[fDownCode]) {
			posY -= walkSpd*dt;
		}
	}

	// return previous state
	r_exit();
	unhookKeys();
	unhookTime();

	fclose(outfile);

	return 0;
}

