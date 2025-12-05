CC = gcc
CFLAGS = -Wall -g -O2
TARGET = const_generics_standalone
SOURCES = const_generics_standalone.c const_generics_parser.c const_generics_codegen.c const_generics.c
OBJECTS = $(SOURCES:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)

%.o: %.c const_generics.h
	$(CC) $(CFLAGS) -c $< -o $@

test: $(TARGET)
	@echo "Testing const_generics..."
	@./$(TARGET)

clean:
	rm -f $(OBJECTS) $(TARGET) const_generics_test *.s

.PHONY: all test clean
