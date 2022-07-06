#include "timer.h"

#include <stdint.h>
#include <mutex>
#include <thread>
#include <unistd.h>

#include "framebuf.h"

std::mutex timer_mutex;
uint8_t delay_timer;
uint8_t sound_timer;

std::thread* timer_thread;

std::mutex control_mutex;
bool should_shutdown;
bool shutdown_confirmed;

void set_delay_timer(int val) {
	timer_mutex.lock();
	delay_timer = val;
	timer_mutex.unlock();
}

void set_sound_timer(int val) {
	timer_mutex.lock();
	sound_timer = val;
	timer_mutex.unlock();
}

int get_delay_timer() {
	int ret;

	timer_mutex.lock();
	ret = delay_timer;
	timer_mutex.unlock();

	return ret;
}

int get_sound_timer() {
	int ret;

	timer_mutex.lock();
	ret = sound_timer;
	timer_mutex.unlock();

	return ret;
}

void process_timers() {
	while (true) {
		control_mutex.lock();
		if (should_shutdown) {
			shutdown_confirmed = true;
			control_mutex.unlock();
			return;
		}
		control_mutex.unlock();

		usleep(16667);

		swap_buffers();

		timer_mutex.lock();

		if (delay_timer)
			delay_timer--;
		if (sound_timer)
			sound_timer--;

		timer_mutex.unlock();
	}
}

void start_timers() {
	should_shutdown = false;
	shutdown_confirmed = false;

	timer_thread = new std::thread(process_timers);
}

void stop_timers() {
	control_mutex.lock();
	should_shutdown = true;
	control_mutex.unlock();

	while(true) {
		usleep(1000);
		control_mutex.lock();
		if (shutdown_confirmed) {
			control_mutex.unlock();
			break;
		}
		control_mutex.unlock();
	}

	delete timer_thread;
}
