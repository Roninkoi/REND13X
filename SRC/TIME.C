#include "SRC\RENDER.H"

extern int far timer();
extern int far timer_start();
extern int far timer_stop();

unsigned volatile itime = 0;

void (interrupt *oldTime) ();

void interrupt getITime()
{
	itime += 1;
}

void hookTime()
{
	oldTime = _dos_getvect(0x1c);
	_dos_setvect(0x1c, getITime);
	timer_start();
}

void unhookTime()
{
	_dos_setvect(0x1c, oldTime);
	timer_stop();
}

unsigned now()
{
	return (unsigned) time(NULL);
}

volatile unsigned timerTicks;

void timerStart()
{
	timerTicks = (unsigned) timer_start();
}

float timerStop()
{
	float timerTime;
	unsigned ticks = (unsigned) timerTicks - (unsigned) timer_stop();
	
	timerTime = (float) ticks * TOSECOND;
	
	return timerTime;
}

