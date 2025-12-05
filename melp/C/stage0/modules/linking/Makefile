CC = gcc
CFLAGS = -Wall -Wextra -g -I../..

OBJS = linking.o

all: $(OBJS) linking_standalone

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@

linking_standalone: linking_standalone.c $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f $(OBJS) linking_standalone *.out

.PHONY: all clean
