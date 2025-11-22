# Phase 5.2 - Coordination Plan

**Phase**: Error Handling & Memory Management
**Status**: Planning
**Start Date**: November 20, 2025
**Timeline**: ~2 weeks
**Session ID**: [TO_BE_FILLED]

---

## 🎯 Phase Goal

Implement robust error handling and memory management features required for self-hosting compiler development.

### Why This Matters for Self-Hosting

A self-hosting compiler needs:
- **Error Reporting**: Syntax errors, semantic errors, file errors
- **Dynamic Memory**: AST nodes, symbol tables, string buffers, token arrays
- **Safe Exit**: Clean shutdown with proper error codes
- **Memory Tracking**: Detect leaks during compiler development

---

## 📦 Features to Implement

### 1. Error Handling (5 functions)
- `exit_with_code(code)` - Exit program with specific return code
- `panic(message)` - Print error message and exit with code 1
- `assert(condition, message)` - Runtime assertion with error message
- `get_error_code()` - Get last error code
- `set_error_code(code)` - Set error code

### 2. Memory Management (6 functions)
- `mlp_malloc(size)` - Allocate memory with tracking
- `mlp_free(ptr)` - Free memory with tracking
- `mlp_realloc(ptr, size)` - Reallocate memory
- `mlp_calloc(count, size)` - Allocate zeroed memory
- `get_allocated_bytes()` - Get total allocated memory
- `check_memory_leaks()` - Report memory leaks

### 3. System Utilities (3 functions)
- `get_env(name)` - Get environment variable
- `current_timestamp()` - Get Unix timestamp
- `sleep_ms(milliseconds)` - Sleep for milliseconds

**Total**: 14 new functions

---

## 👥 Team Assignment

### Claude #1: Core Implementation
**Role**: Implement runtime functions and compiler integration
**Branch**: `claude/phase5-2-implementation-[SESSION_ID]`
**Deliverables**:
- Runtime functions in `runtime/runtime.c` (~250 lines)
- Lexer tokens in `c_compiler/c_lexer.h` (~14 lines)
- Parser support in `c_compiler/c_parser.c` (~30 lines)
- Code generation in `c_compiler/c_generator.c` (~20 lines)
- Integration test file (~150 lines)

**Estimated**: ~450 lines of code

---

### Claude #2: Documentation & Design
**Role**: Create design specs, examples, and API docs
**Branch**: `claude/phase5-2-docs-[SESSION_ID]`
**Deliverables**:
- `ERROR_HANDLING_DESIGN.md` (~600 lines)
- `MEMORY_MANAGEMENT_DESIGN.md` (~700 lines)
- `examples/error_handling_demo.mlp` (~200 lines)
- `examples/memory_management_demo.mlp` (~250 lines)
- Update `API_REFERENCE.md` (~100 lines)
- Update `ROADMAP.md` (mark Phase 5.2 progress)

**Estimated**: ~1,850 lines of documentation

---

### Claude #3: Testing & QA
**Role**: Comprehensive testing and validation
**Branch**: `claude/phase5-2-testing-[SESSION_ID]`
**Deliverables**:
- C unit tests: `test/c_unit_tests/test_error_handling.c` (~300 lines)
- C unit tests: `test/c_unit_tests/test_memory_management.c` (~350 lines)
- MLP tests: `test/test_error_handling.mlp` (~200 lines)
- MLP tests: `test/test_memory_management.mlp` (~250 lines)
- Integration test: `test/test_phase5_2_integration.mlp` (~150 lines)
- Edge cases test: `test/test_phase5_2_edge_cases.mlp` (~200 lines)
- `test/README_TESTS_PHASE5_2.md` (~300 lines)

**Estimated**: ~1,750 lines of test code

---

## 🔄 Development Workflow

### Stage 1: Planning & Design (Day 1-2)
- **All**: Read coordination brief and understand requirements
- **Claude #2**: Create design documents (ERROR_HANDLING_DESIGN.md, MEMORY_MANAGEMENT_DESIGN.md)
- **Claude #1**: Review designs and prepare implementation plan
- **Claude #3**: Review designs and prepare test plan

### Stage 2: Parallel Development (Day 3-8)
- **Claude #1**: Implement runtime functions → Compiler integration → Basic tests
- **Claude #2**: Write examples → Update API docs → Create tutorials
- **Claude #3**: Write C unit tests → Write MLP integration tests → Edge cases

