# Stage 1: Simple MLP Compiler

This directory contains the **Stage 1** compiler for MLP - the first compiler written **in MLP itself**!

## Overview

**Stage 1** is a minimal MLP compiler that demonstrates the core concept of bootstrapping:
- Written in MLP
- Compiled by Stage 0 (Python seed compiler)
- Simulates basic compilation phases

## What is Stage 1?

Stage 1 is a **proof of concept** compiler that:
1. Demonstrates MLP code can define a compiler structure
2. Shows the compilation pipeline (Lexer → Parser → CodeGen)
3. Provides foundation for Stage 2 (full self-hosting)

## Current Features

### Implemented
- ✅ Compiler class structure
- ✅ Compilation phase simulation
- ✅ Basic lexical analysis concept
- ✅ Parsing concept
- ✅ Code generation concept
- ✅ Compiles to working binary

### Not Yet Implemented
- ❌ Real file I/O
- ❌ Actual lexer/tokenizer
- ❌ AST building
- ❌ Real code generation
- ❌ Multiple source file support

## Building

Compile Stage 1 using the seed compiler:

```bash
python3 stage0/seed-compiler.py stage1/compiler-v1.mlp stage1-compiler
```

## Running

Execute the compiled binary:

```bash
./stage1-compiler
```

## Output

The Stage 1 compiler simulates a compilation process:

```
=== Testing Stage 1 Compiler ===

Starting compilation...
Phase 1: Lexical analysis...
  - Tokenizing source code...
  - Found 5 tokens

Phase 2: Parsing...
  - Building AST...
  - Validated 3 statements

Phase 3: Code generation...
  - Generating C code...

=== Generated C Code ===
#include "runtime/runtime.h"

int main() {
    mlp_yazdir("Hello from MLP!");
    return 0;
}

=== Compilation Result ===
Status: SUCCESS
Lines processed: 42
Compilation complete!
```

## Architecture

```
SimpleCompiler (class)
├── Fields
│   ├── source (METIN) - Source code
│   └── lineCount (SAYISAL) - Line counter
│
└── Methods
    ├── constructor() - Initialize compiler
    ├── compile() -> METIN - Main compilation
    ├── countLines() -> SAYISAL - Count source lines
    ├── isAtEnd() -> ZITLIK - Check if parsing complete
    ├── peek() -> METIN - Peek next character
    └── advance() - Move to next character
```

## Limitations

Due to seed compiler constraints:
- No module system (KULLAN)
- No arrays or generics
- No file I/O from MLP code
- No object instantiation (yet)
- Hardcoded source code

## Next Steps (Stage 2)

Stage 2 will add:
1. Real lexer with tokenization
2. Basic parser for simple statements
3. Actual C code generation
4. File I/O support
5. Self-compilation capability

## Significance

**This is MLP compiling MLP code!** 🚀

Even though it's a simulation, Stage 1 proves:
- MLP syntax can express compiler logic
- Seed compiler can compile MLP code to working binaries
- The bootstrap process is feasible

## Files

- `compiler-v1.mlp` - Stage 1 compiler source code (MLP)
- `../stage1-compiler` - Compiled binary (generated)
- `../stage1-compiler.c` - Generated C code (intermediate)
- `README.md` - This file

## Contributing

Stage 1 is a minimal proof of concept. For contributing to the full compiler, see the main [CONTRIBUTING.md](../CONTRIBUTING.md).

---

**Stage 1 Complete!** Next stop: Stage 2 (Real Compiler) 🎯
