# 🤖 FOR AI ASSISTANTS: START HERE

**If you are an AI assistant working on this project, READ THIS IMMEDIATELY:**

## ⚠️ CRITICAL INSTRUCTIONS

1. **STOP** - Do not make any changes yet
2. **READ** `AI_HANDOFF_DOCUMENT.md` in its entirety (771 lines)
3. **UNDERSTAND** why the current modular architecture exists
4. **VERIFY** current state by running `./bootstrap_stage2.sh`
5. **ONLY THEN** proceed with enhancements

## 📖 Essential Reading Order

```
1. AI_HANDOFF_DOCUMENT.md  ← START HERE (comprehensive guide)
2. STAGE2_COMPLETE.md       ← Understand what's working
3. STAGE3_PLAN_REVISED.md   ← Understand what's next
```

## ⛔ CRITICAL DON'Ts

- ❌ DO NOT rewrite existing modules
- ❌ DO NOT merge modules (defeats the purpose)
- ❌ DO NOT suggest "simpler" monolithic approaches
- ❌ DO NOT change the architecture
- ❌ DO NOT start coding before reading documentation

## ✅ CRITICAL DOs

- ✅ READ `AI_HANDOFF_DOCUMENT.md` first
- ✅ Build on existing work incrementally
- ✅ Test before every commit
- ✅ Preserve the modular structure
- ✅ Ask user if uncertain

## 🎯 Current Status (4 Aralık 2025)

```
Branch: melp20241204
Commit: 63a1024

Stage 2: ✅ COMPLETE (1908 lines, 5 modules, all working)
Stage 3: 🔄 IN PROGRESS
  Phase 1: ✅ GC Implementation (270 lines, 7 tests passing)
  Phase 2: ⏳ Next Task (Enhanced Lexer Compiler)
```

## 🚀 Quick Verification

```bash
# Test current state (should all pass)
./bootstrap_stage2.sh
cd melp/runtime && make test
```

## 💬 Quote from User

> "kaçıncı defa yapılan işlerim çöp oldu"  
> (How many times my work has been thrown away)

**This project has failed 10+ times due to AIs rewriting everything.**

**Your job: Enhance, don't restart.**

---

**For detailed instructions, architecture explanation, and complete context:**
👉 **READ `AI_HANDOFF_DOCUMENT.md` NOW** 👈
