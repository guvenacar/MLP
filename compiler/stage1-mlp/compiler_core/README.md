# MLP Self-Hosting Compiler - Core Modules

Bu dizin, MLP dilinin kendisiyle yazılmış derleyici modüllerini içerir. C bootstrap derleyicisi (`mlp_compiler.c`) yerine kullanılacak production-ready modüller.

## 📁 Modül Yapısı

### 🔤 Token Sistemi
- **`token_types.mlp`** (230+ token tipi)
  - TokenType enum (keywords, operators, literals, punctuation)
  - Token struct (type, lexeme, line, column)
  - TokenStream (stream management)
  - Helper functions

### 🌳 AST Sistemi
- **`ast_nodes.mlp`** (70+ AST node tipi)
  - ASTNodeType enum (expressions, statements, declarations)
  - ASTLocation (source location tracking)
  - ASTNode (unified node structure)
  - ASTArena (arena-based allocation)
  - Specialized data carriers (block, literal, control flow, data structures)

### 📚 Function Registry
- **`function_registry.mlp`**
  - ParameterInfo (type, name, default value)
  - FunctionSignature (name, parameters, return type)
  - FunctionRegistry (HashMap-based storage)
  - pre_scan_functions (forward declaration support)
  - apply_default_parameters (default value application)

### 🔍 Lexer
- **`lexer.mlp`** (600+ satır)
  - LexerState (source code, position, line/column tracking)
  - UTF-8 support
  - String interpolation (`"Hello {name}"`)
  - Comments (`--` single line, `{- ... -}` multiline)
  - Operator recognition (60+ operators)
  - Keyword checking (60+ keywords)
  - tokenize() main function

### 🌲 Parser (4 modül)

#### **`parser.mlp`** - Core Parser (600+ satır)
- ParserState (token stream, AST arena, function registry, scope)
- Token navigation (advance, peek, check, expect, match)
- Scope management (enter/exit)
- Pratt parsing (precedence climbing)
- Primary expressions (literals, identifiers, parentheses)
- Binary operators (precedence table)
- Basic statements (print, return, if, while, variable declaration, assignment)
- Block parsing

#### **`parser_functions.mlp`** - Function & Lambda (300+ satır)
- Function declarations
- Parameter parsing (with default values)
- Function calls
- Method calls (object.method())
- Postfix expressions (calls, member access, array indexing)
- Lambda expressions
- Async functions
- Await/Yield expressions

#### **`parser_structures.mlp`** - Data Structures (400+ satır)
- List literals `[1, 2, 3]`
- HashMap literals `{"key": value}`
- Set literals
- Struct definitions & instantiation
- Enum definitions
- Member access (dot notation)
- Type annotations `list[numeric]`, `HashMap[string, numeric]`
- Range expressions `1..10`
- For/foreach loops

#### **`parser_advanced.mlp`** - Advanced Features (350+ satır)
- Import statements
- Try-catch-finally
- Throw statements
- Break/Continue
- Match (pattern matching)
- Defer (cleanup guarantee)
- With (resource management)
- Type aliases
- Unary expressions (!, -, ~)
- Panic statements

### 🔧 Scope Manager
- **`scope_manager.mlp`** (300+ satır)
  - VariableInfo (name, type, scope level, stack offset, capture status)
  - ScopeInfo (level, parent, variables, frame size)
  - ScopeManager (variable map, scope stack)
  - Scope operations (enter, exit, register, lookup)
  - Variable capture tracking (for closures)
  - Label generation
  - Stack frame calculation

### ⚙️ Code Generator (4 modül)

#### **`codegen.mlp`** - Core Code Generator (500+ satır)
- AsmCode (data/text/lambda sections)
- CodeGenState (assembly, scope, arena, counters)
- String literal registration
- Core visit functions (expressions, basic statements)
- x86-64 assembly generation
- Register management
- Main entry point (generate_code)

#### **`codegen_functions.mlp`** - Function Codegen (400+ satır)
- Function declarations (with parameter handling)
- Function calls (argument passing, stack management)
- Method calls (receiver + arguments)
- Lambda expressions (separate section generation)
- Closures (capture tracking, heap allocation)
- Async functions (state machine allocation)
- Await expressions (save/resume state)
- Yield expressions (generator state)

