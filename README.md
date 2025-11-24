# MLP - Multi-Language Programming

> **"Code in Your Language, Compile to Machine Code!"** 🌍

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20macOS-blue)]()
[![Status](https://img.shields.io/badge/status-Self--Hosting%20✅-success)]()
[![Languages](https://img.shields.io/badge/languages-5%2B-green)]()

---

## 🌟 What is MLP?

**MLP (Multi-Language Programming)** is a programming language that lets you write code using keywords from **your native language**.

Instead of forcing everyone to use English keywords like `if`, `while`, `function`, MLP supports:
- 🇹🇷 **Turkish**: `eğer`, `döngü`, `işleç`
- 🇷🇺 **Russian**: `если`, `пока`, `функция`
- 🇨🇳 **Chinese**: `如果`, `当`, `函数`
- 🇮🇳 **Hindi**: `अगर`, `जब_तक`, `फलन`
- ⚙️ **Custom**: Define your own keywords!

**Core Philosophy:** Programming should be accessible in any language, not just English.

---

## 🎉 Project Status: PRODUCTION READY!

- ✅ **English-Native Compiler** (C): Fully working
- ✅ **Multi-Language Preprocessor** (Python): Turkish, Russian, Chinese, Hindi
- ✅ **Self-Hosting Compiler** (MLP): 984 lines of code
- ✅ **x86-64 Assembly Output**: NASM syntax
- ✅ **Struct Support**: Structured data types
- ✅ **One-Command Compilation**: `mlpc` wrapper script

---

## 🚀 Quick Start

### Example: Hello World in 4 Languages

**Turkish:**
```mlp
-- lang: tr-TR
SAYISAL x = 42;
YAZDIR "Merhaba Dünya"
YAZDIR x
```

**Russian:**
```mlp
-- lang: ru-RU
целое x = 42;
печать "Привет Мир"
печать x
```

**Chinese:**
```mlp
-- lang: zh-CN
整数 x = 42;
打印 "你好世界"
打印 x
```

**Hindi:**
```mlp
-- lang: hi-IN
संख्या x = 42;
लिखो "नमस्ते दुनिया"
लिखो x
```

**All four compile to the same executable!**

### Compilation

```bash
# Clone repository
git clone https://github.com/guvenacar/MLP.git
cd MLP

# Build compiler
cd c_compiler
make

# Compile Turkish source
cd ..
./mlpc test_turkish.mlp -o hello
./hello
```

---

## 🌍 Supported Languages

| Language | ID | Keywords Example |
|----------|----|--------------------|
| **English** | en-US | `int`, `if`, `while`, `function`, `print` |
| **Turkish** | tr-TR | `sayısal`, `eğer`, `döngü`, `işleç`, `yazdır` |
| **Russian** | ru-RU | `целое`, `если`, `пока`, `функция`, `печать` |
| **Chinese** | zh-CN | `整数`, `如果`, `当`, `函数`, `打印` |
| **Hindi** | hi-IN | `संख्या`, `अगर`, `जब_तक`, `फलन`, `लिखो` |
| **Custom** | custom-* | Define your own! See `diller.json` |

---

## 🏗️ Architecture

### Pipeline

**🔴 CRITICAL: 3-Stage Pipeline - Lexer NEVER sees non-English keywords!**

```
Multi-Language Source (.mlp)
  Example: if (x > 0) { YAZDIR "Merhaba" }  (C-style syntax + Turkish)
        ↓
┌──────────────────────────────────────────┐
│  STAGE 1: SYNTAX PREPROCESSOR (Python)   │
│  ════════════════════════════════════════│
│  → syntax_preprocessor.py                │
│  1. Detect syntax style (syntax.json):   │
│     - C-style: { } braces                │
│     - Python-style: : indentation        │
│     - MLP-default: keyword blocks        │
│  2. Normalize to MLP base syntax:        │
│     { → then                             │
│     } → end                              │
│     : + indent → then / dedent → end    │
│  3. PRESERVE keywords (any language)     │
│  4. PRESERVE strings & comments          │
└──────────────────────────────────────────┘
        ↓
Normalized MLP (.mlp)
  Example: if x > 0 then YAZDIR "Merhaba" end if
        ↓
┌──────────────────────────────────────────┐
│  STAGE 2: LANGUAGE PREPROCESSOR (C)      │
│  ════════════════════════════════════════│
│  → mlp_preprocessor.c                    │
│  1. Detect: -- lang: tr-TR               │
│  2. Load: diller.json keyword mappings   │
│  3. Translate KEYWORDS:                  │
│     YAZDIR → print                       │
│     EĞER → if                            │
│     SAYISAL → numeric                    │
│  4. PRESERVE string contents (UTF-8):    │
│     "Merhaba" → "Merhaba" (unchanged)   │
│  5. PRESERVE comments                    │
└──────────────────────────────────────────┘
        ↓
English IR (.mlp)
  Keywords: print, if, numeric (English)
  Strings: "Merhaba" (UTF-8 preserved)
        ↓
┌──────────────────────────────────────────┐
│  COMPILER (mlpc - C)                     │
│  ════════════════════════════════════════│
│  → Lexer: English keywords only          │
│  → Parser: Build AST                     │
│  → UTF-8 Handler: Detect non-ASCII       │
│    * UTF-8 string → byte sequence        │
│      "Merhaba" → db 77,101,114,...,0     │
│    * ASCII string → quoted               │
│      "Hello" → db "Hello", 0             │
│  → Generator: Produce Assembly           │
└──────────────────────────────────────────┘
        ↓
x86-64 Assembly (.asm - NASM)
  str_1: db 77,101,114,104,97,98,97,0  ; UTF-8
  str_2: db "Hello", 0                 ; ASCII
        ↓
┌──────────────────────────────────────────┐
│  NASM + GCC                              │
│  → Assemble & Link with runtime          │
└──────────────────────────────────────────┘
        ↓
Native Executable
  ✅ Displays: Merhaba (UTF-8 correct)
  ✅ Displays: Привет (Russian works)
  ✅ Displays: नमस्ते (Hindi works)
```

### Key Design Decisions

**1. English-Native Compiler Core:**
- Compiler ONLY understands English keywords
- Lexer NEVER sees Turkish/Russian/Hindi
- Keeps compiler simple and maintainable
- No hardcoded language-specific logic
- Adding new language = 0 compiler changes

**2. Preprocessor for Multi-Language:**
- Runs BEFORE lexer (critical!)
- State machine: CODE/STRING/COMMENT states
- Translates keywords: YAZDIR → print
- Preserves UTF-8 strings: "Merhaba" unchanged
- Preserves comments completely
- Language-agnostic translation via diller.json
- Easy to add new languages (10 minutes, just JSON edit)

**3. UTF-8 Byte Sequence Approach:**
- Problem: NASM doesn't accept UTF-8 in strings
- Solution: Detect UTF-8, output as byte array
- UTF-8: `"Merhaba"` → `db 77,101,114,104,97,98,97,0`
- ASCII: `"Hello"` → `db "Hello", 0`
- Smart delimiter: Use `'` if string contains `"`
- Result: All languages display correctly

**4. Pipeline Separation:**
- Each layer has single responsibility
- Preprocessor: Language translation
- Compiler: English → Assembly
- UTF-8 Handler: Encoding conversion
- Clean architecture, easy maintenance

**Result:** True multi-language support where all languages are equal!

---

## 📚 Language Features

### Data Types
- `int` - 64-bit integer
- `string` - String (char*)
- `list[Type]` - Dynamic lists (Phase 2 ✅)

### Control Structures
```mlp
-- Conditional
if x > 10 then
    print "Large"
else
    print "Small"
end func

-- Loop
int i = 0;
while
    if i >= 10 then
        break
    end
    print i
    i = i + 1
end func
```

### Functions

**Basic Function:**
```mlp
func fibonacci(n)
    if n <= 1 then
        return n
    end
    return fibonacci(n - 1) + fibonacci(n - 2)
end func

int result = fibonacci(10);
print result
```

**Default Parameters (Phase 5.11 ✅):**
```mlp
func greet(name = "World")
    print "Hello, " + name
end func

greet()           -- Output: Hello, World
greet("Alice")    -- Output: Hello, Alice
```

### Built-in Functions

**Dynamic Lists (Phase 2 ✅):**
- `list_create()` - Create new list
- `list.add(value)` - Append element
- `list.get(index)` - Get element by index
- `list.size()` - Get element count
- `list.clear()` - Remove all elements
- `list_destroy(list)` - Free memory

**String Operations:**
- `string_concat(a, b)` - Concatenate strings
- `string_length(s)` - Get length
- `string_char_at(s, i)` - Get character at index
- `string_substring(s, start, len)` - Extract substring
- `char_code(c)` - Get ASCII code
- `code_to_char(n)` - Convert code to char

**File I/O:**
- `file_open(path, mode)` - Open file
- `file_read(handle)` - Read file
- `file_write(handle, data)` - Write file
- `file_close(handle)` - Close file

**System:**
- `get_cwd()` - Get current working directory

**GUI (Desktop Applications):**
- `gui_window_create(title, w, h)` - Create window
- `gui_button_create(win, text, x, y, w, h)` - Add button
- `gui_label_create(win, text, x, y)` - Add label
- `gui_canvas_draw_rect(canvas, x, y, w, h)` - Draw rectangle
- `gui_poll_event()` - Get next event
- See [GUI Development Guide](#desktop-gui-development) for full API

---

## 🎨 Desktop GUI Development

MLP supports creating **native desktop applications** with graphical interfaces!

### Quick Example

**English:**
```mlp
-- lang: en-US
int window = gui_window_create("My App", 800, 600);
int button = gui_button_create(window, "Click Me", 300, 250, 120, 40);
int count = 0;

gui_window_show(window)

while
    int event = gui_poll_event();

    if event == 1 then  -- Quit event
        break
    end

    if event == 2 then  -- Button click
        int clicked = gui_get_clicked_button();
        if clicked == button then
            count = count + 1
            print "Button clicked! Count:"
            print count
        end
    end
end func

gui_window_destroy(window)
```

**Turkish:**
```mlp
-- lang: tr-TR
SAYISAL pencere = gui_window_create("Uygulamam", 800, 600);
SAYISAL buton = gui_button_create(pencere, "Tıkla", 300, 250, 120, 40);
SAYISAL sayac = 0;

gui_window_show(pencere)

DÖNGÜ
    SAYISAL olay = gui_poll_event();

    EĞER olay == 1 İSE  -- Kapatma olayı
        DÖNGÜ_BITIR
    SON

    EĞER olay == 2 İSE  -- Buton tıklama
        SAYISAL tiklanan = gui_get_clicked_button();
        EĞER tiklanan == buton İSE
            sayac = sayac + 1
            YAZDIR "Buton tıklandı! Sayı:"
            YAZDIR sayac
        SON
    SON
SON

gui_window_destroy(pencere)
```

### 🎨 MLP GUI Designer - Visual Studio Style Editor

**NEW!** Create GUI applications visually with drag-and-drop interface!

**MLP GUI Designer** is a professional, Electron-based visual GUI designer that lets you:

- 🖱️ **Drag & Drop Widgets**: Button, Label, TextBox, CheckBox, and more
- ⚙️ **Live Property Editor**: Edit properties in real-time
- 🔄 **Design/Code View**: Switch between visual design and generated code
- 💾 **Project Management**: Save and load projects (.mlpgui format)
- 📤 **MLP Code Export**: Export to runnable MLP code

**Quick Start:**
```bash
cd mlp_gui_designer
npm install
npm start
```

**Features:**

- Visual Studio-style interface with dark theme
- 9 widget types (Button, Label, TextBox, CheckBox, RadioButton, Panel, GroupBox, ListBox, ComboBox)
- Property panel with live editing
- Event handler support (onClick, onChange, etc.)
- Automatic MLP code generation
- Keyboard shortcuts (Ctrl+S, Ctrl+E, Delete, etc.)
- Resize and reposition widgets with mouse
- Export to .mlp files ready to compile

**See:** [mlp_gui_designer/README.md](mlp_gui_designer/README.md) for detailed documentation

---

### VSCode Development Workflow

**1. Install Prerequisites:**
```bash
# Linux/Ubuntu
sudo apt install libsdl2-dev

# macOS
brew install sdl2
```

**2. Install MLP VSCode Extension:**
```bash
code --install-extension vscode-mlp/mlp-language-*.vsix
```

**3. Create Your GUI App:**
- Create new `.mlp` file
- Write GUI code with IntelliSense
- Press `Ctrl+Shift+B` to build
- Run your application!

**4. Features:**
- ✅ Syntax highlighting for GUI functions
- ✅ Code snippets (`guiwin`, `guibtn`, `guicanvas`)
- ✅ One-click build and run
- ✅ Error highlighting
- ✅ Multi-language support

### Example Projects

**Button Counter:**
```mlp
-- Simple click counter with label update
int window = gui_window_create("Counter", 400, 200);
int label = gui_label_create(window, "Count: 0", 150, 50);
int button = gui_button_create(window, "Click", 150, 100, 100, 40);
-- Event loop with click handling...
```

**Canvas Drawing:**
```mlp
-- Draw shapes on canvas
int canvas = gui_canvas_create(window, 0, 0, 800, 600);
gui_canvas_set_color(canvas, 255, 0, 0);
gui_canvas_draw_rect(canvas, 100, 100, 200, 150);
gui_canvas_draw_circle(canvas, 400, 300, 80);
```

**Complete Documentation:**
- 📖 **[GUI Development Guide](GUI_DEVELOPMENT_GUIDE.md)** - Full tutorial
- 🎨 **[GUI Design Spec](GUI_DESIGN.md)** - API reference
- 💡 **Examples:** `example_gui_button.mlp`, `example_gui_turkish.mlp`

### GUI Backend Options

| Backend | Platform | Use Case | Status |
|---------|----------|----------|--------|
| **Mock** | All | Testing/Demo | ✅ Ready |
| **SDL2** | Cross-platform | Games/Multimedia | 🔄 Recommended |
| **GTK** | Linux | Native Desktop | 📋 Planned |
| **Raylib** | Cross-platform | Graphics | 📋 Planned |

**Current Status:** Mock backend included for testing. Real SDL2 backend ready to implement!

---

## 🛠️ Usage

### The `mlpc` Compiler Wrapper

MLP provides a one-command compilation script:

```bash
./mlpc <input.mlp> [options]

Options:
  -o <output>     Output executable name
  --lang=<id>     Force language (e.g., tr-TR, ru-RU)
  --keep-asm      Keep assembly file
  --keep-prep     Keep preprocessed file
  --no-prep       Skip preprocessor (English-only source)
  --verbose       Show all compilation steps

Examples:
  ./mlpc program.mlp
  ./mlpc program.mlp -o myapp
  ./mlpc turkish.mlp --lang=tr-TR
  ./mlpc english.mlp --no-prep
```

### Manual Compilation

```bash
# 1. Preprocess (multi-language → English)
python3 dil_cevirici.py input.mlp output.preprocessed.mlp

# 2. Compile (English → Assembly)
./c_compiler/compiler_test output.preprocessed.mlp output.asm

# 3. Assemble (Assembly → Object)
nasm -f elf64 output.asm -o output.o

# 4. Link (Object → Executable)
gcc output.o runtime/runtime.o -o output -no-pie
```

---

## 🌐 Adding New Languages

Edit `diller.json`:

```json
{
  "languages": [
    {
      "id": "fr-FR",
      "name": "Français (French)",
      "description": "French programming keywords",
      "keywords": {
        "int": ["entier"],
        "string": ["chaîne"],
        "if": ["si"],
        "then": ["alors"],
        "else": ["sinon"],
        "while": ["tant_que"],
        "break": ["arrêter"],
        "end": ["fin"],
        "function": ["fonction"],
        "return": ["retour"],
        "print": ["afficher"],
        "true": ["vrai"],
        "false": ["faux"]
      }
    }
  ]
}
```

Now you can write:

```mlp
-- lang: fr-FR
entier x = 42;
afficher "Bonjour le monde"
afficher x
```

---

## 📊 Project Statistics

| Metric | Value |
|--------|-------|
| **Bootstrap Compiler (C)** | ~3,700 lines |
| **Self-Hosting Compiler (MLP)** | 984 lines |
| **Runtime Library (C)** | ~200 lines |
| **Preprocessor (Python)** | ~200 lines |
| **Supported Languages** | 5+ (extensible) |
| **Test Programs** | 10+ |
| **Assembly Output Ratio** | 1 MLP → ~2.8 assembly lines |

---

## 🧪 Examples & Tests

### Test Cases

```bash
# Turkish
./mlpc test_direct_turkish.mlp -o test_tr
./test_tr

# Russian
./mlpc test_russian.mlp -o test_ru
./test_ru

# Chinese
./mlpc test_chinese.mlp -o test_zh
./test_zh

# Hindi
./mlpc test_hindi.mlp -o test_hi
./test_hi
```

All produce the same result from equivalent source code!

### Self-Hosting Test

```bash
# Compile the MLP compiler using itself
./mlpc tydc_unified.mlp -o tydc
./tydc input.mlp output.asm
```

---

## 🔬 Technical Details

### Compiler Components

**c_compiler/** - C Bootstrap Compiler
- `c_lexer.c/h` - Lexical analysis (UTF-8 aware)
- `c_parser.c/h` - Syntax analysis (AST construction)
- `c_generator.c/h` - Code generation (x86-64 NASM)
- `main.c` - Entry point

**runtime/** - Runtime Library
- `runtime.c` - Built-in functions (I/O, strings)

**dil_cevirici.py** - Multi-Language Preprocessor
- Language detection from `-- lang:` header
- Keyword translation via `diller.json`
- State machine for safe processing
- Preserves strings and comments

### Code Generation

- **Target:** x86-64 Linux (System V ABI)
- **Assembler:** NASM
- **Linker:** GCC
- **Stack frames:** RBP-based
- **Calling convention:** RDI, RSI, RDX, RCX, R8, R9
- **Return value:** RAX

---

## 🗂️ Project Structure

```
MLP/
├── 📄 Core Documentation
│   ├── README.md            # This file - Project overview
│   ├── SPECS.md             # 🔒 Language specification (LOCKED)
│   ├── AI_RULES.md          # Guidelines for AI assistants
│   ├── TODO.md              # Feature roadmap and status
│   ├── API_REFERENCE.md     # Runtime API documentation
│   ├── ROADMAP.md           # Development roadmap
│   ├── CONTRIBUTING.md      # Contribution guidelines
│   └── LICENSE              # MIT License
│
├── 🏗️ Compiler (Bootstrap - C)
│   ├── stage0/              # C-based bootstrap compiler
│   │   ├── c_lexer.c/h     # Tokenizer
│   │   ├── c_parser.c/h    # AST parser
│   │   └── c_generator.c/h # Assembly generator
│   └── mlpc                 # Compiler wrapper script
│
├── 🚀 Self-Hosting Compiler (MLP)
│   └── self_host/           # MLP compiler written in MLP ✅
│       ├── mlpc.mlp         # Main compiler
│       ├── lexer.mlp        # Tokenizer in MLP
│       ├── parser.mlp       # Parser in MLP
│       ├── generator.mlp    # Code generator in MLP
│       └── howto.md         # Self-hosting guide
│
├── 🔧 Runtime & Libraries
│   ├── runtime/
│   │   ├── runtime.c        # Standard library (I/O, strings, math)
│   │   └── hashmap.c        # Hash map implementation
│   └── mlp_lib/             # MLP standard library modules
│
├── 🌍 Multi-Language Support
│   ├── dil_cevirici.py      # Preprocessor (keyword translation)
│   ├── diller.json          # Language keyword definitions
│   └── syntax.json          # Syntax mappings
│
├── 📦 Examples & Tests
│   ├── examples/            # Sample programs
│   │   ├── merhaba_dunya.mlp  # Hello World (Turkish)
│   │   ├── hello_english.mlp  # Hello World (English)
│   │   └── *.preprocessed.mlp # Preprocessor outputs
│   └── tests/               # Test suite
│       ├── test_*.mlp       # Unit tests
│       └── test_*.asm       # Assembly outputs
│
├── 📚 Archive
│   └── docs/archive/        # Historical documentation
│       ├── PHASE*.md        # Development phase reports
│       ├── *_DESIGN.md      # Design documents
│       └── SESSION*.md      # Session summaries
│
└── 🛠️ Development Tools
    ├── vscode-mlp/          # VS Code extension
    ├── gui_editor/          # GUI editor (experimental)
    └── archive_python/      # Legacy Python implementations
```

### Key Directories

- **`stage0/`**: C bootstrap compiler (production-ready)
- **`self_host/`**: MLP self-hosting compiler ✅ (22 Nov 2024)
- **`runtime/`**: C runtime library with 18+ string functions
- **`examples/`**: Sample programs in multiple languages
- **`tests/`**: Comprehensive test suite
- **`docs/archive/`**: Historical development documentation

### Self-Hosting Status ✅

MLP is **fully self-hosting** as of November 22, 2024:
- Compiler written in MLP: `self_host/mlpc.mlp` (984 lines)
- Two compilation paths available:
  1. **MLP → Assembly** (Direct via `generator.mlp`)
  2. **MLP → C → Assembly** (Default, more optimized)

See [self_host/howto.md](./self_host/howto.md) for usage.

---

## 🤝 Contributing

Contributions are welcome!

### Feature Ideas
- [x] Dynamic lists (Phase 2 ✅)
- [ ] Dictionaries/hash maps (Phase 3 - Planned)
- [ ] Structs/custom types (Phase 4 - Planned)
- [ ] More languages (Arabic, Japanese, Spanish, etc.)
- [ ] Float/double support
- [ ] Module/import system
- [ ] Standard library expansion
- [ ] LLVM backend
- [ ] WebAssembly target

### How to Contribute

1. Fork the repository
2. Create a feature branch
3. Add your changes
4. Test thoroughly
5. Submit a Pull Request

---

## 📖 Documentation

### Core Documentation
- **[SPECS.md](./SPECS.md)** 🔒 - Complete language specification (LOCKED)
- **[AI_RULES.md](./AI_RULES.md)** - Guidelines for AI assistants
- **[TODO.md](./TODO.md)** - Feature roadmap and current status
- **[API_REFERENCE.md](./API_REFERENCE.md)** - Runtime API documentation
- **[ROADMAP.md](./ROADMAP.md)** - Development roadmap

### Implementation Guides
- **[self_host/howto.md](./self_host/howto.md)** - Self-hosting compiler usage
- **[diller.json](./diller.json)** - Language keyword definitions

### Historical Documentation
All historical documents (PHASE*.md, *_DESIGN.md, SESSION*.md) have been moved to `docs/archive/` for reference.

---

## 🎓 Learning Resources

### Compiler Development
- [Crafting Interpreters](https://craftinginterpreters.com/)
- [Engineering a Compiler](https://www.elsevier.com/books/engineering-a-compiler/cooper/978-0-12-088478-0)
- [x86-64 Assembly Guide](https://cs.brown.edu/courses/cs033/docs/guides/x64_cheatsheet.pdf)

### Self-Hosting
- [Self-hosting (compilers)](https://en.wikipedia.org/wiki/Self-hosting_(compilers))
- [Bootstrapping a Compiler](https://www.cs.utexas.edu/users/novak/cs375/bootstrapping.html)

---

## 📜 License

MIT License

Copyright (c) 2025 MLP Contributors

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

---

## 🙏 Acknowledgments

MLP was inspired by:
- **C** - Systems programming and performance
- **Python** - Accessible language design
- **Rust** - Memory safety concepts
- **LLVM** - Modern compiler infrastructure

---

## 📧 Contact

- **GitHub Issues**: [github.com/guvenacar/MLP/issues](https://github.com/guvenacar/MLP/issues)
- **Email**: guven.acar@gmail.com
- **Discussions**: [github.com/guvenacar/MLP/discussions](https://github.com/guvenacar/MLP/discussions)

---

## 🌟 Achievements

**MLP proves that programming languages don't have to be English-only!**

- ✅ **Multi-Language Support** - 5+ languages supported
- ✅ **Production Ready** - Fully working compiler
- ✅ **Self-Hosting** - Compiler written in MLP
- ✅ **Native Performance** - Direct x86-64 assembly
- ✅ **Extensible** - Easy to add new languages
- ✅ **Open Source** - MIT licensed

**Code in your language. Think in your language. Create in your language.** 🌍

---

**⭐ If you like this project, please give it a star on GitHub!**

**🚀 Contributions welcome via Pull Requests!**

**📖 Share your ideas in Discussions!**
