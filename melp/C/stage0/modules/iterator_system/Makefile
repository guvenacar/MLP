CC = gcc
CFLAGS = -Wall -g -O2
TARGET = iterator_system_standalone
OBJS = iterator_system_parser.o iterator_system_codegen.o iterator_system_standalone.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

iterator_system_parser.o: iterator_system_parser.c iterator_system.h
	$(CC) $(CFLAGS) -c iterator_system_parser.c

iterator_system_codegen.o: iterator_system_codegen.c iterator_system.h
	$(CC) $(CFLAGS) -c iterator_system_codegen.c

iterator_system_standalone.o: iterator_system_standalone.c iterator_system.h
	$(CC) $(CFLAGS) -c iterator_system_standalone.c

test: $(TARGET)
	@echo "Testing iterator_system module..."
	@./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET) *.s iterator_system_test

.PHONY: all test clean
