CC = gcc
CFLAGS = -Wall -g -O2
TARGET = performance_standalone
SOURCES = performance_standalone.c performance_parser.c performance_codegen.c
OBJECTS = $(SOURCES:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)

%.o: %.c performance.h
	$(CC) $(CFLAGS) -c $< -o $@

test: $(TARGET)
	@echo "Testing performance..."
	@./$(TARGET)

clean:
	rm -f $(OBJECTS) $(TARGET) performance_test *.s

.PHONY: all test clean
