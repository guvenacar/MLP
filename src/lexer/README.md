# MLP Lexer

The lexer (lexical analyzer) is the first stage of the MLP compiler. It converts source code into a stream of tokens.

## Responsibilities

- Read source code character by character
- Group characters into meaningful tokens
- Handle comments and whitespace
- Report lexical errors
- Support multi-syntax preprocessing

## Token Types

- **Keywords**: `SINIF`, `IŞLEÇ`, `EĞER`, `DÖNGÜ`, etc.
- **Identifiers**: Variable and function names
- **Literals**: Numbers, strings, booleans
- **Operators**: `+`, `-`, `*`, `/`, `==`, etc.
- **Delimiters**: `(`, `)`, `{`, `}`, `;`, etc.

## Implementation Status

🚧 **Under Development** - Will be implemented in MLP itself during bootstrap phase.

## Files

- `tokenizer.mlp` - Main tokenization logic
- `token.mlp` - Token type definitions
- `keywords.mlp` - Language keyword definitions
