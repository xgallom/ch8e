#include "registers.h"
#include "memory.h"

#include <string.h>
#include <stdio.h>

uint8_t R[GENERAL_REGISTERS];
uint16_t I;
uint8_t DT, ST;
uint16_t PC;
uint16_t PS;

void regs_init(void)
{
	memset(R, 0, sizeof(uint8_t) * GENERAL_REGISTERS);
	I = 0;
	DT = ST = 0;
	PC = PROGRAM_ADDR;
	PS = STACK_ADDR;
}

void regs_dump(void)
{
	fprintf(stderr, "R:");

	for(int n = 0; n < GENERAL_REGISTERS; ++n)
		fprintf(stderr, " %02x", R[n]);

	fprintf(stderr,
			"\nI: %04x\nDT: %02x\nST: %02x\nPC: %04x\nPS: %04x\n",
			I, DT, ST, PC, PS
	);
}


