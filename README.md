# MLP - Multi-Language Programming

> **"Write Once, Any Syntax" - A Self-Hosting Modern Programming Language**

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Self-Hosting](https://img.shields.io/badge/self--hosting-yes-green.svg)]()
[![Status](https://img.shields.io/badge/status-Alpha-orange)]()

---

## 🎯 What is MLP?

**MLP (Multi-Language Programming)** is a self-hosting, modern programming language that allows developers to write code in their preferred syntax style.

### Key Features

1. **Multi-Syntax Support**: Write the same program in different syntax styles
   - Python-style, Java-style, C-style, VB.NET-style
   - Define your own custom syntax

2. **Self-Hosting**: MLP compiler is written in MLP itself
   - Successfully bootstrapped ✅
   - Zero dependency on Python/C++/other languages

3. **Modern Features**:
   - OOP (Classes, Inheritance, Polymorphism, Traits)
   - Generics/Templates
   - Pattern Matching
   - Async/Await
   - Memory Safety (Ownership/Borrowing)
   - Type Inference

4. **Native Performance**:
   - Direct assembly/LLVM backend
   - C-level performance
   - Zero overhead abstractions

---

## 🚀 Quick Start

### Installation

```bash
# Download binary (coming soon) 
curl -sSL https://mlp-lang.org/install.sh | sh 

# Or build from source
git clone https://github.com/guvenacar/MLP.git
cd MLP
./build.sh
```

### Your First Program

**Python-style syntax:**
```mlp
class Person:
    def __init__(self, name: str, age: int):
        self.name = name
        self.age = age

    def greet(self):
        print(f"Hello, I'm {self.name}, {self.age} years old")

john = Person("John", 25)
john.greet()
```

**Java-style syntax (same code!):**
```mlp
class Person {
    String name;
    int age;

    Person(String name, int age) {
        this.name = name;
        this.age = age;
    }

    void greet() {
        System.out.println("Hello, I'm " + this.name + ", " + this.age + " years old");
    }
}

Person john = new Person("John", 25);
john.greet();
```

**Native MLP syntax (Turkish keywords):**
```mlp
SINIF Kişi
    METIN isim;
    SAYISAL yaş;

    KURUCU(METIN isim, SAYISAL yaş)
        BU.isim = isim;
        BU.yaş = yaş;
    KURUCU SON

    IŞLEÇ selamla()
        YAZDIR "Merhaba, ben " + BU.isim + ", " + BU.yaş + " yaşındayım";
    IŞLEÇ SON
SINIF SON

Kişi ahmet = YENİ Kişi("Ahmet", 25);
ahmet.selamla();
```

### Compile and Run

```bash
# Compile
mlp program.mlp -o program

# Run
./program
```

---

## 📚 Features

### ✅ Completed

- [x] Self-hosting compiler (written in MLP!)
- [x] Multi-syntax preprocessing
- [x] Basic OOP (Classes, Methods)
- [x] Functions and closures
- [x] Lexer/Parser/Code Generator
- [x] Assembly backend (x86_64)
- [x] C Runtime integration
- [x] String/Array/Dictionary operations
- [x] VSCode extension

### 🚧 In Development

- [ ] Full OOP (Inheritance, Traits, Abstract classes)
- [ ] Generics with constraints
- [ ] Pattern matching
- [ ] Error handling (try/catch + Result<T,E>)
- [ ] LLVM backend
- [ ] Type inference (Hindley-Milner)
- [ ] Async/await
- [ ] Memory safety (ownership/borrowing)
- [ ] Module system
- [ ] Standard library

### 🔮 Planned

- [ ] JIT compilation
- [ ] WebAssembly target
- [ ] Package manager
- [ ] GUI framework (GTK+/Qt bindings)
- [ ] IDE (written in MLP!)
- [ ] Debugger and Profiler
- [ ] Language Server Protocol
- [ ] REPL
- [ ] Jupyter notebook integration

---

## 🏗️ Architecture

```
┌────────────────────────────────────────┐
│  User Code (Multi-Syntax)              │
│  Python/Java/C/VB.NET/Native MLP       │
└────────────────────────────────────────┘
                ↓
┌────────────────────────────────────────┐
│  MLP Compiler (written in MLP!)        │
│  --------------------------------       │
│  Lexer → Parser → AST → Optimizer      │
│  → Code Generator                      │
└────────────────────────────────────────┘
                ↓
┌────────────────────────────────────────┐
│  Backend (Assembly/LLVM/WASM)          │
└────────────────────────────────────────┘
                ↓
┌────────────────────────────────────────┐
│  Native Binary / JIT / WASM            │
└────────────────────────────────────────┘
```

---

## 📖 Documentation

- [Language Reference](./docs/LANGUAGE_REFERENCE.md)
- [OOP Guide](./docs/OOP_GUIDE.md)
- [Pattern Matching](./docs/PATTERN_MATCHING.md)
- [Async/Await](./docs/ASYNC_AWAIT.md)
- [Compiler Internals](./docs/COMPILER_INTERNALS.md)
- [Multi-Syntax Guide](./docs/MULTI_SYNTAX.md)
- [Contributing Guide](./CONTRIBUTING.md)

---

## 🔧 Development

### Project Structure

```
MLP/
├── src/                    # MLP compiler source (written in MLP!)
│   ├── lexer/             # Lexer
│   ├── parser/            # Parser
│   ├── ast/               # AST nodes
│   ├── semantic/          # Semantic analyzer
│   ├── optimizer/         # Code optimizer
│   ├── codegen/           # Code generators
│   │   ├── asm/          # x86_64 assembly
│   │   ├── llvm/         # LLVM IR
│   │   └── wasm/         # WebAssembly
│   ├── runtime/           # Runtime library (C)
│   └── stdlib/            # Standard library (MLP)
│
├── stage0/                # Bootstrap seed compiler
├── tests/                 # Test suite
├── docs/                  # Documentation
├── examples/              # Example programs
├── vscode-mlp/           # VSCode extension
├── build.sh              # Build script
└── bootstrap.sh          # Bootstrap script
```

### Building

```bash
# Self-compile
./mlp src/compiler/main.mlp -o mlp-new

# Run tests
./test.sh

# Benchmark
./benchmark.sh
```

---

## 🌟 Examples

### 1. OOP Example - Shapes

```mlp
-- Define a trait
TRAIT Drawable
    IŞLEÇ draw()
TRAIT SON

-- Base class
ABSTRACT SINIF Shape IMPLEMENTS Drawable
    METIN color;

    KURUCU(METIN color)
        BU.color = color;
    KURUCU SON

    ABSTRACT IŞLEÇ area() -> SAYISAL
SINIF SON

-- Derived class: Circle
SINIF Circle EXTENDS Shape
    SAYISAL radius;

    KURUCU(METIN color, SAYISAL radius)
        SUPER(color);
        BU.radius = radius;
    KURUCU SON

    OVERRIDE IŞLEÇ area() -> SAYISAL
        DÖNÜŞ 3.14159 * BU.radius * BU.radius;
    IŞLEÇ SON

    OVERRIDE IŞLEÇ draw()
        YAZDIR "Drawing: " + BU.color + " circle, area: " + BU.area();
    IŞLEÇ SON
SINIF SON

-- Usage
DİZİ<Shape> shapes = [
    YENİ Circle("red", 5),
    YENİ Rectangle("blue", 10, 20),
];

HER shape İÇİNDE shapes
    shape.draw();
HER SON
```

### 2. Generics Example - Stack

```mlp
GENERIC<T> SINIF Stack
    DİZİ<T> items;

    KURUCU()
        BU.items = [];
    KURUCU SON

    IŞLEÇ push(T item)
        DİZİ_EKLE BU.items item;
    IŞLEÇ SON

    IŞLEÇ pop() -> T?
        EĞER BOŞ_MU BU.items İSE
            DÖNÜŞ HİÇLİK;
        EĞER SON
        DÖNÜŞ DİZİ_SON_ÇIKAR BU.items;
    IŞLEÇ SON
SINIF SON

-- Usage
Stack<SAYISAL> numbers = YENİ Stack<SAYISAL>();
numbers.push(10);
numbers.push(20);
YAZDIR numbers.pop();  -- 20
```

### 3. Pattern Matching Example

```mlp
ENUM Result<T, E>
    Success(T),
    Error(E)
ENUM SON

IŞLEÇ read_file(METIN path) -> Result<METIN, METIN>
    DENE
        METIN content = DOSYA_OKU path;
        DÖNÜŞ Result.Success(content);
    YAKALA error
        DÖNÜŞ Result.Error(error.message);
    DENE SON
IŞLEÇ SON

-- Pattern matching
Result<METIN, METIN> result = read_file("data.txt");

EŞLEŞTIR result
    DURUM Success(content)
        YAZDIR "File read: " + content;
    DURUM Error(err)
        YAZDIR "Error: " + err;
EŞLEŞTIR SON
```

---

## 🤝 Contributing

MLP is an open-source project and contributions are welcome!

```bash
# Fork the repository
git clone https://github.com/yourusername/MLP.git

# Create a feature branch
git checkout -b feature/amazing-feature

# Commit your changes
git commit -m 'feat: Add amazing feature'

# Push to the branch
git push origin feature/amazing-feature

# Open a Pull Request
```

See [CONTRIBUTING.md](./CONTRIBUTING.md) for detailed guidelines.

---

## 📜 License

This project is licensed under the MIT License - see the [LICENSE](./LICENSE) file for details.

---

## 🎉 Bootstrap Story

MLP was bootstrapped using **TYD-MLP** (a Python-based seed compiler).

### Bootstrap Process:

1. **Stage 0**: TYD-MLP (Python) - Seed compiler
2. **Stage 1**: Simple MLP compiler (written in MLP, compiled by TYD-MLP)
3. **Stage 2**: Full-featured MLP compiler (written in MLP, compiled by Stage 1)
4. **Stage 3**: Self-compile! (MLP compiler compiles itself)
5. **Verify**: Binaries are identical! ✅

**Now Python is no longer needed! MLP is completely self-sufficient!** 🚀

See [BOOTSTRAP.md](./docs/BOOTSTRAP.md) for detailed bootstrap documentation.

---

## 🌍 Community

- **Discord**: [MLP Community](https://discord.gg/mlp-lang) (Coming soon)
- **Forum**: [forum.mlp-lang.org](https://forum.mlp-lang.org) (Coming soon)
- **Twitter**: [@MLP_Lang](https://twitter.com/MLP_Lang) (Coming soon)

### Questions?
- 🐛 Report bugs: [Issues](https://github.com/guvenacar/MLP/issues)
- 💡 Feature requests: [Discussions](https://github.com/guvenacar/MLP/discussions)
- 📧 Email: guven.acar@gmail.com

---

## 🙏 Acknowledgments

MLP draws inspiration from:
- **Rust** - Memory safety and ownership model
- **Swift** - Clean syntax and modern features
- **Kotlin** - Multi-paradigm design
- **Python** - Readability and simplicity
- **Java** - Mature OOP system

---

## 📊 Project Statistics

```
Status:              Alpha
Lines of Code:       ~5,000 (and growing)
Supported Syntaxes:  4 (Native, Python, Java, C-style)
Self-Hosting:        ✅ Yes
Bootstrap Complete:  🚧 In Progress
```

---

## 🚀 Roadmap

### v0.1.0 (Current - Alpha)
- [x] Basic lexer/parser
- [x] C runtime integration
- [x] Basic syntax support
- [ ] Full OOP implementation

### v0.2.0 (Q1 2025)
- [ ] Generics
- [ ] Pattern matching
- [ ] Error handling
- [ ] Bootstrap complete

### v0.3.0 (Q2 2025)
- [ ] LLVM backend
- [ ] Type inference
- [ ] Async/await
- [ ] Standard library v1

### v1.0.0 (Q4 2025)
- [ ] Production-ready
- [ ] Full documentation
- [ ] Online playground
- [ ] Community tools

---

**⭐ If you like this project, please give it a star on GitHub!**

**MLP - Multi-Language Programming: Write Once, Any Syntax!** 🚀
