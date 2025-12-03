# MLP Stage 1: Self-Hosting Plan
## Bootstrap Strategy: C → MLP Conversion

### Overview
Convert the C-based compiler to MLP, creating a self-hosting compiler that can compile itself.

---

## Stage 0: Bootstrap Compiler (COMPLETE ✅)
**Status**: All 64 modules implemented in C
**Location**: `melp/C/stage0/modules/`
**Total Lines**: ~50,000+ lines of C code
**Purpose**: Provides runtime and compiler infrastructure

### Completed Modules (64/64):
1. Core language features (variables, functions, types)
2. Advanced features (generics, traits, macros)
3. Type system (linear types, dependent types, type classes)
4. Concurrency (channels, async/await)
5. Memory management (ownership, smart pointers)

---

## Stage 1: MLP Compiler in MLP (IN PROGRESS 🔄)
**Goal**: Translate C compiler source to MLP
**Input**: `melp_monolitik/selfhost_from_c/*.c` (1,596 lines)
**Output**: `melp/MLP/stage1/*.mlp`

### Files to Convert:

#### 1. **lexer.mlp** (270 lines C → ~300 lines MLP)
**Source**: `selfhost_from_c/lexer.c`
**Purpose**: Tokenize MLP source code
**Features**:
- Token recognition (keywords, operators, literals)
- String/number parsing
- Comment handling
- Position tracking

**Key Functions to Port**:
```c
Token* tokenize(const char* source)
bool is_keyword(const char* str)
bool is_operator(char c)
Token* next_token()
```

**MLP Equivalent**:
```mlp
struct Token {
    type: TokenType,
    value: string,
    line: i32,
    column: i32
}

fn tokenize(source: string) -> Vec<Token> {
    // Implement tokenization
}
```

---

#### 2. **parser.mlp** (602 lines C → ~650 lines MLP)
**Source**: `selfhost_from_c/parser.c`
**Purpose**: Parse tokens into AST
**Features**:
- Expression parsing (operators, precedence)
- Statement parsing (if, while, for, return)
- Function definitions
- Type declarations
- Error recovery

**Key Functions to Port**:
```c
ASTNode* parse_expression()
ASTNode* parse_statement()
ASTNode* parse_function()
ASTNode* parse_program()
```

**MLP Equivalent**:
```mlp
enum ASTNode {
    Expression(Expr),
    Statement(Stmt),
    Function(FuncDef),
    Program(Vec<ASTNode>)
}

fn parse_expression(tokens: &mut TokenStream) -> Result<Expr> {
    // Implement expression parsing
}
```

---

#### 3. **statement_parser.mlp** (73 lines C → ~80 lines MLP)
**Source**: `selfhost_from_c/statement_parser.c`
**Purpose**: Parse complex statements
**Features**:
- Control flow (if/else, match)
- Loops (for, while, loop)
- Variable declarations
- Assignments

---

#### 4. **codegen.mlp** (528 lines C → ~600 lines MLP)
**Source**: `selfhost_from_c/codegen.c`
**Purpose**: Generate x86-64 assembly
**Features**:
- Register allocation
- Stack management
- Function calls
- Expression evaluation
- Instruction emission

**Key Functions to Port**:
```c
void codegen_expression(ASTNode* node)
void codegen_statement(ASTNode* node)
void codegen_function(ASTNode* node)
void emit_asm(const char* instruction)
```

**MLP Equivalent**:
```mlp
struct CodeGenerator {
    asm_output: String,
    register_pool: Vec<Register>,
    stack_offset: i32
}

impl CodeGenerator {
    fn emit(&mut self, inst: &str) {
        self.asm_output.push_str(inst);
    }
    
    fn codegen_expr(&mut self, expr: &Expr) -> Register {
        // Generate assembly for expression
    }
}
```

---

#### 5. **main.mlp** (123 lines C → ~150 lines MLP)
**Source**: `selfhost_from_c/main.c`
**Purpose**: CLI entry point
**Features**:
- Argument parsing
- File I/O
- Pipeline orchestration (lex → parse → codegen)
- Error reporting

**MLP Equivalent**:
```mlp
fn main() -> i32 {
    let args = std::env::args();
    
    if args.len() < 2 {
        println!("Usage: mlpc <file.mlp>");
        return 1;
    }
    
    let source = std::fs::read_to_string(&args[1])?;
    let tokens = lexer::tokenize(&source)?;
    let ast = parser::parse(&tokens)?;
    let asm = codegen::generate(&ast)?;
    
    std::fs::write("output.s", asm)?;
    return 0;
}
```

---

### Conversion Strategy:

