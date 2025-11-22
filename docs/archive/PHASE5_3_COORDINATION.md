# Phase 5.3 - Coordination Plan

**Phase**: Advanced I/O & Network Utilities  
**Status**: ✅ READY TO START  
**Start Date**: November 21, 2025  
**Timeline**: ~2 weeks  
**Coordinator**: Claude #3 (VSCode)

---

## 🎯 Phase Goal

Implement advanced I/O operations and basic network utilities needed for building practical applications and preparing for self-hosting.

### Why This Matters for Self-Hosting

A self-hosting compiler and practical applications need:
- **Binary File I/O**: Read/write compiled binaries, object files
- **Directory Operations**: Traverse source trees, manage build artifacts
- **Process Control**: Execute assembler, linker, external tools
- **Network Utilities**: Download dependencies, remote compilation (future)
- **Time Operations**: Build timestamps, profiling, benchmarks

---

## 📦 Features to Implement

### 1. Binary File I/O (4 functions)
- `read_binary(path)` - Read file as byte array
- `write_binary(path, data, size)` - Write bytes to file
- `get_file_info(path)` - Get metadata (size, modified time, permissions)
- `copy_file(source, dest)` - Copy file with error handling

### 2. Directory Operations (6 functions)
- `list_directory(path)` - List files in directory
- `create_directory(path)` - Create directory (mkdir)
- `remove_directory(path)` - Remove empty directory
- `directory_exists(path)` - Check if directory exists
- `get_current_dir()` - Get current working directory
- `change_directory(path)` - Change current directory

### 3. Process Control (4 functions)
- `execute_command(cmd)` - Run shell command, return exit code
- `get_command_output(cmd)` - Run command, capture output
- `get_process_id()` - Get current process ID
- `get_parent_process_id()` - Get parent process ID

### 4. Time & Date (4 functions)
- `format_timestamp(timestamp, format)` - Format Unix timestamp to string
- `parse_timestamp(date_string)` - Parse date string to timestamp
- `get_milliseconds()` - High-precision time for profiling
- `get_time_string()` - Current time as formatted string

### 5. Path Utilities (4 functions)
- `join_path(parts[], count)` - Join path components
- `get_file_extension(path)` - Extract file extension
- `get_file_name(path)` - Extract filename from path
- `get_directory(path)` - Extract directory from path

**Total**: 22 new functions

---

## 👥 Team Assignment

### Claude #3 (Koordinatör): Core Implementation
**Role**: Implement runtime functions and compiler integration  
**Branch**: `claude3` (permanent working branch)  
**Deliverables**:
- Runtime functions in `runtime/runtime.c` (~450 lines)
- Lexer tokens in `c_compiler/c_lexer.h` (~22 lines)
- Parser support in `c_compiler/c_parser.c` (~50 lines)
- Code generation in `c_compiler/c_generator.c` (~40 lines)
- Integration test file (~300 lines)

**Estimated**: ~860 lines of code

---

### Claude #2: Documentation & Examples
**Role**: Create design specs, examples, and API docs  
**Branch**: `claude2` (permanent working branch)  
**Deliverables**:
- `BINARY_IO_DESIGN.md` (~400 lines)
- `DIRECTORY_OPS_DESIGN.md` (~500 lines)
- `PROCESS_CONTROL_DESIGN.md` (~450 lines)
- `TIME_UTILITIES_DESIGN.md` (~350 lines)
- `PATH_UTILITIES_DESIGN.md` (~300 lines)
- `examples/file_manager.mlp` (~250 lines)
- `examples/build_system.mlp` (~350 lines)
- `examples/log_analyzer.mlp` (~200 lines)
- Update `API_REFERENCE.md` (~150 lines)

**Estimated**: ~2,950 lines of documentation

---

## 🔄 Development Workflow

### Stage 1: Planning & Coordination (Day 1)
- **Claude #3**: Read coordination plan, create task breakdown
- **Claude #2**: Review requirements, prepare documentation outline
- **Sync**: Confirm feature list and API signatures

### Stage 2: Parallel Development (Day 2-9)
- **Claude #3**: 
  - Day 2-3: Implement binary I/O + directory ops in runtime.c
  - Day 4-5: Implement process control + time utilities
  - Day 6-7: Implement path utilities + compiler integration
  - Day 8-9: Basic testing and bug fixes

- **Claude #2**:
  - Day 2-3: Write BINARY_IO_DESIGN.md + DIRECTORY_OPS_DESIGN.md
  - Day 4-5: Write PROCESS_CONTROL_DESIGN.md + TIME_UTILITIES_DESIGN.md
  - Day 6-7: Write PATH_UTILITIES_DESIGN.md + examples
  - Day 8-9: Update API docs, write tutorials

### Stage 3: Testing & Integration (Day 10-12)
- **Claude #3**: 
  - Write integration tests
  - Run full test suite
  - Fix bugs discovered during testing
  
- **Claude #2**:
  - Document any API changes
  - Update examples with test results
  - Write troubleshooting guide

### Stage 4: Finalization (Day 13-14)
- **Claude #3**: 
  - Final code review
  - Performance testing
  - Prepare merge to main
  
- **Claude #2**:
  - Final documentation review
  - Create release notes
  - Update ROADMAP.md

---

## 📁 File Structure

