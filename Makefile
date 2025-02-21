CC = gcc
CFLAGS = -Wall -Wextra $(shell sdl2-config --cflags) $(shell pkg-config --cflags portmidi)
LIBS = $(shell sdl2-config --libs) $(shell pkg-config --libs portmidi)

TARGET = build/blipr
SRCS = main.c midi.c utils.c drawing.c drawing_utils.c drawing_components.c drawing_text.c drawing_icons.c colors.c file_handling.c project.c \
	programs/sequencer.c \
	programs/track_selection.c \
	programs/pattern_selection.c \
	programs/sequence_selection.c \
	programs/config_selection.c \
	programs/program_selection.c
OBJS = $(SRCS:.c=.o)

TEST_TARGET = build/test_blipr
TEST_SRCS = tests/main_test.c project.c
TEST_OBJS = $(TEST_SRCS:.c=.o)

# Main build target
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

# Test build target
$(TEST_TARGET): $(TEST_OBJS)
	$(CC) $(CFLAGS) -o $(TEST_TARGET) $^ $(LIBS)

# Generic rule for compiling .c to .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Run
run: $(TARGET)
	./$(TARGET)

# Test target
test: $(TEST_TARGET)
	./$(TEST_TARGET)

# Icon drawing target:
build/icon_tool:
	$(CC) -o build/icon_tool tools/icon_tool.c $(shell sdl2-config --cflags --libs)

icon: build/icon_tool
	./build/icon_tool

# Clean targets
clean:
	rm -f $(TARGET) $(TEST_EXECUTABLE) $(OBJS) $(TEST_OBJS)

.PHONY: clean test