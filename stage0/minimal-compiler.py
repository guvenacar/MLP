#!/usr/bin/env python3
"""
Minimal MLP Compiler - Stage 0
Only essential features for bootstrapping
~200-300 lines total
"""

import sys
import os
import subprocess
from enum import Enum, auto
from dataclasses import dataclass
from typing import List, Optional, Any

# ===============================================
# TOKENS
# ===============================================

class TT(Enum):  # TokenType shortened
    # Keywords
    IŞLEÇ = auto()
    DÖNÜŞ = auto()
    YAZDIR = auto()
    EĞER = auto()
    İSE = auto()
    DEĞILSE = auto()
    SON = auto()

    # Types
    METIN = auto()
    SAYISAL = auto()
    ZITLIK = auto()

    # Literals
    STRING = auto()
    NUMBER = auto()
    IDENT = auto()

    # Operators
    ASSIGN = auto()  # =
    PLUS = auto()
    MINUS = auto()
    MUL = auto()
    DIV = auto()
    EQ = auto()     # ==
    NE = auto()     # !=
    LT = auto()
    GT = auto()

    # Delimiters
    LPAREN = auto()
    RPAREN = auto()
    LBRACE = auto()
    RBRACE = auto()
    SEMI = auto()
    ARROW = auto()  # ->

    EOF = auto()

@dataclass
class Token:
    type: TT
    value: Any

# ===============================================
# LEXER
# ===============================================

class Lexer:
    def __init__(self, source: str):
        self.source = source
        self.pos = 0
        self.keywords = {
            'İŞLEÇ': TT.IŞLEÇ,  # Note: İ (dotted I)
            'DÖNÜŞ': TT.DÖNÜŞ,
            'YAZDIR': TT.YAZDIR,
            'EĞER': TT.EĞER,
            'İSE': TT.İSE,
            'DEĞILSE': TT.DEĞILSE,
            'SON': TT.SON,
            'METIN': TT.METIN,
            'SAYISAL': TT.SAYISAL,
            'ZITLIK': TT.ZITLIK,
        }

    def peek(self) -> str:
        if self.pos >= len(self.source):
            return ''
        return self.source[self.pos]

    def advance(self) -> str:
        ch = self.peek()
        self.pos += 1
        return ch

    def skip_whitespace(self):
        while self.peek() in ' \t\n\r':
            self.advance()

    def skip_comment(self):
        # Skip -- comments
        if self.peek() == '-' and self.pos + 1 < len(self.source) and self.source[self.pos + 1] == '-':
            while self.peek() and self.peek() != '\n':
                self.advance()

    def read_string(self) -> str:
        self.advance()  # Skip opening "
        result = ''
        while self.peek() and self.peek() != '"':
            result += self.advance()
        self.advance()  # Skip closing "
        return result

    def read_number(self) -> float:
        result = ''
        while self.peek().isdigit() or self.peek() == '.':
            result += self.advance()
        return float(result) if '.' in result else int(result)

    def read_ident(self) -> str:
        result = ''
        while self.peek() and (self.peek().isalnum() or self.peek() in '_ĞÜŞİÖÇğüşıöç'):
            result += self.advance()
        return result

    def tokenize(self) -> List[Token]:
        tokens = []
        while self.pos < len(self.source):
            ch = self.peek()

            # Skip whitespace
            if ch in ' \t\n\r':
                self.advance()
                continue

            # Skip comments (-- to end of line)
            if ch == '-' and self.pos + 1 < len(self.source) and self.source[self.pos + 1] == '-':
                while self.peek() and self.peek() != '\n':
                    self.advance()
                continue

            if not ch:
                break

            # String
            if ch == '"':
                tokens.append(Token(TT.STRING, self.read_string()))

            # Number
            elif ch.isdigit():
                tokens.append(Token(TT.NUMBER, self.read_number()))

            # Identifier or keyword
            elif ch.isalpha() or ch in 'ĞÜŞİÖÇğüşıöç':
                ident = self.read_ident()
                tt = self.keywords.get(ident, TT.IDENT)
                tokens.append(Token(tt, ident))

            # Operators
            elif ch == '=':
                self.advance()
                if self.peek() == '=':
                    self.advance()
                    tokens.append(Token(TT.EQ, '=='))
                else:
                    tokens.append(Token(TT.ASSIGN, '='))

            elif ch == '!':
                self.advance()
                if self.peek() == '=':
                    self.advance()
                    tokens.append(Token(TT.NE, '!='))

            elif ch == '-':
                self.advance()
                if self.peek() == '>':
                    self.advance()
                    tokens.append(Token(TT.ARROW, '->'))
                elif self.peek() == '-':
                    # Comment: skip rest of line
                    while self.peek() and self.peek() != '\n':
                        self.advance()
                else:
                    tokens.append(Token(TT.MINUS, '-'))

            elif ch == '+':
                self.advance()
                tokens.append(Token(TT.PLUS, '+'))

            elif ch == '*':
                self.advance()
                tokens.append(Token(TT.MUL, '*'))

            elif ch == '/':
                self.advance()
                tokens.append(Token(TT.DIV, '/'))

            elif ch == '<':
                self.advance()
                tokens.append(Token(TT.LT, '<'))

            elif ch == '>':
                self.advance()
                tokens.append(Token(TT.GT, '>'))

            elif ch == '(':
                self.advance()
                tokens.append(Token(TT.LPAREN, '('))

            elif ch == ')':
                self.advance()
                tokens.append(Token(TT.RPAREN, ')'))

            elif ch == '{':
                self.advance()
                tokens.append(Token(TT.LBRACE, '{'))

            elif ch == '}':
                self.advance()
                tokens.append(Token(TT.RBRACE, '}'))

            elif ch == ';':
                self.advance()
                tokens.append(Token(TT.SEMI, ';'))

            else:
                self.advance()  # Skip unknown

        tokens.append(Token(TT.EOF, None))
        return tokens

