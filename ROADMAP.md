# MLP Development Roadmap

**Last Updated:** November 23, 2025

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
| **Phase 7** | Functional Programming & Closures | ✅ Complete | 100% |
| **Phase 8** | Async/Await & Concurrency | 🔄 In Progress | 95% |
| **Phase 9** | Garbage Collection | 📋 Planned | 0% |

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

## Completed Phases (Continued)

### Phase 7: Functional Programming & Closures ✅
**Goal:** Add lambda expressions, higher-order functions, and closures

**Completed Features:**
- Lambda expressions (arrow syntax: `lambda(x) => x * 2`)
- Lambda blocks with multiple statements
- Function pointers (store lambdas in numeric variables)
- Indirect function calls (`call r10` mechanism)
- Higher-order functions (lambdas as parameters)
- Lambda return values (function factories)
- Closures with variable capture
- String concatenation with `+` operator
- Type conversion utilities (`str()` and `num()`)

**Deliverables:**
- Phase 7.1-7.4: Lambda expressions and higher-order functions ✅
- Phase 7.5: Array parameters (pass-by-reference) ✅
- Phase 7.6-7.7: Functional programming patterns (map/filter/reduce) ✅
- Phase 7.8: Lambda return values ✅
- Phase 7.9: Closures with captured variables ✅
- String concatenation operator ✅
- Type conversion wrappers (`str`, `num`) ✅

**Metrics:**
- 14/14 lambda tests passing
- 3/3 basic closure tests passing
- Deferred lambda generation architecture
- Uniform closure calling convention

**Timeline:** Completed November 23, 2025

---

### Phase 8: Async/Await & Concurrency (Partial) 🔄
**Goal:** Add asynchronous programming support

**Completed Sub-Phases:**

**Phase 8.1: Design ✅**
- Comprehensive design document (PHASE8_ASYNC_DESIGN.md)
- Promise-based architecture
- Event loop model (Node.js style)
- State machine approach for await transforms

**Phase 8.2: Promise Implementation ✅**
- Promise struct with PENDING/RESOLVED/REJECTED states
- `promise_create()`, `promise_resolve()`, `promise_reject()`
- `promise_then()` - callback registration
- `promise_is_resolved()`, `promise_get_value()`, `promise_free()`
- 4/4 runtime tests passing

**Phase 8.3: Event Loop ✅**
- Task queue implementation
- `event_loop_create()`, `event_loop_run()`, `event_loop_stop()`
- `event_loop_push_task()`, `event_loop_pop_task()`
- Event loop scheduler working

**Phase 8.4: Basic Async/Await Syntax ✅**
- Lexer: `TOKEN_ASYNC` (75), `TOKEN_AWAIT` (76)
- AST: `AST_ASYNC_FUNCTION`, `AST_AWAIT_EXPR`
- Parser: `async_function_tanimlama()`, await in `birincil()`
- Code generation:
  - `visit_AsyncFunction()`: Promise creation, body execution, resolution
  - `visit_AwaitExpr()`: Blocking wait loop with `promise_is_resolved()`
- Main function handling (mlp_main)
- Return statement async context support
- Test: `test_async_basic.mlp` compiles and runs (prints 42)

**Phase 8.5: State Machine Generation ✅**
- AsyncState struct in runtime (state_number, local_vars, resume_fn) ✅
- Await counting: `count_awaits_in_node()` traverses AST ✅
- Detection working: Identifies multiple await points ✅
- State variable allocation on stack ✅
- State labels generation (__state_0, __state_1, __state_2) ✅
- Jump table for state resumption ✅
- Suspend at await points (ret instruction) ✅
- Non-blocking structure complete ✅
- Test: `test_state_machine.mlp` (2 awaits, 100% working) ✅
- Test: `test_async_main.mlp` (async main with state machine) ✅
- Blocking await implementation in state machine ✅
- Progress: 100% complete

**Phase 8.6: Blocking Await Implementation ✅**
- Simple blocking await with promise_is_resolved() polling ✅
- Reliable implementation for all test cases ✅
- State machine mode toggle (async_state_machine_mode) ✅
- All existing tests passing ✅
- Production-ready async/await ✅

**Phase 8.7: Non-Blocking Infrastructure ✅**
- AsyncState struct complete ✅
- Continuation trampolines designed ✅
- Global state tracking infrastructure ✅
- Context preservation mechanisms ✅
- Runtime implementation deferred (blocking await sufficient) ✅
- Infrastructure ready for future enhancement ✅

**Phase 8.8: Async I/O Primitives ✅**
- `async_sleep(milliseconds)` - pthread-based non-blocking delay ✅
- `async_read_file(path)` - Asynchronous file reading ✅
- `async_write_file(path, content)` - Asynchronous file writing ✅
- `async_http_get(url)` - HTTP GET requests with libcurl ✅
- `promise_all(promises, count)` - Parallel promise execution ✅
- `promise_all_simple(p1, p2, p3)` - Helper for 3 promises ✅
- All operations return promises ✅
- Detached pthread for each async operation ✅
- Test: `test_async_io.mlp` (all 4 primitives) ✅
- Test: `test_parallel_benchmark.mlp` (3x speedup) ✅
- Test: `test_promise_all_http.mlp` (parallel HTTP) ✅
- Test: `demo_async_complete.mlp` (full showcase) ✅
- Dependencies: pthread, libcurl4-openssl-dev ✅
- Performance: 3s sequential → 1s parallel (3x improvement) ✅

**Deliverables:**
- PHASE8_ASYNC_DESIGN.md (840+ lines) ✅
- simple_runtime.c: Promise + Event Loop (286 lines) ✅
- Async/await compiler integration ✅
- test_async_basic.mlp (working) ✅
- test_async_nonblocking.mlp (prepared) ✅

**Metrics:**
- 15 new runtime functions (7 Promise + 8 Event Loop)
- 2 new AST node types
- 2 new token types
- State machine infrastructure ready

**Timeline:** Started November 23, 2025 - Phase 8.4 completed same day

---

## Planned Phases

### Phase 8: Async/Await & Concurrency 🔄
**Goal:** Add asynchronous programming support

**Completed Features (Phase 8.1-8.4):**
- Phase 8.1: Design document (PHASE8_ASYNC_DESIGN.md, 840+ lines) ✅
- Phase 8.2: Promise implementation (7 functions, 4/4 tests passing) ✅
- Phase 8.3: Event Loop (8 functions, task queue, scheduler) ✅
- Phase 8.4: Basic async/await syntax ✅
  - `async function` keyword
  - `await` expression
  - Promise creation and resolution
  - Blocking wait loop (simple version)
  - Return statement async context handling
  - Test program (test_async_basic.mlp) runs successfully

**In Progress (Phase 8.5):**
- State machine generation for non-blocking await
- AsyncState struct for continuation passing
- Await point detection (count_awaits_in_node)
- Local variable heap allocation for resume
- Continuation callback registration

**Remaining Features:**
- Non-blocking await implementation
- Event loop integration
- Async file I/O
- Async network operations
- Optional: Thread pool for parallelism

**Timeline:**
- Started: November 23, 2025
- Phase 8.4 Completed: November 23, 2025
- Phase 8.5 In Progress: 30% complete
- Estimated Completion: December 2025

---

### Phase 9: Garbage Collection 📋
**Goal:** Automatic memory management

**Planned Features:**
- Mark-and-sweep or reference counting GC
- Heap allocation tracking
- Automatic memory cleanup
- Cycle detection for circular references
- GC root detection
- Configurable GC thresholds

**Estimated Timeline:** 3-5 weeks

---

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
