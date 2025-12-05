CC = gcc
CFLAGS = -Wall -g -O2
TARGET = decorator_system_standalone
SOURCES = decorator_system_standalone.c decorator_system_parser.c decorator_system_codegen.c decorator_system.c
OBJECTS = $(SOURCES:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)

%.o: %.c decorator_system.h
	$(CC) $(CFLAGS) -c $< -o $@

test: $(TARGET)
	@echo "Testing decorator_system..."
	@./$(TARGET)

clean:
	rm -f $(OBJECTS) $(TARGET) decorator_system_test *.s

.PHONY: all test clean