#### **`codegen_structures.mlp`** - Data Structure Codegen (500+ satır)
- List literals (heap allocation, element storage)
- List access (bounds checking, offset calculation)
- HashMap literals (hashmap_create, hashmap_set calls)
- HashMap get operations
- Set literals (set_create, set_add calls)
- Struct instantiation (field initialization)
- Member access (field offset calculation)
- Range expressions (start/end/current tracking)
- For loops (iterator, condition, step)
- Foreach loops (iterable iteration, bounds checking)

#### **`codegen_control.mlp`** - Control Flow Codegen (400+ satır)
- Try-catch (exception handler setup, type checking)
- Throw statements
- Break/Continue (loop label jumping)
- Match statements (pattern matching, case generation)
- Defer statements (cleanup guarantee)
- With statements (resource management)
- Panic statements
- Unary operations (!, -, ~)
- Expression statements
- Import statements (extern declarations)
- Type alias (no codegen)
- Struct/Enum definitions (metadata only)

### 🚀 Main Pipeline
- **`mlp_compiler_main.mlp`** (400+ satır)
  - CompilerOptions (input/output files, debug flags)
  - CompilationResult (success, output, error messages, stats)
  - Compilation pipeline:
    1. **Lexer** → Token stream
    2. **Parser** → AST arena
    3. **CodeGen** → Assembly output
  - File I/O (read_file, write_file)
  - REPL mode
  - CLI argument parsing
  - Error handling

## 📊 İstatistikler

### Module Count
Total: **20 modules** (~8,900+ lines of MLP code)

#### Compiler Core (14 modules)
- `token_types.mlp` - 230+ token type definitions
- `ast_nodes.mlp` - 70+ AST node structures
- `function_registry.mlp` - HashMap-based function lookup
- `lexer.mlp` - UTF-8 tokenization, string interpolation (600+ lines)
- `parser.mlp` - Core parsing, Pratt parsing (500+ lines)
- `parser_functions.mlp` - Function/lambda/async parsing (400+ lines)
- `parser_structures.mlp` - Data structure parsing (400+ lines)
- `parser_advanced.mlp` - Try-catch/match/defer parsing (400+ lines)
- `scope_manager.mlp` - Symbol table, closure capture (400+ lines)
- `codegen.mlp` - Core code generation, visit dispatcher (500+ lines)
- `codegen_functions.mlp` - Function/lambda/closure codegen (400+ lines)
- `codegen_structures.mlp` - Data structure codegen (500+ lines)
- `codegen_control.mlp` - Control flow codegen (400+ lines)
- `mlp_compiler_main.mlp` - Main compilation pipeline (250+ lines)

#### Runtime Wrappers (6 modules)
- `runtime/bigdecimal.mlp` - BigDecimal operations wrapper (250+ lines)
- `runtime/bigstring.mlp` - BigString operations wrapper (250+ lines)
- `runtime/hashmap.mlp` - HashMap/Set operations wrapper (200+ lines)
- `runtime/async.mlp` - Async/await, Promise, Task wrapper (400+ lines)
- `runtime/gc.mlp` - GC memory management wrapper (150+ lines)
- `runtime/file_io.mlp` - File I/O operations wrapper (300+ lines)

### Code Statistics

| Modül | Satır | Node/Token Tipi | Açıklama |
|-------|-------|-----------------|----------|
| token_types.mlp | ~400 | 230+ tokens | Token sistemi |
| ast_nodes.mlp | ~600 | 70+ AST nodes | AST veri yapıları |
| function_registry.mlp | ~200 | - | Fonksiyon kaydı |
| lexer.mlp | ~600 | - | Tokenization |
| parser.mlp | ~600 | - | Core parsing |
| parser_functions.mlp | ~300 | - | Function parsing |
| parser_structures.mlp | ~400 | - | Data structure parsing |
| parser_advanced.mlp | ~350 | - | Advanced features |
| scope_manager.mlp | ~300 | - | Symbol table |
| codegen.mlp | ~500 | - | Core codegen |
| codegen_functions.mlp | ~400 | - | Function codegen |
| codegen_structures.mlp | ~500 | - | Data structure codegen |
| codegen_control.mlp | ~400 | - | Control flow codegen |
| mlp_compiler_main.mlp | ~400 | - | Main pipeline |
| **TOPLAM** | **~6,450+** | **300+** | **14 modül** |

