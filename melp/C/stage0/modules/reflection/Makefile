# MLP Reflection Module - Makefile
# Module #39 - Stage 1

CC = gcc
CFLAGS = -Wall -g -O2

TARGET = reflection_standalone
SOURCES = reflection_standalone.c

TEST_INPUT = test.mlp
TEST_OUTPUT = test.s
TEST_EXEC = test

.PHONY: all clean test

all: $(TARGET)
	@echo "✅ Build: $(TARGET)"
	@ls -lh $(TARGET) | awk '{print $$5"\t"$$9}'

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) -o $@ $^

test: $(TARGET)
	@echo "-- Test: Reflection metadata --"
	@echo '@serializable' > $(TEST_INPUT)
	@echo '@deprecated("use NewPerson")' >> $(TEST_INPUT)
	@echo 'struct Person' >> $(TEST_INPUT)
	@echo '    name: text' >> $(TEST_INPUT)
	@echo '    age: numeric' >> $(TEST_INPUT)
	@echo '    active: boolean' >> $(TEST_INPUT)
	@echo 'end struct' >> $(TEST_INPUT)
	@echo '' >> $(TEST_INPUT)
	@echo 'enum Color' >> $(TEST_INPUT)
	@echo '    Red' >> $(TEST_INPUT)
	@echo '    Green' >> $(TEST_INPUT)
	@echo '    Blue' >> $(TEST_INPUT)
	@echo 'end enum' >> $(TEST_INPUT)
	@echo '' >> $(TEST_INPUT)
	@echo 'trait Drawable' >> $(TEST_INPUT)
	@echo '    function draw' >> $(TEST_INPUT)
	@echo '    function resize' >> $(TEST_INPUT)
	@echo 'end trait' >> $(TEST_INPUT)
	@echo '' >> $(TEST_INPUT)
	@echo 'function main' >> $(TEST_INPUT)
	@echo '    numeric t = typeof(Person)' >> $(TEST_INPUT)
	@echo '    text n = nameof(Person)' >> $(TEST_INPUT)
	@echo 'end function' >> $(TEST_INPUT)
	./$(TARGET) $(TEST_INPUT) $(TEST_OUTPUT) && \
	nasm -f elf64 $(TEST_OUTPUT) -o $(TEST_EXEC).o && \
	ld $(TEST_EXEC).o -o $(TEST_EXEC) && \
	./$(TEST_EXEC) && \
	echo "✅ PASSED!"

clean:
	rm -f $(TARGET) $(TEST_INPUT) $(TEST_OUTPUT) $(TEST_EXEC).o $(TEST_EXEC)
	@echo "🧹 Cleaned"
