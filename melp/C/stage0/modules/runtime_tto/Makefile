CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -I.
TARGET = test_runtime_tto
SOURCES = runtime_tto.c test_runtime_tto.c
OBJECTS = $(SOURCES:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $<

test: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJECTS) $(TARGET)

.PHONY: all test clean
