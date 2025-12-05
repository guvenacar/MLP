# Makefile for melpc bootstrap compiler

CC = gcc
CFLAGS = -Wall -O2
TARGET = melpc

all: $(TARGET)

$(TARGET): melpc.c
	$(CC) $(CFLAGS) -o $(TARGET) melpc.c
	@echo "✅ melpc bootstrap compiler built successfully"

clean:
	rm -f $(TARGET) *.o

test: $(TARGET)
	@echo "Running basic test..."
	@echo "numeric x = 42" > test.mlp
	./$(TARGET) test.mlp test.s
	@echo "✅ Test passed"
	@rm -f test.mlp test.s

.PHONY: all clean test
