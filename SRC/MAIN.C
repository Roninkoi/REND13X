#include "SRC\RENDER.H"

void interrupt getKeys()
{
	// get code from keyboard
	asm cli

	asm {
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

void waitRetrace()
{
	while (inportb(0x3da) & 8); // wait around
	while (!(inportb(0x3da) & 8));
	r_clear();
	//r_scr();
}

int main()
{
	unsigned i;

	long int t;
	int running;
	unsigned lt;
	unsigned nt;

	unsigned frames;
	unsigned fps;

	float posx;
	float posy;

	int key;
	int keydown[256];

	float dt;
	float rt;
	int rs;

	float vt[3][2] = {{50.0f, 50.0f}, {300.0f, 100.0f}, {200.0f, 180.0f}};

	struct mat4 pm = projmat(PI*0.5f, 320.0f/200.0f, 100.0f, 0.1f);

	struct vec4 v0 = vec4(-0.6f, 0.3f, 0.0f, 0.0f);
	struct vec4 v1 = vec4(0.6f, 0.4f, 0.0f, 0.0f);
	struct vec4 v2 = vec4(0.8f, -0.8f, 0.1f, 0.0f);

	t = 0;
	running = 1;
	lt = 0;
	nt = 0;

	frames = 1000;
	fps = 1000;

	posx = 0.0f;
	posy = 0.0f;

	key = 0;

	dt = 1.0f;
	rt = 1.0f;
	rs = 0;

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

		rs -= itime;

		//r_drawrect((posx+1.0f)*160.0f, (-posy+1.0f)*100.0f, 80, 80, 2);
		vt[0][0] = cos(t/100.0f)*160.0f + 160.0f;
		vt[1][0] = cos(t/100.0f+2.0f)*160.0f + 160.0f;
		vt[0][1] = sin(t/200.0f)*80.0f + 100.0f;
		vt[1][1] = sin(t/200.0f+2.0f)*80.0f + 100.0f;

		//r_drawline(&vt[0], &vt[1], 6);

		v2.z = 1.0f + sin(t/100.0f);
		v0.z = 2.0f + 2.0f*sin(t/310.0f);
		v1.z = v2.z;

		r_drawtri3d(&v0, &v1, &v2, (int)(t/10.0f)%128);

		for (i = 0; i < 20 && 1; ++i) {
			vt[0][0] = posx;
			vt[0][1] = posy;
			vt[1][0] = cos(i*PI*0.1f+t/100.0f+PI*0.667f)*0.5f;
			vt[1][1] = sin(i*PI*0.1f+t/100.0f+PI*0.667f)*0.7f;
			vt[2][0] = cos(i*PI*0.1f+t/100.0f+PI*1.334f)*0.5f;
			vt[2][1] = sin(i*PI*0.1f+t/100.0f+PI*1.334f)*0.7f;
			r_drawtri(&vt, (int)(i+t*0.2f)%128);
		}

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

		printf("fps: %u, key: %i, rt: %.1f t: %i \r",
		 fps, keycode, rt, itime);

		//getKeys();

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
		}

		// game input
		if (keydown[(int)'w']) {
			posy += 0.02f*dt;
		}
		if (keydown[(int)'a']) {
			posx -= 0.02f*dt;
		}
		if (keydown[(int)'s']) {
			posy -= 0.02f*dt;
		}
		if (keydown[(int)'d']) {
			posx += 0.02f*dt;
		}
	}

	setvect(9, oldkb);
	setvect(0x1c, oldtime);

	return 0;
}
