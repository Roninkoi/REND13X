#include "SRC\RENDER.H"

int itime = 0;

void interrupt (*oldTime) ();

void interrupt getITime()
{
	itime += 1;
}

void hookTime()
{
	oldTime = getvect(0x1c);
	setvect(0x1c, getITime);
}

void unhookTime()
{
	setvect(0x1c, oldTime);
}

unsigned now()
{
	return (unsigned) time(NULL);
}

byte keycode = 0;
byte keycodeBuffer[256];
byte keycodei = 0;

int keydown[256];

void interrupt (*oldkb) ();

void interrupt getKeys()
{

	// get code from keyboard
	asm {
		cli

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

	keycodeBuffer[keycodei++] = keycode;
}

void hookKeys()
{
	oldkb = getvect(9);
	setvect(9, getKeys);
}

void unhookKeys()
{
	setvect(9, oldkb);
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
}
