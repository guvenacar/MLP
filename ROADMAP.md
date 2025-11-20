# MLP Development Roadmap

**Last Updated:** November 20, 2025

---

## Vision

MLP (Multi-Language Programming) is a self-hosting compiler that supports keywords from multiple natural languages, targeting x86-64 assembly. The project follows a phase-based development approach, incrementally adding features toward full self-hosting capability.

---

## Phase Status

| Phase | Name | Status | Completion |
|-------|------|--------|------------|
| **Phase 0** | Foundation | ✅ Complete | 100% |
| **Phase 1** | Multi-Language | ✅ Complete | 100% |
| **Phase 2** | Lists | ✅ Complete | 100% |
| **Phase 3** | File I/O & Strings & Hash Maps | ✅ Complete | 100% |
| **Phase 4** | Advanced Features | ✅ Complete | 100% |
| **Phase 5** | Self-Hosting Preparation | 🔄 In Progress | 0% |
| **Phase 6** | Self-Hosting Compiler | 📋 Planned | 0% |
| **Phase 7** | Optimization & Refinement | 📋 Planned | 0% |

---

## Completed Phases

### Phase 0: Foundation ✅
**Goal:** Basic compiler infrastructure

**Completed Features:**
- Lexer with Turkish/English keywords
- Parser (expressions, statements, functions)
- AST construction
- x86-64 code generation
- Basic arithmetic operators (`+`, `-`, `*`, `/`, `%`)
- Variables (`int`, `string`)
- Control flow (`if`/`else`, `while`, `for`)
- Functions with parameters and return values
- Arrays (fixed-size)
- Runtime library (print, basic operations)

**Deliverables:**
- Working compiler: `mlp_preprocessor.py`
- Assembly output (NASM format)
- Test suite

---

### Phase 1: Multi-Language Support ✅
**Goal:** Support keywords in Turkish and English

**Completed Features:**
- Bilingual keyword recognition
- Keyword translation infrastructure
- Structs (`struct` / `YAPI`)
- Enhanced type system

**Deliverables:**
- Multi-language keyword support
- Struct definitions and usage
- Updated documentation

---

### Phase 2: Dynamic Lists ✅
**Goal:** Add dynamic arrays (lists)

**Completed Features:**
- `list[T]` type syntax
- Dynamic memory allocation
- List operations: `add()`, `get()`, `size()`, `clear()`
- Generic type support
- Memory management for lists

**Deliverables:**
- List implementation in runtime
- Compiler support for `list[T]`
- Examples and tests

**Metrics:**
- 7 list functions
- ~800 lines of code

---

### Phase 3: File I/O, Strings, Hash Maps ✅
**Goal:** Enable file operations, advanced string manipulation, and hash tables

**Completed Features:**

**File I/O (6 functions):**
- `read_file()` - Read entire file
- `write_file()` - Write to file
- `read_lines()` - Read file line-by-line
- `append_file()` - Append to file
- `file_exists()` - Check file existence
- `file_size()` - Get file size

**String Operations (9 new functions):**
- `string_split()` - Split by delimiter
- `string_join()` - Join list of strings
- `string_replace()` - Replace occurrences
- `string_trim()` - Remove whitespace
- `string_upper()` / `string_lower()` - Case conversion
- `string_find()` - Find substring
- `string_starts_with()` / `string_ends_with()` - Prefix/suffix checks

**Hash Maps (8 functions):**
- `map[K:V]` - Generic hash map type
- `map()` - Create hash map
- `set()`, `get()`, `has()`, `remove()` - CRUD operations
- `size()`, `clear()` - Utility operations
- DJB2 hash function with chaining

**Deliverables:**
- FILE_IO_DESIGN.md (~565 lines)
- STRING_OPS_DESIGN.md (~400 lines)
- HASHMAP_DESIGN.md (~450 lines)
- 3 comprehensive example programs (~870 lines)
- Updated API_REFERENCE.md

**Metrics:**
- 23 new functions
- ~2,520 lines of documentation
- Critical for self-hosting

---

## Current Phase

### Phase 4: Advanced Features 🔄
**Goal:** Add type conversions, math utilities, and hash map compiler integration

**In Progress Features:**

**Hash Map Compiler Integration:**
- `map[KeyType:ValueType]` syntax in lexer/parser
- AST nodes for map operations
- Code generation for map CRUD
- Type safety mechanisms

**Type Conversions (6 functions):**
- `int_to_string()` - Format integers
- `string_to_int()` - Parse integers
- `char_code()` - Get character code
- `char_from_code()` - Create character from code
- `string_concat()` - String concatenation
- `string_char_at()` - Character access

**Math Operations (4 functions):**
- `abs()` - Absolute value
- `min()` / `max()` - Min/max selection
- `clamp()` - Value clamping

**Deliverables:**
- HASHMAP_COMPILER_DESIGN.md (~500 lines) ✅
- TYPE_CONVERSIONS_DESIGN.md (~320 lines) ✅
- MATH_OPERATIONS_DESIGN.md (~270 lines) ✅
- 3 example programs (~700 lines) ✅
- Updated API_REFERENCE.md ✅
- ROADMAP.md (this file) ✅

