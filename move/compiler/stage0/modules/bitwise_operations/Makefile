CC = gcc
CFLAGS = -Wall -Wextra -g -I../..

# Source files
SOURCES = bitwise_operations_standalone.c \
          bitwise_operations.c \
          bitwise_operations_parser.c \
          bitwise_operations_codegen.c \
          ../comments/comments.c \
          ../../lexer.c

# Object files
OBJECTS = $(SOURCES:.c=.o)

TARGET = bitwise_operations_standalone

.PHONY: all clean test

all: $(TARGET)
	@echo "✅ Bitwise Operations Module built successfully!"
	@echo "   Binary: $(TARGET)"
	@cp $(TARGET) bitwise_operations_standalone || true

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

test: $(TARGET)
	@echo "Creating test file..."
	@echo 'numeric a = 5 & 3' > test_bitwise.mlp
	@echo 'numeric b = 5 | 3' >> test_bitwise.mlp
	@echo 'numeric c = 5 ^ 3' >> test_bitwise.mlp
	./$(TARGET) test_bitwise.mlp test_bitwise.s
	@echo "✅ Test completed!"

clean:
	rm -f $(OBJECTS) $(TARGET) test_bitwise.mlp test_bitwise.s

.PHONY: all clean test
