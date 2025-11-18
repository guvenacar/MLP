# Python Preprocessor Archive

This directory contains the **deprecated** Python-based preprocessor that was used in earlier versions of MLP.

## Why Archived?

The MLP project philosophy states that the compiler should be written in MLP itself, without external language dependencies. The Python preprocessor violated this principle.

## Migration to C

As of the latest version, MLP uses a **C-based JSON parser** (`runtime/json_parser.c`) to read `diller.json` and provide keyword translation. This allows:

- ✅ No Python dependency
- ✅ User-editable `diller.json` at runtime
- ✅ Full MLP self-hosting capability
- ✅ Better performance

## Archived Files

- **dil_cevirici.py** (203 lines) - Python preprocessor that translated keywords
- **migrate.py** (2990 bytes) - Migration utility

## Historical Context

These files were created during the multi-language keyword development phase when:
- MLP needed quick prototyping of keyword translation
- String manipulation in MLP was limited
- JSON parsing in MLP was too complex

The Python implementation proved the concept worked, but the final implementation is now in C for compatibility with MLP's architecture.

## Do Not Use

**WARNING**: These files are kept for historical reference only. They are no longer part of the build system and should not be used in new code.

The current preprocessor architecture is:
```
Source Code (any language)
    ↓
C JSON Parser (runtime/json_parser.c)
    ↓ reads diller.json
Keyword Translation
    ↓
MLP Compiler (mlpc.mlp)
    ↓
x86-64 Assembly
    ↓
Binary Executable
```

## If You Need Them

If for some reason you need to reference the Python implementation, it remains in git history:
```bash
git log --all --full-history -- archive_python/dil_cevirici.py
```

---

**Last Updated**: May 2024  
**Status**: Deprecated, Archived  
**Replacement**: `runtime/json_parser.c` + `diller.json`
