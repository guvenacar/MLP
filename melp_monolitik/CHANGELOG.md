# MELP Changelog

All notable changes to MELP will be documented in this file.

## [0.6.0] - 2025-11-28 - Phase 6: Structs & Ternary Operator

### Added
- **Struct Type System**
  - `struct Name ... end struct` syntax (no 'then' keyword)
  - Struct field declarations with all base types
  - Struct instance creation: `TypeName varName`
  - Field access expressions: `object.field`
  - StructInfo registry for compile-time type tracking
  - Automatic field offset calculation (8-byte aligned)
  - Stack-based struct instance allocation

- **Ternary Operator**
  - Conditional expressions: `condition ? true_val : false_val`
  - Works with all types (numeric, text, boolean, decimal)
  - Nested ternary support (unlimited depth)
  - Short-circuit evaluation (only selected branch evaluated)
  - Type-aware code generation

- **Lexer Enhancements**
  - TOKEN_STRUCT: Recognize `struct` keyword
  - TOKEN_DOT: Field access operator `.`
  - TOKEN_QUESTION: Ternary condition operator `?`
  - TOKEN_COLON: Ternary separator `:`

- **Parser Enhancements**
  - STMT_STRUCT_DEF: New statement type for struct definitions
  - EXPR_FIELD_ACCESS: New expression for field access
  - EXPR_TERNARY: New expression for ternary operator
  - Lookahead logic: Detect struct instance declarations
  - StructField: Field metadata storage

- **Codegen Enhancements**
  - StructInfo registry: Tracks struct types with field metadata
  - Field offset calculation: Computes byte offsets for fields
  - Two-phase generation: Register structs before code generation
  - VarInfo extension: Added `struct_name` for instance tracking
  - Label-based ternary evaluation with proper branching

- **Tests**
  - `test_struct_simple.mlp`: Struct definition parsing
  - `test_struct_instance.mlp`: Struct instance allocation (16 bytes)
  - `test_ternary.mlp`: Ternary operator with numbers and strings
  - `test_phase6_comprehensive.mlp`: All Phase 6 features combined

### Changed
- Declaration AST node extended with `struct_name` field
- VarInfo extended with `struct_name` for struct instances
- Two-phase compilation: Struct registration before code generation

### Technical Details
- **Memory Layout**: All struct fields are 8 bytes for simplicity
- **Field Access**: `[rbp - base_offset + field_offset]` addressing
- **Ternary Codegen**: Label-based branching with false/end labels
- **Type Safety**: Compile-time struct type validation

### Known Limitations
- Field assignment not yet implemented (read-only field access)
- No struct initialization with values at declaration
- No nested structs support
- Struct instances cannot be initialized with field values

### Examples
```mlp
-- Struct definition
struct Person
    text name
    numeric age
end struct

-- Instance creation
Person alice

-- Ternary operator
numeric max = a > b ? a : b
text status = age >= 18 ? "adult" : "minor"

-- Nested ternary
text grade = score >= 90 ? "A" : score >= 80 ? "B" : "C"
```

## [0.5.0] - 2025-11-28 - Phase 5: Strings

### Added
- String literals: `text name = "Hello"`
- String concatenation: `"Hello " + "World"`
- String comparison: `==`, `!=`, `<`, `<=`, `>`, `>=`
- Built-in `length()` function
- Print support for strings

## [0.4.0] - 2025-11-27 - Phase 4: Functions

### Added
- Function definitions: `func name(params) ... end func`
- Function parameters with type checking
- Return statements: `return expr`
- Function calls: `result = func(args)`
- Call stack management with proper frame handling

## [0.3.0] - 2025-11-26 - Phase 3: Loops (Turing Complete!)

### Added
- For loops: `for i = start to end step increment`
- While loops: `while condition ... end while`
- Exit statement: Early loop termination
- Continue statement: Skip to next iteration
- Nested loop support

## [0.2.0] - 2025-11-25 - Phase 2: Conditionals

### Added
- If-then-else: `if condition then ... else ... end if`
- Comparison operators: `==`, `!=`, `<`, `<=`, `>`, `>=`
- Conditional branching with labels

## [0.1.0] - 2025-11-24 - Phase 1: Assignment & Print

### Added
- Variable assignments: `x = 42`, `x = y + 10`
- Print statement: `print(expr)`
- Expression evaluation

## [0.0.1] - 2025-11-23 - Phase 0: Declarations

### Added
- Variable declarations: `numeric`, `decimal`, `boolean`
- x86-64 assembly generation
- Stack allocation
- Bootstrap compiler in C

---

## Upcoming Phases

### Phase 7 (Planned)
- String interpolation: `$"Hello {name}"`
- Field assignment: `person.name = "Alice"`
- Struct methods
- Advanced string operations

### Phase 8+ (Future)
- Arrays and lists
- Nested structs
- Pointers/references
- File I/O
- Standard library

---

**Format:** [version] - date - title
**Versioning:** Major.Minor.Patch (0.x.0 for phases)
