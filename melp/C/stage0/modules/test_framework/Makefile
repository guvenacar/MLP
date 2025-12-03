CC = gcc
CFLAGS = -Wall -g -O2
SOURCES = test_framework_standalone.c test_framework_parser.c test_framework_codegen.c
OBJECTS = $(SOURCES:.c=.o)
TARGET = test_framework_test
all: $(TARGET)
%.o: %.c test_framework.h
	$(CC) $(CFLAGS) -c $< -o $@
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(TARGET)
test: $(TARGET)
	@./$(TARGET)
clean:
	rm -f $(OBJECTS) $(TARGET) *.s *.o
