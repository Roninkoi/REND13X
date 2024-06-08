#include "SRC\RENDER.H"

int itime = 0;

void (interrupt *oldTime) ();

void interrupt getITime()
{
	itime += 1;
}

void hookTime()
{
	oldTime = _dos_getvect(0x1c);
	_dos_setvect(0x1c, getITime);
}

void unhookTime()
{
	_dos_setvect(0x1c, oldTime);
}

unsigned now()
{
	return (unsigned) time(NULL);
}

byte keycode = 0;
byte keycodeBuffer[256];
byte keycodei = 0;

int keydown[256];

void (interrupt *oldKeys) ();

extern int get_keycode();

void interrupt getKeys()
{
	keycode = get_keycode();

	keycodeBuffer[keycodei++] = keycode;
}

void hookKeys()
{
	oldKeys = _dos_getvect(9);
	_dos_setvect(9, getKeys);
}

void unhookKeys()
{
	_dos_setvect(9, oldKeys);
}

pix mousepos;
unsigned mouseleft = 0;
unsigned mouseright = 0;

extern void set_mouse(void (interrupt *handler) ());
extern void reset_mouse();

extern unsigned get_mousepos();
extern void set_mousepos(int x, int y);
extern unsigned get_mouseclick();

void /*interrupt*/ getMouse()
{
	unsigned pos = 0, click = 0;
	pos = get_mousepos();

	mousepos.x = pos % W;
	mousepos.y = pos / W;

	click = get_mouseclick();

	mouseleft = click & 0x1;
	mouseright = (click & 0x2) >> 1;

	// reset mouse position to center
	set_mousepos(W/2, H/2);
}

void hookMouse()
{
	mousepos = Pix(W/2, H/2);
	//set_mouse(getMouse);
}

void unhookMouse()
{
	//reset_mouse();
}

void getInput()
{
	int i, key;

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
	
	getMouse();
}

