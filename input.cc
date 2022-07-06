#include "input.h"

#include <termios.h>
#include <stdio.h>
#include <sys/select.h>
#include <unistd.h>

struct termios curr, old;

void setup_terminal() {
	tcgetattr(0, &old);

	curr = old;
	curr.c_lflag &= ~ICANON;
	curr.c_lflag &= ~ECHO;
	tcsetattr(0, TCSANOW, &curr);
}

void reset_terminal() {
	tcsetattr(0, TCSANOW, &old);
}

int get_key(bool block) {
	int ret = -1;
	bool should_get_character = block;

	if (!block) {
		struct timeval tv = {0};
		fd_set fds;
		FD_ZERO(&fds);
		FD_SET(fileno(stdin), &fds);
		should_get_character = select(1, &fds, NULL, NULL, &tv) > 0;
	}
	
	if (should_get_character) {
		ret = getchar();
		if (ret >= '0' && ret <= '9') {
			ret = ret - '0';
		} else if (ret >= 'a' && ret <= 'f') {
			ret = ret - 'a' + 10;
		} else if (ret == 'i') {
			ret = 2;
		} else if (ret == 'j') {
			ret = 4;
		} else if (ret == 'k') {
			ret = 8;
		} else if (ret == 'l') {
			ret = 6;
		}
	}

	return ret;
}
