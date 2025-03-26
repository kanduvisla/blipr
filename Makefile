CC = gcc
CFLAGS = -Wall -Wextra $(shell sdl2-config --cflags) $(shell pkg-config --cflags portmidi)
LIBS = $(shell sdl2-config --libs) $(shell pkg-config --libs portmidi)

# Add C++ compiler
CXX = g++
CXXFLAGS = $(CFLAGS)

TARGET = build/blipr
C_SRCS = print.c \
	midi.c \
	utils.c \
	drawing.c \
	drawing_utils.c \
	drawing_components.c \
	drawing_text.c \
	drawing_icons.c \
	colors.c \
	file_handling.c \
	project.c \
	programs/track_selection.c \
	programs/pattern_selection.c \
	programs/sequence_selection.c \
	programs/config_selection.c \
	programs/program_selection.c \
	programs/track_options.c \
	programs/pattern_options.c	

CPP_SRCS = 	main.cpp \
	programs/abstract_program.cpp \
	programs/sequencer.cpp \
	programs/four_on_the_floor.cpp
SRCS = $(C_SRCS) $(CPP_SRCS)

C_OBJS = $(patsubst %.c,%.o,$(filter %.c,$(SRCS)))
CPP_OBJS = $(patsubst %.cpp,%.o,$(filter %.cpp,$(SRCS)))
OBJS = $(C_OBJS) $(CPP_OBJS)

TEST_TARGET = build/test_blipr
TEST_SRCS = $(SRCS:main.c=tests/main_test.c)
TEST_OBJS = $(TEST_SRCS:.c=.o)

# Debug build settings
# DEBUG ?= 0
# ifeq ($(DEBUG), 1)
# 	CFLAGS += -g -DDEBUG
# endif

# Main build target
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

# Test build target
$(TEST_TARGET): $(TEST_OBJS)
	$(CXX) $(CXXFLAGS) -o $(TEST_TARGET) $(TEST_OBJS) $(LIBS)

# Generic rule for compiling .c to .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Rule for compiling .cpp to .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Make & Run target
run: $(TARGET)
	./$(TARGET)

# Make & Run debug target
debug: CFLAGS += -g -DDEBUG
debug: $(TARGET)
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
	rm -f $(TARGET) $(TEST_EXECUTABLE) build/icon_tool
	rm -f $(OBJS:.cpp=.o) $(TEST_OBJS:.cpp=.o)

.PHONY: clean test