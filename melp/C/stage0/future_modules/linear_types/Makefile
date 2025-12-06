CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Werror -O2 -g
TARGET = linear_types_standalone

SRCS = linear_types.c linear_types_standalone.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	@echo "Linking $(TARGET)..."
	$(CC) $(CFLAGS) -o $@ $^
	@echo "Build complete"

%.o: %.c linear_types.h
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
