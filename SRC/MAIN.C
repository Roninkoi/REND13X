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
extern void drawFloor(vec3 camPos, float rotY, byte c1, byte c2, byte co);
extern void drawWall(float x, float y, float z, float w, float h, int d,
			   int n, int c1, int c2);

int main(void)
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

	Texture textures[4];

	// projection matrix
	mat4 projMatrix = projMat(PI*0.5f, (float) W / (float) H, 100.0f, 0.1f);
	// camera matrix
	mat4 camMatrix = Mat4(1.0f);
	// object matrix
	mat4 objMatrix = Mat4(1.0f);

	int horizon = 0;
	int groundcol = 2;

	vec3 spritePos = Vec3(0.0f, 0.0f, 5.0f);

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

	camPos = Vec3(0.0f, 0.0f, 3.0f);
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

	createAtlas(&textureAtlas);
	
	loadPPM(&textures[0], "GFX/TEST.PPM");
	addAtlasTexture(&textureAtlas, &textures[0]);
	for (i = 1; i < 4; ++i) {
		createTexture(&textures[i], 32, 32, 5+i, 2+i, i % 3);
		addAtlasTexture(&textureAtlas, &textures[i]);
	}
	
	writeAtlasTextures(&textureAtlas);
	
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
		drawIco(Vec3(0.0f, 0.0f, 0.0f), &objMatrix, 1.0f, 64, 1);

		horizon = clamp((float) (tan(camRot.x)*H/2 + H/2), T, B);

		if (horizon < B)
			r_vfill(horizon, B - horizon + 1, groundcol);

		//groundLines(camPos);

		if (horizon > T)
			r_vfill(T, horizon + 1 - T, clearcol);
		
		//drawFloor(camPos, camRot.y, 2, 48, 5);
		drawWall(-8.8f+3.52f/2.0f, -3.0f, 8.8f, 3.52f, 4.0f,
			   1, 5, 20, 18);
		drawWall(8.8f-3.52f/2.0f, -3.0f, -8.8f, 3.52f, 4.0f,
			   -1, 5, 20, 18);
		drawWall(-8.8f, -3.0f, -8.8f+3.52f/2.0f, 3.52f, 4.0f,
			   2, 5, 18, 20);
		drawWall(8.8f, -3.0f, 8.8f-3.52f/2.0f, 3.52f, 4.0f,
			   -2, 5, 18, 20);

		spritePos = Vec3(5.0f*cos(t), 5.0f*sin(t), 4.0f);
		r_addSprite(&spritePos, 1.5f+0.5f*cos(10.0f*t),
				1.5f+0.5f*sin(10.0f*t), &textures[0]);
		
		//lineTest(t);

		wireframe = 0;
		filled = 1;
		faceculling = 1;
		zsort = 1;

		r_sort();

		r_draw();

		//r_drawAtlasSprite((H/2-16)*cos(t)+W/2-16, (H/2-16)*sin(t)+H/2-16, &textureAtlas, 0);
		//r_drawSprite(10, 10, 32, 32, &textures[0]);

		//triDemo();
		//lineDemo();

		rit += itime - rits; // render time difference

		r_sync();

		if (itime - lt >= SECOND) { // runs every second
			rt = (float) rit / (float) frames * TOSECOND * 1000.0f;
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

	destroyAtlas(&textureAtlas);

	// return previous video mode
	r_exit(oldvmode);

	// unhook keys, mouse, timer
	unhookKeys();
	unhookMouse();
	unhookTime();

	fclose(outfile);

	return 0;
}

