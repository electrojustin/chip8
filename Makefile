CC=g++ -std=gnu++11 -g -pthread

chip8: main.o framebuf.o memory.o timer.o input.o interpreter.o
	${CC} main.o framebuf.o memory.o timer.o input.o interpreter.o -o chip8
main.o: main.cc framebuf.h
	${CC} -c main.cc
framebuf.o: framebuf.h framebuf.cc
	${CC} -c framebuf.cc
memory.o: memory.h memory.cc
	${CC} -c memory.cc
timer.o: timer.h timer.cc
	${CC} -c timer.cc
input.o: input.h input.cc
	${CC} -c input.cc
interpreter.o: interpreter.h interpreter.cc memory.h framebuf.h timer.h input.h
	${CC} -c interpreter.cc
clean:
	rm *.o
