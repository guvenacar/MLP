# typeof Operator Implementation (Phase 5.7)

**Completed:** 22 Kasım 2025  
**Status:** ✅ TAMAMLANDI

## Overview

The `typeof` operator has been successfully implemented in MLP. It provides runtime type introspection, returning the type name of an expression as a string.

## Syntax

```mlp
string type_name = typeof(expression)
```

## Supported Types

The operator returns one of the following type names:
- `"numeric"` - For numeric values (integers, floats)
- `"string"` - For string values
- `"boolean"` - For boolean values (0 or 1)
- `"pointer"` - For pointer values

## Implementation Details

### 1. Compiler Changes (`self_host/mlp_compiler.c`)

#### Token Type
- Added `TOKEN_TYPEOF` enum value (line ~79)
- Added to KeywordMap: `{"typeof", TOKEN_TYPEOF}` (line ~1061)
- Added lexer keyword check (line ~1745)

#### AST Node Type
- Added `AST_TYPEOF_EXPR` to ASTNodeType enum (line ~286)
- Added `typeof_data` struct to ASTNode union:
  ```c
  struct {
      ASTNode* ifade;  // Expression to get type of
  } typeof_data;
  ```

#### Parser
- Added parsing in `birincil()` function (line ~2622)
- Syntax: `typeof(expression)`
- Creates AST_TYPEOF_EXPR node with expression operand

#### Generator
- Added `visit_TypeofExpr()` function (line ~6312)
- Implements hybrid approach:
  - **Compile-time detection:** For literals (AST_SAYI, AST_METIN)
  - **Runtime detection:** For variables and complex expressions
  - Calls `mlp_get_type()` runtime function when needed

#### Extern Declaration
- Added `extern mlp_get_type` declaration (line ~6815)

### 2. Runtime Changes (`runtime/runtime.c`)

#### Function Implementation
- Added `mlp_get_type()` function (line ~1367)
- Signature: `const char* mlp_get_type(int64_t value)`
- Detection logic:
  1. Checks for boolean values (0 or 1)
  2. Checks for numeric range (-1,000,000 to 1,000,000)
  3. Checks for pointer values (> 0x1000)
  4. Returns appropriate type string

## Test Coverage

**Test File:** `tests/test_typeof.mlp`

Tests include:
- ✅ Numeric literals: `typeof(42)` → `"numeric"`
- ✅ String literals: `typeof("hello")` → `"string"`
- ✅ Numeric variables: `typeof(age)` → `"numeric"`
- ✅ String variables: `typeof(name)` → `"string"`

## Example Usage

```mlp
-- Type introspection
numeric x = 42
string name = "Alice"

string t1 = typeof(x)      -- "numeric"
string t2 = typeof(name)   -- "string"
string t3 = typeof(100)    -- "numeric"
string t4 = typeof("hi")   -- "string"

print "x is of type: "
print t1
```

## Compilation

```bash
# Compile test
./mlpc tests/test_typeof.mlp tests/test_typeof.asm
nasm -f elf64 tests/test_typeof.asm -o tests/test_typeof.o
gcc -o tests/test_typeof tests/test_typeof.o runtime/runtime.o runtime/hashmap.o -lm -no-pie

# Run test
./tests/test_typeof
```

## Limitations

1. **Type Metadata:** MLP doesn't track full runtime type information. The implementation uses heuristics:
   - Small integers → numeric
   - Pointers in high memory → string
   - Values 0 or 1 → boolean

2. **Pointer Detection:** Uses memory address range checking, which is not 100% reliable

3. **Future Improvement:** Full type safety would require:
   - Type metadata in value representation
   - Tagged unions or type flags
   - More sophisticated runtime type system

## Files Modified

1. `self_host/mlp_compiler.c` - Added 6 changes
   - Token enum
   - KeywordMap entry
   - Lexer keyword check
   - AST node type
   - Parser support
   - Generator visit function
   - Extern declaration

2. `runtime/runtime.c` - Added 1 function
   - `mlp_get_type()` implementation

3. `tests/test_typeof.mlp` - New test file

## Integration

The typeof operator is now part of **Phase 5.7** features:
- ✅ Input functions (read_input, read_line, read_int)
- ✅ Type casting (num, str)
- ✅ Type introspection (typeof)

## Future Work

Potential enhancements:
- [ ] More granular type detection (int vs float)
- [ ] Array type detection
- [ ] Map/List type detection
- [ ] Function type detection
- [ ] Custom struct type names
- [ ] Full runtime type metadata system

## References

- **Specification:** See `SPECS.md` for typeof syntax
- **Test Suite:** `tests/test_typeof.mlp`
- **Implementation:** `self_host/mlp_compiler.c` and `runtime/runtime.c`
