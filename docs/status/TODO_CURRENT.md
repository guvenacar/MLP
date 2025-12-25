# MLP Compiler TODO - Güncel (25 Kasım 2025)

**Self-Hosting Durumu:** %95 Complete

---

## 🔴 P0 - URGENT (Self-Hosting İçin Kritik)

### TODO-001: Fix Codegen Import Parsing Error
**Estimate:** 1-2 gün  
**Status:** 🔴 ACTIVE

**Komutlar:**
```bash
# 1. Full error capture:
./mlpc self_host/mlpc.mlp out.asm 2>&1 > error_full.log
tail -50 error_full.log

# 2. Fix HashMap confusion (if needed):
grep -r "mlp_list_get.*_map" self_host/compiler_core/*.mlp
sed -i 's/mlp_list_get(\([^,]*\)_map,/mlp_map_get(\1_map,/g' FILE.mlp

# 3. Re-test:
./mlpc self_host/mlpc.mlp out.asm
wc -l out.asm  # Should be >1000 lines
```

**Success:** Assembly file generated without errors

---

### TODO-002: Test Assembly & Bootstrap
**Estimate:** 4-6 saat  
**Status:** ⏸️ BLOCKED (TODO-001)

**Komutlar:**
```bash
# Stage 1:
./mlpc self_host/mlpc.mlp mlpc_s1.asm
nasm -f elf64 mlpc_s1.asm -o mlpc_s1.o
gcc mlpc_s1.o runtime/hashmap.o -o mlpc_stage1 -lm

# Stage 2:
./mlpc_stage1 self_host/mlpc.mlp mlpc_s2.asm
nasm -f elf64 mlpc_s2.asm -o mlpc_s2.o
gcc mlpc_s2.o runtime/hashmap.o -o mlpc_stage2 -lm

# Verify:
diff mlpc_s1.asm mlpc_s2.asm  # Should be identical
```

**Success:** Self-hosting complete! 🎉

---

## 🟡 P1 - HIGH (Cleanup Week)

### TODO-003: HashMap Method Audit
**Estimate:** 4-6 saat

```bash
# Fix all wrong calls:
for file in self_host/compiler_core/*.mlp; do
    sed -i 's/mlp_list_get(\([^,]*\)_map,/mlp_map_get(\1_map,/g' "$file"
    sed -i 's/mlp_list_set(\([^,]*\)_map,/mlp_map_set(\1_map,/g' "$file"
done
```

---

### TODO-004: Remove Debug Output
**Estimate:** 2-3 saat

```c
// Add at top:
#ifdef DEBUG_MODE
#define DEBUG_PRINT(...) fprintf(stderr, __VA_ARGS__)
#else
#define DEBUG_PRINT(...)
#endif

// Replace all fprintf(stderr, "DEBUG: ...
DEBUG_PRINT("DEBUG: ...");
```

---

### TODO-005: Document Bootstrap
**Estimate:** 2-3 saat

Create: `BOOTSTRAP.md`, `bootstrap.sh`, update `README.md`

---

## 🟢 P2 - MEDIUM (1-2 Ay)

### TODO-006: Normalizer (2-3 hafta)
- Create `diller.json`, `syntax.json`
- Implement `mlp_normalize.py`
- Integrate pipeline

### TODO-007: Refactor Global State (1 hafta)
- `LexerState` struct
- Thread-safe parsing

### TODO-008: English Naming (3-5 gün)
- Tüm field names → İngilizce

### TODO-009: i18n Errors (1 hafta)
- Error codes system
- Multi-language messages

---

## 🔵 P3 - LOW (Nice-to-Have)

- Arena allocator
- Valgrind analysis
- Magic numbers → constants
- Parallel compilation

---

## 📊 Progress

**By Priority:**
- P0: 2 items
- P1: 3 items  
- P2: 4 items
- P3: 4 items

**Timeline:**
- Self-Hosting: 1-3 gün
- Cleanup: 1 hafta
- Refactoring: 4-6 hafta
- Enhancements: 4-8 hafta (optional)

---

## 🎯 Next Steps

1. Fix TODO-001 (codegen error)
2. Test bootstrap (TODO-002)
3. CELEBRATE! 🎉
4. Start cleanup week
