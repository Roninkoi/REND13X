#include "SRC\RENDER.H"

int running = 1;
FILE *outfile;

#define FPS_TARGET 30

extern void triDemo();
extern void lineDemo();
extern void cubeDemo(float t);
extern void lineTest(float t);

extern void drawIco(vec3 pos, mat4 *rot, float a, byte c, byte ci);
extern void drawCube(vec3 pos, mat4 *rot, float a, byte c, byte ci);

extern void drawWall(float x, float y, float z, float w, float h, int d,
			   int n, byte c1, byte c2);

// projection matrix
mat4 projMatrix;
// camera matrix
mat4 camMatrix;
// object matrix
mat4 objMatrix;

// camera position
vec3 camPos;
// camera rotation
vec3 camRot;

unsigned frame; // frame number
float t; // time in seconds

Texture textures[4];

void load()
{
	// create texture atlas and load textures
	createAtlas(&textureAtlas);
	loadPPM(&textures[0], "GFX\\GRASS.PPM");
	addAtlasTexture(&textureAtlas, &textures[0]);
	loadPPM(&textures[1], "GFX\\FLOWER.PPM");
	addAtlasTexture(&textureAtlas, &textures[1]);
#ifdef MODEX
	textureAlpha = 0; // set font background to black
	loadAtlasFont(&textureAtlas, "GFX\\FONT.PPM");
	writeAtlasTextures(&textureAtlas);
	textureAlpha = TEX_ALPHA;
#endif
}

void loadAfter()
{
#ifdef MODEX
	writeAtlasTextures(&textureAtlas);
#endif
}

void draw()
{
	int i;
	vec3 spritePos = Vec3(0.0f, 0.0f, 5.0f);

	//cubeDemo(3.0f*t);
	//groundDemo(camPos);

	objMatrix = rotMatY(t);
	objMatrix = rotateX(&objMatrix, t);

	//drawCube(Vec3(0.0f, 0.0f, 2.0f), &objMatrix, 1.0f, 48, 3);
	drawIco(Vec3(0.0f, 0.0f, 0.0f), &objMatrix, 1.0f, 64, 1);

	//groundLines(camPos);

	//drawFloor(camPos, camRot.y, 2, 48, 5);
	drawWall(-8.8f+3.52f/2.0f, -3.0f, 8.8f, 3.52f, 4.0f,
		   1, 5, 24, 22);
	drawWall(8.8f-3.52f/2.0f, -3.0f, -8.8f, 3.52f, 4.0f,
		   -1, 5, 24, 22);
	drawWall(-8.8f, -3.0f, -8.8f+3.52f/2.0f, 3.52f, 4.0f,
		   2, 5, 22, 24);
	drawWall(8.8f, -3.0f, 8.8f-3.52f/2.0f, 3.52f, 4.0f,
		   -2, 5, 22, 24);

	spritePos = Vec3(3.0f, -2.0f, 4.0f);
	r_addSprite(&spritePos, 1.5f, 1.5f, textureAtlas.textures[0]);
	spritePos = Vec3(-4.0f, -2.0f, -5.0f);
	r_addSprite(&spritePos, 1.5f, 1.5f, textureAtlas.textures[1]);
	spritePos = Vec3(5.0f, -2.0f, -1.0f);
	r_addSprite(&spritePos, 1.5f, 1.5f, textureAtlas.textures[1]);
	spritePos = Vec3(-1.0f, -2.0f, -1.0f);
	r_addSprite(&spritePos, 1.5f, 1.5f, textureAtlas.textures[0]);
	spritePos = Vec3(-6.0f, -2.0f, 5.0f);
	r_addSprite(&spritePos, 1.5f, 1.5f, textureAtlas.textures[1]);

	//lineTest(t);
}

// walk and look speed
float walkSpd, rotSpd, rotSpdMouse;

