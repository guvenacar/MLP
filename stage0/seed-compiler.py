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
    # Keywords - English
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
    KULLAN = auto()  # import (Turkish: use)
    YAZDIR = auto()  # print (Turkish: write)

    # Keywords - Turkish
    SINIF = auto()      # class
    KURUCU = auto()     # constructor
    IŞLEÇ = auto()      # method/function
    EĞER = auto()       # if
    İSE = auto()        # then
    DEĞILSE = auto()    # else
    YOKSA = auto()      # elsif
    DONGU = auto()      # while/loop
    HER = auto()        # for/each
    İÇİNDE = auto()     # in
    DÖNÜŞ = auto()      # return
    DEĞIŞKEN = auto()   # var
    BU = auto()         # this
    YENİ = auto()       # new
    SON = auto()        # end

    # Types - English
    STRING = auto()
    NUMBER = auto()
    BOOL = auto()
    DYNAMIC = auto()
    DICT = auto()

    # Types - Turkish
    METIN = auto()      # string
    SAYISAL = auto()    # number
    ZITLIK = auto()     # bool
    DİNAMİK = auto()    # dynamic
    SÖZLÜK = auto()     # dict
    DİZİ = auto()       # array

    # Literals - English
    IDENTIFIER = auto()
    STRING_LITERAL = auto()
    INTEGER_LITERAL = auto()
    FLOAT_LITERAL = auto()
    TRUE = auto()
    FALSE = auto()
    NULL = auto()

    # Literals - Turkish
    DOĞRU = auto()      # true
    YANLIŞ = auto()     # false
    HİÇLİK = auto()     # null

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
# AST Node Classes
# ===============================================

class ASTNode:
    """Base class for all AST nodes"""
    pass

@dataclass
class Program(ASTNode):
    """Root node - contains all statements"""
    statements: List[ASTNode]

@dataclass
class VarDecl(ASTNode):
    """Variable declaration: var x = 10"""
    name: str
    type_name: Optional[str]
    value: Optional[ASTNode]

@dataclass
class Assignment(ASTNode):
    """Assignment: x = value"""
    target: str
    value: ASTNode

@dataclass
class BinaryOp(ASTNode):
    """Binary operation: left op right"""
    left: ASTNode
    op: str
    right: ASTNode

@dataclass
class Identifier(ASTNode):
    """Variable reference"""
    name: str

@dataclass
class Literal(ASTNode):
    """Literal value (string, number, bool)"""
    value: Any
    type: str  # 'string', 'int', 'float', 'bool'

@dataclass
class MethodDef(ASTNode):
    """Method definition"""
    name: str
    params: List[tuple]  # [(name, type), ...]
    return_type: Optional[str]
    body: List[ASTNode]
    is_constructor: bool = False
    is_override: bool = False

@dataclass
class ClassDef(ASTNode):
    """Class definition"""
    name: str
    fields: List[tuple]  # [(name, type), ...]
    methods: List[MethodDef]

@dataclass
class MethodCall(ASTNode):
    """Method call: obj.method(args) or method(args)"""
    object: Optional[ASTNode]  # None for standalone functions
    method_name: str
    args: List[ASTNode]

@dataclass
class IfStatement(ASTNode):
    """If statement"""
    condition: ASTNode
    then_body: List[ASTNode]
    else_body: Optional[List[ASTNode]]

@dataclass
class WhileStatement(ASTNode):
    """While loop"""
    condition: ASTNode
    body: List[ASTNode]

@dataclass
class ReturnStatement(ASTNode):
    """Return statement"""
    value: Optional[ASTNode]

