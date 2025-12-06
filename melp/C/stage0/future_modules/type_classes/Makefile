CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Werror -O2 -g
TARGET = type_classes_standalone

SRCS = type_classes.c type_classes_standalone.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	@echo "Linking $(TARGET)..."
	$(CC) $(CFLAGS) -o $@ $^
	@echo "Build complete"

%.o: %.c type_classes.h
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) -c $< -o $@

test: $(TARGET)
	@echo ""
	@./$(TARGET)

clean:
	@rm -f $(OBJS) $(TARGET)
	@echo "Clean complete"

rebuild: clean all

stats:
	@wc -l *.c *.h | tail -1

.PHONY: all test clean rebuild stats
