#include <stdint.h>

#ifndef FRAMEBUF_H
#define FRAMEBUF_H

bool draw_sprite(int x, int y, int height, uint8_t* sprite_buf);
void clear_display();
void swap_buffers();

#endif
