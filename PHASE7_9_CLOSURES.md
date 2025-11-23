# Phase 7.9: Closures Implementation

**Status**: ✅ COMPLETE
**Date**: November 23, 2025
**Version**: MLP v3.0

## Overview

Phase 7.9 implements full closure support for MLP lambda expressions. Closures allow lambda functions to capture and access variables from their enclosing scope, even after the outer function has returned.

## Implementation Summary

### 1. Closure Structure

All lambdas in MLP are treated as closures with a uniform calling convention:

**Closure Struct Layout (Heap-allocated)**:
```
Offset | Size | Content
-------|------|------------------
0-7    | 8B   | Function pointer (lambda code address)
8-15   | 8B   | Captured variable 1 (if any)
16-23  | 8B   | Captured variable 2 (if any)
...    | ...  | Additional captured variables
```

**Minimum Size**: 8 bytes (function pointer only for non-capturing lambdas)

### 2. Calling Convention

**ALL lambdas use closure calling convention**:
- First parameter (`rdi`) = closure pointer (always)
- User parameters start at `rsi`, `rdx`, `rcx`, etc.

Example:
```mlp
lambda(x, y) => x + y
```
Generated assembly receives:
- `rdi` = closure pointer
- `rsi` = x (user parameter 1)
- `rdx` = y (user parameter 2)

### 3. Captured Variable Detection

Captured variables are detected at **code generation time** (not parse time):

**Algorithm** (`find_free_variables`):
1. Traverse lambda body AST
2. For each variable reference:
   - If it's a lambda parameter → skip
   - If it's in outer scope → capture it
3. Store captured variable names in `node->lambda_data.captured_vars`

**Why at codegen time?**
- Scope information is not available during parsing
- Need complete symbol table to determine which variables are in outer scope

### 4. Closure Generation Code

**Two-phase generation**:

#### Phase 1: Closure Allocation (Inline in parent function)
```assembly
; 1. Allocate closure struct
mov rdi, 16              ; Size: 8B func_ptr + 8B captured var
call malloc
mov [rel __closure_0], rax

; 2. Store function pointer
lea rbx, [rel __lambda_0]
mov rcx, [rel __closure_0]
mov [rcx], rbx           ; closure[0] = function pointer

; 3. Copy captured variables
mov rbx, [rbp-8]         ; Load captured 'x'
mov rcx, [rel __closure_0]
mov [rcx+8], rbx         ; closure[8] = x value

; 4. Return closure pointer
mov rax, [rel __closure_0]
```

#### Phase 2: Lambda Function (Deferred to lambda_section)
```assembly
__lambda_0:
    push rbp
    mov rbp, rsp
    sub rsp, 256
    
    ; Save closure pointer
    mov [rbp-8], rdi
    
    ; Save user parameters
    mov [rbp-16], rsi        ; Parameter 'y'
    
    ; Load captured variables from closure
    mov rbx, [rbp-8]         ; Closure pointer
    mov rax, [rbx+8]         ; Load captured 'x'
    mov [rbp-24], rax        ; Store as local 'x'
    
    ; Lambda body (x + y)
    mov rax, [rbp-16]        ; Load y
    push rax
    mov rax, [rbp-24]        ; Load x
    pop rbx
    add rax, rbx
    
    ; Return
    mov rsp, rbp
    pop rbp
    ret
```

### 5. Lambda Deferred Generation

**Problem**: Lambda code inline conflicts with return statement control flow

**Solution**: Defer lambda function generation to end of assembly

**Mechanism**:
- `lambda_section`: Separate `AsmCode` buffer for lambda functions
- During `visit_Lambda`: Generate lambda code into `lambda_section`
- At end of `generate_asm`: Append `lambda_section` after all regular code

**Section Swapping**:
```c
// Save text_section state (field-by-field to avoid stale pointers)
char* saved_text_code = text_section.code;
size_t saved_text_size = text_section.size;
size_t saved_text_capacity = text_section.capacity;

// Swap lambda_section into text_section
text_section.code = lambda_section.code;
text_section.size = lambda_section.size;
text_section.capacity = lambda_section.capacity;

// Visit lambda body (generates into text_section → lambda_section buffer)
visit(node->lambda_data.govde);

// Update lambda_section with potentially reallocated buffer
lambda_section.code = text_section.code;
lambda_section.size = text_section.size;
lambda_section.capacity = text_section.capacity;

// Restore text_section
text_section.code = saved_text_code;
text_section.size = saved_text_size;
text_section.capacity = saved_text_capacity;
```

**Why field-by-field?**
- `asm_append()` uses `realloc()` which can change `code` pointer
- Struct assignment creates shallow copy → stale pointers after realloc
- Field-by-field swap ensures correct pointer tracking

