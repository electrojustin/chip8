#include "memory.h"

uint8_t memory_bank[MEMORY_BANK_SIZE] = {0};

uint8_t* font_sprites;

uint8_t* ptr = memory_bank;
uint8_t* pc = memory_bank + PROGRAM_START;

char ascii_font[16][21] = {
			" ## "
			"#  #"
			"#  #"
			"#  #"
			" ## ",

			"  # "
			"  # "
			"  # "
			"  # "
			"  # ",

			"####"
			"   #"
			"####"
			"#   "
			"####",

			"####"
			"   #"
			"####"
			"   #"
			"####",

			"#  #"
			"#  #"
			"####"
			"   #"
			"   #",

			"####"
			"#   "
			"####"
			"   #"
			"####",

			"#   "
			"#   "
			"####"
			"#  #"
			"####",

			"####"
			"   #"
			"   #"
			"   #"
			"   #",

			"####"
			"#  #"
			"####"
			"#  #"
			"####",

			"####"
			"#  #"
			"####"
			"   #"
			"   #",

			" ## "
			"#  #"
			"####"
			"#  #"
			"#  #",

			"###"
			"#  #"
			"####"
			"#  #"
			"###",

			" ###"
			"#   "
			"#   "
			"#   "
			" ###",

			"### "
			"#  #"
			"#  #"
			"#  #"
			"### ",

			"####"
			"#   "
			"####"
			"#   "
			"####",

			"####"
			"#   "
			"####"
			"#   "
			"#   "};

void init_character(uint8_t* dest_location, char* ascii_representation) {
	for (int y = 0; y < FONT_HEIGHT; y++) {
		uint8_t row = 0;
		char* ascii_row = ascii_representation + y*FONT_WIDTH;
		for (int x = 0; x < FONT_WIDTH; x++) {
			if (ascii_row[x] == '#')
				row |= 1;
			row <<= 1;
		}
		row <<= 7 - FONT_WIDTH;
		*(dest_location++) = row;
	}
}

void init_font_sprites() {
	// Use the first 80 bytes of memory as font ROM
	font_sprites = memory_bank;
	uint8_t* curr_sprite = font_sprites;
	for (int i = 0; i < 16; i++) {
		init_character(curr_sprite, ascii_font[i]);
		curr_sprite += FONT_HEIGHT;
	}
}

uint8_t* stack_top = memory_bank + STACK_START;

void push(int val) {
	*(uint16_t*)stack_top = val;
	stack_top -= 2;
}

int pop() {
	stack_top += 2;
	int ret = *(uint16_t*)stack_top;
	return ret;
}
