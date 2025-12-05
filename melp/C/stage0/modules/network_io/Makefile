CC = gcc
CFLAGS = -Wall -g -O2
SOURCES = network_io_standalone.c network_io_parser.c network_io_codegen.c
OBJECTS = $(SOURCES:.c=.o)
TARGET = network_io_test
all: $(TARGET)
%.o: %.c network_io.h
	$(CC) $(CFLAGS) -c $< -o $@
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(TARGET)
test: $(TARGET)
	@./$(TARGET)
clean:
	rm -f $(OBJECTS) $(TARGET) *.s *.o