### 6. Memory Management

**Heap Allocations**:
1. **Closure struct** (`malloc` in generated assembly) - runtime heap
2. **`captured_vars` array** (`malloc` in compiler) - compiler heap
3. **Captured variable names** (`strdup` for each name) - compiler heap

**Cleanup**:
- Runtime: Closure struct cleanup responsibility of user program (no GC yet)
- Compiler: 
  - `captured_vars` array stored in AST, freed when AST destroyed
  - Variable names freed by AST cleanup
  - Section buffers freed by `free_asm_code()`

**Critical Bug Fixed**: 
- `visit_Lambda` was restoring `kapsam_degisken_sayisi` after `kapsam_cik()`
- This made freed pointers in `kapsam_haritasi` appear valid
- Parent function's `kapsam_cik()` would try to free them again → **double free**
- **Fix**: Only restore `kapsam_yigin_ofseti`, let `kapsam_cik()` update `kapsam_degisken_sayisi`

## Test Results

### test_closure_single.mlp
```mlp
function make_adder(x)
    return lambda(y) => x + y
end
numeric add_5 = make_adder(5)
numeric result = add_5(10)
print result
```
**Output**: `15` ✅

### test_no_closure.mlp
```mlp
function make_doubler()
    return lambda(n) => n * 2
end
numeric double_fn = make_doubler()
numeric result = double_fn(5)
print result
```
**Output**: `10` ✅

### test_multiple_closures.mlp
```mlp
function make_adder(x)
    return lambda(y) => x + y
end
numeric add_5 = make_adder(5)
print add_5(10)           -- 15
numeric add_100 = make_adder(100)
print add_100(23)         -- 123
numeric add_7 = make_adder(7)
print add_7(3)            -- 10
```
**Output**: 
```
15
123
10
```
✅ Each closure maintains its own captured variable!

## Key Lessons Learned

1. **Struct Assignment Creates Shallow Copies**
   - In C, `struct_a = struct_b` copies all fields
   - Pointer fields point to SAME memory
   - After `realloc()`, original struct has stale pointers

2. **Field-by-Field State Management**
   - For dynamically growing buffers, track state by copying individual fields
   - Never copy structs containing pointers that may be reallocated

3. **Scope Cleanup Ordering**
   - `kapsam_cik()` frees variables and updates `kapsam_degisken_sayisi`
   - Restoring old `kapsam_degisken_sayisi` after `kapsam_cik()` → double free
   - Only restore `kapsam_yigin_ofseti` (stack offset)

4. **Deferred Code Generation**
   - Lambda inline generation conflicts with control flow (return, if, etc.)
   - Solution: Defer to separate section, concatenate at end
   - Section swapping enables code generation into deferred buffer

## Architecture Decisions

### Why Uniform Closure Convention?

**Considered approaches**:
1. **Function pointer for non-capturing, closure for capturing**
   - Pros: Simpler non-capturing case
   - Cons: Two calling conventions, complex call site logic

2. **All lambdas as closures** ✅ (chosen)
   - Pros: Single calling convention, simpler call sites
   - Cons: 8-byte overhead for non-capturing lambdas
   - Rationale: Simplicity > minor memory optimization

### Why Detect Captured Vars at Codegen?

**Alternatives**:
1. **Detect during parsing** 
   - Problem: Scope not available yet, can't distinguish local vs outer vars

2. **Detect at codegen** ✅ (chosen)
   - Scope fully resolved
   - Can query symbol table to determine variable origin

## Files Modified

- `self_host/mlp_compiler.c`:
  - `visit_Lambda()`: Complete rewrite for closure support
  - `find_free_variables()`: New function for captured var detection
  - `lambda_section`: New global for deferred lambda code
  - `generate_asm()`: Lambda section concatenation
  - `visit_IslecCagirma()`: Indirect call support for closures

## Future Work

1. **Garbage Collection**: Currently closures leak (no free)
2. **Nested Closures**: Closure returning closure (may need closure chain)
3. **Mutable Captured Vars**: Currently captures by-value, need by-reference for mutation
4. **Optimization**: Escape analysis to avoid heap allocation when closure doesn't escape

## Conclusion

Phase 7.9 successfully implements closures with:
- ✅ Uniform calling convention (all lambdas as closures)
- ✅ Captured variable detection at codegen
- ✅ Heap-allocated closure structs
- ✅ Deferred lambda generation (lambda_section)
- ✅ Multiple independent closures working correctly
- ✅ Memory management (double-free bug fixed)

**Total Time**: ~6 hours (as estimated in ROADMAP)
**Lines of Code**: ~300 lines added/modified in mlp_compiler.c