#### Phase 1: Direct Translation (Week 1)
1. Create MLP equivalents of C structs
2. Port function signatures
3. Translate C logic to MLP syntax
4. Use Stage 0 modules for runtime support

#### Phase 2: Integration (Week 1-2)
1. Link MLP modules together
2. Create build system (Makefile for MLP)
3. Test with simple programs

#### Phase 3: Validation (Week 2)
1. Self-compile test (Stage 1 compiles itself)
2. Feature parity verification
3. Performance benchmarking

---

## Stage 2: Full Self-Hosting (FUTURE ⏳)
**Goal**: Stage 1 compiles Stage 1
**Status**: Not started
**Timeline**: After Stage 1 complete

### Requirements:
- Stage 1 compiler working correctly
- All MLP features supported
- Test suite passing
- Documentation complete

### Process:
```bash
# Stage 1 compiles Stage 1 source
./stage1_mlpc stage1/*.mlp -o stage2_mlpc

# Stage 2 compiles itself (verification)
./stage2_mlpc stage1/*.mlp -o stage2_verify

# Compare outputs
diff stage2_mlpc stage2_verify
```

---

## Current Status

### Completed:
- ✅ Stage 0: All 64 modules in C
- ✅ Test infrastructure
- ✅ Assembly generation working
- ✅ MLP syntax defined

### In Progress:
- 🔄 Stage 1: C → MLP translation
  - [ ] lexer.mlp (0%)
  - [ ] parser.mlp (0%)
  - [ ] statement_parser.mlp (0%)
  - [ ] codegen.mlp (0%)
  - [ ] main.mlp (0%)

### TODO:
- ⏳ Stage 2: Self-hosting verification
- ⏳ Documentation
- ⏳ Performance optimization

---

## File Structure (After Stage 1)

```
melp/
├── C/
│   └── stage0/           # Bootstrap C modules (64 modules)
│       └── modules/
│           ├── lexer/
│           ├── parser/
│           ├── codegen/
│           └── ... (61 more)
│
├── MLP/
│   ├── stage1/           # MLP compiler in MLP
│   │   ├── lexer.mlp
│   │   ├── parser.mlp
│   │   ├── statement_parser.mlp
│   │   ├── codegen.mlp
│   │   ├── main.mlp
│   │   ├── Makefile
│   │   └── README.md
│   │
│   └── stage2/           # Self-compiled output (future)
│       └── mlpc          # Fully self-hosted compiler
│
└── runtime/              # Runtime library (shared)
    └── runtime.c
```

---

## Build Commands

### Stage 0 → Stage 1:
```bash
cd melp/MLP/stage1

# Use Stage 0 C modules to compile MLP source
../../C/stage0/bin/mlpc_stage0 lexer.mlp -o lexer.o
../../C/stage0/bin/mlpc_stage0 parser.mlp -o parser.o
../../C/stage0/bin/mlpc_stage0 codegen.mlp -o codegen.o
../../C/stage0/bin/mlpc_stage0 main.mlp -o main.o

# Link with Stage 0 runtime
gcc lexer.o parser.o codegen.o main.o \
    -L../../C/stage0/lib -lmlp_runtime \
    -o mlpc_stage1
```

### Stage 1 → Stage 2:
```bash
cd melp/MLP/stage2

# Use Stage 1 to compile itself
../stage1/mlpc_stage1 ../stage1/*.mlp -o mlpc_stage2

# Verify self-hosting
./mlpc_stage2 ../stage1/*.mlp -o mlpc_verify
diff mlpc_stage2 mlpc_verify
```

---

## Next Steps

1. **Start with lexer.mlp**:
   - Read `selfhost_from_c/lexer.c`
   - Create equivalent MLP structs/functions
   - Test with simple token streams

2. **Continue with parser.mlp**:
   - Port AST node definitions
   - Implement recursive descent parser
   - Test with simple expressions

3. **Implement codegen.mlp**:
   - Port assembly generation logic
   - Handle register allocation
   - Test with basic programs

4. **Complete main.mlp**:
   - CLI argument handling
   - Pipeline orchestration
   - Error reporting

5. **Integration & Testing**:
   - Build full compiler
   - Run test suite
   - Fix any issues

---

## Success Criteria

### Stage 1 Complete When:
- ✅ All 5 MLP files created
- ✅ Compiler builds successfully
- ✅ Can compile simple MLP programs
- ✅ Can compile itself (self-hosting test)
- ✅ All tests pass

### Stage 2 Complete When:
- ✅ Stage 1 compiles Stage 1 source
- ✅ Output is bit-identical
- ✅ No C dependencies remain
- ✅ Full feature parity with C version

---

**Total Estimated Effort**: 2-3 weeks
**Current Priority**: Start with lexer.mlp
**Last Updated**: 3 Aralık 2025
