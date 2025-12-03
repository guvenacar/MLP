CC = gcc
CFLAGS = -Wall -g -O2
TARGET = unsafe_blocks_test
OBJS = unsafe_blocks_parser.o unsafe_blocks_codegen.o unsafe_blocks_standalone.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

unsafe_blocks_parser.o: unsafe_blocks_parser.c unsafe_blocks.h
	$(CC) $(CFLAGS) -c unsafe_blocks_parser.c

unsafe_blocks_codegen.o: unsafe_blocks_codegen.c unsafe_blocks.h
	$(CC) $(CFLAGS) -c unsafe_blocks_codegen.c

unsafe_blocks_standalone.o: unsafe_blocks_standalone.c unsafe_blocks.h
	$(CC) $(CFLAGS) -c unsafe_blocks_standalone.c

test: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET) unsafe_blocks_test.s

.PHONY: all test clean
