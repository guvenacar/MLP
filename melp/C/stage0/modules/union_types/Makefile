CC = gcc
CFLAGS = -Wall -g -O2
TARGET = union_types_test
OBJS = union_types_parser.o union_types_codegen.o union_types_standalone.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

union_types_parser.o: union_types_parser.c union_types.h
	$(CC) $(CFLAGS) -c union_types_parser.c

union_types_codegen.o: union_types_codegen.c union_types.h
	$(CC) $(CFLAGS) -c union_types_codegen.c

union_types_standalone.o: union_types_standalone.c union_types.h
	$(CC) $(CFLAGS) -c union_types_standalone.c

test: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET) union_types_test.s

.PHONY: all test clean
