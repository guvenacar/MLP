CC = gcc
CFLAGS = -Wall -Wextra -g -I../..

SOURCES = linking_standalone.c \
          linking.c

OBJECTS = $(SOURCES:.c=.o)
TARGET = linking_standalone

.PHONY: all clean test

all: $(TARGET)
	@echo "✅ Linking Module built successfully!"
	@cp $(TARGET) linking_standalone || true

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

test: $(TARGET)
	@echo 'section .data' > test1.s
	@echo '    msg1: db "Hello"' >> test1.s
	@echo 'section .text' >> test1.s
	@echo '    mov rax, 1' >> test1.s
	@echo 'section .data' > test2.s
	@echo '    msg2: db "World"' >> test2.s
	@echo 'section .text' >> test2.s
	@echo '    mov rbx, 2' >> test2.s
	./$(TARGET) test1.s test2.s linked.s
	@echo "✅ Test completed!"

clean:
	rm -f $(OBJECTS) $(TARGET) test1.s test2.s linked.s

.PHONY: all clean test
