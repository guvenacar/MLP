CC = gcc
CFLAGS = -Wall -g -O2
TARGET = unsafe_blocks_standalone
SOURCES = unsafe_blocks_standalone.c unsafe_blocks_parser.c unsafe_blocks_codegen.c
OBJECTS = $(SOURCES:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)

%.o: %.c unsafe_blocks.h
	$(CC) $(CFLAGS) -c $< -o $@

test: $(TARGET)
	@echo "Testing unsafe_blocks..."
	@./$(TARGET)

clean:
	rm -f $(OBJECTS) $(TARGET) unsafe_blocks_test *.s

.PHONY: all test clean
