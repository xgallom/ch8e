#include "interpreter.h"

#include "registers.h"
#include "memory.h"
#include "random.h"
#include "io.h"

#include <string.h>
#include <stdio.h>
#include <stdint.h>

#define ADDR(INST) ((INST) & 0x0fff)
#define LOW(INST)  ((uint8_t) ((INST) & 0x00ff))
#define HIGH(INST) ((uint8_t) ((INST) >> 0x8))

static void jmp(uint16_t addr)
{
	fprintf(stderr, "JMP %04x\n", addr);

	PC = ADDR(addr);
}

static inline uint16_t wmem_at(uint16_t addr)
{
	uint8_t *m = mem_at(addr);
	return (((uint16_t) m[0]) << 0x8) | m[1];
}

static inline void adv_pc(void)
{
	PC += 2;
}

static void store_bcd(uint8_t *buf, uint8_t val)
{
	buf[2] = val % 10;
	val /= 10;

	buf[1] = val % 10;
	val /= 10;

	buf[0] = val;

	fprintf(stderr, "BCD %03d -> %01d%01d%01d\n", val, buf[2], buf[1], buf[0]);
}

int step(void)
{
	static uint16_t suspend = 0x0000;

	if(suspend) {
		uint8_t key = key_get();
		`
		if(key != 0xff) {
			R_1(suspend) = key;
			fprintf(stderr, "KEY %02x\n", key);
			suspend = 0x0000;
		}
		else
			return 1;
	}

	uint16_t inst = wmem_at(PC);
	adv_pc();

	fprintf(stderr, "\nInst: %04x\n", inst);
	regs_dump();

	uint16_t tmp;

	switch(inst >> 0x0c) { // First nibble
		case 0x0:
			switch(inst) {
				case 0x00e0:
					win_clr();
					break;

				case 0x00ee:
					mem_dump();
					PC = wmem_at(++PS);
					++PS;
					break;

				default:
					fprintf(stderr, "Warning: Unhandled machine instruction %04x\n", inst);
					break;
			}
			break;

		case 0x1:
			jmp(inst);
			break;

		case 0x2: // CALL
			*mem_at(PS--) = LOW(PC);
			*mem_at(PS--) = HIGH(PC);
			jmp(inst);
			break;

		case 0x3:
			if(R_1(inst) == LOW(inst))
				adv_pc();
			break;

		case 0x4:
			if(R_1(inst) != LOW(inst))
				adv_pc();
			break;

		case 0x5:
			if(R_1(inst) == R_2(inst))
				adv_pc();
			break;

		case 0x6:
			R_1(inst) = LOW(inst);
			break;

		case 0x7:
			R_1(inst) += LOW(inst);
			break;

		case 0x8:
			switch(inst & 0x000f) {
				case 0x0:
					R_1(inst) = R_2(inst);
					break;

				case 0x1:
					R_1(inst) |= R_2(inst);
					break;

				case 0x2:
					R_1(inst) &= R_2(inst);
					break;

				case 0x3:
					R_1(inst) ^= R_2(inst);
					break;

				case 0x4:
					tmp = ((uint16_t) R_1(inst)) + ((uint16_t) R_2(inst));
					RV = tmp > 0x00ff ? 0x01 : 0x00;
					R_1(inst) = (uint8_t) tmp;
					break;

				case 0x5:
					R_1(inst) -= R_2(inst);
					break;

				case 0x6:
					RV = R_1(inst) & 0x01;
					R_1(inst) >>= 0x1;
					break;

				case 0x7:
					RV = R_2(inst) > R_1(inst) ? 0x01 : 0x00;
					R_1(inst) -= R_2(inst);
					break;

				case 0xE:
					RV = R_1(inst) & 0x80 ? 0x01 : 0x00;
					R_1(inst) <<= 1;
					break;

				default:
					fprintf(stderr, "Warning: Unhandled instruction %04x\n", inst);
					break;
			}
			break;

		case 0xa:
			I = ADDR(inst);
			break;

		case 0xb:
			jmp(inst + R[0]);
			break;

		case 0xc:
			R_1(inst) = rand_get() & LOW(inst);
			break;

		case 0xd:
			RV = render_sprite(
					R_2(inst),
					R_1(inst),
					I,
					(uint8_t) (inst & 0x000f)
			);
			break;

		case 0xe:
			switch(LOW(inst)) {
				case 0x9e:
					if(key_down(R_1(inst)))
						adv_pc();
					break;

				case 0xa1:
					if(key_up(R_1(inst)))
						adv_pc();
					break;

				default:
					fprintf(stderr, "Warning: Unhandled instruction %04x\n", inst);
					break;
			}
			break;

		case 0xf:
			switch(LOW(inst)) {
				case 0x07:
					R_1(inst) = DT;
					break;

				case 0x0a:
					suspend = inst;
					break;

				case 0x15:
					DT = R_1(inst);
					break;

				case 0x18:
					ST = R_1(inst);
					break;

				case 0x1e:
					I += (uint16_t) R_1(inst);
					break;

				case 0x29:
					I = ((uint16_t) R_1(inst) * DIGIT_SIZE);
					break;

				case 0x33:
					store_bcd(mem_at(I), R_1(inst));
					mem_dump();
					break;

				case 0x55:
					memcpy(mem_at(I), R, sizeof(uint8_t) * ((HIGH(inst) & 0x0f) + 0x01));
					mem_dump();
					break;

				case 0x65:
					memcpy(R, mem_at(I), sizeof(uint8_t) * ((HIGH(inst) & 0x0f) + 0x01));
					mem_dump();
					break;

				default:
					fprintf(stderr, "Warning: Unhandled instruction %04x\n", inst);
					break;
			}
			break;

		default:
			break;
	}

	return 1;
}