@dataclass
class PrintStatement(ASTNode):
    """YAZDIR statement"""
    value: ASTNode

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
            # English keywords
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

            # Turkish keywords
            'SINIF': TokenType.SINIF,
            'KURUCU': TokenType.KURUCU,
            'IŞLEÇ': TokenType.IŞLEÇ,
            'EĞER': TokenType.EĞER,
            'İSE': TokenType.İSE,
            'DEĞILSE': TokenType.DEĞILSE,
            'YOKSA': TokenType.YOKSA,
            'DONGU': TokenType.DONGU,
            'HER': TokenType.HER,
            'İÇİNDE': TokenType.İÇİNDE,
            'DÖNÜŞ': TokenType.DÖNÜŞ,
            'DEĞIŞKEN': TokenType.DEĞIŞKEN,
            'BU': TokenType.BU,
            'YENİ': TokenType.YENİ,
            'SON': TokenType.SON,

            # Turkish types
            'METIN': TokenType.METIN,
            'SAYISAL': TokenType.SAYISAL,
            'ZITLIK': TokenType.ZITLIK,
            'DİNAMİK': TokenType.DİNAMİK,
            'SÖZLÜK': TokenType.SÖZLÜK,
            'DİZİ': TokenType.DİZİ,

            # Turkish literals
            'DOĞRU': TokenType.DOĞRU,
            'YANLIŞ': TokenType.YANLIŞ,
            'HİÇLİK': TokenType.HİÇLİK,
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
# Parser
# ===============================================