## 🔄 Derleyici Akışı

```
Source Code (.mlp)
    ↓
[Lexer] → TokenStream (230+ token types)
    ↓
[Parser] → ASTArena (70+ node types)
    ↓
[Code Generator] → Assembly (.asm)
    ↓
[Assembler (nasm)] → Object (.o)
    ↓
[Linker (ld)] → Executable
```

## 🎯 Özellikler

### Tam Desteklenen:
✅ Lexical analysis (UTF-8, string interpolation)  
✅ Parsing (Pratt algorithm, precedence climbing)  
✅ AST construction (arena allocation)  
✅ Function registry (default parameters)  
✅ Scope management (symbol table)  
✅ Code generation (x86-64 assembly)  
✅ Control flow (if/else, while, for/foreach)  
✅ Data structures (list, HashMap, Set)  
✅ Structs & Enums  
✅ Lambda expressions  
✅ Closures (capture tracking)  
✅ Function declarations & calls  
✅ Method calls  
✅ Async/await (basic state machine)  
✅ Pattern matching (match)  
✅ Exception handling (try-catch, throw, panic)  
✅ Loop control (break, continue)  
✅ Resource management (defer, with)  
✅ Import system (AST level)  
✅ Unary operations (!, -, ~)  
✅ Array/member access  
✅ Range expressions

### Kısmen Desteklenen:
🔄 Full async state machine (basic implementation hazır)  
🔄 Import resolver (AST hazır, runtime linkage eksik)  
🔄 Optimizations (pipeline hazır, optimizer yok)

### Yapılacak:
⏳ Import resolver (modül sistemi)
⏳ Type checker (semantic analysis)
⏳ Closure code generation
⏳ Async state machine codegen
⏳ Garbage collector entegrasyonu
⏳ Optimizer passes

## 🚀 Kullanım

### Derleme
```bash
# C bootstrap ile MLP derleyicisini derle
./mlp_compiler mlp_compiler_main.mlp -o mlp_self.asm

# Assembly'yi derle
nasm -f elf64 mlp_self.asm -o mlp_self.o
ld mlp_self.o -o mlp_self

# Artık MLP derleyicisi kendini derleyebilir!
./mlp_self input.mlp -o output.asm
```

### Debug Modları
```bash
# Token stream göster
./mlp_self --debug-tokens input.mlp

# AST yapısını göster
./mlp_self --debug-ast input.mlp

# Scope bilgilerini göster
./mlp_self --debug-scope input.mlp

# Verbose mode
./mlp_self -v input.mlp -o output.asm
```

### REPL Mode
```bash
./mlp_self --repl
```

## 📝 Notlar

- **Arena Allocation**: AST düğümleri arena-based allocation ile yönetilir (memory efficient)
- **Pratt Parsing**: Expression parsing için precedence climbing algoritması kullanılır
- **Scope Stack**: Lexical scoping için stack-based scope yönetimi
- **String Interning**: String literaller data section'da saklanır (label-based)
- **x86-64 ABI**: System V ABI (Linux) kullanılır
- **UTF-8**: Kaynak kod ve stringler tam UTF-8 desteğine sahip

## 🔗 Bağımlılıklar

Bu modüller birbirini import eder:
- `parser.mlp` → `token_types.mlp`, `ast_nodes.mlp`, `function_registry.mlp`
- `codegen.mlp` → `ast_nodes.mlp`, `scope_manager.mlp`
- `mlp_compiler_main.mlp` → Tüm modüller

## 🎓 Mimari Kararlar

1. **Modüler Yapı**: Her aşama ayrı modül (maintainability)
2. **Arena Allocation**: AST için efficient memory management
3. **Immutable Data**: Functional programming prensipleri
4. **Error Recovery**: Panic-based error handling (şimdilik)
5. **Forward Declarations**: Function registry ile destek
6. **Type Inference**: (Gelecek) Type checker eklenecek

## 📚 Referanslar

- C Bootstrap: `../mlp_compiler.c` (9693 satır)
- Specs: `../../SPECS.md`
- API Reference: `../../API_REFERENCE.md`
- Migration Plan: `../../c_den_mlp_ye_gecis_plani.md`
