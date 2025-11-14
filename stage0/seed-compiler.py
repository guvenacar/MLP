#!/usr/bin/env python3
"""
===============================================
MLP Seed Compiler (Stage 0)
===============================================
Minimal Python-based compiler to bootstrap MLP
Compiles basic MLP code to C, then to binary

Features:
- Basic lexer/parser
- Simple functions and classes
- Basic data structures
- Assembly/C code generation
"""

import sys
import os
import re
import subprocess
from typing import List, Dict, Any, Optional
from enum import Enum, auto
from dataclasses import dataclass

# ===============================================
# Token Types
# ===============================================

class TokenType(Enum):
    # Keywords
    CLASS = auto()
    CONSTRUCTOR = auto()
    METHOD = auto()
    OVERRIDE = auto()
    END = auto()
    IF = auto()
    ELSE = auto()
    ELSIF = auto()
    WHILE = auto()
    FOR = auto()
    RETURN = auto()
    VAR = auto()
    THIS = auto()
    NEW = auto()
    KULLAN = auto()  # import
    YAZDIR = auto()  # print

    # Types
    STRING = auto()
    NUMBER = auto()
    BOOL = auto()
    DYNAMIC = auto()
    DICT = auto()

    # Literals
    IDENTIFIER = auto()
    STRING_LITERAL = auto()
    INTEGER_LITERAL = auto()
    FLOAT_LITERAL = auto()
    TRUE = auto()
    FALSE = auto()
    NULL = auto()

    # Operators
    ASSIGN = auto()
    PLUS = auto()
    MINUS = auto()
    MULTIPLY = auto()
    DIVIDE = auto()
    EQUAL = auto()
    NOT_EQUAL = auto()
    LESS_THAN = auto()
    GREATER_THAN = auto()
    LESS_EQUAL = auto()
    GREATER_EQUAL = auto()
    AND = auto()
    OR = auto()
    NOT = auto()

    # Delimiters
    LPAREN = auto()
    RPAREN = auto()
    LBRACE = auto()
    RBRACE = auto()
    LBRACKET = auto()
    RBRACKET = auto()
    COMMA = auto()
    DOT = auto()
    SEMICOLON = auto()
    COLON = auto()
    ARROW = auto()

    # Special
    NEWLINE = auto()
    EOF = auto()

@dataclass
class Token:
    type: TokenType
    value: Any
    line: int
    column: int

# ===============================================
# Lexer
# ===============================================

