# Phase 3 - Documentation Master Brief (Claude #2)

**Session**: Phase 3 - Self-Hosting Completion
**Your Role**: Documentation & Example Programs
**Branch**: `claude/phase3-documentation-[SESSION_ID]`
**Timeline**: ~2-3 hours

---

## 🎯 Your Mission

Create comprehensive documentation and example programs for Phase 3 features:
- File I/O operations
- String manipulation functions
- Hash tables/Maps
- Full MLP compiler written in MLP (example/reference)

---

## 📋 Tasks

### 1. Update Design Documents

#### A. Create `FILE_IO_DESIGN.md`
**Content**:
```markdown
# File I/O Design Specification

## Overview
Phase 3 adds file operations for reading/writing files.

## API Design

### Functions
- `string read_file(string path)` - Read entire file
- `int write_file(string path, string content)` - Write to file
- `list[string] read_lines(string path)` - Read file as lines
- `int append_file(string path, string content)` - Append to file
- `int file_exists(string path)` - Check if file exists
- `int file_size(string path)` - Get file size in bytes

### Error Handling
- Return empty string on read failure
- Return -1 on write failure
- Return 0 on success

### Example Usage
```mlp
-- Read source code
string source = read_file("program.mlp");

-- Process and write
string result = compile(source);
write_file("output.asm", result);
```

## Runtime Implementation
- C functions in runtime.c
- Error checking with errno
- Memory management for large files

## Security Considerations
- Path traversal prevention
- File size limits
- Permission checks
```

#### B. Create `STRING_OPS_DESIGN.md`
**Functions to document**:
```mlp
-- Existing (already have)
string substring(string str, int start, int len)
int string_length(string str)
int char_code(string ch)

-- NEW Phase 3 functions
list[string] string_split(string str, string delim)
string string_join(list[string] parts, string delim)
string string_replace(string str, string old, string new)
string string_trim(string str)
string string_upper(string str)
string string_lower(string str)
int string_find(string str, string needle)
int string_starts_with(string str, string prefix)
int string_ends_with(string str, string suffix)
```

#### C. Create `HASHMAP_DESIGN.md`
**Content**:
```markdown
# Hash Map / Dictionary Design

## Syntax
```mlp
map[string:int] ages = map();
ages.set("Alice", 25);
ages.set("Bob", 30);

int alice_age = ages.get("Alice");
int has_bob = ages.has("Bob");
ages.remove("Alice");
```

## Use Cases
- Symbol table (variable name → type)
- String interning
- Configuration storage
- AST attributes

## Implementation Strategy
- Chaining or open addressing
- Dynamic resizing (load factor 0.75)
- String keys (most important)
```

#### D. Update `TURING_COMPLETE_PLAN.md`
Mark Phase 2 as complete, add Phase 3 details:
```markdown
## Phase 2: Dynamic Lists ✅ COMPLETE
- Runtime: 196 lines
- Parser: 178 lines
- JSON Lexer: 269 lines MLP code
- Status: Self-hosting capable!

## Phase 3: File I/O & Self-Hosting (IN PROGRESS)
**Goals**:
- File operations (read/write)
- String manipulation (split/join/replace)
- Hash maps for symbol tables
- Full MLP compiler in MLP

**Timeline**: 2-3 weeks
**Status**: Starting...
```

### 2. Example Programs

#### A. Create `examples/file_processor.mlp`
**Demonstrates file I/O**:
```mlp
-- File processor example
function count_lines(filename) then
    list[string] lines = read_lines(filename);
    int count = lines.size();
    return count
end

function process_file(input, output) then
    string content = read_file(input);

    -- Transform content (example: uppercase)
    string result = string_upper(content);

    write_file(output, result);

    print "Processed: ";
    print input;
    print " -> ";
    print output;
    return 0
end

-- Usage
process_file("input.txt", "output.txt");
```

#### B. Create `examples/simple_compiler.mlp`
**Mini compiler in MLP** (simplified for demonstration):
```mlp
-- Simple expression compiler
-- Input: "x + 5 * 3"
-- Output: Assembly code

struct Token then
    string type;
    string value;
end

function lex(source) then
    list[Token] tokens = list();
    -- Tokenization logic...
    return tokens
end

function parse(tokens) then
    -- Build AST from tokens
    return ast
end

function generate(ast) then
    string asm = "";
    -- Generate assembly...
    return asm
end

function compile_file(input_path, output_path) then
    string source = read_file(input_path);

    list[Token] tokens = lex(source);
    AST tree = parse(tokens);
    string code = generate(tree);

    write_file(output_path, code);

    print "Compiled successfully!";
    return 0
end

-- Compile program
compile_file("program.mlp", "program.asm");
```

