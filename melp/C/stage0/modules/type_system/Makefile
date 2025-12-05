CC = gcc
CFLAGS = -Wall -Wextra -g -I../..

SOURCES = type_system_standalone.c \
          type_system.c \
          type_system_parser.c \
          type_system_codegen.c \
          ../comments/comments.c \
          ../../lexer.c

OBJECTS = $(SOURCES:.c=.o)
TARGET = type_system_standalone

.PHONY: all clean test

all: $(TARGET)
	@echo "✅ Type System Module built successfully!"
	@cp $(TARGET) type_system_standalone || true

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

test: $(TARGET)
	@echo 'numeric x: int = 42' > test_types.mlp
	@echo 'text s: string = "hello"' >> test_types.mlp
	./$(TARGET) test_types.mlp test_types.s
	@echo "✅ Test completed!"

clean:
	rm -f $(OBJECTS) $(TARGET) test_types.mlp test_types.s

.PHONY: all clean test
