CC = gcc
CFLAGS = -Wall -g -O2
TARGET = ffi_standalone
SOURCES = ffi_standalone.c ffi_parser.c ffi_codegen.c
OBJECTS = $(SOURCES:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)

%.o: %.c ffi.h
	$(CC) $(CFLAGS) -c $< -o $@

test: $(TARGET)
	@echo "Testing ffi..."
	@./$(TARGET)

clean:
	rm -f $(OBJECTS) $(TARGET) ffi_test *.s

.PHONY: all test clean
