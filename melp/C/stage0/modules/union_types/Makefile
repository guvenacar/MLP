CC = gcc
CFLAGS = -Wall -g -O2
TARGET = union_types_standalone
SOURCES = union_types_standalone.c union_types_parser.c union_types_codegen.c
OBJECTS = $(SOURCES:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)

%.o: %.c union_types.h
	$(CC) $(CFLAGS) -c $< -o $@

test: $(TARGET)
	@echo "Testing union_types..."
	@./$(TARGET)

clean:
	rm -f $(OBJECTS) $(TARGET) union_types_test *.s

.PHONY: all test clean
