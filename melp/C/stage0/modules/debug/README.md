# Debug Module

**Date:** 6 Aralık 2025  
**Status:** Activated from future_modules  
**Original Name:** debug_features

---

## 🎯 DEBUG FEATURES

### Keywords
- `debug goto @label` - Debug modunda etikete git
- `debug pause` - Debug breakpoint
- `debug label @name` - Debug etiketi tanımla
- `debug print variable` - Debug modunda değişken yazdır

### Features
- ✅ Conditional debug blocks (only in --debug mode)
- ✅ Debug labels and goto
- ✅ Debug pause (breakpoints)
- ✅ Debug print (variable inspection)
- ✅ Debug if/then/else

---

## 📋 STRUCTURES

### DebugLabel
```c
typedef struct DebugLabel {
    char* name;             // Label name
    int line_number;        // Line number
    void* location;         // Code location
} DebugLabel;
```

### DebugGoto
```c
typedef struct DebugGoto {
    char* target_label;     // Target label
    int is_conditional;     // Conditional?
    void* condition;        // Condition
} DebugGoto;
```

### DebugPause
```c
typedef struct DebugPause {
    int line_number;        // Line number
    char* message;          // Optional message
    int is_conditional;     // Conditional?
    void* condition;        // Condition
} DebugPause;
```

### DebugPrint
```c
typedef struct DebugPrint {
    char* var_name;         // Variable name
    void* value;            // Value
    char* format;           // Optional format
} DebugPrint;
```

---

## 🔧 USAGE EXAMPLES

### Debug Label & Goto
```mlp
debug label @checkpoint1

if error then
    debug goto @checkpoint1
end if
```

### Debug Pause
```mlp
for i = 0 to 100
    if i == 50 then
        debug pause  -- Breakpoint at i=50
    end if
end for
```

### Debug Print
```mlp
numeric x = 42
debug print x  -- Only prints in debug mode
```

### Conditional Debug
```mlp
debug if x > threshold then
    debug pause
    debug print x
end if
```

---

## 🚀 COMPILATION

### Production Mode (default)
```bash
./melp program.mlp program.s
# All debug statements are NO-OP
```

### Debug Mode
```bash
./melp --debug program.mlp program.s
# Debug statements are active
```

---

## 📝 NOTES

- Debug features are **compile-time conditional**
- Zero overhead in production mode
- Useful for development and troubleshooting
- Compatible with existing control flow

---

**Activated by:** GitHub Copilot (Claude Sonnet 4.5)  
**Original Location:** `future_modules/debug_features/`
