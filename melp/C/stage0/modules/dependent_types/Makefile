CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Werror -O2 -g
TARGET = dependent_types_standalone

SRCS = dependent_types.c dependent_types_standalone.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	@echo "Linking $(TARGET)..."
	$(CC) $(CFLAGS) -o $@ $^
	@echo "Build complete"

%.o: %.c dependent_types.h
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
