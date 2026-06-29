#include "SRC\RENDER.H"

extern int far timer(void);
extern int far timer_start(void);
extern int far timer_stop(void);
extern void far timer_end(void);

unsigned volatile itime = 0;
unsigned volatile dtime = 0;

void (interrupt far *oldTime) ();

void interrupt getITime(void)
{
	disable();

	++itime;
	++dtime;

	if (dtime >= TIMERF) {
		dtime = 0;

		((void (interrupt far *) (void)) oldTime)();
	}
	else {
		timer_end();
		enable();
	}
}

void hookTime()
{
	oldTime = _dos_getvect(0x08);
	_dos_setvect(0x08, getITime);
	timer_start();
}

void unhookTime()
{
	_dos_setvect(0x08, oldTime);
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

