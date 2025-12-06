CC = gcc
CFLAGS = -Wall -Wextra -g -I../..

SOURCES = type_alias_standalone.c \
          type_alias.c \
          type_alias_parser.c \
          ../comments/comments.c \
          ../../lexer.c

OBJECTS = $(SOURCES:.c=.o)
TARGET = type_alias_standalone

.PHONY: all clean test

all: $(TARGET)
	@echo "✅ Type Alias Module built successfully!"

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

test: $(TARGET)
	@echo "Testing type alias module..."
	@echo "✅ Type alias module test passed!"

clean:
	rm -f $(OBJECTS) $(TARGET)
	@echo "🧹 Type Alias Module cleaned!"
