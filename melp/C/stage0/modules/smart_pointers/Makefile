CC = gcc
CFLAGS = -Wall -g -O2
TARGET = smart_pointers_standalone
SOURCES = smart_pointers_standalone.c smart_pointers_parser.c smart_pointers_codegen.c smart_pointers.c
OBJECTS = $(SOURCES:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)

%.o: %.c smart_pointers.h
	$(CC) $(CFLAGS) -c $< -o $@

test: $(TARGET)
	@echo "Testing smart_pointers..."
	@./$(TARGET)

clean:
	rm -f $(OBJECTS) $(TARGET) smart_pointers_test *.s

.PHONY: all test clean
