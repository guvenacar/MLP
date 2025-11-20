# MLP JSON Parser Design Document

## Overview
A pure MLP implementation of a JSON parser that requires no external dependencies (no regex, no HPL, no Python).

## Architecture

```
JSON String
    ↓
[1] Lexer: Tokenization
    → Break into tokens: {, }, [, ], :, comma, string, number, true, false, null
    ↓
[2] Parser: Structure Building
    → Build in-memory representation of JSON data
    ↓
[3] API: Query Interface
    → get_value(key), get_array_item(index), etc.
```

## Design Constraints

### MLP Current Capabilities
✅ Strings: string_uzunluk(), string_alt(), string_birlestir()
✅ Characters: char_code()
✅ Control flow: if/else, while, for
✅ Functions & recursion
✅ Integers: int (64-bit)

### MLP Limitations
❌ No dynamic arrays (fixed-size only)
❌ No structs (yet - under development)
❌ No hash maps / dictionaries
❌ No string builder (concatenation creates new strings)

## Solution: Simple Key-Value Storage

### Phase 1: Flat JSON Parser (MVP)
For preprocessor use case, we only need:
```json
{
  "languages": [
    {
      "id": "tr-TR",
      "keywords": {
        "int": ["sayısal", "tamsayı"],
        "if": ["eğer", "eger"]
      }
    }
  ]
}
```

**Simplified Approach:**
- Parse only what we need: language definitions
- Store as flat arrays of strings
- No nested object support initially

### Data Structures

#### Token (Simulated with parallel arrays)
```mlp
-- Token storage (global arrays)
int token_types[1000]       -- Token type codes
string token_values[1000]   -- Token values (for STRING, NUMBER)
int token_count = 0         -- Current token count
```

#### JSON Value Storage (Flat)
```mlp
-- Simple key-value storage
string json_keys[500]       -- Keys
string json_values[500]     -- Values (all stored as strings)
int json_count = 0          -- Entry count
```

## Token Types

```mlp
int TOKEN_LBRACE = 1        -- {
int TOKEN_RBRACE = 2        -- }
int TOKEN_LBRACKET = 3      -- [
int TOKEN_RBRACKET = 4      -- ]
int TOKEN_COLON = 5         -- :
int TOKEN_COMMA = 6         -- ,
int TOKEN_STRING = 7        -- "text"
int TOKEN_NUMBER = 8        -- 123, 45.67
int TOKEN_TRUE = 9          -- true
int TOKEN_FALSE = 10        -- false
int TOKEN_NULL = 11         -- null
int TOKEN_EOF = 12          -- End of input
```

## Lexer Algorithm

```
1. Initialize: pos = 0
2. While pos < length:
   a. Skip whitespace
   b. Read current character
   c. Match token type:
      - '{', '}', '[', ']', ':', ',' → Single-char tokens
      - '"' → Parse string until closing quote
      - '0'-'9' → Parse number
      - 't' → Check for "true"
      - 'f' → Check for "false"
      - 'n' → Check for "null"
   d. Store token
   e. Advance position
3. Return token array
```

## Parser Algorithm (Recursive Descent)

```
parse_value():
    token = current_token()
    if token == LBRACE:
        return parse_object()
    elif token == LBRACKET:
        return parse_array()
    elif token == STRING:
        return parse_string()
    elif token == NUMBER:
        return parse_number()
    elif token == TRUE/FALSE/NULL:
        return parse_literal()

parse_object():
    expect(LBRACE)
    while current_token() != RBRACE:
        key = expect(STRING)
        expect(COLON)
        value = parse_value()  // RECURSIVE!
        store(key, value)
        if current_token() == COMMA:
            advance()
    expect(RBRACE)
    return object

parse_array():
    expect(LBRACKET)
    index = 0
    while current_token() != RBRACKET:
        value = parse_value()  // RECURSIVE!
        store_array_item(index, value)
        index++
        if current_token() == COMMA:
            advance()
    expect(RBRACKET)
    return array
```

## API Design

### Core Functions

```mlp
-- Lexer
function json_tokenize(text) then
    -- Returns: token_count
end function

-- Parser
function json_parse(text) then
    -- Tokenize first
    int count = json_tokenize(text)
    -- Parse tokens into data structure
    return parse_value()
end function

-- Query API (simplified for preprocessor)
function json_get_string(key) then
    -- Returns: value as string, or "" if not found
end function

function json_get_language_keyword(lang_id, english_keyword) then
    -- Special query for language definitions
    -- Returns: native keyword (e.g., "tr-TR", "if" → "eğer")
end function
```

## Implementation Plan

### Step 1: Lexer (Token Scanner)
- File: `mlp_json/lexer.mlp`
- Functions:
  - `skip_whitespace(text, pos)`
  - `scan_string(text, pos)`
  - `scan_number(text, pos)`
  - `scan_keyword(text, pos)`
  - `tokenize(text)`

### Step 2: Parser (Structure Builder)
- File: `mlp_json/parser.mlp`
- Functions:
  - `parse_value()`
  - `parse_object()`
  - `parse_array()`
  - `parse_string()`
  - `parse_number()`

### Step 3: Query API
- File: `mlp_json/query.mlp`
- Functions:
  - `json_get(key)`
  - `json_get_nested(path)` // e.g., "languages.0.id"

### Step 4: Integration Test
- File: `mlp_json/test.mlp`
- Load actual `diller_comprehensive.json`
- Query language definitions
- Verify translations

## Limitations & Future Work

### MVP Limitations (Phase 1)
- Max 1000 tokens per JSON file
- Max 500 key-value pairs
- No nested object support (flat storage)
- All values stored as strings (manual conversion needed)
- No Unicode escape sequences
- No floating-point numbers (integers only)

### Future Enhancements (Phase 2)
- Dynamic arrays (when MLP adds support)
- True nested object support
- Hash map for O(1) lookups
- Full JSON spec compliance
- Error reporting with line numbers
- Streaming parser for large files

## Performance Estimates

### diller_comprehensive.json
- File size: ~15 KB
- Estimated tokens: ~800
- Estimated key-value pairs: ~200
- Parse time: < 1 second (acceptable for preprocessor)

### Memory Usage
- Tokens: 1000 × (4 bytes type + ~20 bytes value) = ~24 KB
- Key-value storage: 500 × (2 × ~50 bytes) = ~50 KB
- Total: ~100 KB (very reasonable)

## Success Criteria

Phase 1 is successful if:
1. ✅ Can tokenize diller_comprehensive.json
2. ✅ Can parse top-level structure
3. ✅ Can query: language ID → keyword mapping
4. ✅ Can return: "tr-TR", "if" → "eğer"
5. ✅ No crashes, no memory errors
6. ✅ Preprocessor can use this instead of Python

## Timeline

- **Day 1-2**: Lexer implementation & tests
- **Day 3-4**: Parser implementation & tests
- **Day 5**: Query API & integration tests
- **Day 6-7**: Real-world testing with diller_comprehensive.json

**Total: 1 week for JSON parser MVP**
