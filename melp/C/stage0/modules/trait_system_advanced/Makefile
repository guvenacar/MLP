# Makefile for MLP Advanced Trait System
# Module #60: Rust-style trait bounds and associated types

CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Werror -O2
TARGET = trait_system_advanced_standalone
OBJS = trait_system_advanced.o trait_system_advanced_parser.o \
       trait_system_advanced_codegen.o trait_system_advanced_standalone.o

all: $(TARGET)

$(TARGET): $(OBJS)
	@echo "Linking $(TARGET)..."
	$(CC) -o $@ $^
	@echo "Build complete: $(TARGET)"

trait_system_advanced.o: trait_system_advanced.c trait_system_advanced.h
	@echo "Compiling trait_system_advanced.c..."
	$(CC) $(CFLAGS) -c $< -o $@

trait_system_advanced_parser.o: trait_system_advanced_parser.c trait_system_advanced.h
	@echo "Compiling trait_system_advanced_parser.c..."
	$(CC) $(CFLAGS) -c $< -o $@

trait_system_advanced_codegen.o: trait_system_advanced_codegen.c trait_system_advanced.h
	@echo "Compiling trait_system_advanced_codegen.c..."
	$(CC) $(CFLAGS) -c $< -o $@

trait_system_advanced_standalone.o: trait_system_advanced_standalone.c trait_system_advanced.h
	@echo "Compiling trait_system_advanced_standalone.c..."
	$(CC) $(CFLAGS) -c $< -o $@

test: $(TARGET)
	@echo ""
	@echo "============================================="
	@echo "   Running Advanced Trait System Tests"
	@echo "============================================="
	@./$(TARGET)

clean:
	@echo "Cleaning build artifacts..."
	rm -f $(OBJS) $(TARGET)
	@echo "Clean complete"

rebuild: clean all

stats: *.c *.h
	@echo ""
	@echo "============================================="
	@echo "   Module Statistics"
	@echo "============================================="
	@wc -l $^
	@echo ""
	@echo "Total lines: $$(cat $^ | wc -l)"
	@echo "============================================="

.PHONY: all test clean rebuild stats