# ===============================================
# AST NODES
# ===============================================

@dataclass
class Program:
    functions: List['Function']

@dataclass
class Function:
    name: str
    params: List[tuple]  # [(type, name), ...]
    return_type: Optional[str]
    body: List['Stmt']

@dataclass
class VarDecl:
    type: str
    name: str
    value: Optional['Expr'] = None

@dataclass
class Assign:
    name: str
    value: 'Expr'

@dataclass
class Return:
    value: Optional['Expr']

@dataclass
class Print:
    value: 'Expr'

@dataclass
class IfStmt:
    condition: 'Expr'
    then_body: List['Stmt']
    else_body: Optional[List['Stmt']]

@dataclass
class BinOp:
    left: 'Expr'
    op: str
    right: 'Expr'

@dataclass
class Ident:
    name: str

@dataclass
class Literal:
    value: Any

# ===============================================
# PARSER
# ===============================================

class Parser:
    def __init__(self, tokens: List[Token]):
        self.tokens = tokens
        self.pos = 0

    def peek(self) -> Token:
        return self.tokens[self.pos] if self.pos < len(self.tokens) else Token(TT.EOF, None)

    def advance(self) -> Token:
        tok = self.peek()
        self.pos += 1
        return tok

    def expect(self, tt: TT) -> Token:
        tok = self.advance()
        if tok.type != tt:
            raise Exception(f"Expected {tt}, got {tok.type}")
        return tok

    def parse(self) -> Program:
        functions = []
        while self.peek().type != TT.EOF:
            functions.append(self.parse_function())
        return Program(functions)

    def parse_function(self) -> Function:
        self.expect(TT.IŞLEÇ)
        name = self.expect(TT.IDENT).value

        # Parameters
        self.expect(TT.LPAREN)
        params = []
        while self.peek().type != TT.RPAREN:
            param_type = self.advance().value  # METIN, SAYISAL, etc
            param_name = self.expect(TT.IDENT).value
            params.append((param_type, param_name))
        self.expect(TT.RPAREN)

        # Return type
        return_type = None
        if self.peek().type == TT.ARROW:
            self.advance()
            return_type = self.advance().value

        # Body
        self.expect(TT.LBRACE)
        body = []
        while self.peek().type != TT.RBRACE:
            body.append(self.parse_statement())
        self.expect(TT.RBRACE)

        return Function(name, params, return_type, body)

    def parse_statement(self):
        tok = self.peek()

        # Variable declaration: METIN x;
        if tok.type in [TT.METIN, TT.SAYISAL, TT.ZITLIK]:
            var_type = self.advance().value
            var_name = self.expect(TT.IDENT).value
            self.expect(TT.SEMI)
            return VarDecl(var_type, var_name)

        # Assignment: x = value;
        if tok.type == TT.IDENT:
            name = self.advance().value
            self.expect(TT.ASSIGN)
            value = self.parse_expr()
            self.expect(TT.SEMI)
            return Assign(name, value)

        # Return: DÖNÜŞ value;
        if tok.type == TT.DÖNÜŞ:
            self.advance()
            value = self.parse_expr() if self.peek().type != TT.SEMI else None
            self.expect(TT.SEMI)
            return Return(value)

        # Print: YAZDIR(value);
        if tok.type == TT.YAZDIR:
            self.advance()
            self.expect(TT.LPAREN)
            value = self.parse_expr()
            self.expect(TT.RPAREN)
            self.expect(TT.SEMI)
            return Print(value)

        # If: EĞER cond İSE body SON
        if tok.type == TT.EĞER:
            self.advance()
            cond = self.parse_expr()
            self.expect(TT.İSE)
            then_body = []
            while self.peek().type not in [TT.DEĞILSE, TT.SON]:
                then_body.append(self.parse_statement())
            else_body = None
            if self.peek().type == TT.DEĞILSE:
                self.advance()
                else_body = []
                while self.peek().type != TT.SON:
                    else_body.append(self.parse_statement())
            self.expect(TT.SON)
            return IfStmt(cond, then_body, else_body)

        raise Exception(f"Unexpected token: {tok}")

    def parse_expr(self):
        # Simple expression: just additive
        return self.parse_additive()

    def parse_additive(self):
        left = self.parse_primary()
        while self.peek().type in [TT.PLUS, TT.MINUS, TT.EQ, TT.NE, TT.LT, TT.GT]:
            op = self.advance().value
            right = self.parse_primary()
            left = BinOp(left, op, right)
        return left

    def parse_primary(self):
        tok = self.peek()

        if tok.type == TT.STRING:
            self.advance()
            return Literal(tok.value)

        if tok.type == TT.NUMBER:
            self.advance()
            return Literal(tok.value)

        if tok.type == TT.IDENT:
            self.advance()
            return Ident(tok.value)

        if tok.type == TT.LPAREN:
            self.advance()
            expr = self.parse_expr()
            self.expect(TT.RPAREN)
            return expr

        raise Exception(f"Unexpected token in expression: {tok}")