class Parser:
    def __init__(self, tokens: List[Token]):
        self.tokens = tokens
        self.pos = 0

    def current(self) -> Token:
        if self.pos >= len(self.tokens):
            return self.tokens[-1]
        return self.tokens[self.pos]

    def peek(self, offset=1) -> Token:
        pos = self.pos + offset
        if pos >= len(self.tokens):
            return self.tokens[-1]
        return self.tokens[pos]

    def advance(self):
        self.pos += 1

    def expect(self, token_type: TokenType) -> Token:
        if self.current().type != token_type:
            raise Exception(f"Expected {token_type}, got {self.current().type} at line {self.current().line}")
        token = self.current()
        self.advance()
        return token

    def parse(self) -> Program:
        """Parse tokens into AST"""
        statements = []
        while self.current().type != TokenType.EOF:
            stmt = self.parse_statement()
            if stmt:
                statements.append(stmt)
        return Program(statements)

    def parse_statement(self) -> Optional[ASTNode]:
        """Parse a single statement"""
        current = self.current()

        # Skip comments and newlines
        if current.type in [TokenType.NEWLINE]:
            self.advance()
            return None

        # Variable declaration (var or DEĞIŞKEN)
        if current.type in [TokenType.VAR, TokenType.DEĞIŞKEN]:
            return self.parse_var_decl()

        # Class definition (class or SINIF)
        if current.type in [TokenType.CLASS, TokenType.SINIF]:
            return self.parse_class()

        # If statement (if or EĞER)
        if current.type in [TokenType.IF, TokenType.EĞER]:
            return self.parse_if()

        # While statement (while or DONGU)
        if current.type in [TokenType.WHILE, TokenType.DONGU]:
            return self.parse_while()

        # Return statement (return or DÖNÜŞ)
        if current.type in [TokenType.RETURN, TokenType.DÖNÜŞ]:
            return self.parse_return()

        # Print statement (YAZDIR)
        if current.type == TokenType.YAZDIR:
            return self.parse_print()

        # Assignment or method call
        if current.type == TokenType.IDENTIFIER:
            # Look ahead to determine if it's assignment or method call
            if self.peek().type == TokenType.ASSIGN:
                return self.parse_assignment()
            elif self.peek().type == TokenType.LPAREN:
                return self.parse_method_call()
            else:
                # Just skip unknown identifier statements for now
                self.advance()
                return None

        # Skip unknown tokens
        self.advance()
        return None

    def parse_var_decl(self) -> VarDecl:
        """Parse variable declaration: var x = 10 or DEĞIŞKEN x METIN = "hello" """
        # Accept both English and Turkish
        if self.current().type == TokenType.VAR:
            self.advance()
        elif self.current().type == TokenType.DEĞIŞKEN:
            self.advance()

        name = self.expect(TokenType.IDENTIFIER).value

        type_name = None
        value = None

        # Optional type (English or Turkish)
        type_tokens = [
            TokenType.STRING, TokenType.NUMBER, TokenType.BOOL, TokenType.DYNAMIC, TokenType.DICT,
            TokenType.METIN, TokenType.SAYISAL, TokenType.ZITLIK, TokenType.DİNAMİK, TokenType.SÖZLÜK, TokenType.DİZİ
        ]
        if self.current().type in type_tokens:
            type_name = self.current().value
            self.advance()

        # Optional initialization
        if self.current().type == TokenType.ASSIGN:
            self.advance()
            value = self.parse_expression()

        return VarDecl(name, type_name, value)

    def parse_assignment(self) -> Assignment:
        """Parse assignment: x = value"""
        name = self.expect(TokenType.IDENTIFIER).value
        self.expect(TokenType.ASSIGN)
        value = self.parse_expression()
        return Assignment(name, value)

    def parse_class(self) -> ClassDef:
        """Parse class definition (class or SINIF)"""
        # Accept both English and Turkish
        if self.current().type == TokenType.CLASS:
            self.advance()
        elif self.current().type == TokenType.SINIF:
            self.advance()

        name = self.expect(TokenType.IDENTIFIER).value

        fields = []
        methods = []

        # Parse class body until 'end' or 'SON'
        while self.current().type not in [TokenType.END, TokenType.SON, TokenType.EOF]:
            # Field declaration (name type) - support Turkish types
            type_tokens_all = [
                TokenType.STRING, TokenType.NUMBER, TokenType.BOOL, TokenType.DYNAMIC, TokenType.DICT,
                TokenType.METIN, TokenType.SAYISAL, TokenType.ZITLIK, TokenType.DİNAMİK, TokenType.SÖZLÜK, TokenType.DİZİ
            ]
            if self.current().type == TokenType.IDENTIFIER and self.peek().type in type_tokens_all:
                field_name = self.expect(TokenType.IDENTIFIER).value
                field_type = self.current().value
                self.advance()
                fields.append((field_name, field_type))

            # Constructor (constructor or KURUCU)
            elif self.current().type in [TokenType.CONSTRUCTOR, TokenType.KURUCU]:
                self.advance()  # Consume token
                method = self.parse_method(is_constructor=True)
                methods.append(method)

            # Method (method or IŞLEÇ)
            elif self.current().type in [TokenType.METHOD, TokenType.IŞLEÇ, TokenType.OVERRIDE]:
                is_override = self.current().type == TokenType.OVERRIDE
                if is_override:
                    self.advance()
                method = self.parse_method(is_override=is_override)
                methods.append(method)

            else:
                self.advance()

        # Expect end or SON
        if self.current().type == TokenType.END:
            self.advance()
        elif self.current().type == TokenType.SON:
            self.advance()
        return ClassDef(name, fields, methods)

    def parse_method(self, is_constructor=False, is_override=False) -> MethodDef:
        """Parse method definition (method/IŞLEÇ)"""
        if not is_constructor:
            # Expect METHOD or IŞLEÇ
            if self.current().type == TokenType.METHOD:
                self.advance()
            elif self.current().type == TokenType.IŞLEÇ:
                self.advance()
            name = self.expect(TokenType.IDENTIFIER).value
        else:
            # Constructor - no name, directly to parameters
            name = "constructor"

        # Parameters
        self.expect(TokenType.LPAREN)
        params = []
        type_tokens_all = [
            TokenType.STRING, TokenType.NUMBER, TokenType.BOOL, TokenType.DYNAMIC, TokenType.DICT,
            TokenType.METIN, TokenType.SAYISAL, TokenType.ZITLIK, TokenType.DİNAMİK, TokenType.SÖZLÜK, TokenType.DİZİ
        ]
        while self.current().type != TokenType.RPAREN:
            # Support both "TYPE name" and "name TYPE" syntax
            param_type = None
            param_name = None

            # Check if type comes first (TYPE name)
            if self.current().type in type_tokens_all:
                param_type = self.current().value
                self.advance()
                param_name = self.expect(TokenType.IDENTIFIER).value
            # Otherwise expect name first (name TYPE or just name)
            else:
                param_name = self.expect(TokenType.IDENTIFIER).value
                if self.current().type in type_tokens_all:
                    param_type = self.current().value
                    self.advance()

            params.append((param_name, param_type))

            if self.current().type == TokenType.COMMA:
                self.advance()
        self.expect(TokenType.RPAREN)

        # Return type
        return_type = None
        if self.current().type == TokenType.ARROW:
            self.advance()
            return_type = self.current().value
            self.advance()

        # Body
        body = []
        while self.current().type not in [TokenType.END, TokenType.SON, TokenType.EOF]:
            stmt = self.parse_statement()
            if stmt:
                body.append(stmt)

        # Expect END or SON
        if self.current().type == TokenType.END:
            self.advance()
        elif self.current().type == TokenType.SON:
            self.advance()
        return MethodDef(name, params, return_type, body, is_constructor, is_override)

    def parse_if(self) -> IfStatement:
        """Parse if statement (if/EĞER)"""
        # Already consumed by parse_statement
        condition = self.parse_expression()

        then_body = []
        end_tokens = [TokenType.ELSE, TokenType.DEĞILSE, TokenType.END, TokenType.SON, TokenType.EOF]
        while self.current().type not in end_tokens:
            stmt = self.parse_statement()
            if stmt:
                then_body.append(stmt)

        else_body = None
        if self.current().type in [TokenType.ELSE, TokenType.DEĞILSE]:
            self.advance()
            else_body = []
            while self.current().type not in [TokenType.END, TokenType.SON, TokenType.EOF]:
                stmt = self.parse_statement()
                if stmt:
                    else_body.append(stmt)

        # Expect END or SON
        if self.current().type == TokenType.END:
            self.advance()
        elif self.current().type == TokenType.SON:
            self.advance()
        return IfStatement(condition, then_body, else_body)

    def parse_while(self) -> WhileStatement:
        """Parse while loop (while/DONGU)"""
        # Already consumed by parse_statement
        condition = self.parse_expression()

        body = []
        while self.current().type not in [TokenType.END, TokenType.SON, TokenType.EOF]:
            stmt = self.parse_statement()
            if stmt:
                body.append(stmt)

        # Expect END or SON
        if self.current().type == TokenType.END:
            self.advance()
        elif self.current().type == TokenType.SON:
            self.advance()
        return WhileStatement(condition, body)

    def parse_return(self) -> ReturnStatement:
        """Parse return statement (return/DÖNÜŞ)"""
        # Consume RETURN or DÖNÜŞ token
        if self.current().type in [TokenType.RETURN, TokenType.DÖNÜŞ]:
            self.advance()

        value = None
        if self.current().type not in [TokenType.NEWLINE, TokenType.EOF, TokenType.END, TokenType.SON]:
            value = self.parse_expression()
        return ReturnStatement(value)

    def parse_print(self) -> PrintStatement:
        """Parse YAZDIR statement"""
        self.expect(TokenType.YAZDIR)
        value = self.parse_expression()
        return PrintStatement(value)

    def parse_method_call(self) -> MethodCall:
        """Parse method call: method(args)"""
        name = self.expect(TokenType.IDENTIFIER).value
        self.expect(TokenType.LPAREN)

        args = []
        while self.current().type != TokenType.RPAREN:
            args.append(self.parse_expression())
            if self.current().type == TokenType.COMMA:
                self.advance()

        self.expect(TokenType.RPAREN)
        return MethodCall(None, name, args)

    def parse_expression(self) -> ASTNode:
        """Parse expression (simplified - just handle basic cases)"""
        return self.parse_additive()

    def parse_additive(self) -> ASTNode:
        """Parse addition/subtraction"""
        left = self.parse_multiplicative()

        while self.current().type in [TokenType.PLUS, TokenType.MINUS]:
            op = '+' if self.current().type == TokenType.PLUS else '-'
            self.advance()
            right = self.parse_multiplicative()
            left = BinaryOp(left, op, right)

        return left

    def parse_multiplicative(self) -> ASTNode:
        """Parse multiplication/division"""
        left = self.parse_primary()

        while self.current().type in [TokenType.MULTIPLY, TokenType.DIVIDE]:
            op = '*' if self.current().type == TokenType.MULTIPLY else '/'
            self.advance()
            right = self.parse_primary()
            left = BinaryOp(left, op, right)

        return left

    def parse_primary(self) -> ASTNode:
        """Parse primary expression"""
        current = self.current()

        # String literal
        if current.type == TokenType.STRING_LITERAL:
            self.advance()
            return Literal(current.value, 'string')

        # Number literal
        if current.type in [TokenType.INTEGER_LITERAL, TokenType.FLOAT_LITERAL]:
            self.advance()
            lit_type = 'int' if current.type == TokenType.INTEGER_LITERAL else 'float'
            return Literal(current.value, lit_type)

        # Boolean literal
        if current.type in [TokenType.TRUE, TokenType.FALSE]:
            self.advance()
            return Literal(current.type == TokenType.TRUE, 'bool')

        # Identifier or method call
        if current.type == TokenType.IDENTIFIER:
            name = current.value
            self.advance()

            # Method call
            if self.current().type == TokenType.LPAREN:
                self.expect(TokenType.LPAREN)
                args = []
                while self.current().type != TokenType.RPAREN:
                    args.append(self.parse_expression())
                    if self.current().type == TokenType.COMMA:
                        self.advance()
                self.expect(TokenType.RPAREN)
                return MethodCall(None, name, args)

            # Just identifier
            return Identifier(name)

        # Parenthesized expression
        if current.type == TokenType.LPAREN:
            self.advance()
            expr = self.parse_expression()
            self.expect(TokenType.RPAREN)
            return expr

        # Unknown - just return null literal
        self.advance()
        return Literal(None, 'null')

