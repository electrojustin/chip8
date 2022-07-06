#include <stdio.h>
#include <stdlib.h>

#include "interpreter.h"
#include "memory.h"
#include "timer.h"
#include "input.h"

int main(int argc, char** argv) {
	if (argc != 2) {
		printf("invalid arguments!\n");
		printf("usage: chip8 <program_file>\n");
		exit(-1);
	}

	FILE* program_file = fopen(argv[1], "r");
	if (!program_file) {
		printf("could not open %s", argv[1]);
		exit(-1);
	}

	fread(memory_bank + PROGRAM_START, 1, 512, program_file);
	fclose(program_file);

	setup_terminal();

	init_font_sprites();
	start_timers();

	interpret();

	stop_timers();

	reset_terminal();
}
