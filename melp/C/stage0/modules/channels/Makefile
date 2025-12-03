# Makefile for MLP Channels Module
# Module #59: Go-style concurrency channels

CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Werror -O2 -pthread
LDFLAGS = -pthread
TARGET = channels_standalone
OBJS = channels.o channels_parser.o channels_codegen.o channels_standalone.o

# ============================================================================
# MAIN TARGETS
# ============================================================================

all: $(TARGET)

$(TARGET): $(OBJS)
	@echo "Linking $(TARGET)..."
	$(CC) $(LDFLAGS) -o $@ $^
	@echo "Build complete: $(TARGET)"

# ============================================================================
# OBJECT FILES
# ============================================================================

channels.o: channels.c channels.h
	@echo "Compiling channels.c..."
	$(CC) $(CFLAGS) -c $< -o $@

channels_parser.o: channels_parser.c channels.h
	@echo "Compiling channels_parser.c..."
	$(CC) $(CFLAGS) -c $< -o $@

channels_codegen.o: channels_codegen.c channels.h
	@echo "Compiling channels_codegen.c..."
	$(CC) $(CFLAGS) -c $< -o $@

channels_standalone.o: channels_standalone.c channels.h
	@echo "Compiling channels_standalone.c..."
	$(CC) $(CFLAGS) -c $< -o $@

# ============================================================================
# TEST TARGETS
# ============================================================================

test: $(TARGET)
	@echo ""
	@echo "============================================="
	@echo "   Running Channels Module Tests"
	@echo "============================================="
	@./$(TARGET)

# ============================================================================
# CLEANUP
# ============================================================================

clean:
	@echo "Cleaning build artifacts..."
	rm -f $(OBJS) $(TARGET)
	@echo "Clean complete"

# ============================================================================
# UTILITIES
# ============================================================================

rebuild: clean all

check: $(TARGET)
	@echo "Checking for memory leaks..."
	valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET)

stats: channels.c channels_parser.c channels_codegen.c channels_standalone.c channels.h
	@echo ""
	@echo "============================================="
	@echo "   Module Statistics"
	@echo "============================================="
	@wc -l $^
	@echo ""
	@echo "File Breakdown:"
	@echo "  channels.h:              $$(wc -l < channels.h) lines (header)"
	@echo "  channels.c:              $$(wc -l < channels.c) lines (core)"
	@echo "  channels_parser.c:       $$(wc -l < channels_parser.c) lines (parser)"
	@echo "  channels_codegen.c:      $$(wc -l < channels_codegen.c) lines (codegen)"
	@echo "  channels_standalone.c:   $$(wc -l < channels_standalone.c) lines (tests)"
	@echo "============================================="

.PHONY: all test clean rebuild check stats
