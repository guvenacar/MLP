# Phase 14: Iterator/Generator Pattern - COMPLETE ✓

## Overview
This phase implements iterator and generator patterns for MELP, allowing iteration over collections and lazy value generation.

## Features Implemented

### 1. For-In Loop (`for x in collection`)
Allows iteration over arrays without explicit index management.

**Syntax:**
```mlp
for element in array
    -- use element
end for
```

**Supports:**
- Stack-allocated arrays (fixed size)
- Dynamic arrays (heap allocated)
- Generator function results

### 2. Generator Functions (`yields` + `yield`)
Functions that lazily generate sequences of values.

**Syntax:**
```mlp
func generator_name(params) yields type
    -- body with yield statements
    yield value1
    yield value2
    -- ...
end func
```

**Usage:**
```mlp
for item in generator_name(args)
    -- process item
end for
```

### 3. Built-in `range()` Function
Generates numeric sequences, similar to Python's range.

**Variants:**
- `range(end)` - 0 to end-1
- `range(start, end)` - start to end-1
- `range(start, end, step)` - start to end-1 with step increment

## Implementation Details

### Lexer Changes (lexer.c)
- Added `TOKEN_YIELD` - for yield statements
- Added `TOKEN_YIELDS` - for generator return type declaration
- Added `TOKEN_IN` - for for-in loop syntax

### Parser Changes (parser.c)
- Added `STMT_FOR_IN` statement type
- Added `STMT_YIELD` statement type
- Added `for_in` struct: var_name, var_type, iterable, body, body_count
- Added `yield_stmt` struct: value expression
- Added `is_generator` flag to func_def struct
- Modified for loop parsing to detect `in` keyword

### Codegen Changes (codegen.c)
- Added `in_generator` flag to Codegen struct
- Added `is_array` and `array_size` fields to VarInfo
- Added `codegen_add_stack_array()` function
- Added `codegen_generate_for_in()` function
- Added STMT_YIELD handling in codegen_generate_statement
- Modified func_def generation for generator setup/teardown

### Runtime Changes (runtime.c/runtime.h)
- Added `mlp_range()` - full 3-argument range
- Added `mlp_range1()` - single argument (end)
- Added `mlp_range2()` - two arguments (start, end)

## Test Files
- `test_iterator.mlp` - Basic for-in with stack array
- `test_iterator2.mlp` - For-in with sum calculation
- `test_iterator3.mlp` - Traditional for loop (baseline)
- `test_range.mlp` - Range function variants
- `test_generator.mlp` - Generator with yields

## Example Programs

### Stack Array Iteration
```mlp
func main() returns numeric
    numeric[] arr = [1, 2, 3, 4, 5]
    for x in arr
        print(x)
    end for
    return 0
end func
```

### Generator Function
```mlp
func make_squares(numeric n) yields numeric
    for i = 0 to n
        yield i * i
    end for
end func

func main() returns numeric
    for sq in make_squares(5)
        print(sq)
    end for
    return 0
end func
```

### Range-based Iteration
```mlp
func main() returns numeric
    -- 0 to 4
    for i in range(5)
        print(i)
    end for
    
    -- 2, 4, 6, 8
    for i in range(2, 10, 2)
        print(i)
    end for
    
    return 0
end func
```

## Technical Notes

### Generator Implementation
Generators use a simple "collect-all" pattern:
1. Allocate an internal result array on function entry
2. Each `yield` appends to the array
3. On function exit, resize array to actual size and return
4. Caller iterates over returned array

This is simpler than true coroutines but works well for finite sequences.

### For-In Optimization
Stack arrays are handled specially:
- Use LEA to get base address (not load value)
- Array size known at compile time from VarInfo
- No runtime `mlp_array_length` call needed

Dynamic arrays use standard `mlp_array_length` runtime function.

## Limitations
- Infinite generators not supported (would require coroutines)
- Generator early termination (break) returns partial results
- No iterator protocol for custom types (only arrays and generators)

## Date Completed
2025-01-13
