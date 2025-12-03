# Makefile for Router Module

CC = gcc
CFLAGS = -Wall -Wextra -O2
ASM = as
LD = gcc

# Targets
COMPILER = router_compiler
TEST_MLP = test_router.mlp
TEST_ASM = test_router.s
TEST_OBJ = test_router.o
TEST_BIN = test_router

.PHONY: all clean test

all: $(COMPILER)

$(COMPILER): router_compiler.c
	$(CC) $(CFLAGS) -o $@ $<

test: $(COMPILER) $(TEST_BIN)
	./$(TEST_BIN)

$(TEST_ASM): $(COMPILER) melpc.mlp
	./$(COMPILER) melpc.mlp $@

$(TEST_OBJ): $(TEST_ASM)
	$(ASM) $< -o $@

$(TEST_BIN): $(TEST_OBJ)
	$(LD) $< -o $@

clean:
	rm -f $(COMPILER) $(TEST_ASM) $(TEST_OBJ) $(TEST_BIN)
