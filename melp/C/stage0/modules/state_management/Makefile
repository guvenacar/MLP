CC = gcc
CFLAGS = -Wall -g
TARGET = state_management_standalone

all: $(TARGET)
	@echo "✅ Build: $(TARGET)"
	@ls -lh $(TARGET) | awk '{print $$5, $$9}'

$(TARGET): state_management_standalone.c
	$(CC) $(CFLAGS) -o $@ $<

test: $(TARGET)
	@printf 'state { mut counter = 0; const MAX = 100; }\nstatic cache = [];\nget value() { return counter; }\nset value(v) { counter = v; }' > test.mlp
	./$(TARGET) test.mlp test.s && nasm -f elf64 test.s -o test.o && ld test.o -o test && ./test && echo "✅ PASSED!"

clean:
	rm -f $(TARGET) test.mlp test.s test.o test
	@echo "🧹 Cleaned"
