CC = gcc
CFLAGS = -Wall -Wextra -g -I../..

SOURCES = optimization_pass_standalone.c \
          optimization_pass.c

OBJECTS = $(SOURCES:.c=.o)
TARGET = optimization_pass_standalone

.PHONY: all clean test

all: $(TARGET)
	@echo "✅ Optimization Pass Module built successfully!"
	@cp $(TARGET) optimization_pass_standalone || true

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

test: $(TARGET)
	@echo 'section .text' > test_opt.s
	@echo '    mov rax, rax' >> test_opt.s
	@echo '    add rax, 0' >> test_opt.s
	@echo '    nop' >> test_opt.s
	./$(TARGET) test_opt.s test_opt_optimized.s
	@echo "✅ Test completed!"

clean:
	rm -f $(OBJECTS) $(TARGET) test_opt.s test_opt_optimized.s

.PHONY: all clean test
