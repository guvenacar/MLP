CC = gcc
CFLAGS = -Wall -g -O2
TARGET = test_framework_standalone
SOURCES = test_framework_standalone.c test_framework_parser.c test_framework_codegen.c
OBJECTS = $(SOURCES:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)

%.o: %.c test_framework.h
	$(CC) $(CFLAGS) -c $< -o $@

test: $(TARGET)
	@echo "Testing test_framework..."
	@./$(TARGET)

clean:
	rm -f $(OBJECTS) $(TARGET) test_framework_test *.s

.PHONY: all test clean
