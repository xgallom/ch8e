/**
 * A Chip8 emulator written in C
 *
 * Utilizing NCurses and standard linux C libraries
 *
 * Written by Milan Gallo on 2018-10-18
 *
 */

#include "defines.h"
#include "memory.h"
#include "registers.h"
#include "random.h"
#include "interpreter.h"
#include "io.h"
#include "clock.h"

#include <stdio.h>
#include <stdint.h>

#define STATUS_RUNNING 1
#define STATUS_QUIT    2

static const char DEFAULT_FILE[] = "data.ch8";

static int load_file(const char file_name[]);
static void prompt(const char msg[]);

static int run(void);

int main(int argc, char *argv[])
{
	const char *data_file = NULL;

	if(argc < 2)
		data_file = DEFAULT_FILE;
	else
		data_file = argv[1];

	ERROR_HANDLE(load_file(data_file));

	mem_dump();

	prompt("Press return to run the program");

	ERROR_HANDLE(io_init());

	ERROR_HANDLE(run());

	int status = STATUS_RUNNING;
	while(!(status & STATUS_QUIT)) {
		switch(key_read()) {
			case 'q':
				status = STATUS_QUIT;

			case 's':
				status ^= STATUS_RUNNING;
			break;

			default:
			break;
		}

		if(status & STATUS_RUNNING) {
			step();
			clock_tick();
		}
	}

	io_end();

	return SUCCESS;
}

static int run(void)
{
	rand_init();
	ERROR_HANDLE(clock_init());
	regs_init();

	return SUCCESS;
}

static int load_file(const char file_name[])
{
	FILE *f = fopen(file_name, "rb");

	if(!f) {
		fprintf(stderr, "Error: Couldn't open %s\n", file_name);
		return ERROR_IO;
	}

	fprintf(stderr, "Loading %s at 0x%04x\n", file_name, PROGRAM_ADDR);

	int c;
	uint8_t *mem_index = mem_at(PROGRAM_ADDR);

	while((c = getc(f)) != EOF)
		*mem_index++ = (uint8_t) c;
	
	fclose(f);

	return SUCCESS;
}

static void prompt(const char msg[])
{
	printf("%s\n$ ", msg);

	while(getchar() != '\n');
}

