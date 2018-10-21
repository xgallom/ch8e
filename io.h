/**
 * NCurses interface and Chip8 video and keyboard emulator
 *
 * Written by Milan Gallo
 */

#ifndef _IO_H_
#define _IO_H_

#include <stdint.h>

#define RES_X 64
#define RES_Y 32

void win_clr(void);
void win_refresh(void);
uint8_t render_sprite(uint8_t y, uint8_t x, uint16_t mem, uint8_t n);

int key_read(void);
int key_down(uint8_t key);
int key_up(uint8_t key);
uint8_t key_get(void);

int io_init(void);
void io_end(void);

#endif /* _IO_H_ */

