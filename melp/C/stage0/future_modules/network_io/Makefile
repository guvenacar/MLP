CC = gcc
CFLAGS = -Wall -g -O2
TARGET = network_io_standalone
SOURCES = network_io_standalone.c network_io_parser.c network_io_codegen.c
OBJECTS = $(SOURCES:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)

%.o: %.c network_io.h
	$(CC) $(CFLAGS) -c $< -o $@

test: $(TARGET)
	@echo "Testing network_io..."
	@./$(TARGET)

clean:
	rm -f $(OBJECTS) $(TARGET) network_io_test *.s

.PHONY: all test clean
