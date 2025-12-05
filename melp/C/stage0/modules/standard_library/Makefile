CC = gcc
CFLAGS = -Wall -Wextra -g -I../..

OBJS = standard_library.o

all: $(OBJS) standard_library_standalone

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@

standard_library_standalone: standard_library_standalone.c $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ -lm

clean:
	rm -f $(OBJS) standard_library_standalone

.PHONY: all clean
