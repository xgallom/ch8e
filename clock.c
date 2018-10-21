#include "clock.h"

#include "defines.h"
#include "registers.h"
#include "io.h"

#include <stdio.h>
#include <stdint.h>
#include <time.h>

#define CLOCK CLOCK_REALTIME
#define CLOCK_CYCLE 2000000 /* 500Hz */
#define SECOND_TO_NS 1000000000
#define CLOCK_CYCLES_PER_TIMER (60.0 / 500.0) /* 60Hz / 500Hz ~= 8.333 */

static struct timespec clk;

void update_clock(void)
{
	clock_gettime(CLOCK, &clk);

	clk.tv_nsec += CLOCK_CYCLE;

	if(clk.tv_nsec > SECOND_TO_NS) {
		++clk.tv_sec;
		clk.tv_nsec -= SECOND_TO_NS;
	}
}

int clock_init(void)
{
	clock_getres(CLOCK, &clk); // Store resolution to clock temporarily

	fprintf(stderr, "Clock resolution: %ld ns\n", clk.tv_nsec);

	if(clk.tv_sec || clk.tv_nsec > CLOCK_CYCLE) {
		fprintf(stderr, "Error: Clock resolution too low\n");
		return ERROR_CLOCK;
	}

	update_clock();

	return SUCCESS;
}

void clock_tick(void)
{
	static float ticks = 0;

	clock_nanosleep(
		CLOCK_REALTIME,
		TIMER_ABSTIME,
		&clk,
		NULL
	);

	update_clock();
	
	if(ticks++ >= CLOCK_CYCLES_PER_TIMER) {
		ticks -= CLOCK_CYCLES_PER_TIMER;

		if(DT)
			--DT;

		if(ST) {
			--ST;
			//sound_update();
		}

		static int vs = 0;

		if(vs) {
			--vs;
			win_refresh();
		}
		else
			++vs;
	}
}

