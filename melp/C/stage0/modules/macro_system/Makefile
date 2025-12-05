CC = gcc
CFLAGS = -Wall -g -O2
TARGET = macro_system_standalone
SOURCES = macro_system_standalone.c macro_system_parser.c macro_system_codegen.c macro_system.c
OBJECTS = $(SOURCES:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)

%.o: %.c macro_system.h
	$(CC) $(CFLAGS) -c $< -o $@

test: $(TARGET)
	@echo "Testing macro_system..."
	@./$(TARGET)

clean:
	rm -f $(OBJECTS) $(TARGET) macro_system_test *.s

.PHONY: all test clean
