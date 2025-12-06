# MLP Preprocessor Module - Makefile
# Module #38 - Stage 1

CC = gcc
CFLAGS = -Wall -g -O2

TARGET = preprocessor_standalone
SOURCES = preprocessor_standalone.c

TEST_INPUT = test.mlp
TEST_OUTPUT = test.s
TEST_EXEC = test

.PHONY: all clean test

all: $(TARGET)
	@echo "✅ Build: $(TARGET)"
	@ls -lh $(TARGET) | awk '{print $$5"\t"$$9}'

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) -o $@ $^

test: $(TARGET)
	@echo "-- Test: Preprocessor directives --"
	@echo '#define PI 3.14' > $(TEST_INPUT)
	@echo '#define VERSION "1.0"' >> $(TEST_INPUT)
	@echo '#define MAX(a, b) ((a) > (b) ? (a) : (b))' >> $(TEST_INPUT)
	@echo '#define DEBUG' >> $(TEST_INPUT)
	@echo '' >> $(TEST_INPUT)
	@echo '#ifdef DEBUG' >> $(TEST_INPUT)
	@echo 'numeric x = PI' >> $(TEST_INPUT)
	@echo '#else' >> $(TEST_INPUT)
	@echo 'numeric x = 0' >> $(TEST_INPUT)
	@echo '#endif' >> $(TEST_INPUT)
	@echo '' >> $(TEST_INPUT)
	@echo '#ifndef RELEASE' >> $(TEST_INPUT)
	@echo 'text msg = VERSION' >> $(TEST_INPUT)
	@echo '#endif' >> $(TEST_INPUT)
	@echo '' >> $(TEST_INPUT)
	@echo '#include "math.mlp"' >> $(TEST_INPUT)
	@echo '#pragma once' >> $(TEST_INPUT)
	./$(TARGET) $(TEST_INPUT) $(TEST_OUTPUT) && \
	nasm -f elf64 $(TEST_OUTPUT) -o $(TEST_EXEC).o && \
	ld $(TEST_EXEC).o -o $(TEST_EXEC) && \
	./$(TEST_EXEC) && \
	echo "✅ PASSED!"

clean:
	rm -f $(TARGET) $(TEST_INPUT) $(TEST_OUTPUT) $(TEST_EXEC).o $(TEST_EXEC)
	@echo "🧹 Cleaned"
