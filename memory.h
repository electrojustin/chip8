#include <stdint.h>

#ifndef MEMORY_H
#define MEMORY_H

#define MEMORY_BANK_SIZE 4096

#define PROGRAM_START 0x200
#define STACK_START 0xEFE

#define FONT_WIDTH 4
#define FONT_HEIGHT 5

extern uint8_t memory_bank[MEMORY_BANK_SIZE];

extern uint8_t* font_sprites;

extern uint8_t* ptr;
extern uint8_t* pc;

void init_font_sprites();

void push(int val);
int pop();

#endif
