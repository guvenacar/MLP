CC = gcc
CFLAGS = -Wall -g
TARGET = codegen_context_standalone

all: $(TARGET)
	@echo "✅ Build: $(TARGET)"
	@ls -lh $(TARGET) | awk '{print $$5, $$9}'

$(TARGET): codegen_context_standalone.c
	$(CC) $(CFLAGS) -o $@ $<

test: $(TARGET)
	@echo 'fn main() { int a = 1; int b = 2; } let x = 10;' > test.mlp
	./$(TARGET) test.mlp test.s && nasm -f elf64 test.s -o test.o && ld test.o -o test && ./test && echo "✅ PASSED!"

clean:
	rm -f $(TARGET) test.mlp test.s test.o test
	@echo "🧹 Cleaned"
