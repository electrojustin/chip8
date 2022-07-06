#include "framebuf.h"

#include <stdio.h>
#include <mutex>

#define FRAMEBUF_HEIGHT 32
#define FRAMEBUF_WIDTH 64

std::mutex framebuf_mutex;
uint64_t framebuf[FRAMEBUF_HEIGHT] = {0};

void swap_buffers() {
	framebuf_mutex.lock();

	printf("\033[0;0H");
	for (int y = 0; y < FRAMEBUF_HEIGHT; y++) {
		uint64_t row = framebuf[y];
		for (int x = 0; x < FRAMEBUF_WIDTH; x++) {
			if (row & 0x8000000000000000) {
				printf("\033[42m \033[m");
			} else {
				printf(" ");
			}
			row <<= 1;
		}
		printf("\n");
	}

	framebuf_mutex.unlock();
}

bool draw_sprite(int x, int y, int height, uint8_t* sprite_buf) {
	framebuf_mutex.lock();

	uint64_t ret = 0;
	for (int dest_y = y; dest_y < y+height; dest_y++) {
		if (dest_y >= 0 && dest_y < FRAMEBUF_HEIGHT) {
			uint64_t row_flip = *(sprite_buf++);
			if (x < 56) {
				row_flip <<= FRAMEBUF_WIDTH - 8 - x;
			} else {
				row_flip >>= x - (FRAMEBUF_WIDTH - 8);
			}
			ret |= framebuf[dest_y] & row_flip;
			framebuf[dest_y] ^= row_flip;
		}
	}

	framebuf_mutex.unlock();

	//swap_buffers();

	return ret != 0;
}

void clear_display() {
	framebuf_mutex.lock();

	for (int i = 0; i < FRAMEBUF_HEIGHT; i++)
		framebuf[i] = 0;

	framebuf_mutex.unlock();

	//swap_buffers();
}
