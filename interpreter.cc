#include "interpreter.h"

#include "memory.h"
#include "framebuf.h"
#include "timer.h"
#include "input.h"

#include <stdint.h>
#include <stdlib.h>
#include <random>

#define OPCODE_LEN sizeof(uint16_t)

uint8_t registers[16] = {0};

uint8_t* interpret_opcode(uint8_t* pc) {
	// Chip-8 is big-endian
	uint16_t opcode = (uint16_t)*pc << 8;
	opcode |= *(pc+1);
	uint8_t prefix = opcode >> 12;
	uint8_t suffix = opcode & 0xF;
	uint8_t nibble2 = (opcode >> 8) & 0xF;
	uint8_t nibble3 = (opcode >> 4) & 0xF;

	switch(prefix) {
		case 0:
			if (opcode == 0x00E0) {
				clear_display();
			} else if (opcode == 0x00EE) {
				return pop() + memory_bank;
			} else {
				goto error;
			}
			break;
		case 1:
			return memory_bank + (opcode & 0xFFF);
		case 2:
			push(pc - memory_bank + OPCODE_LEN);
			return memory_bank + (opcode & 0xFFF);
		case 3:
			if (registers[nibble2] == (opcode & 0xFF))
				return pc + 2*OPCODE_LEN;
			break;
		case 4:
			if (registers[nibble2] != (opcode & 0xFF))
				return pc + 2*OPCODE_LEN;
			break;
		case 5:
			if (suffix == 0) {
				if (registers[nibble2] == registers[nibble3])
					return pc + 2*OPCODE_LEN;
			} else {
				goto error;
			}
			break;
		case 6:
			registers[nibble2] = opcode & 0xFF;
			break;
		case 7:
			registers[nibble2] += opcode & 0xFF;
			break;
		case 8:
			switch (suffix) {
				case 0:
					registers[nibble2] = registers[nibble3];
					break;
				case 1:
					registers[nibble2] |= registers[nibble3];
					break;
				case 2:
					registers[nibble2] &= registers[nibble3];
					break;
				case 3:
					registers[nibble2] ^= registers[nibble3];
					break;
				case 4:
					registers[nibble2] += registers[nibble3];
					if ((uint16_t)registers[nibble2] + registers[nibble3] > 255) {
						registers[0xF] = 1;
					} else {
						registers[0xF] = 0;
					}
					break;
				case 5:
					registers[nibble2] -= registers[nibble3];
					if ((int16_t)registers[nibble2] - registers[nibble3] < 0) {
						registers[0xF] = 1;
					} else {
						registers[0xF] = 0;
					}
					break;
				case 6:
					registers[nibble2] >>= 1;
					break;
				case 7:
					registers[nibble2] = registers[nibble3] - registers[nibble2];
					if ((int16_t)registers[nibble3] - registers[nibble2] < 0) {
						registers[0xF] = 1;
					} else {
						registers[0xF] = 0;
					}
					break;
				case 0xE:
					registers[nibble2] <<= 1;
					break;
				default:
					goto error;
					break;
			}
			break;
		case 9:
			if (suffix == 0) {
				if (registers[nibble2] != registers[nibble3])
					return pc + 2*OPCODE_LEN;
			} else {
				goto error;
			}
			break;
		case 0xA:
			ptr = memory_bank + (opcode & 0xFFF);
			break;
		case 0xB:
			return memory_bank + registers[0] + (opcode & 0xFFF);
			break;
		case 0xC:
			registers[nibble2] = rand() & (opcode & 0xFF);
			break;
		case 0xD:
			if (draw_sprite(registers[nibble2], registers[nibble3], suffix, ptr)) {
				registers[0xF] = 1;
			} else {
				registers[0xF] = 0;
			}
			break;
		case 0xE:
			if ((opcode & 0xFF) == 0x9E) {
				int key = get_key(false);
				if ((int16_t)registers[nibble2] == key)
					return pc + 2*OPCODE_LEN;
			} else if ((opcode & 0xFF) == 0xA1) {
				int key = get_key(false);
				if ((int16_t)registers[nibble2] != key)
					return pc + 2*OPCODE_LEN;
			} else {
				goto error;
			}	
			break;
		case 0xF:
			switch (suffix) {
				case 3:
					if (nibble3 == 3) {
						int val = registers[nibble2];
						ptr[2] = val % 10;
						ptr[1] = (val / 10) % 10;
						ptr[0] = val / 100;
					} else {
						goto error;
					}
					break;
				case 5:
					if (nibble3 == 1) {
						set_delay_timer(registers[nibble2]);
					} else if (nibble3 == 5) {
						uint8_t* dump_ptr = ptr;
						for (int i = 0; i <= nibble2; i++)
							*(dump_ptr++) = registers[i];
					} else if (nibble3 == 6) {
						uint8_t* dump_ptr = ptr;
						for (int i = 0; i <= nibble2; i++)
							registers[i] = *(dump_ptr++);
					} else {
						goto error;
					}
					break;
				case 7:
					if (nibble3 == 0) {
						registers[nibble2] = get_delay_timer();
					} else {
						goto error;
					}
					break;
				case 8:
					if (nibble3 == 1) {
						set_sound_timer(registers[nibble2]);
					} else {
						goto error;
					}
					break;
				case 9:
					if (nibble3 == 2) {
						ptr = font_sprites + FONT_HEIGHT*registers[nibble2];
					} else {
						goto error;
					}
					break;
				case 0xA:
					if (nibble3 == 0) {
						registers[nibble2] = get_key(true);
					} else {
						goto error;
					}
					break;
				case 0xE:
					if (nibble3 == 1) {
						ptr += registers[nibble2];
					} else {
						goto error;
					}
					break;
				default:
					goto error;
					break;
			}
			break;
		default:
			goto error;
			break;
	}

	return pc + OPCODE_LEN;

error:
	clear_display();
	printf("Invalid opcode! %x at %x\n", opcode, (uint16_t)(pc - memory_bank));
	reset_terminal();
	exit(-1);
}

void interpret() {
	while(pc - memory_bank < MEMORY_BANK_SIZE)
		pc = interpret_opcode(pc);
}
