CC = gcc
CFLAGS = -Wall -g
TARGET = program_standalone

all: $(TARGET)
	@echo "✅ Build: $(TARGET)"
	@ls -lh $(TARGET) | awk '{print $$5, $$9}'

$(TARGET): program_standalone.c
	$(CC) $(CFLAGS) -o $@ $<

test: $(TARGET)
	@echo 'fn main() { print("hello"); } fn helper() { return 42; } int x = 10;' > test.mlp
	./$(TARGET) test.mlp test.s && nasm -f elf64 test.s -o test.o && ld test.o -o test && ./test && echo "✅ PASSED!"

clean:
	rm -f $(TARGET) test.mlp test.s test.o test
	@echo "🧹 Cleaned"