# ===============================================
# AST-based C Code Generator
# ===============================================

class CCodeGenerator:
    def __init__(self, ast: Program):
        self.ast = ast
        self.c_code = []
        self.includes = set(['#include "runtime/runtime.h"', '#include <stdbool.h>'])
        self.indent_level = 0
        self.class_structs = []  # Generated struct definitions
        self.class_methods = []  # Generated method implementations
        self.current_class = None  # Track current class being generated
        self.current_class_fields = set()  # Track fields of current class

    def emit(self, code: str):
        indent = '    ' * self.indent_level
        self.c_code.append(indent + code)

    def generate(self) -> str:
        """Generate C code from AST"""
        # Process all top-level statements
        has_main = False
        for stmt in self.ast.statements:
            if isinstance(stmt, ClassDef):
                self.generate_class(stmt)
            elif isinstance(stmt, PrintStatement):
                # Top-level print - put in main
                has_main = True

        # Generate main function if we have top-level statements
        if has_main or any(not isinstance(stmt, ClassDef) for stmt in self.ast.statements):
            self.emit("int main(int argc, char** argv) {")
            self.indent_level += 1

            for stmt in self.ast.statements:
                if not isinstance(stmt, ClassDef):
                    self.generate_statement(stmt)

            self.emit("return 0;")
            self.indent_level -= 1
            self.emit("}")

        # Build final code
        result = '\n'.join(self.includes) + '\n\n'
        if self.class_structs:
            result += '\n'.join(self.class_structs) + '\n\n'
        if self.class_methods:
            result += '\n'.join(self.class_methods) + '\n\n'
        result += '\n'.join(self.c_code)
        return result

    def generate_class(self, node: ClassDef):
        """Generate C struct for class"""
        struct_code = []
        struct_code.append(f"typedef struct {node.name} {{")

        # Store class context
        self.current_class = node.name
        self.current_class_fields = set()

        # Fields
        for field_name, field_type in node.fields:
            c_type = self.mlp_type_to_c(field_type) if field_type else 'void*'
            struct_code.append(f"    {c_type} {field_name};")
            self.current_class_fields.add(field_name)

        struct_code.append(f"}} {node.name};")
        self.class_structs.append('\n'.join(struct_code))

        # Methods
        for method in node.methods:
            self.generate_method(node.name, method)

        # Clear class context
        self.current_class = None
        self.current_class_fields = set()

    def generate_method(self, class_name: str, method: MethodDef):
        """Generate C function for method"""
        # Build function signature
        return_type = self.mlp_type_to_c(method.return_type) if method.return_type else 'void'

        # Parameters - first param is always 'this' pointer for non-constructors
        params = []
        if not method.is_constructor:
            params.append(f"{class_name}* this")

        for param_name, param_type in method.params:
            c_type = self.mlp_type_to_c(param_type) if param_type else 'void*'
            params.append(f"{c_type} {param_name}")

        params_str = ', '.join(params) if params else 'void'
        func_name = f"{class_name}_{method.name}"

        method_code = []
        method_code.append(f"{return_type} {func_name}({params_str}) {{")

        # Body
        old_c_code = self.c_code
        old_indent = self.indent_level
        self.c_code = []
        self.indent_level = 1

        for stmt in method.body:
            self.generate_statement(stmt)

        method_code.extend(self.c_code)
        method_code.append("}")

        self.c_code = old_c_code
        self.indent_level = old_indent
        self.class_methods.append('\n'.join(method_code))

    def generate_statement(self, node: ASTNode):
        """Generate code for a statement"""
        if isinstance(node, VarDecl):
            # Infer type from value if not specified
            if node.type_name:
                c_type = self.mlp_type_to_c(node.type_name)
            elif node.value:
                c_type = self.infer_c_type(node.value)
            else:
                c_type = 'void*'

            if node.value:
                value_code = self.generate_expression(node.value)
                self.emit(f"{c_type} {node.name} = {value_code};")
            else:
                self.emit(f"{c_type} {node.name};")

        elif isinstance(node, Assignment):
            value_code = self.generate_expression(node.value)
            self.emit(f"{node.target} = {value_code};")

        elif isinstance(node, PrintStatement):
            value_code = self.generate_expression(node.value)
            # Check if it's a string literal or needs conversion
            if isinstance(node.value, Literal) and node.value.type == 'string':
                self.emit(f'mlp_yazdir({value_code});')
            else:
                self.emit(f'printf("%s\\n", {value_code});')

        elif isinstance(node, IfStatement):
            cond = self.generate_expression(node.condition)
            self.emit(f"if ({cond}) {{")
            self.indent_level += 1
            for stmt in node.then_body:
                self.generate_statement(stmt)
            self.indent_level -= 1
            if node.else_body:
                self.emit("} else {")
                self.indent_level += 1
                for stmt in node.else_body:
                    self.generate_statement(stmt)
                self.indent_level -= 1
            self.emit("}")

        elif isinstance(node, WhileStatement):
            cond = self.generate_expression(node.condition)
            self.emit(f"while ({cond}) {{")
            self.indent_level += 1
            for stmt in node.body:
                self.generate_statement(stmt)
            self.indent_level -= 1
            self.emit("}")

        elif isinstance(node, ReturnStatement):
            if node.value:
                value_code = self.generate_expression(node.value)
                self.emit(f"return {value_code};")
            else:
                self.emit("return;")

        elif isinstance(node, MethodCall):
            # Standalone method call (not in expression)
            call_code = self.generate_expression(node)
            self.emit(f"{call_code};")

    def generate_expression(self, node: ASTNode) -> str:
        """Generate code for an expression"""
        if isinstance(node, Literal):
            if node.type == 'string':
                # Escape quotes in string
                escaped = node.value.replace('"', '\\"')
                return f'"{escaped}"'
            elif node.type == 'int':
                return str(node.value)
            elif node.type == 'float':
                return str(node.value)
            elif node.type == 'bool':
                return 'true' if node.value else 'false'
            else:
                return 'NULL'

        elif isinstance(node, Identifier):
            # Check if this is a class field - add this-> prefix
            if node.name in self.current_class_fields:
                return f"this->{node.name}"
            return node.name

        elif isinstance(node, BinaryOp):
            left = self.generate_expression(node.left)
            right = self.generate_expression(node.right)
            return f"({left} {node.op} {right})"

        elif isinstance(node, MethodCall):
            args = ', '.join([self.generate_expression(arg) for arg in node.args])
            return f"{node.method_name}({args})"

        else:
            return "NULL"

    def mlp_type_to_c(self, mlp_type: str) -> str:
        """Convert MLP type to C type (English or Turkish)"""
        type_map = {
            # English
            'string': 'char*',
            'number': 'double',
            'bool': 'bool',
            'dynamic': 'void*',
            'dict': 'mlp_dict_t*',
            # Turkish
            'METIN': 'char*',
            'SAYISAL': 'double',
            'ZITLIK': 'bool',
            'DİNAMİK': 'void*',
            'SÖZLÜK': 'mlp_dict_t*',
            'DİZİ': 'void*',  # array - for now
        }
        return type_map.get(mlp_type, 'void*')

    def infer_c_type(self, node: ASTNode) -> str:
        """Infer C type from expression"""
        if isinstance(node, Literal):
            if node.type == 'int':
                return 'int'
            elif node.type == 'float':
                return 'double'
            elif node.type == 'string':
                return 'char*'
            elif node.type == 'bool':
                return 'bool'
        elif isinstance(node, BinaryOp):
            # For arithmetic operations, default to double
            if node.op in ['+', '-', '*', '/']:
                left_type = self.infer_c_type(node.left)
                right_type = self.infer_c_type(node.right)
                # If either side is double, result is double
                if left_type == 'double' or right_type == 'double':
                    return 'double'
                # If both are int, result is int
                if left_type == 'int' and right_type == 'int':
                    return 'int'
                # Default to double for numeric operations
                return 'double'
            # For comparison ops, result is bool
            elif node.op in ['==', '!=', '<', '>', '<=', '>=']:
                return 'bool'
            # For other ops, try to infer from left operand
            return self.infer_c_type(node.left)
        elif isinstance(node, Identifier):
            # Check if it's a class field - look up its type
            # For now, default to double for unknown identifiers in expressions
            return 'double'
        return 'void*'

