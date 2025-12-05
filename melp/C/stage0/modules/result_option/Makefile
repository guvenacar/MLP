CC = gcc
CFLAGS = -Wall -g -O2
TARGET = result_option_standalone
SOURCES = result_option_standalone.c result_option_parser.c result_option_codegen.c
OBJECTS = $(SOURCES:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)

%.o: %.c result_option.h
	$(CC) $(CFLAGS) -c $< -o $@

test: $(TARGET)
	@echo "Testing result_option..."
	@./$(TARGET)

clean:
	rm -f $(OBJECTS) $(TARGET) result_option_test *.s

.PHONY: all test clean
