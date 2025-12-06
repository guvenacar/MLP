CC = gcc
CFLAGS = -Wall -g -O2
TARGET = ownership_system_standalone
SOURCES = ownership_system_standalone.c ownership_system_parser.c ownership_system_codegen.c ownership_system.c
OBJECTS = $(SOURCES:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)

%.o: %.c ownership_system.h
	$(CC) $(CFLAGS) -c $< -o $@

test: $(TARGET)
	@echo "Testing ownership_system..."
	@./$(TARGET)

clean:
	rm -f $(OBJECTS) $(TARGET) ownership_system_test *.s

.PHONY: all test clean
