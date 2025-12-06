CC = gcc
CFLAGS = -Wall -g -O2
TARGET = documentation_standalone
SOURCES = documentation_standalone.c documentation_parser.c documentation_codegen.c
OBJECTS = $(SOURCES:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)

%.o: %.c documentation.h
	$(CC) $(CFLAGS) -c $< -o $@

test: $(TARGET)
	@echo "Testing documentation..."
	@./$(TARGET)

clean:
	rm -f $(OBJECTS) $(TARGET) documentation_test *.s

.PHONY: all test clean