#### C. Create `examples/symbol_table.mlp`
**Symbol table with hash map**:
```mlp
-- Symbol table for compiler

map[string:string] symbols = map();

function define_variable(name, type) then
    symbols.set(name, type);
    return 0
end

function lookup_variable(name) then
    if symbols.has(name) == 1 then
        string type = symbols.get(name);
        return type
    end
    return "";
end

-- Usage
define_variable("x", "int");
define_variable("name", "string");

string x_type = lookup_variable("x");
print x_type;  -- Prints: int
```

### 3. Tutorial Documentation

#### Create `TUTORIAL_PHASE3.md`
```markdown
# Phase 3 Tutorial: Building Your First Compiler

## Introduction
With Phase 3, MLP can now read/write files and process text.
Let's build a simple calculator compiler!

## Step 1: Setup
Create your project structure:
```
calculator/
  ├── lexer.mlp
  ├── parser.mlp
  ├── codegen.mlp
  └── main.mlp
```

## Step 2: Lexer (lexer.mlp)
[Complete lexer example with file I/O]

## Step 3: Parser (parser.mlp)
[Parser implementation]

## Step 4: Code Generator (codegen.mlp)
[Assembly generation]

## Step 5: Main Driver (main.mlp)
[File reading, compilation pipeline]

## Running Your Compiler
```bash
./mlp_compiler main.mlp
./output "2 + 3 * 4"
# Output: 14
```

## Next Steps
- Add more operators
- Support variables
- Implement functions
```

### 4. API Reference

#### Create `API_REFERENCE.md`
Complete reference for all MLP built-in functions:

```markdown
# MLP API Reference

## File Operations

### `read_file(path: string) -> string`
Reads entire file into string.
- **Returns**: File contents or empty string on error
- **Example**: `string code = read_file("program.mlp");`

### `write_file(path: string, content: string) -> int`
Writes string to file.
- **Returns**: 0 on success, -1 on error
- **Example**: `write_file("output.txt", result);`

[Continue for all functions...]

## String Operations
[Document all string functions...]

## List Operations
[Document from Phase 2...]

## Struct Operations
[Document from Phase 1...]
```

### 5. Session Summary

#### Create `SESSION_SUMMARY_PHASE3.md`
Document your work process:
- Design decisions made
- Examples created
- Documentation structure
- Cross-references between docs

---

## 📂 File Structure

Your work should create:
```
/home/user/MLP/
├── FILE_IO_DESIGN.md           (NEW)
├── STRING_OPS_DESIGN.md        (NEW)
├── HASHMAP_DESIGN.md           (NEW)
├── TUTORIAL_PHASE3.md          (NEW)
├── API_REFERENCE.md            (NEW)
├── TURING_COMPLETE_PLAN.md     (UPDATE)
├── SESSION_SUMMARY_PHASE3.md   (NEW)
└── examples/
    ├── file_processor.mlp      (NEW)
    ├── simple_compiler.mlp     (NEW)
    └── symbol_table.mlp        (NEW)
```

---

## ✅ Success Criteria

1. **Complete documentation** for all Phase 3 features
2. **3+ working example programs** demonstrating new features
3. **Tutorial** for building first compiler in MLP
4. **API reference** for all built-in functions
5. **Design specs** for file I/O, strings, hashmaps
6. **Updated roadmap** (TURING_COMPLETE_PLAN.md)

---

## 🎯 Key Points

- **Focus**: Documentation, examples, tutorials
- **Quality over quantity**: Clear, executable examples
- **Think like a user**: What would you want to see in docs?
- **Cross-reference**: Link between documents
- **Keep it practical**: Examples should solve real problems

---

## 🔧 Git Workflow

```bash
# Create your branch
git checkout -b claude/phase3-documentation-[YOUR_SESSION_ID]

# Commit as you go
git add FILE_IO_DESIGN.md
git commit -m "docs: Add File I/O design specification"

git add examples/file_processor.mlp
git commit -m "docs: Add file processing example"

# Push when done
git push -u origin claude/phase3-documentation-[YOUR_SESSION_ID]
```

---

## 📝 Notes

- Coordinate with Claude #1 (implementation) on API details
- Ask for clarification if needed
- Test your examples for correctness (pseudo-code is OK if feature not implemented yet)
- Include comments in example code for clarity

---

## 🎉 When You're Done

Report back with:
1. List of files created/updated
2. Number of examples written
3. Documentation coverage percentage
4. Any suggestions for Phase 3 features

**Branch**: `claude/phase3-documentation-[SESSION_ID]`
**Expected commits**: 5-10 commits
**Expected time**: 2-3 hours

Good luck! 🚀
