# MELP Bootstrap Compiler Makefile
# This is the last C code we'll write - used only to bootstrap Stage 1

CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -O2
TARGET = melpc
OBJS = main.o lexer.o parser.o codegen.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)
	@echo "✅ melpc bootstrap compiler built successfully!"

main.o: main.c lexer.h parser.h codegen.h
	$(CC) $(CFLAGS) -c main.c

lexer.o: lexer.c lexer.h
	$(CC) $(CFLAGS) -c lexer.c

parser.o: parser.c parser.h lexer.h
	$(CC) $(CFLAGS) -c parser.c

codegen.o: codegen.c codegen.h parser.h
	$(CC) $(CFLAGS) -c codegen.c

clean:
	rm -f $(OBJS) $(TARGET)
	@echo "🧹 Cleaned build files"

test: $(TARGET)
	@echo "🧪 Testing melpc with simple program..."
	@echo 'numeric x = 42\nstring name = "MELP"' > test.mlp
	./$(TARGET) test.mlp test.s
	@echo "✅ Test passed!"
	@rm -f test.mlp test.s

.PHONY: all clean test
