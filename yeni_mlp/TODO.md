# Yeni MLP - TODO List

**Created:** 25 Kasım 2025  
**Status:** Planning Phase

---

## 📋 Implementation Checklist

### **Phase 1: Normalizer (1-2 weeks)**

- [ ] Task 1.1: Test normalizer with Turkish samples
  - [ ] Create 20+ Turkish MLP examples
  - [ ] Verify keyword replacement
  - [ ] Check edge cases (strings, comments)

- [ ] Task 1.2: Add more language support
  - [ ] Spanish mapping
  - [ ] French mapping
  - [ ] German mapping

- [ ] Task 1.3: Error handling
  - [ ] Invalid keyword detection
  - [ ] Unicode support
  - [ ] Line number preservation

---

### **Phase 2: Lexer (1 week)**

- [ ] Task 2.1: Token type definitions
  - [ ] Define all 80+ token types
  - [ ] Create token struct
  - [ ] Implement getNextToken()

- [ ] Task 2.2: Keyword recognition
  - [ ] check_keyword() function
  - [ ] Load from syntax.json
  - [ ] Case sensitivity handling

- [ ] Task 2.3: String/Number literals
  - [ ] String parsing (single/double/triple quotes)
  - [ ] Number parsing (int, float, hex, binary)
  - [ ] Escape sequence handling

- [ ] Task 2.4: Unit tests
  - [ ] 50+ test cases
  - [ ] Code coverage >90%
  - [ ] Edge case testing

---

### **Phase 3: Parser (2-3 weeks)**

- [ ] Task 3.1: AST node definitions
  - [ ] Define 40+ AST node types
  - [ ] Create AST struct (English naming)
  - [ ] Memory management strategy

- [ ] Task 3.2: Context stack
  - [ ] Implement push/pop_context
  - [ ] Test nested blocks
  - [ ] Verify proper cleanup

- [ ] Task 3.3: Expression parsing
  - [ ] Operator precedence
  - [ ] Binary/unary operations
  - [ ] Function calls
  - [ ] Array/member access

- [ ] Task 3.4: Statement parsing
  - [ ] if/while/for statements
  - [ ] Function declarations
  - [ ] Struct/enum declarations
  - [ ] Import statements

- [ ] Task 3.5: Import system
  - [ ] Circular import detection
  - [ ] Nested import handling
  - [ ] Path resolution

- [ ] Task 3.6: Unit tests
  - [ ] 100+ test cases
  - [ ] Complex nested structures
  - [ ] Error recovery

---

### **Phase 4: Code Generator (3-4 weeks)**

- [ ] Task 4.1: Basic code generation
  - [ ] Function prologue/epilogue
  - [ ] Variable allocation
  - [ ] Expression evaluation

- [ ] Task 4.2: Register allocation
  - [ ] Simple allocator
  - [ ] Spill to stack
  - [ ] Optimization passes

- [ ] Task 4.3: Control flow
  - [ ] if/while/for labels
  - [ ] Jump instructions
  - [ ] Break/continue handling

- [ ] Task 4.4: Function calls
  - [ ] System V AMD64 ABI
  - [ ] Parameter passing
  - [ ] Return value handling

- [ ] Task 4.5: Runtime integration
  - [ ] Built-in function calls
  - [ ] String operations
  - [ ] Memory management

- [ ] Task 4.6: Unit tests
  - [ ] 80+ test cases
  - [ ] Verify assembly correctness
  - [ ] Performance benchmarks

---

### **Phase 5: Self-Hosting (2-3 weeks)**

- [ ] Task 5.1: Write mlpc.mlp
  - [ ] Port lexer.c → lexer.mlp
  - [ ] Port parser.c → parser.mlp
  - [ ] Port codegen.c → codegen.mlp

- [ ] Task 5.2: Bootstrap process
  - [ ] Stage0 (C) compiles mlpc.mlp
  - [ ] Stage1 (MLP) compiles itself
  - [ ] Verify Stage1 == Stage2

- [ ] Task 5.3: Integration tests
  - [ ] Compile standard library
  - [ ] Compile example programs
  - [ ] Performance comparison

- [ ] Task 5.4: Documentation
  - [ ] Bootstrap guide
  - [ ] API reference
  - [ ] Tutorial examples

---

## 🎯 Milestones

- **M1:** Normalizer working (Week 2)
- **M2:** Lexer complete (Week 3)
- **M3:** Parser complete (Week 6)
- **M4:** Codegen working (Week 10)
- **M5:** Self-hosting achieved (Week 13)

---

## 📊 Progress Tracking

**Overall:** 0%

- Phase 1: 0%
- Phase 2: 0%
- Phase 3: 0%
- Phase 4: 0%
- Phase 5: 0%

---

## 🚀 Next Steps

1. Implement normalizer (Task 1.1)
2. Create Turkish test samples
3. Run first normalization test

---

**Last Updated:** 25 Kasım 2025
