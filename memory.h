/**
 * Chip 8 memory emulation
 *
 * Written by Milan Gallo
 */

#ifndef _MEMORY_H_
#define _MEMORY_H_

#include <stdint.h>

#define MEMORY_SIZE  0x1000
#define PROGRAM_ADDR 0x0200
#define STACK_ADDR   0x0fff
#define DIGIT_SIZE   0x0005

uint8_t *mem_at(uint16_t addr);

void mem_dump();

#endif /* _MEMORY_H_ */
