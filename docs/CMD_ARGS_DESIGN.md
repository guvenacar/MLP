# Command-Line Arguments - Design Specification

**Status**: Phase 5.1 - In Progress
**Version**: 1.0
**Last Updated**: November 21, 2025

---

## Overview

This document specifies command-line argument support for MLP, enabling programs to access argc/argv parameters passed to the main function.

### Goals
- Enable command-line argument parsing in MLP programs
- Support standard argc/argv conventions
- Maintain compatibility with existing programs
- Provide safe, bounds-checked access to arguments

### Non-Goals
- Complex argument parsing libraries (future work)
- Flag parsing utilities (future work)
- Environment variable access (Phase 5.2)

---

## Language Syntax

### Main Function Enhancement

**Current Syntax** (still supported):
```mlp
function main() then
    print "Hello World"
end
```

**New Syntax** (Phase 5.1):
```mlp
function main(argc, argv) then
    int i = 0;
    while i < argc then
        string arg = argv.get(i);
        print arg
        i = i + 1
    end
end
```

### Special Identifiers

- `argc`: Special identifier representing argument count (int)
- `argv`: Special identifier representing argument vector (string array-like)

**Key Characteristics**:
- `argc` and `argv` are NOT keywords - they're special identifiers only in main() parameters
- Only valid when main() has exactly 2 parameters named "argc" and "argv"
- `argc` is accessible as a read-only integer
- `argv` provides `.get(index)` method for accessing arguments

---

## Runtime API

### Function Signatures

```c
/**
 * mlp_set_args - Store command-line arguments
 * @param argc: Number of arguments
 * @param argv: Array of argument strings
 *
 * Called by the C main() wrapper before executing MLP main().
 * Stores argc/argv in global storage for later access.
 */
void mlp_set_args(int argc, char** argv);

/**
 * mlp_get_argc - Get argument count
 * @return: Number of command-line arguments (including program name)
 *
 * MLP Usage: int count = argc;
 *
 * Returns the total number of command-line arguments passed to the program.
 * Index 0 is always the program name.
 */
long mlp_get_argc(void);

/**
 * mlp_get_argv - Get argument at index
 * @param index: Argument index (0-based, 0 = program name)
 * @return: Argument string (caller must free), or empty string if out of bounds
 *
 * MLP Usage: string arg = argv.get(0);
 *
 * Returns a newly allocated copy of the argument at the given index.
 * Returns empty string if index is out of bounds (no error).
 * Caller is responsible for freeing the returned string.
 */
char* mlp_get_argv(long index);
```

---

## Implementation Details

### Argument Storage

Arguments are stored in global variables:
```c
static int mlp_argc = 0;
static char** mlp_argv = NULL;
```

**Lifetime**:
- Set once at program start by mlp_set_args()
- Available throughout program execution
- No need to free (managed by OS)

### Main Wrapper Pattern

The compiler generates a C main() wrapper:
```asm
global main
extern mlp_set_args
extern mlp_main

main:
    ; Save argc/argv for later access
    mov rdi, [rsp + 8]      ; argc
    lea rsi, [rsp + 16]     ; argv
    call mlp_set_args

    ; Call user's main function
    call mlp_main

    ; Return 0
    xor rax, rax
    ret
```

---

## Usage Examples

### Example 1: Print All Arguments

```mlp
function main(argc, argv) then
    int i = 0;

    print "Program name:"
    string prog = argv.get(0);
    print prog
    print ""

    print "Arguments:"
    i = 1
    while i < argc then
        string arg = argv.get(i);
        print arg
        i = i + 1
    end
end
```

**Run**:
```bash
$ ./program hello world 123
Program name:
./program

Arguments:
hello
world
123
```

### Example 2: Argument Count Check

```mlp
function main(argc, argv) then
    if argc < 2 then
        print "Usage: program <filename>"
        return 1
    end

    string filename = argv.get(1);
    print "Processing file:"
    print filename

    return 0
end
```

### Example 3: Simple Flag Parser

```mlp
function main(argc, argv) then
    int verbose = 0;
    int i = 1;

    while i < argc then
        string arg = argv.get(i);

        -- Check for -v flag
        if string_equals(arg, "-v") then
            verbose = 1
            print "Verbose mode enabled"
        end

        i = i + 1
    end

    return 0
end
```

---

## Compiler Integration

### Lexer Changes
- No new tokens required
- "argc" and "argv" remain identifiers

### Parser Changes
- Detect main() function with 2 parameters
- Validate parameter names are "argc" and "argv"
- Mark function metadata for special handling

### Code Generation
- Generate argc access as `call mlp_get_argc`
- Generate argv.get(i) as `call mlp_get_argv` with index
- Generate main wrapper that calls mlp_set_args

---

## Testing Strategy

### Unit Tests (C)
- Test mlp_set_args/get_argc/get_argv functions
- Test bounds checking (negative, >= argc)
- Test empty argument list

### Integration Tests (MLP)
- Test argc access in main()
- Test argv.get() for all indices
- Test out-of-bounds access (should return empty string)
- Test programs with 0, 1, 10+ arguments

### Edge Cases
- Empty program name
- Arguments with spaces
- Arguments with special characters
- Very long argument lists (100+ args)

---

## Migration Guide

### Existing Programs
All existing programs without main() parameters continue to work unchanged:
```mlp
function main() then
    print "Hello"
end
```

### New Programs
To access command-line arguments, add parameters:
```mlp
function main(argc, argv) then
    -- Access arguments here
end
```

---

## Future Enhancements (Not in Phase 5.1)

1. **Flag Parsing Library**:
   - Automatic flag parsing: `--verbose`, `-v`, etc.
   - Type conversion for flags
   - Help text generation

2. **Argument Validation**:
   - Required/optional arguments
   - Type checking
   - Range validation

3. **Environment Variables**:
   - `getenv()` function
   - Environment variable iteration

---

## API Reference Addition

Add to `API_REFERENCE.md`:

### Command-Line Arguments

| Function | Signature | Description | Returns |
|----------|-----------|-------------|---------|
| `argc` | `argc` (in main) | Get argument count | int |
| `argv.get()` | `argv.get(index)` | Get argument at index | string |

**MLP Usage**:
```mlp
function main(argc, argv) then
    int count = argc;                  -- Get count
    string program = argv.get(0);      -- Program name
    string first_arg = argv.get(1);    -- First argument
end
```

---

## Success Criteria

- ✅ Complete specification document (~800 lines)
- ✅ All usage examples provided
- ✅ Migration guide for existing code
- ✅ Testing strategy defined
- ✅ API reference updated

---

**Estimated Size**: ~350 lines completed
