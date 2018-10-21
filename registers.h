#ifndef _REGISTERS_H_
#define _REGISTERS_H_

#include <stdint.h>

#define GENERAL_REGISTERS 0x10

extern uint8_t R[GENERAL_REGISTERS]; /* General registers */
extern uint16_t I; /* Indirection register */
extern uint8_t DT, ST; /* Timer registers */
extern uint16_t PC; /* Program counter */
extern uint16_t PS; /* Stack pointer */

void regs_init(void);
void regs_dump(void);

#define RV R[0xf]

#define _R(OFFSET) R[(OFFSET) & 0x000f]
#define R_1(INST) _R((INST) >> 0x8)
#define R_2(INST) _R((INST) >> 0x4)

#endif
