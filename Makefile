CC = gcc
CFLAGS = -Wall -Wextra $(shell sdl2-config --cflags)
LIBS = $(shell sdl2-config --libs)
OBJS = main.o drawing.o drawing_utils.o
EXECUTABLE = build/blipr

$(EXECUTABLE): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)

main.o: main.c globals.h drawing.h drawing_utils.h
	$(CC) $(CFLAGS) -c main.c

drawing.o: drawing.c drawing.h drawing_utils.h globals.h
	$(CC) $(CFLAGS) -c drawing.c

drawing_utils.o: drawing_utils.c drawing_utils.h globals.h
	$(CC) $(CFLAGS) -c drawing_utils.c

clean:
	rm -f $(EXECUTABLE) $(OBJS)

.PHONY: clean