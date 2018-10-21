/**
 * Chip8 random number generator emulator
 */

#ifndef _RANDOM_H_
#define _RANDOM_H_

#include <stdint.h>

void rand_init(void);
uint8_t rand_get(void);

#endif /* _RANDOM_H_ */

