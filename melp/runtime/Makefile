# Makefile for GC Runtime

CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -O2 -g
LDFLAGS =

# Targets
GC_LIB = libgc.a
GC_TEST = gc_test

# Source files
GC_SRC = gc.c
GC_OBJ = gc.o
GC_HDR = gc.h

TEST_SRC = gc_test.c
TEST_OBJ = gc_test.o

.PHONY: all clean test

all: $(GC_LIB)

$(GC_LIB): $(GC_OBJ)
	ar rcs $@ $<

$(GC_OBJ): $(GC_SRC) $(GC_HDR)
	$(CC) $(CFLAGS) -c $< -o $@

test: $(GC_TEST)
	./$(GC_TEST)

$(GC_TEST): $(TEST_OBJ) $(GC_LIB)
	$(CC) $(LDFLAGS) $^ -o $@

$(TEST_OBJ): $(TEST_SRC) $(GC_HDR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(GC_OBJ) $(TEST_OBJ) $(GC_LIB) $(GC_TEST)

# Install library
install: $(GC_LIB)
	@echo "GC library ready: $(GC_LIB)"
