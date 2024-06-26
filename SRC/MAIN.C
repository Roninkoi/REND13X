#include "SRC\RENDER.H"

int running = 1;
FILE *outfile;

extern void triDemo();
extern void lineDemo();
extern void drawIco(vec3 pos, mat4 *rot, float a, byte c, byte ci);
extern void drawCube(vec3 pos, mat4 *rot, float a, byte c, byte ci);
extern void groundDemo(vec3 cam);
extern void groundLines(vec3 cam);
extern void cubeDemo(float t);
extern void lineTest(float t);
//extern void drawFloor(vec3 camPos, float rotY, byte c1, byte c2);

int main()
{
	unsigned i, n;

	unsigned frames; // frame number
	unsigned fps; // frames per second
	float t; // time in seconds
	unsigned lt; // last frame time
	float dt; // time between frames
	float rt; // render time
	unsigned rit, rits; // render itime

	// walk and look speed
	float walkSpd, rotSpd, rotSpdMouse;
	int mouseDiffX, mouseDiffY;

	// camera position
	vec3 camPos;
	// camera rotation
	vec3 camRot;

	// projection matrix
	mat4 projMatrix = projMat(PI*0.5f, W/H, 100.0f, 0.1f);
	// camera matrix
	mat4 camMatrix = Mat4(1.0f);
	// object matrix
	mat4 objMatrix = Mat4(1.0f);

	int horizon = 0;

	// open log file
	outfile = fopen("out.log", "w");

	running = 1;
	
	frames = 0;
	fps = 75;
	t = 0.0f;
	lt = 0;
	dt = 1.0f / (float) fps;
	rt = 1.0f;
	rit = 0;

	camPos = Vec3(0.0f, 0.0f, 2.0f);
	camRot = Vec3(0.0f, 0.0f, 0.0f);

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
	clearscr = 0;
	clearcol = 52;

	while (running) {
		++frames;
		t += dt;

		if (clearscr)
			r_clear();

		rits = itime; // render time start

		camMatrix = rotMatX(camRot.x);
		camMatrix = rotateY(&camMatrix, camRot.y);

		camMatrix = translate(&camMatrix, camPos);

		r_matrix = mat4mat4(&projMatrix, &camMatrix);

		//cubeDemo(3.0f*t);
		//groundDemo(camPos);

		objMatrix = rotMatY(t);
		objMatrix = rotateX(&objMatrix, t);
		
		//drawCube(Vec3(0.0f, 0.0f, 2.0f), &objMatrix, 1.0f, 48, 3);
		drawIco(Vec3(0.0f, 0.0f, 0.3f), &objMatrix, 1.0f, 64, 1);

		horizon = clamp((float) (tan(camRot.x)*H/2 + H/2), T, B);
		
		if (horizon < B)
			r_vfill(horizon, H - horizon, 2);

		groundLines(camPos);

		if (horizon > T)
			r_vfill(0, horizon + 1, clearcol);
		
		//drawFloor(camPos, camRot.y, 2, 48);

		//lineTest(t);

		wireframe = 0;
		filled = 1;
		faceculling = 1;
		zsort = 1;

		r_sort();

		r_draw();

		//triDemo();
		//lineDemo();

		rit += itime - rits; // render time difference

		if (itime - lt >= SECOND) { // runs every second
			rt = (float) rit / (float) frames * TOSECOND *
				1000.0f /* 0.25f + rt*0.75f*/;
			rit = 0;
			dt = min((float) (itime - lt), 2.0f * SECOND) *
				TOSECOND / (float) frames;
			lt = itime;
			fps = frames;
			frames = 0;
		}

#ifdef MODE13
		printf("fps: %u, k: %i %i%i, r: %.2g, d: %u   \r",
				 fps, keycode, mouseLeft, mouseRight, rt, drawCount);
#endif
		fprintf(outfile, "fps: %u, k: %i %i%i, r: %.2g, d: %u   \r",
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

		if (abs(camRot.x) > PI/2)
			camRot.x = PI/2 * sign(camRot.x);
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

