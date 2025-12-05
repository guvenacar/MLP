CC = gcc
CFLAGS = -Wall -g -O2
SOURCES = advanced_numeric_standalone.c advanced_numeric_parser.c advanced_numeric_codegen.c
OBJECTS = $(SOURCES:.c=.o)
TARGET = advanced_numeric_standalone

all: $(TARGET)

%.o: %.c advanced_numeric.h
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(TARGET)

test: $(TARGET)
	@echo "Testing advanced_numeric module..."
	@./$(TARGET)

clean:
	rm -f $(OBJECTS) $(TARGET) *.s *.o advanced_numeric_test
