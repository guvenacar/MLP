CC = gcc
CFLAGS = -Wall -g -O2
TARGET = regex_pattern_standalone
SOURCES = regex_pattern_standalone.c regex_pattern_parser.c regex_pattern_codegen.c
OBJECTS = $(SOURCES:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)

%.o: %.c regex_pattern.h
	$(CC) $(CFLAGS) -c $< -o $@

test: $(TARGET)
	@echo "Testing regex_pattern..."
	@./$(TARGET)

clean:
	rm -f $(OBJECTS) $(TARGET) regex_pattern_test *.s

.PHONY: all test clean
