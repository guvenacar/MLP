#ifndef FILE_IO_PARSER_H
#define FILE_IO_PARSER_H

#include "file_io.h"
#include "../../lexer.h"

// Parser fonksiyonları - Stage 1'de implement edilecek
FileOp* parse_file_open(Lexer* lexer);
FileRead* parse_file_read(Lexer* lexer);
FileWrite* parse_file_write(Lexer* lexer);
FileClose* parse_file_close(Lexer* lexer);
FileSeek* parse_file_seek(Lexer* lexer);
FileCheck* parse_file_check(Lexer* lexer);

#endif // FILE_IO_PARSER_H
