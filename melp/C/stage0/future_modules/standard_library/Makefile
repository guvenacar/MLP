CC = gcc
CFLAGS = -Wall -Wextra -g -I../..

SOURCES = standard_library_standalone.c \
          standard_library.c

OBJECTS = $(SOURCES:.c=.o)
TARGET = standard_library_standalone

.PHONY: all clean test

all: $(TARGET)
	@echo "✅ Standard Library Module built successfully!"
	@cp $(TARGET) standard_library_standalone || true

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS) -lm

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

test: $(TARGET)
	@echo 'printf("Hello World")' > test_stdlib.mlp
	@echo 'malloc(100)' >> test_stdlib.mlp
	@echo 'sqrt(16.0)' >> test_stdlib.mlp
	./$(TARGET) test_stdlib.mlp test_stdlib.s
	@echo "✅ Test completed!"

clean:
	rm -f $(OBJECTS) $(TARGET) test_stdlib.mlp test_stdlib.s

.PHONY: all clean test
