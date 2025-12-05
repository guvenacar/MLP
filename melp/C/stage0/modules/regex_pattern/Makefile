CC = gcc
CFLAGS = -Wall -g -O2
SOURCES = regex_pattern_standalone.c regex_pattern_parser.c regex_pattern_codegen.c
OBJECTS = $(SOURCES:.c=.o)
TARGET = regex_pattern_test
all: $(TARGET)
%.o: %.c regex_pattern.h
	$(CC) $(CFLAGS) -c $< -o $@
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(TARGET)
test: $(TARGET)
	@./$(TARGET)
clean:
	rm -f $(OBJECTS) $(TARGET) *.s *.o
