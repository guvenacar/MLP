CC = gcc
CFLAGS = -Wall -g
TARGET = melpc
OBJS = main.o lexer.o modules/variable/variable.o modules/variable/variable_parser.o modules/variable/variable_codegen.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

main.o: main.c lexer.h modules/variable/variable_parser.h modules/variable/variable_codegen.h
	$(CC) $(CFLAGS) -c main.c

lexer.o: lexer.c lexer.h
	$(CC) $(CFLAGS) -c lexer.c

modules/variable/variable.o: modules/variable/variable.c modules/variable/variable.h
	$(CC) $(CFLAGS) -c modules/variable/variable.c -o modules/variable/variable.o

modules/variable/variable_parser.o: modules/variable/variable_parser.c modules/variable/variable_parser.h modules/variable/variable.h lexer.h
	$(CC) $(CFLAGS) -c modules/variable/variable_parser.c -o modules/variable/variable_parser.o

modules/variable/variable_codegen.o: modules/variable/variable_codegen.c modules/variable/variable_codegen.h modules/variable/variable.h
	$(CC) $(CFLAGS) -c modules/variable/variable_codegen.c -o modules/variable/variable_codegen.o

clean:
	rm -f $(OBJS) $(TARGET) *.s *.o test_program modules/variable/*.o

.PHONY: all clean
