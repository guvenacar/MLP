CC = gcc
CFLAGS = -Wall -Wextra -g -I../..

OBJS = optimization_pass.o

all: $(OBJS) optimization_pass_standalone

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@

optimization_pass_standalone: optimization_pass_standalone.c $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f $(OBJS) optimization_pass_standalone *.s

.PHONY: all clean
