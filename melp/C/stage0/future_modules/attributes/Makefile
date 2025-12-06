CC = gcc
CFLAGS = -Wall -g -O2
TARGET = attributes_standalone
SOURCES = attributes_standalone.c attributes_parser.c attributes_codegen.c
OBJECTS = $(SOURCES:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)

%.o: %.c attributes.h
	$(CC) $(CFLAGS) -c $< -o $@

test: $(TARGET)
	@echo "Testing attributes..."
	@./$(TARGET)

clean:
	rm -f $(OBJECTS) $(TARGET) attributes_test *.s

.PHONY: all test clean