# ===============================================
# CODE GENERATOR
# ===============================================

class CodeGen:
    def __init__(self, ast: Program):
        self.ast = ast
        self.code = []

    def emit(self, line: str):
        self.code.append(line)

    def generate(self) -> str:
        self.emit('#include <stdio.h>')
        self.emit('#include "runtime/runtime.h"')
        self.emit('')

        for func in self.ast.functions:
            self.gen_function(func)

        return '\n'.join(self.code)

    def gen_function(self, func: Function):
        # Return type
        ret_type = self.map_type(func.return_type) if func.return_type else 'void'

        # Special case for main function
        if func.name == 'main':
            ret_type = 'int'
            params_c = 'int argc, char** argv'
        else:
            # Parameters
            params_c = ', '.join([f"{self.map_type(t)} {n}" for t, n in func.params])

        self.emit(f'{ret_type} {func.name}({params_c}) {{')

        # Body
        for stmt in func.body:
            self.gen_statement(stmt)

        self.emit('}')
        self.emit('')

    def gen_statement(self, stmt):
        if isinstance(stmt, VarDecl):
            c_type = self.map_type(stmt.type)
            self.emit(f'    {c_type} {stmt.name};')

        elif isinstance(stmt, Assign):
            value_c = self.gen_expr(stmt.value)
            self.emit(f'    {stmt.name} = {value_c};')

        elif isinstance(stmt, Return):
            if stmt.value:
                value_c = self.gen_expr(stmt.value)
                self.emit(f'    return {value_c};')
            else:
                self.emit('    return;')

        elif isinstance(stmt, Print):
            value_c = self.gen_expr(stmt.value)
            self.emit(f'    mlp_yazdir({value_c});')

        elif isinstance(stmt, IfStmt):
            cond_c = self.gen_expr(stmt.condition)
            self.emit(f'    if ({cond_c}) {{')
            for s in stmt.then_body:
                self.gen_statement(s)
            if stmt.else_body:
                self.emit('    } else {')
                for s in stmt.else_body:
                    self.gen_statement(s)
            self.emit('    }')

    def gen_expr(self, expr):
        if isinstance(expr, Literal):
            if isinstance(expr.value, str):
                return f'"{expr.value}"'
            return str(expr.value)

        if isinstance(expr, Ident):
            return expr.name

        if isinstance(expr, BinOp):
            left = self.gen_expr(expr.left)
            right = self.gen_expr(expr.right)
            return f'({left} {expr.op} {right})'

        return 'NULL'

    def map_type(self, mlp_type: str) -> str:
        mapping = {
            'METIN': 'char*',
            'SAYISAL': 'double',
            'ZITLIK': 'bool',
        }
        return mapping.get(mlp_type, 'void*')

# ===============================================
# MAIN
# ===============================================

def compile_file(input_file: str, output_file: str):
    print(f"[Minimal Compiler] Compiling {input_file}...")

    # Read source
    with open(input_file, 'r', encoding='utf-8') as f:
        source = f.read()

    # Tokenize
    print("[Minimal Compiler] Starting lexer...")
    lexer = Lexer(source)
    print("[Minimal Compiler] Calling tokenize...")
    tokens = lexer.tokenize()
    print(f"[Minimal Compiler] Tokenized: {len(tokens)} tokens")

    # Parse
    parser = Parser(tokens)
    ast = parser.parse()
    print(f"[Minimal Compiler] Parsed: {len(ast.functions)} functions")

    # Generate C code
    codegen = CodeGen(ast)
    c_code = codegen.generate()

    # Write C file
    c_file = output_file + '.c'
    with open(c_file, 'w') as f:
        f.write(c_code)
    print(f"[Minimal Compiler] Generated: {c_file}")

    # Compile to binary
    runtime_c = 'runtime/runtime.c'
    if os.path.exists(runtime_c):
        subprocess.run(['gcc', '-c', runtime_c, '-o', 'runtime.o'], check=True)

    subprocess.run([
        'gcc', c_file, 'runtime.o',
        '-o', output_file,
        '-I.', '-std=c99'
    ], check=True)

    print(f"[Minimal Compiler] ✅ Success: {output_file}")

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print("Usage: python3 minimal-compiler.py <input.mlp> [output]")
        sys.exit(1)

    input_file = sys.argv[1]
    output_file = sys.argv[2] if len(sys.argv) > 2 else 'output'

    try:
        compile_file(input_file, output_file)
    except Exception as e:
        print(f"[Minimal Compiler] ❌ Error: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)
