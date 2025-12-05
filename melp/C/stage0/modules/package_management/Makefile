CC = gcc
CFLAGS = -Wall -g -O2
SOURCES = package_management_standalone.c package_management_parser.c package_management_codegen.c
OBJECTS = $(SOURCES:.c=.o)
TARGET = package_management_test

all: $(TARGET)

%.o: %.c package_management.h
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(TARGET)

test: $(TARGET)
	@./$(TARGET)

clean:
	rm -f $(OBJECTS) $(TARGET) *.s *.o package_management_exec
