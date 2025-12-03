# Comments Module - Standalone Makefile
# =======================================

CC = gcc
CFLAGS = -Wall -g -I../..
LDFLAGS = 

# Source files
SOURCES = comments_standalone.c comments.c ../../lexer.c
OBJECTS = $(SOURCES:.c=.o)
TARGET = comments_compiler

# Build standalone compiler
all: $(TARGET)
	@echo "✅ Comments module standalone compiler built!"
	@echo "   Usage: ./$(TARGET) input.mlp output.mlp"

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean
clean:
	rm -f $(OBJECTS) $(TARGET) test*.mlp test*.s test*.o test_program
	@echo "🧹 Cleaned comments module"

# Test
test: $(TARGET)
	@echo "🧪 Testing comments module..."
	@echo "-- Tek satır yorum" > test_comments.mlp
	@echo "numeric x = 42" >> test_comments.mlp
	@echo "---" >> test_comments.mlp
	@echo "Çok satırlı" >> test_comments.mlp
	@echo "yorum bloğu" >> test_comments.mlp
	@echo "---" >> test_comments.mlp
	@echo "numeric y = 100" >> test_comments.mlp
	@echo "" >> test_comments.mlp
	@./$(TARGET) test_comments.mlp test_cleaned.mlp
	@echo ""
	@echo "📄 Cleaned code:"
	@cat test_cleaned.mlp
	@echo ""
	@echo "✅ Test passed!"

.PHONY: all clean test
