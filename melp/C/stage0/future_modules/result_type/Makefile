CC = gcc
CFLAGS = -Wall -g
TARGET = result_type_standalone

all: $(TARGET)
	@echo "✅ Build: $(TARGET)"
	@ls -lh $(TARGET) | awk '{print $$5, $$9}'

$(TARGET): result_type_standalone.c
	$(CC) $(CFLAGS) -o $@ $<

test: $(TARGET)
	@printf 'Result<int, str> r = Ok(42);\nlet e = Err("error");\nmatch r { Ok(v) => v, Err(e) => 0 }' > test.mlp
	./$(TARGET) test.mlp test.s && nasm -f elf64 test.s -o test.o && ld test.o -o test && ./test && echo "✅ PASSED!"

clean:
	rm -f $(TARGET) test.mlp test.s test.o test
	@echo "🧹 Cleaned"