void input(float dt)
{
	int mouseDiffX, mouseDiffY;
	
	getInput();

	// game input
	if (keyDown[wDownCode]) {
		camPos.z -= cosf(camRot.y)*walkSpd*dt;
		camPos.x += sinf(camRot.y)*walkSpd*dt;
	}
	if (keyDown[aDownCode]) {
		camPos.x += cosf(camRot.y)*walkSpd*dt;
		camPos.z += sinf(camRot.y)*walkSpd*dt;
	}
	if (keyDown[sDownCode]) {
		camPos.z += cosf(camRot.y)*walkSpd*dt;
		camPos.x -= sinf(camRot.y)*walkSpd*dt;
	}
	if (keyDown[dDownCode]) {
		camPos.x -= cosf(camRot.y)*walkSpd*dt;
		camPos.z -= sinf(camRot.y)*walkSpd*dt;
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
			camRot.x += rotSpdMouse*mouseDiffY;
		if (abs(mouseDiffX) > 0)
			camRot.y += rotSpdMouse*mouseDiffX;
			// reset mouse position
		mousePos = Pix(W/2, H/2);
	}
	if (abs(camRot.x) > PI/2)
		camRot.x = PI/2 * sign(camRot.x);
}

int main(void)
{
	unsigned fps; // frames per second
	unsigned lt; // last frame time
	float dt; // time between frames
	float rt, rts; // render time

	char header[64];
	char footer[64];

	int horizon = 0;
	byte groundcol = 2;
	        
	const float dt_target = 1.0f / (float) FPS_TARGET;

	// projection matrix
	projMatrix = projMat(PI*0.5f, (float) W / (float) H, 100.0f, 0.1f);
	// camera matrix
	camMatrix = Mat4(1.0f);
	// object matrix
	objMatrix = Mat4(1.0f);

	// open log file
	outfile = fopen("out.log", "w");

	running = 1;
	
	frame = 0;
	fps = FPS_TARGET;
	t = 0.0f;
	dt = dt_target;
	rt = 0.0f;
	rts = 0.0f;

	camPos = Vec3(0.0f, 0.0f, 3.0f);
	camRot = Vec3(0.0f, 0.0f, 0.0f);

	walkSpd = 6.0f;
	rotSpd = 3.0f;
	rotSpdMouse = 0.006f;

	load();
	
	// initialize renderer
	oldvmode = r_init();

	loadAfter();

	// hook up keyboard, mouse, timer
	hookKeys();
	hookMouse();
	hookTime();
	
	lt = itime;
	
	doublebuffer = 1;
	clearcol = 0;
	
	r_clear();
	r_sync();
	r_clear();

	clearscr = 0;
	clearcol = 52;

	while (running) {
		++frame;
		t += dt;
		
		if (clearscr)
			r_clear();

		horizon = clamp((int) (tanf(camRot.x)*((float) H/2.0f)) + H/2, T, B);

		if (horizon < B)
			r_vfill(horizon, B - horizon + 1, groundcol);

		if (horizon > T)
			r_vfill(T, horizon + 1 - T, clearcol);
		
		camMatrix = rotMatX(camRot.x);
		camMatrix = rotateY(&camMatrix, camRot.y);

		camMatrix = translate(&camMatrix, camPos);

		r_matrix = mat4mat4(&projMatrix, &camMatrix);
		
		draw();

		wireframe = 0;
		filled = 1;
		faceculling = 1;
		zsort = 1;

		r_sort();

		r_draw();

		sprintf(header, "FPS: %-2u, key: %-3i %-1i%-1i, rt: %2.1f, dc: %-3u\r",
			  fps, keycode, mouseLeft, mouseRight, rt*1000.0f, drawCount);
		
#ifdef MODEX
		r_drawString(0, 0, header);
		//r_drawString(0, B+1, footer);
#endif
#ifdef MODE13
		printf("%s\r", header);
#endif
		
		rts += (float) (itime - lt) * TOSECOND; // render time difference
		
		input(dt);
		
		r_sync();
		
		do {
			dt = (float) (itime - lt) * TOSECOND;
		} while (dt < dt_target);
	    
		if (itime >= SECOND) { // runs every second
			rt = rts / (float) frame;
			rts = 0;
			fps = frame;
			frame = 0;
			itime = 0;
			
			fprintf(outfile, "%s\n", header);
			//fprintf(outfile, "%s\n", footer);
		}
		
		lt = itime;
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