**Current Status:** Documentation complete, implementation in progress

**Remaining Work:**
- Implement runtime functions in C
- Update parser for hash map syntax
- Add code generation for new functions
- Testing and validation

---

## Planned Phases

### Phase 5: Self-Hosting Preparation 📋
**Goal:** Implement remaining features needed for self-hosting

**Planned Features:**
- Command-line argument parsing
- Error handling improvements
- Module system (import/export)
- Preprocessor macros
- Conditional compilation
- Better memory management
- Floating-point support
- Standard library organization

**Estimated Timeline:** 4-6 weeks

---

### Phase 6: Self-Hosting Compiler 📋
**Goal:** Rewrite compiler in MLP itself

**Approach:**
1. Port lexer to MLP
2. Port parser to MLP
3. Port code generator to MLP
4. Bootstrap: Compile compiler with itself
5. Validation and testing

**Success Criteria:**
- MLP compiler can compile itself
- Generated compiler produces identical output
- All tests pass with self-hosted compiler

**Estimated Timeline:** 8-12 weeks

---

### Phase 7: Optimization & Refinement 📋
**Goal:** Improve performance, usability, and robustness

**Planned Features:**
- Optimization passes (constant folding, dead code elimination)
- Better error messages with suggestions
- Debugging support (DWARF symbols)
- Profiling and benchmarking
- Standard library expansion
- Package manager
- Documentation website
- Community building

**Estimated Timeline:** Ongoing

---

## Feature Matrix

### Core Language Features

| Feature | Phase | Status |
|---------|-------|--------|
| Variables (int, string) | 0 | ✅ |
| Arithmetic operators | 0 | ✅ |
| Comparison operators | 0 | ✅ |
| Logical operators | 0 | ✅ |
| If/else | 0 | ✅ |
| While loops | 0 | ✅ |
| For loops | 0 | ✅ |
| Functions | 0 | ✅ |
| Arrays | 0 | ✅ |
| Structs | 1 | ✅ |
| Lists | 2 | ✅ |
| Hash maps | 3 | ✅ |
| Enums | 5 | 📋 |
| Pointers | 5 | 📋 |
| Modules | 5 | 📋 |

### Standard Library

| Category | Phase | Functions | Status |
|----------|-------|-----------|--------|
| **Print/Debug** | 0 | `print` | ✅ |
| **Lists** | 2 | 7 functions | ✅ |
| **File I/O** | 3 | 6 functions | ✅ |
| **Strings** | 3 | 15 functions | ✅ |
| **Hash Maps** | 3 | 8 functions | ✅ |
| **Type Conversions** | 4 | 6 functions | 🔄 |
| **Math** | 4 | 4 functions | 🔄 |
| **Advanced Math** | 5 | Power, sqrt, trig | 📋 |
| **System** | 5 | Args, env, exit | 📋 |
| **Memory** | 7 | malloc, free, GC | 📋 |

---

## Development Principles

1. **Incremental Progress** - Each phase builds on previous work
2. **Documentation First** - Design before implementation
3. **Test-Driven** - Examples and tests for all features
4. **Self-Hosting Goal** - Every feature evaluated for compiler needs
5. **Simplicity** - Avoid over-engineering, keep it simple
6. **Multi-Language** - Support Turkish and English throughout

---

## Community & Contributions

**Current State:** Early development, single maintainer

**Future Plans:**
- Open source release (Phase 6+)
- Contribution guidelines
- Issue tracker
- Discussion forum
- Tutorial series
- Example gallery

---

## Technical Debt

**Known Issues:**
- Memory management is manual (no GC yet)
- Error messages could be more helpful
- No optimization passes
- Limited standard library
- No debugger integration

**Planned Resolutions:**
- Phase 5: Memory management improvements
- Phase 7: Optimization and refinement

---

## Success Metrics

### Phase 4 Goals
- [ ] 10 new functions implemented
- [ ] All examples compile and run
- [ ] Documentation complete (~2,000 lines)
- [ ] Runtime tests pass

### Overall Project Goals
- [ ] Self-hosting compiler (Phase 6)
- [ ] Performance: Compile 1000 LOC in < 1 second
- [ ] Reliability: 100% test coverage
- [ ] Usability: Helpful error messages
- [ ] Community: 100+ GitHub stars

---

## Resources

**Documentation:**
- [API_REFERENCE.md](API_REFERENCE.md) - Complete API
- [TURING_COMPLETE_PLAN.md](TURING_COMPLETE_PLAN.md) - Detailed planning
- Phase-specific design documents

**Examples:**
- [examples/](examples/) - Sample programs
- Phase 3: File processor, compiler, symbol table
- Phase 4: Hash maps, conversions, math

**Tools:**
- Python-based compiler (bootstrap)
- NASM assembler
- GCC linker
- Custom runtime library

---

**Last Updated:** November 20, 2025
**Next Milestone:** Complete Phase 4 implementation
**Long-term Goal:** Self-hosting compiler by Q2 2026
