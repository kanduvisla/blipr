CC = gcc
CFLAGS = -Wall -Wextra -std=c11 $(shell sdl2-config --cflags)
LIBS = $(shell sdl2-config --libs)
OBJS = main.o drawing.o

program: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)

main.o: main.c drawing.h
	$(CC) $(CFLAGS) -c main.c

drawing.o: drawing.c drawing.h
	$(CC) $(CFLAGS) -c drawing.c

clean:
	rm -f program $(OBJS)

.PHONY: clean