#include "SRC\RENDER.H"

int running = 1;
FILE *outfile;

int triCount = 0;

void triDemo()
{
	int i;
	float size = 1.0f;
	vec2 vt0, vt1, vt2;

	clearscr = 0;
	doublebuffer = 0;

	for (i = 0; i < 1; ++i) {
		vt0.x = size * (RANDF * 2.0f - 1.0f);
		vt0.y = size * (RANDF * 2.0f - 1.0f);

		vt1.x = size * (RANDF * 2.0f - 1.0f);
		vt1.y = size * (RANDF * 2.0f - 1.0f);

		vt2.x = size * (RANDF * 2.0f - 1.0f);
		vt2.y = size * (RANDF * 2.0f - 1.0f);

		r_drawTriClip(&vt0, &vt1, &vt2, (byte) (RANDF * 256.0f));

		++triCount;
	}

	drawCount = triCount;
}

void lineDemo()
{
	unsigned i;
	float a;
	unsigned n = 64000;
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

void groundDemo(vec3 cam)
{
	int x, z;
	int y = -3.0f;
	int c1 = 50;
	int c2 = 49;
	int xn = 10, zn = 10;
	float size = 1.5f;

	for (x = 0; x < xn; ++x) {
		for (z = 0; z < zn; ++z) {
			r_addf(size*(x-round(cam.x/size)-xn/2.0f), y,
				size*(z-round(cam.z/size)-zn/2.0f),
				size*(1.0f+x-round(cam.x/size)-xn/2.0f), y,
				size*(z-round(cam.z/size)-zn/2.0f),
				size*(1.0f+x-round(cam.x/size)-xn/2.0f), y,
				size*(1.0f+z-round(cam.z/size)-zn/2.0f), c1);
			r_addf(size*(x-round(cam.x/size)-xn/2.0f), y,
				size*(z-round(cam.z/size)-zn/2.0f),
				size*(1.0f+x-round(cam.x/size)-xn/2.0f), y,
				size*(1.0f+z-round(cam.z/size)-zn/2.0f),
				size*(x-round(cam.x/size)-xn/2.0f), y,
				size*(1.0f+z-round(cam.z/size)-zn/2.0f), c2);
		}
	}
}

#define groundLineX(x0, x1, z0, z1) \
for (x = x0; x < x1; ++x) { \
z = z0; \
v0 = Vec3(size*(0.5f+x-round(cam.x/size)-xoffs), y, \
size*(-0.5f+z-round(cam.z/size)-zoffs)); \
v0 = mat4vec3(&r_matrix, &v0); \
z = z1; \
v1 = Vec3(size*(0.5f+x-round(cam.x/size)-xoffs), y, \
size*(-0.5f+z-round(cam.z/size)-zoffs)); \
v1 = mat4vec3(&r_matrix, &v1); \
r_drawLine3D(&v0, &v1, c); \
}

#define groundLineZ(z0, z1, x0, x1) \
for (z = z0; z < z1; ++z) { \
x = x0; \
v0 = Vec3(size*(x-round(cam.x/size)-xoffs), y, \
size*(z-round(cam.z/size)-zoffs)); \
v0 = mat4vec3(&r_matrix, &v0); \
x = x1; \
v1 = Vec3(size*(x-round(cam.x/size)-xoffs), y, \
size*(z-round(cam.z/size)-zoffs)); \
v1 = mat4vec3(&r_matrix, &v1); \
r_drawLine3D(&v0, &v1, c); \
}

void groundLines(vec3 cam)
{
	int x, z;
	vec3 v0, v1;
	int y = -3.0f;
	int c = 49;
	int xn = 9, zn = 9;
	int fnear = 3;
	float xoffs, zoffs;
	float size = 2.0f;
	xoffs = (xn)/2.0f;
	zoffs = (zn-1)/2.0f;

	// draw crosses closest 1/fnear to camera (TODO: z clipping)
	for (x = xn/fnear; x < xn - xn/fnear; x++) {
		for (z = xn/fnear; z < zn - zn/fnear; z++) {
			v0 = Vec3(size*(x-round(cam.x/size)-xoffs), y,
				size*(z-round(cam.z/size)-zoffs));
			v0 = mat4vec3(&r_matrix, &v0);
			v1 = Vec3(size*(1.0f+x-round(cam.x/size)-xoffs), y,
				size*(z-round(cam.z/size)-zoffs));
			v1 = mat4vec3(&r_matrix, &v1);
			r_drawLine3D(&v0, &v1, c);

			v0 = Vec3(size*(0.5f+x-round(cam.x/size)-xoffs), y,
				size*(-0.5f+z-round(cam.z/size)-zoffs));
			v0 = mat4vec3(&r_matrix, &v0);
			v1 = Vec3(size*(0.5f+x-round(cam.x/size)-xoffs), y,
				size*(0.5f+z-round(cam.z/size)-zoffs));
			v1 = mat4vec3(&r_matrix, &v1);
			r_drawLine3D(&v0, &v1, c);
		}
	}

	// draw long lines far away
	groundLineX(0, xn, 0, zn/fnear);
	groundLineX(0, xn, zn - zn/fnear, zn);
	groundLineZ(0, zn/fnear, xn/fnear, xn/2);
	groundLineZ(0, zn/fnear, xn/2, xn - xn/fnear);
	groundLineZ(zn - zn/fnear, zn, xn/fnear, xn/2);
	groundLineZ(zn - zn/fnear, zn, xn/2, xn - xn/fnear);
	
	groundLineZ(0, zn, 0, xn/fnear);
	groundLineZ(0, zn, xn - xn/fnear, xn);
	groundLineX(0, xn/fnear, zn/fnear, zn/2);
	groundLineX(0, xn/fnear, zn/2, zn - zn/fnear);
	groundLineX(xn - xn/fnear, xn, zn/fnear, zn/2);
	groundLineX(xn - xn/fnear, xn, zn/2, zn - zn/fnear);
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

void lineTest(float t) {
	int i;
	float o;
	vec2 v0, v1;
	int num = 64;
	for (i = 0; i < num; ++i) {
		o = 2.0f * PI / (float) num * i;
		v0 = Vec2(0.0f, 0.0f);
		v1 = Vec2(0.9f*cos(t+o), 0.9f*sin(t+o));
		r_drawLineClip(&v0, &v1, i+32);
	}
}

int main()
{
	unsigned i, n;

	unsigned frames; // frame number
	unsigned fps; // frames per second
	float t; // time in seconds
	unsigned lt; // last frame time
	float dt; // time between frames
	float rt; // render time
	unsigned rit; // render itime

	// walk and look speed
	float walkSpd, rotSpd, rotSpdMouse;
	int mouseDiffX, mouseDiffY;

	// projection matrix
	mat4 projMatrix = projMat(PI*0.5f, W/H, 100.0f, 0.1f);
	// camera matrix
	mat4 camMatrix = Mat4(1.0f);
	// object matrix
	mat4 objMatrix = Mat4(1.0f);

	// camera position
	vec3 camPos;
	// camera rotation
	vec3 camRot;

	// open log file
	outfile = fopen("out.log", "w");

	running = 1;
	
	frames = 0;
	fps = 75;
	t = 0.0f;
	lt = 0;
	dt = 1.0f/(float) fps;
	rt = 1.0f;
	rit = 0;

	camPos = Vec3(0.0f, 0.0f, 2.0f);
	camRot = Zvec3();

	walkSpd = 5.6f;
	rotSpd = 1.4f;
	rotSpdMouse = 0.3f;

	// initialize renderer
	oldvmode = r_init();
	// hook up keyboard, mouse, timer
	hookKeys();
	hookMouse();
	hookTime();

	doublebuffer = 1;
	clearscr = 1;
	clearcol = 3;

	while (running) {
		++frames;
		t += dt;

		if (clearscr)
			r_clear();

		rit -= itime;

		camMatrix = rotMatX(camRot.x);
		camMatrix = rotateY(&camMatrix, camRot.y);

		camMatrix = translate(&camMatrix, camPos);

		r_matrix = mat4mat4(&projMatrix, &camMatrix);

		//cubeDemo(3.0f*t);
		//groundDemo(camPos);

		objMatrix = rotMatY(t);
		objMatrix = rotateX(&objMatrix, t);
		
		//drawCube(Vec3(0.0f, 0.0f, 2.0f), &objMatrix, 1.0f, 42, 12);
		drawIco(Vec3(0.0f, 0.0f, 0.3f), &objMatrix, 1.0f, 64, 1);

		groundLines(camPos);
		//drawFloor(camPos, camRot.y, 49, 2);

		//lineTest(t);

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
		printf("fps: %u, k: %i %i%i, r: %.1f, d: %u   \r",
				 fps, keycode, mouseLeft, mouseRight, rt, drawCount);
#endif
		fprintf(outfile, "fps: %u, k: %i %i%i, r: %.1f, d: %u   \r",
				 fps, keycode, mouseLeft, mouseRight, rt, drawCount);

		r_sync();

		getInput();

		// game input
		if (keyDown[wDownCode]) {
			camPos.z -= cos(camRot.y)*walkSpd*dt;
			camPos.x += sin(camRot.y)*walkSpd*dt;
		}
		if (keyDown[aDownCode]) {
			camPos.x += cos(camRot.y)*walkSpd*dt;
			camPos.z += sin(camRot.y)*walkSpd*dt;
		}
		if (keyDown[sDownCode]) {
			camPos.z += cos(camRot.y)*walkSpd*dt;
			camPos.x -= sin(camRot.y)*walkSpd*dt;
		}
		if (keyDown[dDownCode]) {
			camPos.x -= cos(camRot.y)*walkSpd*dt;
			camPos.z -= sin(camRot.y)*walkSpd*dt;
		}
		
		if (keyDown[rDownCode])
			camPos.y -= walkSpd*dt;
		if (keyDown[fDownCode])
			camPos.y += walkSpd*dt;
		if (keyDown[rightDownCode])
			camRot.y -= rotSpd*dt;
		if (keyDown[leftDownCode])
			camRot.y += rotSpd*dt;
		if (keyDown[upDownCode])
			camRot.x += rotSpd*dt;
		if (keyDown[downDownCode])
			camRot.x -= rotSpd*dt;

		if (hasMouse) {
			mouseDiffX = W/2 - mousePos.x;
			mouseDiffY = H/2 - mousePos.y;
			
			// mouse input
			if (abs(mouseDiffY) > 0)
				camRot.x += rotSpdMouse*dt*mouseDiffY;
			if (abs(mouseDiffX) > 0)
				camRot.y += rotSpdMouse*dt*mouseDiffX;

			// reset mouse position
			mousePos = Pix(W/2, H/2);
		}
	}

	// return previous video mode
	r_exit(oldvmode);

	// unhook keys, mouse, timer
	unhookKeys();
	unhookMouse();
	unhookTime();

	fclose(outfile);

	return 0;
}

