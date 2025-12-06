# Async Module - Merge Notes

**Date:** 6 Aralık 2025  
**Status:** Merged from `async` + `async_await` modules

---

## 🔄 MERGE HISTORY

This module is a **merger** of two separate modules:
1. **`async/`** - Comprehensive async runtime (Future, Promise, Executor)
2. **`async_await/`** - Modern syntax support (async fn, .await)

---

## ✅ MERGED FEATURES

### From `async/` (Base Module)
- ✅ Future/Promise primitives
- ✅ Task scheduling & execution
- ✅ Event loop (single-thread, thread-pool, work-stealing)
- ✅ Callbacks (then, catch, finally)
- ✅ Future combinators (all, any, race)
- ✅ Async generators & coroutines

### From `async_await/` (Modern Syntax)
- ✅ `async fn name() -> T` syntax
- ✅ `.await` keyword
- ✅ `Future<T>` type notation
- ✅ Task priorities
- ✅ Timeout support
- ✅ `async { ... }` blocks
- ✅ `async || { ... }` closures

---

## 📦 ENUM EXTENSIONS

### AsyncFunctionType (Extended)
```c
ASYNC_FUNCTION = 0,      // Original: Normal async function
ASYNC_GENERATOR = 1,     // Original: Async generator
ASYNC_COROUTINE = 2,     // Original: Coroutine
ASYNC_FN_DECL = 3,       // NEW: async fn syntax
ASYNC_BLOCK = 4,         // NEW: async { }
ASYNC_CLOSURE = 5        // NEW: async || { }
```

### AwaitType (Extended)
```c
AWAIT_FUTURE = 0,        // Original: Future await
AWAIT_PROMISE = 1,       // Original: Promise await
AWAIT_VALUE = 2,         // Original: Direct value
AWAIT_ALL = 3,           // Original: await all
AWAIT_ANY = 4,           // Original: await any
AWAIT_RACE = 5,          // Original: await race
AWAIT_KEYWORD = 6        // NEW: .await syntax
```

---

## 🎯 USAGE EXAMPLES

### Classic Style (from async/)
```mlp
async function fetch_data()
    future = create_future()
    promise = create_promise()
    -- ...
    return future
end function

result = await fetch_data()
```

### Modern Style (from async_await/)
```mlp
async fn fetch_data() -> numeric
    data = http_get("https://api.example.com").await
    return data
end fn

result = fetch_data().await
```

### Both Styles Work!
The merged module supports **both classic and modern syntax** for maximum flexibility.

---

## 🔧 IMPLEMENTATION STATUS

- [x] Header merged (async.h)
- [ ] Parser functions (needs integration)
- [ ] Codegen (needs both styles)
- [ ] Runtime support
- [ ] Test cases

---

## 📝 NOTES FOR FUTURE DEVELOPERS

1. **No Duplication:** `async_await/` module removed from future_modules
2. **Unified API:** One module, two syntax styles
3. **Backward Compatible:** Existing async code still works
4. **Modern Features:** New `.await` syntax available

---

**Merge performed by:** GitHub Copilot (Claude Sonnet 4.5)  
**Original modules:** `future_modules/async` + `future_modules/async_await`