# ===============================================
# Main Compiler
# ===============================================

def compile_mlp_file(input_file: str, output_file: str, gui_backend: str = None):
    """Compile MLP file to executable"""
    print(f"[Seed Compiler] Compiling {input_file}...")

    # Read source
    with open(input_file, 'r', encoding='utf-8') as f:
        source = f.read()

    # Tokenize
    lexer = Lexer(source)
    tokens = lexer.tokenize()
    print(f"[Seed Compiler] Tokenized: {len(tokens)} tokens")

    # Parse into AST
    parser = Parser(tokens)
    ast = parser.parse()
    print(f"[Seed Compiler] Parsed: {len(ast.statements)} top-level statements")

    # Generate C code from AST
    codegen = CCodeGenerator(ast)
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

    # Compile GUI backend if specified
    gui_objects = []
    gui_libs = []
    if gui_backend == 'sdl2':
        gui_sdl_c = 'runtime/gui_sdl.c'
        if os.path.exists(gui_sdl_c):
            print("[Seed Compiler] Compiling SDL2 GUI backend...")
            # Get SDL2 flags
            try:
                sdl_cflags = subprocess.check_output(['sdl2-config', '--cflags'], text=True).strip().split()
                sdl_libs = subprocess.check_output(['sdl2-config', '--libs'], text=True).strip().split()
            except (subprocess.CalledProcessError, FileNotFoundError):
                print("[Seed Compiler] Warning: sdl2-config not found, using default flags")
                sdl_cflags = ['-I/usr/include/SDL2']
                sdl_libs = ['-lSDL2']

            # Compile GUI SDL
            subprocess.run(['gcc', '-c', gui_sdl_c, '-o', 'gui_sdl.o'] + sdl_cflags, check=True)
            gui_objects.append('gui_sdl.o')
            gui_libs.extend(sdl_libs)
            print("[Seed Compiler] SDL2 GUI backend compiled")
        else:
            print(f"[Seed Compiler] Warning: SDL2 GUI backend not found at {gui_sdl_c}")

    # Compile C to binary
    print(f"[Seed Compiler] Compiling to binary: {output_file}")
    compile_cmd = [
        'gcc',
        c_file,
        'runtime.o'
    ] + gui_objects + [
        '-o', output_file,
        '-I.', '-std=c99', '-lm'
    ] + gui_libs

    subprocess.run(compile_cmd, check=True)

    print(f"[Seed Compiler] ✅ Success! Created: {output_file}")
    if gui_backend:
        print(f"[Seed Compiler] GUI Backend: {gui_backend}")

def main():
    if len(sys.argv) < 2:
        print("Usage: python3 seed-compiler.py <input.mlp> [output] [--gui=sdl2]")
        print("   or: python3 seed-compiler.py <input.mlp> -o <output> [--gui=sdl2]")
        print("")
        print("Options:")
        print("  --gui=sdl2    Enable SDL2 GUI backend")
        sys.exit(1)

    input_file = sys.argv[1]
    output_file = 'output'
    gui_backend = None

    # Parse arguments
    i = 2
    while i < len(sys.argv):
        arg = sys.argv[i]

        if arg == '-o' and i + 1 < len(sys.argv):
            output_file = sys.argv[i + 1]
            i += 2
        elif arg.startswith('--gui='):
            gui_backend = arg.split('=')[1]
            i += 1
        elif not arg.startswith('-'):
            # Assume it's the output file
            output_file = arg
            i += 1
        else:
            print(f"Unknown option: {arg}")
            sys.exit(1)

    try:
        compile_mlp_file(input_file, output_file, gui_backend)
    except Exception as e:
        print(f"[Seed Compiler] ❌ Error: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)

if __name__ == '__main__':
    main()
