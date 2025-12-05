CC = gcc
CFLAGS = -Wall -g -O2
SOURCES = performance_standalone.c performance_parser.c performance_codegen.c
OBJECTS = $(SOURCES:.c=.o)
TARGET = performance_test
all: $(TARGET)
%.o: %.c performance.h
	$(CC) $(CFLAGS) -c $< -o $@
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(TARGET)
test: $(TARGET)
	@./$(TARGET)
clean:
	rm -f $(OBJECTS) $(TARGET) *.s *.o
