CC = gcc
CFLAGS = -Wall -g -O2
SOURCES = documentation_standalone.c documentation_parser.c documentation_codegen.c
OBJECTS = $(SOURCES:.c=.o)
TARGET = documentation_test
all: $(TARGET)
%.o: %.c documentation.h
	$(CC) $(CFLAGS) -c $< -o $@
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(TARGET)
test: $(TARGET)
	@./$(TARGET)
clean:
	rm -f $(OBJECTS) $(TARGET) *.s *.o