### Stage 3: Integration & Testing (Day 9-11)
- **Claude #1**: Fix bugs reported by Claude #3
- **Claude #2**: Document any API changes
- **Claude #3**: Run full test suite, report results

### Stage 4: Finalization (Day 12-14)
- **All**: Final testing and validation
- **Claude #1**: Create PR for implementation
- **Claude #2**: Create PR for documentation
- **Claude #3**: Create PR for tests
- **Coordinator**: Review and merge PRs

---

## 📁 File Structure

```
/home/user/MLP/
├── runtime/
│   └── runtime.c                              (MODIFIED: +250 lines)
├── c_compiler/
│   ├── c_lexer.h                              (MODIFIED: +14 lines)
│   ├── c_lexer.c                              (MODIFIED: +14 lines)
│   ├── c_parser.c                             (MODIFIED: +30 lines)
│   └── c_generator.c                          (MODIFIED: +20 lines)
├── docs/
│   ├── ERROR_HANDLING_DESIGN.md               (NEW: ~600 lines)
│   └── MEMORY_MANAGEMENT_DESIGN.md            (NEW: ~700 lines)
├── examples/
│   ├── error_handling_demo.mlp                (NEW: ~200 lines)
│   └── memory_management_demo.mlp             (NEW: ~250 lines)
├── test/
│   ├── c_unit_tests/
│   │   ├── test_error_handling.c              (NEW: ~300 lines)
│   │   └── test_memory_management.c           (NEW: ~350 lines)
│   ├── test_error_handling.mlp                (NEW: ~200 lines)
│   ├── test_memory_management.mlp             (NEW: ~250 lines)
│   ├── test_phase5_2_integration.mlp          (NEW: ~150 lines)
│   ├── test_phase5_2_edge_cases.mlp           (NEW: ~200 lines)
│   └── README_TESTS_PHASE5_2.md               (NEW: ~300 lines)
├── API_REFERENCE.md                           (MODIFIED: +100 lines)
└── ROADMAP.md                                 (MODIFIED: +20 lines)
```

**Total New Code**: ~4,050 lines

---

## ✅ Success Criteria

1. ✅ All 14 functions implemented and working
2. ✅ Compiler integration complete (lexer, parser, codegen)
3. ✅ All C unit tests passing
4. ✅ All MLP integration tests passing
5. ✅ Memory tracking accurately reports allocations
6. ✅ Error handling properly exits with correct codes
7. ✅ Documentation complete and comprehensive
8. ✅ No breaking changes to existing code
9. ✅ Example programs compile and run correctly
10. ✅ All PRs reviewed and merged

---

## 🔗 Dependencies

**Phase 5.2 depends on**:
- ✅ Phase 5.1 (Command-line args & enhanced strings) - COMPLETE

**Phase 5.2 enables**:
- Phase 5.3: Module system (needs error handling)
- Phase 5.4: Advanced features (needs memory management)
- Phase 6: Self-hosting (needs both)

---

## 📞 Communication Protocol

### Progress Updates
- Each Claude commits regularly with clear messages
- Use conventional commit format: `feat(phase5.2):`, `test(phase5.2):`, `docs(phase5.2):`

### Blockers & Questions
- Document blockers in brief files
- Critical issues: Alert coordinator

### Coordination Points
- Day 2: Design review checkpoint
- Day 8: Integration checkpoint
- Day 11: Testing checkpoint
- Day 14: Final review

---

## 🎯 Next Steps

1. **Coordinator** (NOW): Create individual brief files for each Claude
2. **All Claudes**: Read coordination plan and individual briefs
3. **Claude #2**: Start design documents (Day 1-2)
4. **Claude #1**: Wait for design review, then implement (Day 3+)
5. **Claude #3**: Wait for initial implementation, then test (Day 5+)

---

## 📚 Reference Materials

- [Phase 5.1 Implementation](PHASE5_1_BRIEF_CLAUDE1_IMPLEMENTATION.md) - Previous phase example
- [API_REFERENCE.md](API_REFERENCE.md) - Current API state
- [ROADMAP.md](ROADMAP.md) - Overall project plan
- [TURING_COMPLETE_PLAN.md](TURING_COMPLETE_PLAN.md) - Self-hosting requirements

---

**Status**: Ready for brief creation
**Next Action**: Coordinator creates 3 individual briefs
