CC = gcc
CFLAGS = -Wall -g -O2
TARGET = smart_pointers_test
OBJS = smart_pointers_parser.o smart_pointers_codegen.o smart_pointers_standalone.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

smart_pointers_parser.o: smart_pointers_parser.c smart_pointers.h
	$(CC) $(CFLAGS) -c smart_pointers_parser.c

smart_pointers_codegen.o: smart_pointers_codegen.c smart_pointers.h
	$(CC) $(CFLAGS) -c smart_pointers_codegen.c

smart_pointers_standalone.o: smart_pointers_standalone.c smart_pointers.h
	$(CC) $(CFLAGS) -c smart_pointers_standalone.c

test: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET) smart_pointers_test.s

.PHONY: all test clean
