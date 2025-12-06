# TTO Runtime Library Makefile
# Phase 3: Runtime support for Transparent Type Optimization

CC = gcc
CFLAGS = -Wall -Wextra -g -std=c11
LDFLAGS = -lm

# Source files
SOURCES = tto_runtime.c
OBJECTS = $(SOURCES:.c=.o)
LIBRARY = libtto_runtime.a

# Test program
TEST_SRC = test_tto_runtime.c
TEST_OBJ = test_tto_runtime.o
TEST_BIN = test_tto_runtime

.PHONY: all test clean

all: $(LIBRARY) $(TEST_BIN)
	@echo "✅ TTO Runtime library built!"
	@echo "   Library: $(LIBRARY)"
	@echo "   Test: $(TEST_BIN)"

# Build static library
$(LIBRARY): $(OBJECTS)
	ar rcs $@ $^
	@echo "📦 Created static library: $@"

# Build test program
$(TEST_BIN): $(TEST_OBJ) $(LIBRARY)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "🧪 Built test program: $@"

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Run tests
test: $(TEST_BIN)
	@echo ""
	@echo "🚀 Running TTO Runtime tests..."
	@echo ""
	@./$(TEST_BIN)

# Clean build artifacts
clean:
	rm -f $(OBJECTS) $(TEST_OBJ) $(LIBRARY) $(TEST_BIN)
	@echo "🧹 Cleaned TTO Runtime build artifacts"

# Install to stage0 modules (for linking)
install: $(LIBRARY)
	cp $(LIBRARY) ../stage0/
	cp tto_runtime.h ../stage0/modules/codegen_context/
	@echo "📥 Installed TTO Runtime to stage0"
