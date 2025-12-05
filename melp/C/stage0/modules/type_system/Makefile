CC = gcc
CFLAGS = -Wall -Wextra -g -I../..

OBJS = type_system.o type_system_parser.o type_system_codegen.o

all: $(OBJS) type_system_standalone

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@

type_system_standalone: type_system_standalone.c $(OBJS) ../../lexer.o
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f $(OBJS) type_system_standalone *.s

.PHONY: all clean
