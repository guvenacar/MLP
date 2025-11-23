# Phase 7.6: Array Parameters - Implementation Complete

**Status:** ✅ Fully Implemented and Tested  
**Date:** November 2024  
**Compiler Version:** self_host/mlp_compiler.c (8671 lines)

## Overview

Phase 7.6 adds support for passing arrays as function parameters using pointer semantics. Arrays are passed by reference, allowing functions to read and modify array elements efficiently without copying.

## Syntax

```mlp
function name(arr[], size)
    // arr is a pointer to the array
    // arr[i] accesses elements by reference
end
```

### Key Features

1. **Array Parameter Declaration:** `arr[]` syntax after parameter name
2. **Pass by Reference:** Arrays passed as pointers (not copied)
3. **Element Access:** `arr[i]` works inside functions
4. **Element Modification:** Changes to `arr[i]` affect the original array
5. **Size Parameter:** Convention to pass array size separately

## Implementation Details

### 1. Parser Changes (`self_host/mlp_compiler.c`)

**Location:** Lines ~4356-4390 in `islec_tanimlama()`

```c
// After parsing parameter name, check for [] syntax
if (current_token->type == TOKEN_LEFT_BRACKET) {
    consume(TOKEN_LEFT_BRACKET);
    if (current_token->type != TOKEN_RIGHT_BRACKET) {
        parseError("Array parameter must be []", "]");
    }
    consume(TOKEN_RIGHT_BRACKET);
    parametre_is_array[p_sayisi] = 1;  // Mark as array
} else {
    parametre_is_array[p_sayisi] = 0;  // Regular param
}
```

**Result:** Parser detects `[]` after parameter names and stores metadata

### 2. AST Structure

**Location:** Lines ~465-472

```c
struct {
    Token* ad;
    Token** parametreler;
    int parametre_sayisi;
    int* parametre_is_array;  // NEW: Phase 7.6
    ASTNode** parametre_default_degerleri;
    ASTNode* govde;
} islec_tanimlama_data;
```

**Changes:**
- Added `int* parametre_is_array` field to store which parameters are arrays
- Modified `createAST_IslecTanimlama()` to accept and store this metadata

### 3. Code Generation

#### 3a. Function Definition (`visit_IslecTanimlama`)

**Location:** Lines ~6048-6068

```c
for (int i = 0; i < param_sayisi; i++) {
    char* param_adi = node->islec_tanimlama_data.parametreler[i]->value;
    
    // Check if array parameter
    int is_array = 0;
    if (node->islec_tanimlama_data.parametre_is_array != NULL) {
        is_array = node->islec_tanimlama_data.parametre_is_array[i];
    }
    
    // Arrays use pointer type
    char* tip = is_array ? "ARRAY_SAYISAL" : "SAYISAL";
    char* adres = kapsam_degisken_yer_ayir(param_adi, tip);
    sprintf(buffer, "    mov %s, %s", adres, arg_registerleri[i]);
    asm_append(&text_section, buffer);
    free(adres);
}
```

**Result:** Array parameters stored with `ARRAY_SAYISAL` type, indicating pointer semantics

#### 3b. Array Element Access (`visit_ArrayErisim`)

**Location:** Lines ~6431-6469

```c
// Check if this is an array parameter (pointer) or local array
Degisken* var = NULL;
for (int i = 0; i < kapsam_degisken_sayisi; i++) {
    if (strcmp(kapsam_haritasi[i].ad, array_adi) == 0) {
        var = &kapsam_haritasi[i];
        break;
    }
}

// Array parameters (pointers) need mov, local arrays need lea
int is_pointer = (var && var->tip && strncmp(var->tip, "ARRAY_", 6) == 0);

if (is_pointer) {
    // Array parameter - load pointer value
    sprintf(buffer, "    mov rbx, %s  ; Load array pointer", array_base);
} else {
    // Local array - compute address
    sprintf(buffer, "    lea rbx, %s  ; Array base adresi", array_base);
}
```

**Key Distinction:**
- **Local arrays:** Use `lea rbx, [rbp-40]` to compute stack address
- **Array parameters:** Use `mov rbx, [rbp-8]` to load pointer value

Same logic applied to `visit_ArrayAtama()` for array element assignment.

### 4. Scope Management

**Critical Fix:** Functions now properly enter/exit scope levels

```c
// Before function body
kapsam_gir();  // Enter function scope (scope_level++)

// After function body
kapsam_cik();  // Exit function scope (scope_level--)
```

**Impact:** Function-local variables now have `scope_level > 0`, preventing global naming conflicts

## Compilation Requirements

### hashmap.o Must Use GNU Extensions

**Problem:** Original `hashmap.o` caused segfault when calling `hashmap_get()`

**Root Cause:** ABI mismatch - `mlp_compiler.c` compiled with `-D_GNU_SOURCE` but `hashmap.o` was not

**Solution:** Recompile hashmap with matching flags:

