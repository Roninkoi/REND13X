#include "SRC\RENDER.H"

byte volatile keycode = 0;
byte volatile keycodeBuffer[256];
byte volatile keycodei = 0;

int keyDown[256];

void (interrupt far *oldKeys) ();

extern unsigned far get_keycode(void);

void interrupt getKeys(void)
{
	keycode = get_keycode();

	keycodeBuffer[keycodei++] = keycode;
}

void hookKeys()
{
	int i;
	for (i = 0; i < 256; ++i) {
		keyDown[i] = 0;
	}

	oldKeys = _dos_getvect(9);
	_dos_setvect(9, getKeys);
}

void unhookKeys()
{
	_dos_setvect(9, oldKeys);
}

pix mousePos;
unsigned mouseLeft = 0;
unsigned mouseRight = 0;
unsigned hasMouse = 0;

extern void far set_mouse(void (interrupt *handler) ());
extern int far reset_mouse(void);

extern unsigned far get_mousepos(void);
extern void far set_mousepos(int x, int y);
extern unsigned far get_mouseclick(void);

void /*interrupt*/ getMouse(void)
{
	unsigned pos = 0, click = 0;
	pos = get_mousepos();

	mousePos.x = pos % W;
	mousePos.y = pos / W;

	click = get_mouseclick();

	mouseLeft = click & 0x1;
	mouseRight = (click & 0x2) >> 1;

	// reset mouse position to center
	set_mousepos(W/2, H/2);
}

void hookMouse()
{
	// reset and check for mouse driver
	hasMouse = reset_mouse();
	hasMouse = hasMouse & 0x1;

	mousePos = Pix(W/2, H/2);

	// set mouse interrupt handler
	//set_mouse(getMouse);
}

void unhookMouse()
{
	reset_mouse();
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
				keyDown[wDownCode] = 1;
				break;
			case wUpCode:
				keyDown[wDownCode] = 0;
				break;
			case aDownCode:
				keyDown[aDownCode] = 1;
				break;
			case aUpCode:
				keyDown[aDownCode] = 0;
				break;
			case sDownCode:
				keyDown[sDownCode] = 1;
				break;
			case sUpCode:
				keyDown[sDownCode] = 0;
				break;
			case dDownCode:
				keyDown[dDownCode] = 1;
				break;
			case dUpCode:
				keyDown[dDownCode] = 0;
				break;
			case rightDownCode:
				keyDown[rightDownCode] = 1;
				break;
			case rightUpCode:
				keyDown[rightDownCode] = 0;
				break;
			case leftDownCode:
				keyDown[leftDownCode] = 1;
				break;
			case leftUpCode:
				keyDown[leftDownCode] = 0;
				break;
			case upDownCode:
				keyDown[upDownCode] = 1;
				break;
			case upUpCode:
				keyDown[upDownCode] = 0;
				break;
			case downDownCode:
				keyDown[downDownCode] = 1;
				break;
			case downUpCode:
				keyDown[downDownCode] = 0;
				break;
			case rDownCode:
				keyDown[rDownCode] = 1;
				break;
			case rUpCode:
				keyDown[rDownCode] = 0;
				break;
			case fDownCode:
				keyDown[fDownCode] = 1;
				break;
			case fUpCode:
				keyDown[fDownCode] = 0;
				break;
		}
	}

	// get mouse position and button state
	if (hasMouse)
		getMouse();
}

