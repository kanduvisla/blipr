CC = gcc
CFLAGS = -Wall -Wextra $(shell sdl2-config --cflags)
LIBS = $(shell sdl2-config --libs)

TARGET = build/blipr
SRCS = main.c drawing.c drawing_utils.c drawing_components.c drawing_text.c drawing_icons.c colors.c file_handling.c project.c programs/sequencer.c
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

# Clean targets
clean:
	rm -f $(TARGET) $(TEST_EXECUTABLE) $(OBJS) $(TEST_OBJS)

.PHONY: clean test