class Lexer:
    def __init__(self, source: str):
        self.source = source
        self.pos = 0
        self.line = 1
        self.column = 1
        self.tokens = []

        self.keywords = {
            'class': TokenType.CLASS,
            'constructor': TokenType.CONSTRUCTOR,
            'method': TokenType.METHOD,
            'override': TokenType.OVERRIDE,
            'end': TokenType.END,
            'if': TokenType.IF,
            'else': TokenType.ELSE,
            'elsif': TokenType.ELSIF,
            'while': TokenType.WHILE,
            'for': TokenType.FOR,
            'return': TokenType.RETURN,
            'var': TokenType.VAR,
            'this': TokenType.THIS,
            'new': TokenType.NEW,
            'KULLAN': TokenType.KULLAN,
            'YAZDIR': TokenType.YAZDIR,
            'string': TokenType.STRING,
            'number': TokenType.NUMBER,
            'bool': TokenType.BOOL,
            'dynamic': TokenType.DYNAMIC,
            'dict': TokenType.DICT,
            'true': TokenType.TRUE,
            'false': TokenType.FALSE,
            'null': TokenType.NULL,
            'and': TokenType.AND,
            'or': TokenType.OR,
            'not': TokenType.NOT,
            'in': TokenType.IDENTIFIER,  # Special case
        }

    def current_char(self) -> Optional[str]:
        if self.pos >= len(self.source):
            return None
        return self.source[self.pos]

    def peek(self, offset=1) -> Optional[str]:
        pos = self.pos + offset
        if pos >= len(self.source):
            return None
        return self.source[pos]

    def advance(self):
        if self.pos < len(self.source):
            if self.source[self.pos] == '\n':
                self.line += 1
                self.column = 1
            else:
                self.column += 1
            self.pos += 1

    def skip_whitespace(self):
        while self.current_char() and self.current_char() in ' \t\r':
            self.advance()

    def skip_comment(self):
        if self.current_char() == '-' and self.peek() == '-':
            while self.current_char() and self.current_char() != '\n':
                self.advance()

    def read_string(self) -> str:
        quote = self.current_char()
        self.advance()
        result = ''
        while self.current_char() and self.current_char() != quote:
            if self.current_char() == '\\':
                self.advance()
                if self.current_char() == 'n':
                    result += '\n'
                elif self.current_char() == 't':
                    result += '\t'
                elif self.current_char() == '\\':
                    result += '\\'
                elif self.current_char() == quote:
                    result += quote
                else:
                    result += self.current_char()
                self.advance()
            else:
                result += self.current_char()
                self.advance()
        self.advance()  # Skip closing quote
        return result

    def read_number(self) -> Token:
        start_line, start_col = self.line, self.column
        num_str = ''
        is_float = False

        while self.current_char() and (self.current_char().isdigit() or self.current_char() == '.'):
            if self.current_char() == '.':
                is_float = True
            num_str += self.current_char()
            self.advance()

        if is_float:
            return Token(TokenType.FLOAT_LITERAL, float(num_str), start_line, start_col)
        else:
            return Token(TokenType.INTEGER_LITERAL, int(num_str), start_line, start_col)

    def read_identifier(self) -> Token:
        start_line, start_col = self.line, self.column
        result = ''

        while self.current_char() and (self.current_char().isalnum() or self.current_char() in '_'):
            result += self.current_char()
            self.advance()

        token_type = self.keywords.get(result, TokenType.IDENTIFIER)
        return Token(token_type, result, start_line, start_col)

    def tokenize(self) -> List[Token]:
        while self.pos < len(self.source):
            self.skip_whitespace()

            if not self.current_char():
                break

            # Skip comments
            if self.current_char() == '-' and self.peek() == '-':
                self.skip_comment()
                continue

            # Newline
            if self.current_char() == '\n':
                self.advance()
                continue

            # Strings
            if self.current_char() in '"\'':
                start_line, start_col = self.line, self.column
                value = self.read_string()
                self.tokens.append(Token(TokenType.STRING_LITERAL, value, start_line, start_col))
                continue

            # Numbers
            if self.current_char().isdigit():
                self.tokens.append(self.read_number())
                continue

            # Identifiers and keywords
            if self.current_char().isalpha() or self.current_char() == '_':
                self.tokens.append(self.read_identifier())
                continue

            # Operators and delimiters
            start_line, start_col = self.line, self.column
            char = self.current_char()

            # Two-character operators
            if char == '=' and self.peek() == '=':
                self.tokens.append(Token(TokenType.EQUAL, '==', start_line, start_col))
                self.advance()
                self.advance()
                continue

            if char == '!' and self.peek() == '=':
                self.tokens.append(Token(TokenType.NOT_EQUAL, '!=', start_line, start_col))
                self.advance()
                self.advance()
                continue

            if char == '<' and self.peek() == '=':
                self.tokens.append(Token(TokenType.LESS_EQUAL, '<=', start_line, start_col))
                self.advance()
                self.advance()
                continue

            if char == '>' and self.peek() == '=':
                self.tokens.append(Token(TokenType.GREATER_EQUAL, '>=', start_line, start_col))
                self.advance()
                self.advance()
                continue

            if char == '-' and self.peek() == '>':
                self.tokens.append(Token(TokenType.ARROW, '->', start_line, start_col))
                self.advance()
                self.advance()
                continue

            # Single-character operators
            single_char_tokens = {
                '=': TokenType.ASSIGN,
                '+': TokenType.PLUS,
                '-': TokenType.MINUS,
                '*': TokenType.MULTIPLY,
                '/': TokenType.DIVIDE,
                '<': TokenType.LESS_THAN,
                '>': TokenType.GREATER_THAN,
                '(': TokenType.LPAREN,
                ')': TokenType.RPAREN,
                '{': TokenType.LBRACE,
                '}': TokenType.RBRACE,
                '[': TokenType.LBRACKET,
                ']': TokenType.RBRACKET,
                ',': TokenType.COMMA,
                '.': TokenType.DOT,
                ';': TokenType.SEMICOLON,
                ':': TokenType.COLON,
            }

            if char in single_char_tokens:
                self.tokens.append(Token(single_char_tokens[char], char, start_line, start_col))
                self.advance()
                continue

            # Unknown character
            print(f"Warning: Unknown character '{char}' at line {self.line}, column {self.column}")
            self.advance()

        self.tokens.append(Token(TokenType.EOF, None, self.line, self.column))
        return self.tokens