```bash
gcc -c runtime/hashmap.c -o runtime/hashmap.o -std=gnu99 -D_GNU_SOURCE
```

### Full Build Command

```bash
# Compile compiler
gcc self_host/mlp_compiler.c runtime/hashmap.o -o mlpc_new -std=gnu99 -D_GNU_SOURCE

# Compile MLP program
./mlpc_new program.mlp out.asm

# Assemble and link
nasm -f elf64 out.asm -o out.o
gcc out.o runtime/runtime.o -o program -no-pie -lm
```

## Test Results

### Test File: `test/phase7_6_array_params.mlp`

```mlp
// Test 1: Sum array elements
function sum_array(arr[], size)
    numeric total = 0
    numeric i = 0
    while i < size
        total = total + arr[i]
        i = i + 1
    end
    return total
end

// Test 2: Modify array elements (pass by reference)
function double_array(arr[], size)
    numeric i = 0
    while i < size
        arr[i] = arr[i] * 2
        i = i + 1
    end
end

// Test 3: Find maximum
function find_max(arr[], size)
    numeric max = arr[0]
    numeric i = 1
    while i < size
        if arr[i] > max then
            max = arr[i]
        end
        i = i + 1
    end
    return max
end

// Main tests
numeric numbers[] = [10, 20, 30, 40, 50]

numeric total = sum_array(numbers, 5)
print(total)  // Output: 150 ✅

double_array(numbers, 5)
total = sum_array(numbers, 5)
print(total)  // Output: 300 ✅

numeric maximum = find_max(numbers, 5)
print(maximum)  // Output: 100 ✅
```

**Results:** All tests pass ✅

## Known Limitations

1. **Size Parameter Required:** Array size must be passed separately (no built-in length)
2. **No Type Safety:** No compile-time check that parameter is actually an array
3. **Pointer Arithmetic Only:** Cannot pass array slice (e.g., `&arr[5]`)
4. **No Multi-dimensional Arrays:** `arr[][]` syntax not yet supported

## Assembly Output Example

### Function Call Site

```asm
; Call sum_array(numbers, 5)
lea rdi, [rbp-48]    ; Load address of numbers array
mov rsi, 5           ; Size = 5
call sum_array
```

### Function Definition

```asm
sum_array:
    push rbp
    mov rbp, rsp
    sub rsp, 256
    mov [rbp-8], rdi     ; Store arr pointer
    mov [rbp-16], rsi    ; Store size
    
    ; Access arr[i]:
    mov rax, [rbp-24]    ; Load i
    imul rax, 8          ; i * 8 (element size)
    push rax
    mov rbx, [rbp-8]     ; Load array pointer (not lea!)
    pop rax
    add rbx, rax         ; base + offset
    mov rax, [rbx]       ; Read element
```

**Key:** `mov rbx, [rbp-8]` loads the pointer value, not the address of the storage location

## Integration with Existing Features

- ✅ Works with array literals: `numeric arr[] = [1, 2, 3]`
- ✅ Works with while loops
- ✅ Works with if statements
- ✅ Works with function returns
- ✅ Works with default parameters (other params)
- ⚠️ **NOT** compatible with typed parameters (pre-existing bug)

## Next Steps

- **Phase 7.7:** Map/Filter/Reduce patterns using array parameters
- **Phase 7.8:** Lambda return values
- **Phase 7.9:** Closures (variable capture)

## Debugging Notes

### Issues Encountered

1. **Lexer Bug:** `end\nfunction` combined across newlines → Fixed by checking for newlines
2. **Hashmap Crash:** ABI mismatch in compilation flags → Fixed by recompiling hashmap
3. **Global Variable Conflict:** Function variables treated as global → Fixed by adding `kapsam_gir()`/`kapsam_cik()`
4. **Pointer vs Address:** Used `lea` for array parameters → Fixed by checking type and using `mov`

### Debug Workflow

```bash
# Enable debug output (if needed)
grep "fprintf(stderr, \"DEBUG" self_host/mlp_compiler.c

# Check assembly output
./mlpc_new test.mlp out.asm
cat out.asm | grep -A20 "function_name:"

# Test with minimal case
echo "function test(arr[]) print(42) end" > minimal.mlp
echo "test(0)" >> minimal.mlp
```

## Files Modified

- `self_host/mlp_compiler.c` - Parser, AST, Codegen
- `runtime/hashmap.o` - Recompiled with GNU flags
- `test/phase7_6_array_params.mlp` - Comprehensive test

## Summary

Phase 7.6 successfully implements array parameters with pass-by-reference semantics. Arrays are passed as pointers, enabling efficient data passing and modification. The implementation correctly distinguishes between local arrays (use `lea`) and array parameters (use `mov`) when accessing elements.

**Total Time:** ~3 hours (including debugging compilation issues)  
**Lines Changed:** ~150 lines across parser, AST, and codegen  
**Tests Created:** 3 comprehensive tests (sum, modify, max)

✅ **Phase 7.6 Complete**
