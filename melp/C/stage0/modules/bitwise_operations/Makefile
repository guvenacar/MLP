CC = gcc
CFLAGS = -Wall -Wextra -g -I../..

OBJS = bitwise_operations.o bitwise_operations_parser.o bitwise_operations_codegen.o

all: $(OBJS) bitwise_operations_standalone

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@

bitwise_operations_standalone: bitwise_operations_standalone.c $(OBJS) ../../lexer.o
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f $(OBJS) bitwise_operations_standalone *.s

.PHONY: all clean
