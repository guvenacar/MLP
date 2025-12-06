CC = gcc
CFLAGS = -Wall -g -O2
TARGET = package_management_standalone
SOURCES = package_management_standalone.c package_management_parser.c package_management_codegen.c
OBJECTS = $(SOURCES:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)

%.o: %.c package_management.h
	$(CC) $(CFLAGS) -c $< -o $@

test: $(TARGET)
	@echo "Testing package_management..."
	@./$(TARGET)

clean:
	rm -f $(OBJECTS) $(TARGET) package_management_test *.s

.PHONY: all test clean