# ===============================================
# Simple C Code Generator
# ===============================================

class SimpleCCodeGen:
    def __init__(self, tokens: List[Token]):
        self.tokens = tokens
        self.pos = 0
        self.c_code = []
        self.includes = set(['#include "runtime/runtime.h"'])
        self.indent_level = 0

    def emit(self, code: str):
        indent = '    ' * self.indent_level
        self.c_code.append(indent + code)

    def current_token(self) -> Token:
        if self.pos >= len(self.tokens):
            return self.tokens[-1]
        return self.tokens[self.pos]

    def advance(self):
        self.pos += 1

    def generate(self) -> str:
        """Simple code generation - just handle YAZDIR for now"""
        self.emit("int main(int argc, char** argv) {")
        self.indent_level += 1

        while self.current_token().type != TokenType.EOF:
            if self.current_token().type == TokenType.YAZDIR:
                self.advance()
                if self.current_token().type == TokenType.STRING_LITERAL:
                    value = self.current_token().value
                    self.emit(f'mlp_yazdir("{value}");')
                    self.advance()
            else:
                # Skip unknown tokens (comments, etc.)
                self.advance()

        self.emit("return 0;")
        self.indent_level -= 1
        self.emit("}")

        # Build final code
        result = '\n'.join(self.includes) + '\n\n' + '\n'.join(self.c_code)
        return result

# ===============================================
# Main Compiler
# ===============================================

def compile_mlp_file(input_file: str, output_file: str):
    """Compile MLP file to executable"""
    print(f"[Seed Compiler] Compiling {input_file}...")

    # Read source
    with open(input_file, 'r', encoding='utf-8') as f:
        source = f.read()

    # Tokenize
    lexer = Lexer(source)
    tokens = lexer.tokenize()
    print(f"[Seed Compiler] Tokenized: {len(tokens)} tokens")

    # Generate C code
    codegen = SimpleCCodeGen(tokens)
    c_code = codegen.generate()

    # Write C file
    c_file = output_file + '.c'
    with open(c_file, 'w') as f:
        f.write(c_code)
    print(f"[Seed Compiler] Generated C code: {c_file}")

    # Compile runtime library
    runtime_c = 'runtime/runtime.c'
    if os.path.exists(runtime_c):
        print("[Seed Compiler] Compiling runtime library...")
        subprocess.run(['gcc', '-c', runtime_c, '-o', 'runtime.o'], check=True)

    # Compile C to binary
    print(f"[Seed Compiler] Compiling to binary: {output_file}")
    subprocess.run([
        'gcc',
        c_file,
        'runtime.o',
        '-o', output_file,
        '-I.', '-std=c99'
    ], check=True)

    print(f"[Seed Compiler] ✅ Success! Created: {output_file}")

def main():
    if len(sys.argv) < 2:
        print("Usage: python3 seed-compiler.py <input.mlp> [output]")
        print("   or: python3 seed-compiler.py <input.mlp> -o <output>")
        sys.exit(1)

    input_file = sys.argv[1]
    output_file = 'output'

    # Parse output file: supports both "-o output" and just "output"
    if len(sys.argv) >= 3:
        if sys.argv[2] == '-o' and len(sys.argv) >= 4:
            output_file = sys.argv[3]
        elif sys.argv[2] != '-o':
            output_file = sys.argv[2]

    try:
        compile_mlp_file(input_file, output_file)
    except Exception as e:
        print(f"[Seed Compiler] ❌ Error: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)

if __name__ == '__main__':
    main()
