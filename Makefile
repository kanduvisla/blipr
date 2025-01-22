CC = gcc
CFLAGS = -Wall -Wextra $(shell sdl2-config --cflags)
LIBS = $(shell sdl2-config --libs)
OBJS = main.o drawing.o drawing_utils.o drawing_components.o colors.o drawing_text.o
EXECUTABLE = build/blipr

$(EXECUTABLE): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)

main.o: main.c globals.h colors.h drawing.h drawing_components.h
	$(CC) $(CFLAGS) -c main.c

drawing.o: drawing.c globals.h drawing.h drawing_utils.h
	$(CC) $(CFLAGS) -c drawing.c

drawing_utils.o: drawing_utils.c globals.h drawing_utils.h
	$(CC) $(CFLAGS) -c drawing_utils.c

drawing_components.o: drawing_components.c globals.h colors.h drawing.h drawing_utils.h drawing_components.h
	$(CC) $(CFLAGS) -c drawing_components.c

drawing_text.o: drawing_text.c drawing_text.h drawing.h

colors.o: colors.c colors.h
	$(CC) $(CFLAGS) -c colors.c

clean:
	rm -f $(EXECUTABLE) $(OBJS)

.PHONY: clean