```
MLP/
├── runtime/
│   └── runtime.c                    (Add ~450 lines)
│
├── c_compiler/
│   ├── c_lexer.h                    (Add ~22 tokens)
│   ├── c_lexer.c                    (Update keyword map)
│   ├── c_parser.c                   (Add ~50 lines)
│   └── c_generator.c                (Add ~40 lines)
│
├── docs/
│   ├── BINARY_IO_DESIGN.md          (NEW: ~400 lines)
│   ├── DIRECTORY_OPS_DESIGN.md      (NEW: ~500 lines)
│   ├── PROCESS_CONTROL_DESIGN.md    (NEW: ~450 lines)
│   ├── TIME_UTILITIES_DESIGN.md     (NEW: ~350 lines)
│   ├── PATH_UTILITIES_DESIGN.md     (NEW: ~300 lines)
│   └── API_REFERENCE.md             (UPDATE: +150 lines)
│
├── examples/
│   ├── file_manager.mlp             (NEW: ~250 lines)
│   ├── build_system.mlp             (NEW: ~350 lines)
│   └── log_analyzer.mlp             (NEW: ~200 lines)
│
├── test/
│   ├── test_binary_io.mlp           (NEW: ~100 lines)
│   ├── test_directory_ops.mlp       (NEW: ~120 lines)
│   ├── test_process_control.mlp     (NEW: ~80 lines)
│   └── test_phase5_3_integration.mlp (NEW: ~300 lines)
│
├── PHASE5_3_COORDINATION.md         (THIS FILE)
└── ROADMAP.md                       (UPDATE Phase 5.3 status)
```

---

## ✅ Success Criteria

1. ✅ All 22 functions implemented and working
2. ✅ Compiler integration complete (lexer, parser, codegen)
3. ✅ Binary file I/O handles large files correctly
4. ✅ Directory operations work cross-platform (Linux focus)
5. ✅ Process execution captures output and exit codes
6. ✅ Time functions accurate to millisecond precision
7. ✅ Path utilities handle edge cases (empty, absolute, relative)
8. ✅ Documentation complete with examples
9. ✅ Integration tests pass (100%)
10. ✅ No memory leaks in file/directory operations

---

## 🔗 Dependencies

**Phase 5.3 depends on**:
- ✅ Phase 5.1 (Command-line args & enhanced strings) - COMPLETE
- ✅ Phase 5.2 (Error handling & memory management) - COMPLETE

**Phase 5.3 enables**:
- Phase 5.4: Module System (needs directory operations)
- Phase 5.5: Package Manager (needs network + file ops)
- Phase 6: Self-Hosting (needs process control for build system)

---

## 📞 Communication Protocol

### Daily Sync
- **Time**: End of each development day
- **Format**: GitHub commit messages + branch push
- **Content**: Progress update, blockers, questions

### Branch Strategy
- **claude3**: Claude #3 implementation branch
- **claude2**: Claude #2 documentation branch
- **Merge Target**: Main branch after both complete

### Issue Reporting
- Use GitHub Issues for bugs
- Tag with `phase5.3` label
- Assign to appropriate Claude

---

## 🎯 Priority Order

### Week 1 (Must Have)
1. Binary file I/O - Critical for compiler
2. Directory operations - Needed for source tree traversal
3. Process control - Execute assembler/linker

### Week 2 (Should Have)
4. Time utilities - Profiling and timestamps
5. Path utilities - Helper functions

### Nice to Have (If Time Permits)
- Network utilities (HTTP GET)
- Archive operations (tar/zip)
- Symbolic links support

---

## 📚 Reference Materials

### C Library Functions to Use
- **File I/O**: `fopen`, `fread`, `fwrite`, `fclose`, `stat`
- **Directory**: `opendir`, `readdir`, `closedir`, `mkdir`, `rmdir`
- **Process**: `system`, `popen`, `pclose`, `getpid`, `getppid`
- **Time**: `time`, `strftime`, `strptime`, `clock_gettime`
- **Path**: `dirname`, `basename`, `realpath`

### Security Considerations
- Validate all paths (prevent directory traversal)
- Limit command execution (prevent injection)
- Check file permissions before operations
- Handle symlinks carefully

### Performance Notes
- Buffer large file reads (4KB chunks)
- Cache directory listings when possible
- Async process execution (future)
- Minimize system calls

---

## 🚀 Getting Started

### For Claude #3 (Implementation)
1. Read this coordination plan
2. Review Phase 5.2 runtime.c for code style
3. Create feature branch from main
4. Start with binary I/O functions
5. Test each function before moving on

### For Claude #2 (Documentation)
1. Read this coordination plan
2. Review existing design docs for format
3. Create documentation branch
4. Start with BINARY_IO_DESIGN.md
5. Include code examples in every section

---

## 📊 Estimated Effort

| Task | Lines | Time | Owner |
|------|-------|------|-------|
| Runtime implementation | ~450 | 4 days | Claude #3 |
| Compiler integration | ~112 | 2 days | Claude #3 |
| Testing | ~600 | 2 days | Claude #3 |
| Design docs | ~2,000 | 3 days | Claude #2 |
| Example programs | ~800 | 2 days | Claude #2 |
| API updates | ~150 | 1 day | Claude #2 |
| **Total** | **~4,112** | **14 days** | **Both** |

---

## 🎉 Success Definition

Phase 5.3 is complete when:
- ✅ All 22 functions work correctly
- ✅ Can build a simple build system in MLP
- ✅ Can create a file manager tool
- ✅ Can execute external commands and capture output
- ✅ All documentation clear and comprehensive
- ✅ Zero known bugs in core functionality
- ✅ Ready to merge to main branch

---

**Created**: November 21, 2025  
**Coordinator**: Claude #3 (VSCode)  
**Status**: ✅ READY TO START

**Next Step**: Create implementation and documentation briefs for each Claude!
